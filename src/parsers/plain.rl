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

LineCount parse_plain(const char *path, size_t size)
{
    init(path, size);
    %% write init;
    cs = plain_en_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
