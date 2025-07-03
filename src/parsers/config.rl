#include "prelude.h"

// This is intended for parsing line-based/Unix-style configs, where
// comments are lines starting with '#'
%%{
    machine config;
    write data;
    include common "common.rl";

    comment = '#' @comment nonnewline*;

    line := |*
        spaces => ls;
        comment;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_config(const char *text, size_t size)
{
    init(text, size);
    %% write init;
    cs = config_en_line;
    %% write exec;
    process_last_line();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
