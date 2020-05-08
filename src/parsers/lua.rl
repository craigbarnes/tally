#include "prelude.h"

// TODO: handle long strings and long comments

%%{
    machine lua;
    write data;
    include common "common.rl";

    sq_str = '\'' @code (
        newline %std_internal_newline
        | ws
        | [^\r\n\f\t '\\] @code
        | '\\' nonnewline @code
    )* '\'';

    dq_str = '"' @code (
        newline %std_internal_newline
        | ws
        | [^\r\n\f\t "\\] @code
        | '\\' nonnewline @code
    )* '"';

    string = sq_str | dq_str;
    line_comment = '--' @comment nonnewline*;
    comment = line_comment;

    line := |*
        spaces => ls;
        comment;
        string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_lua(const char *path, size_t size)
{
    init(path, size);
    %% write init;
    cs = lua_en_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
