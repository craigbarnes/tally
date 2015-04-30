#include "macros.h"

%%{
    machine c;
    write data;
    include common "common.rl";

    c_line_comment =
        '//' @comment (
            escaped_newline %std_internal_newline
            |
            ws
            |
            (nonnewline - ws) @comment
        )*;

    c_block_comment =
        '/*' @comment (
            newline %std_internal_newline
            |
            ws
            |
            (nonnewline - ws) @comment
        )* :>> '*/';

    c_comment = c_line_comment | c_block_comment;

    c_sq_str =
        '\'' @code (
            escaped_newline %std_internal_newline
            |
            ws
            |
            [^\t '\\] @code
            |
            '\\' nonnewline @code
        )* '\'';

    c_dq_str =
        '"' @code (
            escaped_newline %std_internal_newline
            |
            ws
            |
            [^\t "\\] @code
            |
            '\\' nonnewline @code
        )* '"';

    c_string = c_sq_str | c_dq_str;

    c_line := |*
        spaces => ls;
        c_comment;
        c_string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_c(const char *path, size_t size) {
    init(path, size);
    %% write init;
    cs = c_en_c_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
