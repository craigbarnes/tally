#ifndef LANGUAGES_H
#define LANGUAGES_H

#include "parsers/parse.h"

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
    COCCINELLE,
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
    DEVICETREE,
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
    RAGEL,
    RESTRUCTUREDTEXT,
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

const LanguageHashSlot *lookup_language_by_extension(const char*, size_t);
const LanguageHashSlot *lookup_language_by_filename(const char*, size_t);
Language detect_language(const char *path, int base, int level, size_t size);

#endif
