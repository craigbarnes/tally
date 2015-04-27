#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "languages.h"

static const char *const language_names[NUM_LANGUAGES] = {
    [UNKNOWN] = "?",
    [IGNORED] = "!",
    [ADA] = "Ada",
    [APPLESCRIPT] = "AppleScript",
    [ASSEMBLY] = "Assembly",
    [AWK] = "AWK",
    [BATCHFILE] = "Batchfile",
    [C] = "C",
    [CHEADER] = "C Header",
    [COFFEESCRIPT] = "CoffeeScript",
    [COMMONLISP] = "Common Lisp",
    [CPLUSPLUS] = "C++",
    [CPLUSPLUSHEADER] = "C++ Header",
    [CSHARP] = "C#",
    [CSS] = "CSS",
    [D] = "D",
    [DART] = "Dart",
    [EMACSLISP] = "Emacs Lisp",
    [GO] = "Go",
    [GPERF] = "gperf",
    [GROOVY] = "Groovy",
    [HASKELL] = "Haskell",
    [HTML] = "HTML",
    [INI] = "INI",
    [JAVA] = "Java",
    [JAVASCRIPT] = "JavaScript",
    [JSON] = "JSON",
    [LEX] = "Lex",
    [LUA] = "Lua",
    [MAKE] = "Make",
    [MALLARD] = "Mallard",
    [MARKDOWN] = "Markdown",
    [MOONSCRIPT] = "MoonScript",
    [OBJECTIVEC] = "Objective-C",
    [PERL] = "Perl",
    [PHP] = "PHP",
    [PKGCONFIG] = "pkg-config",
    [PROTOBUF] = "Protocol Buffer",
    [PYTHON] = "Python",
    [RUBY] = "Ruby",
    [RUST] = "Rust",
    [SCHEME] = "Scheme",
    [SCSS] = "SCSS",
    [SED] = "SED",
    [SHELL] = "Shell",
    [SQL] = "SQL",
    [TCL] = "TCL",
    [TEX] = "TeX",
    [VALA] = "Vala",
    [VIML] = "VimL",
    [XML] = "XML",
    [YACC] = "YACC",
    [YAML] = "YAML"
};

static inline const char *lookup_language_name(Language lang) {
    assert(lang >= 0);
    assert(lang < NUM_LANGUAGES);
    const char *name = language_names[lang];
    if (!name) {
        fprintf(stderr, "Error: language_names[%d] is missing\n", lang);
        abort();
    }
    return name;
}
