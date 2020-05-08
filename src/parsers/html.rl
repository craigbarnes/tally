#include "prelude.h"

%%{
    machine html;
    write data;
    include common "common.rl";

    comment = '<!--' @comment (
        newline %std_internal_newline
        | ws
        | (nonnewline - ws) @comment
    )* :>> '-->';

    sq_str = '\'' ([^\r\n\f'\\] | '\\' nonnewline)* '\'' @code;
    dq_str = '"' ([^\r\n\f"\\] | '\\' nonnewline)* '"' @code;
    string = sq_str | dq_str;

    line := |*
        spaces => ls;
        comment;
        string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_html(const char *path, size_t size) {
    init(path, size);
    %% write init;
    cs = html_en_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
