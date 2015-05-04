#include "prelude.h"

%%{
    machine html;
    write data;
    include common "common.rl";

    html_comment = '<!--' @comment (
        newline %std_internal_newline
        |
        ws
        |
        (nonnewline - ws) @comment
    )* :>> '-->';

    html_sq_str = '\'' ([^\r\n\f'\\] | '\\' nonnewline)* '\'' @code;
    html_dq_str = '"' ([^\r\n\f"\\] | '\\' nonnewline)* '"' @code;
    html_string = html_sq_str | html_dq_str;

    html_line := |*
        spaces => ls;
        html_comment;
        html_string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_html(const char *path, size_t size) {
    init(path, size);
    %% write init;
    cs = html_en_html_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
