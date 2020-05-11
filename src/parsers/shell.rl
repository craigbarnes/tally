#include "prelude.h"

%%{
    machine shell;
    write data;
    include common "common.rl";

    comment = '#' @comment nonnewline*;

    sq_str = '\'' @block (
        newline %std_internal_newline
        | '\\' newline %std_internal_newline
        | ws
        | [^\r\n\f\t '\\] @code
        | '\\' nonnewline @code
    )* $/block_eof '\'' @block_end;

    dq_str = '"' @block (
        newline %std_internal_newline
        | '\\' newline %std_internal_newline
        | ws
        | [^\r\n\f\t "\\] @code
        | '\\' nonnewline @code
    )* $/block_eof '"' @block_end;

    # TODO: heredocs
    string = sq_str | dq_str;

    line := |*
        spaces => ls;
        comment;
        string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_shell(const char *path, size_t size)
{
    init(path, size);
    %% write init;
    cs = shell_en_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
