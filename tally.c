/*
 Tally: a simple, command-line SLOC counter.
 Copyright (C) 2014-2015 Craig Barnes.

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
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <locale.h>
#include "languages.h"

static LineCount line_counts[NUM_LANGUAGES] = {{0ULL, 0ULL, 0ULL}};
static u64 file_counts[NUM_LANGUAGES] = {0ULL};

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
    const char *const optstring = "sdh";
    const char *const help =
        "Usage: tally [OPTION] [PATH]...\n\n"
        "Options:\n\n"
        "   -s   Show line counts per language (default)\n"
        "   -d   Show detected file types\n"
        "   -h   Show usage information\n"
    ;

    int (*cb)(const char*, const struct stat*, int, struct FTW*) = summary;
    setlocale(LC_ALL, "");

    for (int opt; (opt = getopt(argc, argv, optstring)) != -1; ) {
        switch (opt) {
        case 'd':
            cb = detect;
            break;
        case 's':
            cb = summary;
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

        const char *fmt_header, *fmt_totals, *fmt_dimrow;
        const char *fmt_row = "%-15s %'10llu %'10llu %'10llu %'10llu\n";
        if (isatty(fileno(stdout))) {
            fmt_header = "\033[1m%-15s %10s %10s %10s %10s\033[0m\n";
            fmt_totals = "\033[1m%-15s %'10u %'10u %'10u %'10u\033[0m\n";
            fmt_dimrow = "\033[2m%-15s %'10llu\033[0m\n";
        } else {
            fmt_header = "%-15s %10s %10s %10s %10s\n";
            fmt_totals = "%-15s %'10u %'10u %'10u %'10u\n";
            fmt_dimrow = "%-15s %'10llu\n";
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
