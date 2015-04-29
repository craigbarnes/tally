#pragma once

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
    RUBY,
    RUST,
    SCHEME,
    SCSS,
    SED,
    SHELL,
    SQL,
    TCL,
    TEX,
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
