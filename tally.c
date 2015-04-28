/*
 Tally: a simple, command-line SLOC counter.
 Copyright (C) 2014-2015 Craig Barnes.

 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License version 3, as published
 by the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 Public License version 3 for more details.
*/

#define _XOPEN_SOURCE 500 // For nftw(3)
#define _GNU_SOURCE // For FTW_ACTIONRETVAL
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <locale.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "languages.h"
#include "names.h"
#include "parse.h"

const LanguageHashSlot *lookup_language_by_extension(const char*, unsigned int);
const LanguageHashSlot *lookup_language_by_filename(const char*, unsigned int);

typedef unsigned long long int u64;
typedef struct {u64 code, comment, blank;} LineCount;
static LineCount line_counts[NUM_LANGUAGES] = {{0ULL, 0ULL, 0ULL}};
static u64 file_counts[NUM_LANGUAGES] = {0ULL};

static const char *fmt_header = "\033[1m%-15s %10s %10s %10s %10s\033[0m\n";
static const char *fmt_totals = "\033[1m%-15s %'10u %'10u %'10u %'10u\033[0m\n";
static const char *fmt_row = "%-15s %'10llu %'10llu %'10llu %'10llu\n";
static const char *fmt_dimrow = "\033[2m%-15s %'10llu\033[0m\n";

static inline const char *file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) {
        return NULL;
    }
    return dot + 1;
}

static Language detect_language(const char *filename, int base) {
    const char *const basename = filename + base;
    const char *const ext = file_extension(basename);
    const LanguageHashSlot *slot = NULL;

    if (ext && (slot = lookup_language_by_extension(ext, strlen(ext)))) {
        return slot->language;
    }

    if ((slot = lookup_language_by_filename(basename, strlen(basename)))) {
        return slot->language;
    }

    return UNKNOWN;
}

static int detect(const char *f, const struct stat *s, int t, struct FTW *w) {
    (void)s; // Unused
    const char *basename = f + w->base;
    const bool ignored = (basename[0] == '.' && w->level > 0);

    if (t == FTW_F && !ignored) {
        const char *path = (f[0] == '.' && f[1] == '/') ? f + 2 : f;
        Language lang = detect_language(f, w->base);
        printf("%-20s  %s\n", path, lookup_language_name(lang));
    }

    if (t == FTW_D && ignored) {
        return FTW_SKIP_SUBTREE;
    }

    return FTW_CONTINUE;
}

static char *mmapfile(const char *path, size_t size) {
    assert(path);
    assert(size > 0);
    int fd = open(path, O_RDONLY | O_NOFOLLOW);
    if (fd == -1) {
        perror("open");
        abort();
    }
    char *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        abort();
    }
    close(fd);
    return addr;
}

static void count_callback(Language language, LineType type, void *ud) {
    (void)ud;
    assert(language > IGNORED);
    assert(language < NUM_LANGUAGES);
    LineCount *count = &line_counts[language];
    switch (type) {
    case CODE:
        count->code += 1;
        break;
    case COMMENT:
        count->comment += 1;
        break;
    case BLANK:
        count->blank += 1;
        break;
    }
}

static void count_generic(const char *filename, Language language) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    LineCount *count = &line_counts[language];
    FILE *stream = fopen(filename, "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, stream)) != -1) {
        count->code += 1;
    }
    free(line);
    fclose(stream);
}

static int summary(const char *f, const struct stat *s, int t, struct FTW *w) {
    const char *basename = f + w->base;
    const bool ignored = (basename[0] == '.' && w->level > 0);

    if (t == FTW_F && !ignored) {
        Language language = detect_language(f, w->base);
        file_counts[language] += 1;
        size_t size = s->st_size;
        if (size > 0 && language != IGNORED && language != UNKNOWN) {
            ParserFunc parser = lookup_parser(language);
            if (parser) {
                char *map = mmapfile(f, size);
                parser(map, size, count_callback, NULL);
                munmap(map, size);
            } else {
                count_generic(f, language);
            }
        }
    } else if (t == FTW_D && ignored) {
        return FTW_SKIP_SUBTREE;
    }

    return FTW_CONTINUE;
}

int compare(const void *p1, const void *p2) {
    const Language l1 = *(const Language*)p1, l2 = *(const Language*)p2;
    const u64 c1 = (&line_counts[l1])->code, c2 = (&line_counts[l2])->code;
    if (c1 == c2) {
        return 0;
    } else if (c1 > c2) {
        return -1;
    } else {
        return 1;
    }
}

int main(int argc, char *argv[]) {
    int (*cb)(const char*, const struct stat*, int, struct FTW*) = summary;
    setlocale(LC_ALL, "");

    for (int opt; (opt = getopt(argc, argv, "ds")) != -1; ) {
        switch (opt) {
        case 'd':
            cb = detect;
            break;
        case 's':
            cb = summary;
            break;
        case '?':
            exit(EXIT_FAILURE);
        default:
            fprintf(stderr, "Error: unhandled option: -%c\n", opt);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        const char *path = argv[i];
        if (access(path, R_OK) != 0) {
            perror(path);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        const char *path = argv[i];
        if (nftw(path, cb, 20, FTW_PHYS | FTW_ACTIONRETVAL) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) {
        if (nftw(".", cb, 20, FTW_PHYS | FTW_ACTIONRETVAL) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
    }

    if (cb == summary) {
        Language index[NUM_LANGUAGES];
        unsigned int n = 0U;
        u64 tfiles = 0ULL, tcode = 0ULL, tcomment = 0ULL, tblank = 0ULL;
        for (Language lang = 2; lang < NUM_LANGUAGES; lang++) {
            if (file_counts[lang] > 0ULL) {
                index[n++] = lang;
                tfiles += file_counts[lang];
            }
        }
        if (n == 0) {
            puts("Nothing found");
            exit(EXIT_SUCCESS);
        }
        printf(fmt_header, "Language", "Files", "Code", "Comment", "Blank");
        if (n == 1) {
            Language lang = index[0];
            const char *name = lookup_language_name(lang);
            const u64 nfiles = file_counts[lang];
            const LineCount *const c = &line_counts[lang];
            printf(fmt_row, name, nfiles, c->code, c->comment, c->blank);
            exit(EXIT_SUCCESS);
        }
        qsort(index, n, sizeof(Language), compare);
        for (unsigned int i = 0U; i < n; i++) {
            Language lang = index[i];
            const char *name = lookup_language_name(lang);
            const u64 nfiles = file_counts[lang];
            const LineCount *const c = &line_counts[lang];
            printf(fmt_row, name, nfiles, c->code, c->comment, c->blank);
            tcode += c->code;
            tcomment += c->comment;
            tblank += c->blank;
        }
        printf(fmt_totals, "Total:", tfiles, tcode, tcomment, tblank);
        if (file_counts[UNKNOWN] > 0ULL) {
            printf(fmt_dimrow, "Unrecognized", file_counts[UNKNOWN]);
        }
        if (file_counts[IGNORED] > 0ULL) {
            printf(fmt_dimrow, "Ignored", file_counts[IGNORED]);
        }
    }

    exit(EXIT_SUCCESS);
}
