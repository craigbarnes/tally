#include "prelude.h"

%%{
    machine sql;
    write data;
    include common "common.rl";

    line_comment = '--' @comment nonnewline*;

    block_comment =
        '/*' @comment (
            newline %std_internal_newline
            | ws
            | (nonnewline - ws) @comment
        )* :>> '*/';

    comment = line_comment | block_comment;

    sq_str = '\'' @code ([^\r\n\f'\\] | '\\' nonnewline)* '\'';
    dq_str = '"' @code ([^\r\n\f"\\] | '\\' nonnewline)* '"';
    string = sq_str | dq_str;

    line := |*
        spaces => ls;
        comment;
        string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_sql(const char *path, size_t size)
{
    init(path, size);
    %% write init;
    cs = sql_en_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
