/*
 Tally: a simple, command-line SLOC counter.
 Copyright (C) 2014-2025 Craig Barnes.

 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by the
 Free Software Foundation; either version 2 of the License, or (at your
 option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 Public License for more details.
*/

#define _GNU_SOURCE // For FTW_ACTIONRETVAL
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <inttypes.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "exitcode.h"
#include "languages.h"
#include "macros.h"

#define COL " %'10" PRIu64

typedef enum {
    OFLAG_PER_FILE = 1 << 0,
    OFLAG_TOTALS = 1 << 1,
    OFLAG_SHOW_UNKNOWN = 1 << 2,
} OutputFlags;

typedef struct {
    Language lang;
    uint64_t files;
    uint64_t code;
    uint64_t comment;
    uint64_t blank;
} FileAndLineCount;

static FileAndLineCount counts[NUM_LANGUAGES];
static OutputFlags outflags = 0;
static const char *bold = "";
static const char *dim = "";
static const char *reset = "";

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
    [COCCINELLE] = {"Coccinelle", parse_c},
    [COFFEESCRIPT] = {"CoffeeScript", parse_shell},
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
    [FISH] = {"fish", parse_shell},
    [GDSCRIPT] = {"GDScript", parse_python},
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
    [KCONFIG] = {"Kconfig", parse_config},
    [KOTLIN] = {"Kotlin", parse_c},
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
    [ODIN] = {"Odin", parse_c},
    [PERL] = {"Perl", parse_shell},
    [PHP] = {"PHP", parse_c},
    [PKGCONFIG] = {"pkg-config", parse_shell},
    [PROTOBUF] = {"Protobuf", parse_c},
    [PYTHON] = {"Python", parse_python},
    [RACKET] = {"Racket", parse_lisp},
    [RAGEL] = {"Ragel", parse_c},
    [RESTRUCTUREDTEXT] = {"reST", parse_plain},
    [RUBY] = {"Ruby", parse_shell},
    [RUST] = {"Rust", parse_c},
    [SCALA] = {"Scala", parse_c},
    [SCHEME] = {"Scheme", parse_lisp},
    [SCSS] = {"SCSS", parse_c},
    [SED] = {"SED", parse_shell},
    [SHELL] = {"Shell", parse_shell},
    [SQL] = {"SQL", parse_sql},
    [SWIFT] = {"Swift", parse_c}, // TODO: support nested long comments
    [TCL] = {"TCL", parse_shell},
    [TEXINFO] = {"Texinfo", parse_plain},
    [TEX] = {"TeX", parse_plain}, // TODO: comment-aware parser
    [TLVERILOG] = {"TL-Verilog", parse_c},
    [TOML] = {"TOML", parse_shell},
    [TSV] = {"TSV", parse_plain},
    [TYPESCRIPT] = {"TypeScript", parse_c},
    [VALA] = {"Vala", parse_c},
    [VERILOG] = {"Verilog", parse_c},
    [VHDL] = {"VHDL", parse_plain}, // TODO: comment-aware parser
    [VIML] = {"VimL", parse_plain},
    [XML] = {"XML", parse_xml},
    [YACC] = {"YACC", parse_c},
    [YAML] = {"YAML", parse_shell},
    [ZIG] = {"Zig", parse_c}, // TODO: Zig-specific parser (no long comments)
};

static bool is_ignored_dir(const char *path, const struct FTW *w)
{
    const char *base = path + w->base;
    return w->level > 0 && base[0] == '.';
}

// Remove the leading "./" prefix that nftw(3) includes in some paths
static const char *clean_path(const char *path)
{
    return (path[0] == '.' && path[1] == '/') ? path + 2 : path;
}

static char *mmapfile(const char *path, size_t size)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return NULL;
    }

    char *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    bool mmap_failed = unlikely(addr == MAP_FAILED);
    int err = mmap_failed ? errno : 0;
    close(fd);
    if (mmap_failed) {
        errno = err;
        perror("mmap");
        return NULL;
    }

    return addr;
}

static int count_lines (
    const char *path,
    const struct stat *s,
    int type,
    struct FTW *w
) {
    switch (type) {
    case FTW_F: // File
        break;
    case FTW_D: // Directory
        return is_ignored_dir(path, w) ? FTW_SKIP_SUBTREE : FTW_CONTINUE;
    default:
        return FTW_CONTINUE;
    }

    Language lang = detect_language(path, w->base, w->level);
    FileAndLineCount *count = &counts[lang];
    const Parser parser = languages[lang].parser;
    uintmax_t code_lines = 0;
    count->files++;
    if (!parser) {
        // lang == UNKNOWN || lang == IGNORED
        if (outflags & OFLAG_SHOW_UNKNOWN) {
            goto maybe_print;
        }
        return FTW_CONTINUE;
    }

    const size_t size = s->st_size;
    if (size == 0) {
        goto maybe_print;
    }

    char *text = mmapfile(path, size);
    if (!text) {
        return FTW_CONTINUE;
    }

    LineCount c = parser(text, size);
    code_lines = c.code;
    count->code += c.code;
    count->comment += c.comment;
    count->blank += c.blank;
    munmap(text, size);

maybe_print:
    if (outflags & OFLAG_PER_FILE) {
        const char *lang_name = languages[lang].name;
        printf("%'8ju  %-12s %s\n", code_lines, lang_name, clean_path(path));
    }

    return FTW_CONTINUE;
}

