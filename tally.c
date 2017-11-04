/*
 Tally: a simple, command-line SLOC counter.
 Copyright (C) 2014-2017 Craig Barnes.

 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by the
 Free Software Foundation; either version 2 of the License, or (at your
 option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 Public License for more details.
*/

#define _XOPEN_SOURCE 500 // For nftw(3)
#define _GNU_SOURCE // For FTW_ACTIONRETVAL
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <locale.h>
#include "languages.h"

#define COL " %'10" PRIu64

static LineCount line_counts[NUM_LANGUAGES] = {{0, 0, 0}};
static uint64_t file_counts[NUM_LANGUAGES] = {0};

static int detect(const char *f, const struct stat *s, int t, struct FTW *w) {
    if (t == FTW_F) {
        Language lang = detect_language(f, w->base, w->level, s->st_size);
        bool dotslash = f[0] == '.' && f[1] == '/';
        printf("%-20s  %s\n", dotslash? f+2 : f, lookup_language_name(lang));
    } else if (t == FTW_D && (f + w->base)[0] == '.' && w->level > 0) {
        return FTW_SKIP_SUBTREE;
    }

    return FTW_CONTINUE;
}

static int summary(const char *f, const struct stat *s, int t, struct FTW *w) {
    if (t == FTW_F) {
        Language language = detect_language(f, w->base, w->level, s->st_size);
        file_counts[language] += 1;
        if (language != IGNORED && language != UNKNOWN) {
            Parser parser = lookup_language_parser(language);
            LineCount c = parser(f, s->st_size);
            LineCount *count = &line_counts[language];
            count->code += c.code;
            count->comment += c.comment;
            count->blank += c.blank;
        }
    } else if (t == FTW_D && (f + w->base)[0] == '.' && w->level > 0) {
        return FTW_SKIP_SUBTREE;
    }

    return FTW_CONTINUE;
}

static int perfile(const char *f, const struct stat *s, int t, struct FTW *w) {
    if (t == FTW_F) {
        Language language = detect_language(f, w->base, w->level, s->st_size);
        if (language != IGNORED && language != UNKNOWN) {
            const char *name = lookup_language_name(language);
            Parser parser = lookup_language_parser(language);
            LineCount c = parser(f, s->st_size);
            printf("%'8" PRIu64 "  %-12s %s\n", c.code, name, f);
        }
    } else if (t == FTW_D && (f + w->base)[0] == '.' && w->level > 0) {
        return FTW_SKIP_SUBTREE;
    }

    return FTW_CONTINUE;
}

static int compare(const void *p1, const void *p2) {
    const Language l1 = *(const Language*)p1, l2 = *(const Language*)p2;
    const uint64_t c1 = (&line_counts[l1])->code, c2 = (&line_counts[l2])->code;
    if (c1 == c2) {
        return 0;
    } else if (c1 > c2) {
        return -1;
    } else {
        return 1;
    }
}

int main(int argc, char *argv[]) {
    static const char help[] =
        "Usage: tally [OPTION] [PATH]...\n\n"
        "Options:\n\n"
        "   -s   Show line counts per language (default)\n"
        "   -i   Show line counts per file\n"
        "   -d   Show detected file types\n"
        "   -h   Show usage information\n"
    ;

    int (*cb)(const char*, const struct stat*, int, struct FTW*) = summary;
    setlocale(LC_ALL, "");

    for (int opt; (opt = getopt(argc, argv, "sidh")) != -1; ) {
        switch (opt) {
        case 'd':
            cb = detect;
            break;
        case 's':
            cb = summary;
            break;
        case 'i':
            if (isatty(fileno(stdout))) {
                printf("\033[1m%8s  %-12s File\033[0m\n", "SLOC", "Language");
            } else {
                printf("%8s  %-12s File\n", "SLOC", "Language");
            }
            cb = perfile;
            break;
        case 'h':
            puts(help);
            exit(EXIT_SUCCESS);
        case '?':
            exit(EXIT_FAILURE);
        default:
            fprintf(stderr, "Error: unhandled option: -%c\n", opt);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        const char *const path = argv[i];
        if (access(path, R_OK) != 0) {
            perror(path);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        const char *const path = argv[i];
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
        size_t n = 0;
        uint64_t tfiles = 0, tcode = 0, tcomment = 0, tblank = 0;

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

        static const char *const fmt_row = "%-15s" COL COL COL COL "\n";
        static const char *bold = "";
        static const char *dim = "";
        static const char *reset = "";

        if (isatty(fileno(stdout))) {
            bold = "\033[1m";
            dim = "\033[2m";
            reset = "\033[0m";
        }

        printf (
            "%s%-15s %10s %10s %10s %10s%s\n",
            bold, "Language", "Files", "Code", "Comment", "Blank", reset
        );

        if (n == 1) {
            const Language lang = index[0];
            const char *const name = lookup_language_name(lang);
            const uint64_t nfiles = file_counts[lang];
            const LineCount *const c = &line_counts[lang];
            printf(fmt_row, name, nfiles, c->code, c->comment, c->blank);
            exit(EXIT_SUCCESS);
        }

        qsort(index, n, sizeof(Language), compare);
        for (size_t i = 0; i < n; i++) {
            const Language lang = index[i];
            const char *const name = lookup_language_name(lang);
            const uint64_t nfiles = file_counts[lang];
            const LineCount *const c = &line_counts[lang];
            printf(fmt_row, name, nfiles, c->code, c->comment, c->blank);
            tcode += c->code;
            tcomment += c->comment;
            tblank += c->blank;
        }

        printf (
            "%s%-15s" COL COL COL COL "%s\n",
            bold, "Total:", tfiles, tcode, tcomment, tblank, reset
        );

        if (file_counts[UNKNOWN] > 0ULL) {
            printf (
                "%s%-15s" COL "%s\n",
                dim, "Unrecognized", file_counts[UNKNOWN], reset
            );
        }

        if (file_counts[IGNORED] > 0ULL) {
            printf (
                "%s%-15s" COL "%s\n",
                dim, "Ignored", file_counts[IGNORED], reset
            );
        }
    }

    exit(EXIT_SUCCESS);
}
