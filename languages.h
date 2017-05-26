#pragma once
#include "parse.h"

typedef enum {
    UNKNOWN = 0,
    IGNORED,
    ADA,
    APPLESCRIPT,
    ASSEMBLY,
    AWK,
    BATCHFILE,
    C,
    CHEADER,
    CLOJURE,
    CMAKE,
    COFFEESCRIPT,
    COMMONLISP,
    CPLUSPLUS,
    CPLUSPLUSHEADER,
    CSHARP,
    CSS,
    D,
    DART,
    EMACSLISP,
    GLSL,
    GO,
    GPERF,
    GROOVY,
    HASKELL,
    HTML,
    INI,
    JAVA,
    JAVASCRIPT,
    JSON,
    LEX,
    LUA,
    MAKE,
    MALLARD,
    MARKDOWN,
    MOONSCRIPT,
    OBJECTIVEC,
    PERL,
    PHP,
    PKGCONFIG,
    PROTOBUF,
    PYTHON,
    RACKET,
    RUBY,
    RUST,
    SCHEME,
    SCSS,
    SED,
    SHELL,
    SQL,
    TCL,
    TEX,
    TOML,
    VALA,
    VIML,
    XML,
    YACC,
    YAML,
    NUM_LANGUAGES
} Language;

typedef struct {
    const char *key;
    Language language;
} LanguageHashSlot;

const LanguageHashSlot *lookup_language_by_extension(const char*, size_t);
const LanguageHashSlot *lookup_language_by_filename(const char*, size_t);
const char *lookup_language_name(Language lang);
Parser lookup_language_parser(Language);
Language detect_language(const char *path, int base, int level, size_t size);
