#include "prelude.h"

%%{
    machine sql;
    write data;
    include common "common.rl";

    sql_line_comment = '--' @comment nonnewline*;

    sql_block_comment =
        '/*' @comment (
            newline %std_internal_newline
            |
            ws
            |
            (nonnewline - ws) @comment
        )* :>> '*/';

    sql_comment = sql_line_comment | sql_block_comment;

    sql_sq_str = '\'' @code ([^\r\n\f'\\] | '\\' nonnewline)* '\'';
    sql_dq_str = '"' @code ([^\r\n\f"\\] | '\\' nonnewline)* '"';
    sql_string = sql_sq_str | sql_dq_str;

    sql_line := |*
        spaces => ls;
        sql_comment;
        sql_string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_sql(const char *path, size_t size) {
    init(path, size);
    %% write init;
    cs = sql_en_sql_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
