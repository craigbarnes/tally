#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "languages.h"

typedef struct {
    const char *name;
    const Parser parser;
} LanguageInfo;

static const LanguageInfo languages[NUM_LANGUAGES] = {
    [UNKNOWN] = {"?", NULL},
    [IGNORED] = {"!", NULL},
    [ADA] = {"Ada", parse_plain}, // TODO: comment-aware parser
    [APPLESCRIPT] = {"AppleScript", parse_plain}, // TODO: comment-aware parser
    [ASSEMBLY] = {"Assembly", parse_plain}, // TODO: comment-aware parser
    [AWK] = {"AWK", parse_shell},
    [BATCHFILE] = {"Batchfile", parse_plain},
    [C] = {"C", parse_c},
    [CHEADER] = {"C Header", parse_c},
    [CLOJURE] = {"Clojure", parse_lisp},
    [CMAKE] = {"CMake", parse_shell},
    [COFFEESCRIPT] = {"CoffeScript", parse_shell},
    [COMMONLISP] = {"Common Lisp", parse_lisp},
    [CPLUSPLUS] = {"C++", parse_c},
    [CPLUSPLUSHEADER] = {"C++ Header", parse_c},
    [CSHARP] = {"C#", parse_c},
    [CSS] = {"CSS", parse_css},
    [D] = {"D", parse_c},
    [DART] = {"Dart", parse_c},
    [DOCKER] = {"Dockerfile", parse_shell},
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
    [MAKE] = {"Make", parse_shell},
    [MALLARD] = {"Mallard", parse_xml},
    [MARKDOWN] = {"Markdown", parse_plain},
    [MESON] = {"Meson", parse_meson},
    [MOONSCRIPT] = {"MoonScript", parse_plain}, // TODO: comment-aware parser
    [OBJECTIVEC] = {"Objective-C", parse_c},
    [PERL] = {"Perl", parse_shell},
    [PHP] = {"PHP", parse_c},
    [PKGCONFIG] = {"pkg-config", parse_shell},
    [PROTOBUF] = {"Protobuf", parse_c},
    [PYTHON] = {"Python", parse_python},
    [RACKET] = {"Racket", parse_lisp},
    [RUBY] = {"Ruby", parse_shell},
    [RUST] = {"Rust", parse_c},
    [SCHEME] = {"Scheme", parse_lisp},
    [SCSS] = {"SCSS", parse_c},
    [SED] = {"SED", parse_shell},
    [SHELL] = {"Shell", parse_shell},
    [SQL] = {"SQL", parse_sql},
    [TCL] = {"TCL", parse_shell},
    [TEX] = {"TeX", parse_plain}, // TODO: comment-aware parser
    [TOML] = {"TOML", parse_shell},
    [VALA] = {"Vala", parse_c},
    [VIML] = {"VimL", parse_plain},
    [XML] = {"XML", parse_xml},
    [YACC] = {"YACC", parse_c},
    [YAML] = {"YAML", parse_shell},
};

Parser lookup_language_parser(Language id) {
    Parser parser = languages[id].parser;
    return parser ? parser : parse_plain;
}

const char *lookup_language_name(Language id) {
    const char *const name = languages[id].name;
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
