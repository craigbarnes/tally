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
    [AWK] = NULL,
    [BATCHFILE] = NULL,
    [C] = parse_c,
    [CHEADER] = parse_c,
    [COFFEESCRIPT] = NULL,
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
    [INI] = NULL,
    [JAVA] = parse_c,
    [JAVASCRIPT] = parse_c,
    [JSON] = parse_plain,
    [LEX] = NULL,
    [LUA] = NULL,
    [MAKE] = NULL,
    [MALLARD] = NULL,
    [MARKDOWN] = parse_plain,
    [MOONSCRIPT] = NULL,
    [OBJECTIVEC] = parse_c,
    [PERL] = NULL,
    [PHP] = parse_c,
    [PKGCONFIG] = NULL,
    [PROTOBUF] = NULL,
    [PYTHON] = NULL,
    [RUBY] = NULL,
    [RUST] = parse_c,
    [SCHEME] = NULL,
    [SCSS] = NULL,
    [SED] = NULL,
    [SHELL] = NULL,
    [SQL] = NULL,
    [TCL] = NULL,
    [TEX] = NULL,
    [VALA] = parse_c,
    [VIML] = NULL,
    [XML] = NULL,
    [YACC] = NULL,
    [YAML] = NULL,
};

ParserFunc lookup_parser(Language language) {
    return parsers[language];
}

LineCount parse_plain(const char *path, size_t size) {
    (void)size;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    u64 code = 0ULL, blank = 0ULL;
    FILE *stream = fopen(path, "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, stream)) != -1) {
        for (ssize_t i = 0; i < read; i++) {
            switch (line[i]) {
            case ' ': case '\t': case '\f': case '\v': case '\r':
                break;
            case '\n':
                blank += 1;
                goto nextline;
            default:
                code += 1;
                goto nextline;
            }
        }
        nextline:;
    }
    free(line);
    fclose(stream);
    return (LineCount){.code = code, .comment = 0ULL, .blank = blank};
}
