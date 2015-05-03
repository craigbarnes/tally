#include "prelude.h"

%%{
    machine python;
    write data;
    include common "common.rl";

    python_sq_doc_str =
        '\'\'\'' @comment (
            newline %std_internal_newline
            |
            ws
            |
            (nonnewline - ws) @comment
        )* :>> '\'\'\'' @comment;

    python_dq_doc_str =
        '"""' @comment (
            newline %std_internal_newline
            |
            ws
            |
            (nonnewline - ws) @comment
        )* :>> '"""' @comment;

    python_line_comment = '#' @comment nonnewline*;
    python_doc_str = python_dq_doc_str | python_sq_doc_str;
    python_comment = python_line_comment | python_doc_str;

    python_sq_str =
        '\'' ([^'] | '\'' [^'] @{ fhold; }) @{ fhold; }
        ([^\r\n\f'\\] | '\\' nonnewline)* '\'';

    python_dq_str =
        '"' ([^"] | '"' [^"] @{ fhold; }) @{ fhold; }
        ([^\r\n\f"\\] | '\\' nonnewline)* '"';

    python_string = (python_sq_str | python_dq_str) @code;

    python_line := |*
        spaces => ls;
        python_comment;
        python_string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_python(const char *path, size_t size) {
    init(path, size);
    %% write init;
    cs = python_en_python_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
