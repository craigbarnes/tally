#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parse.h"
#include "languages.h"

static const ParserFunc parsers[NUM_LANGUAGES] = {
    [ADA] = NULL,
    [APPLESCRIPT] = NULL,
    [ASSEMBLY] = NULL,
    [AWK] = parse_shell,
    [BATCHFILE] = NULL,
    [C] = parse_c,
    [CHEADER] = parse_c,
    [COFFEESCRIPT] = parse_shell,
    [COMMONLISP] = NULL,
    [CPLUSPLUS] = parse_c,
    [CPLUSPLUSHEADER] = parse_c,
    [CSHARP] = parse_c,
    [CSS] = NULL,
    [D] = parse_c,
    [DART] = parse_c,
    [EMACSLISP] = NULL,
    [GO] = parse_c,
    [GPERF] = NULL,
    [GROOVY] = parse_c,
    [HASKELL] = NULL,
    [HTML] = NULL,
    [INI] = parse_shell,
    [JAVA] = parse_c,
    [JAVASCRIPT] = parse_c,
    [JSON] = parse_plain,
    [LEX] = NULL,
    [LUA] = NULL,
    [MAKE] = parse_shell,
    [MALLARD] = NULL,
    [MARKDOWN] = parse_plain,
    [MOONSCRIPT] = NULL,
    [OBJECTIVEC] = parse_c,
    [PERL] = parse_shell,
    [PHP] = parse_c,
    [PKGCONFIG] = NULL,
    [PROTOBUF] = NULL,
    [PYTHON] = parse_shell,
    [RUBY] = parse_shell,
    [RUST] = parse_c,
    [SCHEME] = NULL,
    [SCSS] = NULL,
    [SED] = parse_shell,
    [SHELL] = parse_shell,
    [SQL] = NULL,
    [TCL] = parse_shell,
    [TEX] = NULL,
    [VALA] = parse_c,
    [VIML] = NULL,
    [XML] = NULL,
    [YACC] = NULL,
    [YAML] = parse_shell,
};

ParserFunc lookup_parser(Language language) {
    return parsers[language];
}

static inline FILE *xfopen(const char *path) {
    FILE *stream = fopen(path, "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    return stream;
}

static inline char first_nonspace_char(const char *str, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char c = str[i];
        switch (c) {
        case ' ': case '\t': case '\f': case '\v': case '\r':
            break;
        case '\n':
            return '\0';
        default:
            return c;
        }
    }
    return '\0';
}

LineCount parse_plain(const char *path, size_t size) {
    (void)size;
    FILE *stream = xfopen(path);
    u64 code = 0ULL, blank = 0ULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stream)) != -1) {
        if (first_nonspace_char(line, read) == '\0') {
            blank += 1;
        } else {
            code += 1;
        }
    }
    free(line);
    fclose(stream);
    return (LineCount){.code = code, .comment = 0ULL, .blank = blank};
}

LineCount parse_shell(const char *path, size_t size) {
    (void)size;
    FILE *stream = xfopen(path);
    u64 code = 0ULL, comment = 0ULL, blank = 0ULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stream)) != -1) {
        char c = first_nonspace_char(line, read);
        if (c == '\0') {
            blank += 1;
        } else if (c == '#') {
            comment += 1;
        } else {
            code += 1;
        }
    }
    free(line);
    fclose(stream);
    return (LineCount){.code = code, .comment = comment, .blank = blank};
}
