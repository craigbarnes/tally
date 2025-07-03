#include "prelude.h"

%%{
    machine c;
    write data;
    include common "common.rl";

    line_comment =
        '//' @comment (
            escaped_newline %std_internal_newline
            | ws
            | (nonnewline - ws) @comment
        )*;

    block_comment =
        '/*' @comment (
            newline %std_internal_newline
            | ws
            | (nonnewline - ws) @comment
        )* :>> '*/';

    comment = line_comment | block_comment;

    sq_str =
        '\'' @code (
            escaped_newline %std_internal_newline
            | ws
            | [^\t '\\] @code
            | '\\' nonnewline @code
        )* '\'';

    dq_str =
        '"' @code (
            escaped_newline %std_internal_newline
            | ws
            | [^\t "\\] @code
            | '\\' nonnewline @code
        )* '"';

    string = sq_str | dq_str;

    line := |*
        spaces => ls;
        comment;
        string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_c(const char *text, size_t size)
{
    init(text, size);
    %% write init;
    cs = c_en_line;
    %% write exec;
    process_last_line();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