static int compare(const void *p1, const void *p2)
{
    const FileAndLineCount *c1 = p1;
    const FileAndLineCount *c2 = p2;
    const uint64_t n1 = c1->code;
    const uint64_t n2 = c2->code;
    if (n1 == n2) {
        return 0;
    } else if (n1 > n2) {
        return -1;
    } else {
        return 1;
    }
}

static void print_summary(void)
{
    FileAndLineCount total = {0};
    uint64_t total_lines = 0;
    unsigned int nlangs = 0;

    for (Language lang = 2; lang < NUM_LANGUAGES; lang++) {
        FileAndLineCount *c = &counts[lang];

        // The array indices no longer correspond to Language enum
        // values after sorting, so each entry must be tagged such
        // that the name can be looked up by the loop further below
        c->lang = lang;

        if (c->files == 0) {
            continue;
        }

        nlangs++;
        total.files += c->files;
        total.code += c->code;
        total.comment += c->comment;
        total.blank += c->blank;
        total_lines += c->code + c->comment + c->blank;
    }

    if (nlangs == 0) {
        fputs("Nothing found\n", stderr);
        return;
    }

    qsort(counts + 2, NUM_LANGUAGES - 2, sizeof(FileAndLineCount), compare);

    printf (
        "%s%-15s %10s %10s %10s %10s %10s%s\n",
        bold, "Language", "Files", "Code", "Comment", "Blank", "Lines", reset
    );

    for (size_t i = 2; i < NUM_LANGUAGES; i++) {
        const FileAndLineCount *c = &counts[i];
        if (c->files == 0) {
            continue;
        }
        const char *name = languages[c->lang].name;
        uint64_t lines = c->code + c->comment + c->blank;
        printf (
            "%-15s" COL COL COL COL COL "\n",
            name, c->files, c->code, c->comment, c->blank, lines
        );
    }

    if (nlangs > 1) {
        printf (
            "%s%-15s" COL COL COL COL COL "%s\n",
            bold, "Total:",
            total.files,
            total.code,
            total.comment,
            total.blank,
            total_lines,
            reset
        );
    }

    uint64_t unknown = counts[UNKNOWN].files;
    if (unknown > 0) {
        printf (
            "%s%-15s" COL "%s\n",
            dim, "Unrecognized", unknown, reset
        );
    }

    uint64_t ignored = counts[IGNORED].files;
    if (ignored > 0) {
        printf (
            "%s%-15s" COL "%s\n",
            dim, "Ignored", ignored, reset
        );
    }
}

int main(int argc, char *argv[])
{
    static const char help[] =
        "Usage: tally [OPTION] [PATH]...\n\n"
        "Options:\n\n"
        "   -s   Show line counts per language (default)\n"
        "   -i   Show line counts per file\n"
        "   -d   Like -i, but also showing unknown/ignored files\n"
        "   -a   All of the above (equivalent to -sid)\n"
        "   -h   Show usage information\n"
    ;

    setlocale(LC_ALL, "");

    for (int opt; (opt = getopt(argc, argv, "sidah")) != -1; ) {
        switch (opt) {
        case 'a':
            outflags |= OFLAG_SHOW_UNKNOWN | OFLAG_PER_FILE | OFLAG_TOTALS;
            continue;
        case 'd':
            outflags |= OFLAG_SHOW_UNKNOWN | OFLAG_PER_FILE;
            continue;
        case 'i':
            outflags |= OFLAG_PER_FILE;
            continue;
        case 's':
            outflags |= OFLAG_TOTALS;
            continue;
        case 'h':
            puts(help);
            return EX_OK;
        case '?':
            return EX_USAGE;
        default:
            fprintf(stderr, "Error: unhandled option: -%c\n", opt);
            return EX_SOFTWARE;
        }
    }

    // Default to showing just the totals, if no flags are specified
    outflags |= outflags ? 0 : OFLAG_TOTALS;

    for (int i = optind; i < argc; i++) {
        const char *path = argv[i];
        if (access(path, R_OK) != 0) {
            perror(path);
            return EX_NOINPUT;
        }
    }

    if (isatty(STDOUT_FILENO)) {
        bold = "\033[1m";
        dim = "\033[2m";
        reset = "\033[0m";
    }

    if (outflags & OFLAG_PER_FILE) {
        printf("%s%8s  %-12s File%s\n", bold, "SLOC", "Language", reset);
    }

    for (int i = optind; i < argc; i++) {
        const char *path = argv[i];
        if (nftw(path, count_lines, 20, FTW_PHYS | FTW_ACTIONRETVAL) == -1) {
            perror("nftw");
            return EX_IOERR;
        }
    }

    if (optind == argc) {
        if (nftw(".", count_lines, 20, FTW_PHYS | FTW_ACTIONRETVAL) == -1) {
            perror("nftw");
            return EX_IOERR;
        }
    }

    if (outflags & OFLAG_TOTALS) {
        if (outflags & OFLAG_PER_FILE) {
            fputc('\n', stdout);
        }
        print_summary();
    }

    return EX_OK;
}
