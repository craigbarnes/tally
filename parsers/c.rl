#ifndef OHCOUNT_C_PARSER_H
#define OHCOUNT_C_PARSER_H

#include "macros.h"
#include "../languages.h"
#include "../parse.h"

Language C_LANG = C;

enum {
    C_SPACE = 0,
    C_ANY
};

%%{
    machine c;
    write data;
    include common "common.rl";

    action c_ccallback {
        switch(entity) {
        case C_SPACE:
            ls
            break;
        case C_ANY:
            code
            break;
        case INTERNAL_NL:
            std_internal_newline(C_LANG)
            break;
        case NEWLINE:
            std_newline(C_LANG)
        }
    }

    c_line_comment =
        '//' @comment (
            escaped_newline %{ entity = INTERNAL_NL; } %c_ccallback
            |
            ws
            |
            (nonnewline - ws) @comment
        )*;

    c_block_comment =
        '/*' @comment (
            newline %{ entity = INTERNAL_NL; } %c_ccallback
            |
            ws
            |
            (nonnewline - ws) @comment
        )* :>> '*/';

    c_comment = c_line_comment | c_block_comment;

    c_sq_str =
        '\'' @code (
            escaped_newline %{ entity = INTERNAL_NL; } %c_ccallback
            |
            ws
            |
            [^\t '\\] @code
            |
            '\\' nonnewline @code
        )* '\'';

    c_dq_str =
        '"' @code (
            escaped_newline %{ entity = INTERNAL_NL; } %c_ccallback
            |
            ws
            |
            [^\t "\\] @code
            |
            '\\' nonnewline @code
        )* '"';

    c_string = c_sq_str | c_dq_str;

    c_line := |*
        spaces    ${ entity = C_SPACE; } => c_ccallback;
        c_comment;
        c_string;
        newline   ${ entity = NEWLINE; } => c_ccallback;
        ^space    ${ entity = C_ANY;   } => c_ccallback;
    *|;

}%%

void parse_c(char *buffer, size_t length, ParserCallback callback, void *userdata) {
    init
    %% write init;
    cs = c_en_c_line;
    %% write exec;
    // if no newline at EOF; callback contents of last line
    process_last_line(C_LANG);
}

void parse_cheader(char *buffer, size_t length, ParserCallback callback, void *ud) {
    C_LANG = CHEADER;
    parse_c(buffer, length, callback, ud);
    C_LANG = C;
}

void parse_cpp(char *buffer, size_t length, ParserCallback callback, void *ud) {
    C_LANG = CPLUSPLUS;
    parse_c(buffer, length, callback, ud);
    C_LANG = C;
}

void parse_cppheader(char *buffer, size_t length, ParserCallback callback, void *ud) {
    C_LANG = CPLUSPLUSHEADER;
    parse_c(buffer, length, callback, ud);
    C_LANG = C;
}

void parse_csharp(char *buffer, size_t length, ParserCallback callback, void *ud) {
    C_LANG = CSHARP;
    parse_c(buffer, length, callback, ud);
    C_LANG = C;
}

void parse_vala(char *buffer, size_t length, ParserCallback callback, void *ud) {
    C_LANG = VALA;
    parse_c(buffer, length, callback, ud);
    C_LANG = C;
}

#endif
