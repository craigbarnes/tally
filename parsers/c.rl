#include "macros.h"
#include "../languages.h"
#include "../parse.h"

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
            zcode
            break;
        case INTERNAL_NL:
            std_internal_newline
            break;
        case NEWLINE:
            std_newline
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
        '\'' @zcode (
            escaped_newline %{ entity = INTERNAL_NL; } %c_ccallback
            |
            ws
            |
            [^\t '\\] @zcode
            |
            '\\' nonnewline @zcode
        )* '\'';

    c_dq_str =
        '"' @zcode (
            escaped_newline %{ entity = INTERNAL_NL; } %c_ccallback
            |
            ws
            |
            [^\t "\\] @zcode
            |
            '\\' nonnewline @zcode
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

LineCount parse_c(char *buffer, size_t length) {
    init
    %% write init;
    cs = c_en_c_line;
    %% write exec;
    process_last_line;
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
