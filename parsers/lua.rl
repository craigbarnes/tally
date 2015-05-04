#include "prelude.h"

// TODO: handle long strings and long comments

%%{
    machine lua;
    write data;
    include common "common.rl";

    lua_sq_str = '\'' @code (
        newline %std_internal_newline
        |
        ws
        |
        [^\r\n\f\t '\\] @code
        |
        '\\' nonnewline @code
    )* '\'';

    lua_dq_str = '"' @code (
        newline %std_internal_newline
        |
        ws
        |
        [^\r\n\f\t "\\] @code
        |
        '\\' nonnewline @code
    )* '"';

    lua_string = lua_sq_str | lua_dq_str;
    lua_line_comment = '--' @comment nonnewline*;
    lua_comment = lua_line_comment;

    lua_line := |*
        spaces => ls;
        lua_comment;
        lua_string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_lua(const char *path, size_t size) {
    init(path, size);
    %% write init;
    cs = lua_en_lua_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
