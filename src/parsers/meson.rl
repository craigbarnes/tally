#include "prelude.h"

%%{
    machine meson;
    write data;
    include common "common.rl";

    long_str =
        '\'\'\'' @code (
            newline %std_internal_newline
            | ws
            | (nonnewline - ws) @code
        )* :>> '\'\'\'' @code;

    sq_str =
        '\'' ([^'] | '\'' [^'] @{ fhold; }) @{ fhold; }
        ([^\r\n\f'\\] | '\\' nonnewline)* '\'';

    dq_str =
        '"' ([^"] | '"' [^"] @{ fhold; }) @{ fhold; }
        ([^\r\n\f"\\] | '\\' nonnewline)* '"';

    string = (long_str | sq_str | dq_str) @code;
    comment = '#' @comment nonnewline*;

    line := |*
        spaces => ls;
        comment;
        string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_meson(const char *text, size_t size)
{
    init(text, size);
    %% write init;
    cs = meson_en_line;
    %% write exec;
    process_last_line();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
