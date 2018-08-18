#pragma once
#include "parse.h"

#ifndef LENGTH_TYPE
#define LENGTH_TYPE size_t
#endif

typedef enum {
    UNKNOWN = 0,
    IGNORED,
    ADA,
    APPLESCRIPT,
    ASSEMBLY,
    AWK,
    BATCHFILE,
    BIBTEX,
    C,
    CHEADER,
    CLOJURE,
    CMAKE,
    COFFEESCRIPT,
    COMMONLISP,
    CPLUSPLUS,
    CPLUSPLUSHEADER,
    CRYSTAL,
    CSHARP,
    CSS,
    CSV,
    D,
    DART,
    DOCKER,
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
    M4,
    MAKE,
    MALLARD,
    MARKDOWN,
    MESON,
    MOONSCRIPT,
    NGINX,
    OBJECTIVEC,
    PERL,
    PHP,
    PKGCONFIG,
    PROTOBUF,
    PYTHON,
    RACKET,
    RESTRUCTUREDTEXT,
    ROFF,
    RUBY,
    RUST,
    SCALA,
    SCHEME,
    SCSS,
    SED,
    SHELL,
    SQL,
    TCL,
    TEX,
    TEXINFO,
    TOML,
    TYPESCRIPT,
    VALA,
    VERILOG,
    VHDL,
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

const LanguageHashSlot *lookup_language_by_extension(const char*, LENGTH_TYPE);
const LanguageHashSlot *lookup_language_by_filename(const char*, LENGTH_TYPE);
Language detect_language(const char *path, int base, int level, size_t size);
