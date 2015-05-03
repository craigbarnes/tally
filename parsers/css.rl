#include "prelude.h"

%%{
    machine css;
    write data;
    include common "common.rl";

    css_comment =
        '/*' @comment (
            newline %std_internal_newline
            |
            ws
            |
            (nonnewline - ws) @comment
        )* :>> '*/';

    css_sq_str =
        '\'' @code (
            newline %std_internal_newline
            |
            ws
            |
            [^\r\n\f\t '\\] @code
            |
            '\\' nonnewline @code
        )* '\'';

    css_dq_str =
        '"' @code (
            newline %std_internal_newline
            |
            ws
            |
            [^\r\n\f\t "\\] @code
            |
            '\\' nonnewline @code
        )* '"';

    css_string = css_sq_str | css_dq_str;

    css_line := |*
        spaces => ls;
        css_comment;
        css_string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_css(const char *path, size_t size) {
    init(path, size);
    %% write init;
    cs = css_en_css_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
