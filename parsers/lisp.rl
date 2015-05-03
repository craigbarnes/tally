#include "prelude.h"

%%{
    machine lisp;
    write data;
    include common "common.rl";

    lisp_docstring = '"'{3} @comment (
        newline %std_internal_newline
        |
        ws
        |
        (nonnewline - ws) @comment
    )* :>> '"""' @comment;

    lisp_regular_string = '"' ([^"]{2}) @code (
        newline %std_internal_newline
        |
        ws
        |
        [^\r\n\f\t "\\] @code
        |
        '\\' nonnewline @code
    )* '"';

    lisp_line_comment = ';' @comment nonnewline*;
    lisp_comment = lisp_line_comment | lisp_docstring;
    lisp_empty_string = '"'{2} [^"] @{fhold;} @code;
    lisp_char_string = '"' [^"] '"' @code;
    lisp_string = lisp_empty_string | lisp_char_string | lisp_regular_string;

    lisp_line := |*
        spaces => ls;
        lisp_comment;
        lisp_string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_lisp(const char *path, size_t size) {
    init(path, size);
    %% write init;
    cs = lisp_en_lisp_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
