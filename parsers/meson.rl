#include "prelude.h"

%%{
    machine meson;
    write data;
    include common "common.rl";

    meson_long_str =
        '\'\'\'' @code (
            newline %std_internal_newline
            |
            ws
            |
            (nonnewline - ws) @code
        )* :>> '\'\'\'' @code;

    meson_sq_str =
        '\'' ([^'] | '\'' [^'] @{ fhold; }) @{ fhold; }
        ([^\r\n\f'\\] | '\\' nonnewline)* '\'';

    meson_dq_str =
        '"' ([^"] | '"' [^"] @{ fhold; }) @{ fhold; }
        ([^\r\n\f"\\] | '\\' nonnewline)* '"';

    meson_string = (meson_long_str | meson_sq_str | meson_dq_str) @code;
    meson_comment = '#' @comment nonnewline*;

    meson_line := |*
        spaces => ls;
        meson_comment;
        meson_string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_meson(const char *path, size_t size) {
    init(path, size);
    %% write init;
    cs = meson_en_meson_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
