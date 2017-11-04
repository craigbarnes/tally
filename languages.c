#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "languages.h"

static const Parser parsers[NUM_LANGUAGES] = {
    [ADA] = NULL,
    [APPLESCRIPT] = NULL,
    [ASSEMBLY] = NULL,
    [AWK] = parse_shell,
    [BATCHFILE] = NULL,
    [C] = parse_c,
    [CHEADER] = parse_c,
    [CLOJURE] = parse_lisp,
    [CMAKE] = parse_shell,
    [COFFEESCRIPT] = parse_shell,
    [COMMONLISP] = parse_lisp,
    [CPLUSPLUS] = parse_c,
    [CPLUSPLUSHEADER] = parse_c,
    [CSHARP] = parse_c,
    [CSS] = parse_css,
    [D] = parse_c,
    [DART] = parse_c,
    [EMACSLISP] = parse_lisp,
    [GLSL] = parse_c,
    [GO] = parse_c,
    [GPERF] = NULL,
    [GROOVY] = parse_c,
    [HASKELL] = NULL,
    [HTML] = parse_html,
    [INI] = parse_shell,
    [JAVA] = parse_c,
    [JAVASCRIPT] = parse_c,
    [JSON] = parse_plain,
    [LEX] = NULL,
    [LUA] = parse_lua,
    [MAKE] = parse_shell,
    [MALLARD] = parse_xml,
    [MARKDOWN] = parse_plain,
    [MESON] = parse_meson,
    [MOONSCRIPT] = NULL,
    [OBJECTIVEC] = parse_c,
    [PERL] = parse_shell,
    [PHP] = parse_c,
    [PKGCONFIG] = parse_shell,
    [PROTOBUF] = NULL,
    [PYTHON] = parse_python,
    [RACKET] = parse_lisp,
    [RUBY] = parse_shell,
    [RUST] = parse_c,
    [SCHEME] = parse_lisp,
    [SCSS] = parse_c,
    [SED] = parse_shell,
    [SHELL] = parse_shell,
    [SQL] = parse_sql,
    [TCL] = parse_shell,
    [TEX] = NULL,
    [TOML] = parse_shell,
    [VALA] = parse_c,
    [VIML] = NULL,
    [XML] = parse_xml,
    [YACC] = NULL,
    [YAML] = parse_shell,
};

static const char *const language_names[NUM_LANGUAGES] = {
    [UNKNOWN] = "?",
    [IGNORED] = "!",
    [ADA] = "Ada",
    [APPLESCRIPT] = "AppleScript",
    [ASSEMBLY] = "Assembly",
    [AWK] = "AWK",
    [BATCHFILE] = "Batchfile",
    [C] = "C",
    [CLOJURE] = "Clojure",
    [CHEADER] = "C Header",
    [CMAKE] = "CMake",
    [COFFEESCRIPT] = "CoffeeScript",
    [COMMONLISP] = "Common Lisp",
    [CPLUSPLUS] = "C++",
    [CPLUSPLUSHEADER] = "C++ Header",
    [CSHARP] = "C#",
    [CSS] = "CSS",
    [D] = "D",
    [DART] = "Dart",
    [EMACSLISP] = "Emacs Lisp",
    [GLSL] = "GLSL",
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
    [MESON] = "Meson",
    [MOONSCRIPT] = "MoonScript",
    [OBJECTIVEC] = "Objective-C",
    [PERL] = "Perl",
    [PHP] = "PHP",
    [PKGCONFIG] = "pkg-config",
    [PROTOBUF] = "Protocol Buffer",
    [PYTHON] = "Python",
    [RACKET] = "Racket",
    [RUBY] = "Ruby",
    [RUST] = "Rust",
    [SCHEME] = "Scheme",
    [SCSS] = "SCSS",
    [SED] = "SED",
    [SHELL] = "Shell",
    [SQL] = "SQL",
    [TCL] = "TCL",
    [TEX] = "TeX",
    [TOML] = "TOML",
    [VALA] = "Vala",
    [VIML] = "VimL",
    [XML] = "XML",
    [YACC] = "YACC",
    [YAML] = "YAML"
};

Parser lookup_language_parser(Language language) {
    Parser parser = parsers[language];
    return parser ? parser : parse_plain;
}

const char *lookup_language_name(Language lang) {
    const char *const name = language_names[lang];
    assert(name);
    return name;
}

static inline const char *file_extension(const char *const filename) {
    const char *const dot = strrchr(filename, '.');
    if(!dot || dot == filename) {
        return NULL;
    }
    return dot + 1;
}

Language detect_language(const char *path, int base, int level, size_t size) {
    if (size == 0 || access(path, R_OK) != 0) {
        return IGNORED;
    }

    const char *const basename = path + base;

    if (basename[0] == '.' && level > 0) {
        return IGNORED;
    }

    const char *const ext = file_extension(basename);
    const LanguageHashSlot *slot = NULL;

    if (ext && (slot = lookup_language_by_extension(ext, strlen(ext)))) {
        return slot->language;
    }

    if ((slot = lookup_language_by_filename(basename, strlen(basename)))) {
        return slot->language;
    }

    return UNKNOWN;
}
