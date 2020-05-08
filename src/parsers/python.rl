#include "prelude.h"

%%{
    machine python;
    write data;
    include common "common.rl";

    sq_doc_str =
        '\'\'\'' @comment (
            newline %std_internal_newline
            | ws
            | (nonnewline - ws) @comment
        )* :>> '\'\'\'' @comment;

    dq_doc_str =
        '"""' @comment (
            newline %std_internal_newline
            | ws
            | (nonnewline - ws) @comment
        )* :>> '"""' @comment;

    line_comment = '#' @comment nonnewline*;
    doc_str = dq_doc_str | sq_doc_str;
    comment = line_comment | doc_str;

    sq_str =
        '\'' ([^'] | '\'' [^'] @{ fhold; }) @{ fhold; }
        ([^\r\n\f'\\] | '\\' nonnewline)* '\'';

    dq_str =
        '"' ([^"] | '"' [^"] @{ fhold; }) @{ fhold; }
        ([^\r\n\f"\\] | '\\' nonnewline)* '"';

    string = (sq_str | dq_str) @code;

    line := |*
        spaces => ls;
        comment;
        string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_python(const char *path, size_t size)
{
    init(path, size);
    %% write init;
    cs = python_en_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
