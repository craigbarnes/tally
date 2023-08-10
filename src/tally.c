/*
 Tally: a simple, command-line SLOC counter.
 Copyright (C) 2014-2021 Craig Barnes.

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
#include <locale.h>
#include "languages.h"

#define COL " %'10" PRIu64

static LineCount line_counts[NUM_LANGUAGES] = {{0, 0, 0}};
static uint64_t file_counts[NUM_LANGUAGES] = {0};

static const struct {
    const char name[16];
    const Parser parser;
} languages[NUM_LANGUAGES] = {
    [UNKNOWN] = {"?", NULL},
    [IGNORED] = {"!", NULL},
    [ADA] = {"Ada", parse_plain}, // TODO: comment-aware parser
    [APPLESCRIPT] = {"AppleScript", parse_plain}, // TODO: comment-aware parser
    [ASSEMBLY] = {"Assembly", parse_plain}, // TODO: comment-aware parser
    [AWK] = {"AWK", parse_shell},
    [BATCHFILE] = {"Batchfile", parse_plain},
    [BIBTEX] = {"BibTeX", parse_plain},
    [C] = {"C", parse_c},
    [CHEADER] = {"C Header", parse_c},
    [CLOJURE] = {"Clojure", parse_lisp},
    [CMAKE] = {"CMake", parse_shell},
    [COFFEESCRIPT] = {"CoffeScript", parse_shell},
    [COMMONLISP] = {"Common Lisp", parse_lisp},
    [CPLUSPLUS] = {"C++", parse_c},
    [CPLUSPLUSHEADER] = {"C++ Header", parse_c},
    [CRYSTAL] = {"Crystal", parse_shell},
    [CSHARP] = {"C#", parse_c},
    [CSS] = {"CSS", parse_css},
    [CSV] = {"CSV", parse_plain},
    [D] = {"D", parse_c},
    [DART] = {"Dart", parse_c},
    [DOCKER] = {"Dockerfile", parse_shell},
    [DEVICETREE] = {"Device Tree", parse_c},
    [EMACSLISP] = {"Emacs Lisp", parse_lisp},
    [GLSL] = {"GLSL", parse_c},
    [GO] = {"Go", parse_c},
    [GPERF] = {"gperf", parse_plain},
    [GROOVY] = {"Groovy", parse_c},
    [HASKELL] = {"Haskell", parse_plain}, // TODO: comment-aware parser
    [HTML] = {"HTML", parse_html},
    [INI] = {"INI", parse_shell},
    [JAVA] = {"Java", parse_c},
    [JAVASCRIPT] = {"JavaScript", parse_c},
    [JSON] = {"JSON", parse_plain},
    [LEX] = {"Lex", parse_c},
    [LUA] = {"Lua", parse_lua},
    [M4] = {"M4", parse_plain},
    [MAKE] = {"Make", parse_shell},
    [MALLARD] = {"Mallard", parse_xml},
    [MARKDOWN] = {"Markdown", parse_plain},
    [MESON] = {"Meson", parse_meson},
    [MOONSCRIPT] = {"MoonScript", parse_plain}, // TODO: comment-aware parser
    [NGINX] = {"Nginx config", parse_shell},
    [OBJECTIVEC] = {"Objective-C", parse_c},
    [PERL] = {"Perl", parse_shell},
    [PHP] = {"PHP", parse_c},
    [PKGCONFIG] = {"pkg-config", parse_shell},
    [PROTOBUF] = {"Protobuf", parse_c},
    [PYTHON] = {"Python", parse_python},
    [RACKET] = {"Racket", parse_lisp},
    [RESTRUCTUREDTEXT] = {"reST", parse_plain},
    [RUBY] = {"Ruby", parse_shell},
    [RUST] = {"Rust", parse_c},
    [SCALA] = {"Scala", parse_c},
    [SCHEME] = {"Scheme", parse_lisp},
    [SCSS] = {"SCSS", parse_c},
    [SED] = {"SED", parse_shell},
    [SHELL] = {"Shell", parse_shell},
    [SQL] = {"SQL", parse_sql},
    [TCL] = {"TCL", parse_shell},
    [TEXINFO] = {"Texinfo", parse_plain},
    [TEX] = {"TeX", parse_plain}, // TODO: comment-aware parser
    [TOML] = {"TOML", parse_shell},
    [TYPESCRIPT] = {"TypeScript", parse_c},
    [VALA] = {"Vala", parse_c},
    [VERILOG] = {"Verilog", parse_c},
    [VHDL] = {"VHDL", parse_plain}, // TODO: comment-aware parser
    [VIML] = {"VimL", parse_plain},
    [XML] = {"XML", parse_xml},
    [YACC] = {"YACC", parse_c},
    [YAML] = {"YAML", parse_shell},
};

static int detect(const char *f, const struct stat *s, int t, struct FTW *w)
{
    if (t == FTW_F) {
        Language lang = detect_language(f, w->base, w->level, s->st_size);
        bool dotslash = f[0] == '.' && f[1] == '/';
        printf("%12s  %s\n", languages[lang].name, dotslash? f+2 : f);
    } else if (t == FTW_D && (f + w->base)[0] == '.' && w->level > 0) {
        return FTW_SKIP_SUBTREE;
    }

    return FTW_CONTINUE;
}

static int summary(const char *f, const struct stat *s, int t, struct FTW *w)
{
    if (t == FTW_F) {
        Language lang = detect_language(f, w->base, w->level, s->st_size);
        file_counts[lang] += 1;
        Parser parser = languages[lang].parser;
        if (parser != NULL) {
            LineCount c = parser(f, s->st_size);
            LineCount *count = &line_counts[lang];
            count->code += c.code;
            count->comment += c.comment;
            count->blank += c.blank;
        }
    } else if (t == FTW_D && (f + w->base)[0] == '.' && w->level > 0) {
        return FTW_SKIP_SUBTREE;
    }

    return FTW_CONTINUE;
}

static int perfile(const char *f, const struct stat *s, int t, struct FTW *w)
{
    if (t == FTW_F) {
        Language lang = detect_language(f, w->base, w->level, s->st_size);
        Parser parser = languages[lang].parser;
        if (parser != NULL) {
            const char *const name = languages[lang].name;
            LineCount c = parser(f, s->st_size);
            printf("%'8" PRIu64 "  %-12s %s\n", c.code, name, f);
        }
    } else if (t == FTW_D && (f + w->base)[0] == '.' && w->level > 0) {
        return FTW_SKIP_SUBTREE;
    }

    return FTW_CONTINUE;
}

static int compare(const void *p1, const void *p2)
{
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

int main(int argc, char *argv[])
{
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
            if (isatty(STDOUT_FILENO)) {
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
            fputs("Nothing found\n", stderr);
            exit(EXIT_SUCCESS);
        }

        static const char *const fmt_row = "%-15s" COL COL COL COL "\n";
        static const char *bold = "";
        static const char *dim = "";
        static const char *reset = "";

        if (isatty(STDOUT_FILENO)) {
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
            const char *const name = languages[lang].name;
            const uint64_t nfiles = file_counts[lang];
            const LineCount *const c = &line_counts[lang];
            printf(fmt_row, name, nfiles, c->code, c->comment, c->blank);
            exit(EXIT_SUCCESS);
        }

        qsort(index, n, sizeof(Language), compare);
        for (size_t i = 0; i < n; i++) {
            const Language lang = index[i];
            const char *const name = languages[lang].name;
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
