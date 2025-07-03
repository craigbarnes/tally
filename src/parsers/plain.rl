#include "prelude.h"

%%{
    machine plain;
    write data;
    include common "common.rl";

    line := |*
        spaces => ls;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_plain(const char *text, size_t size)
{
    init(text, size);
    %% write init;
    cs = plain_en_line;
    %% write exec;
    process_last_line();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
