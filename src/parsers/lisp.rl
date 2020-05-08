#include "prelude.h"

%%{
    machine lisp;
    write data;
    include common "common.rl";

    docstring = '"'{3} @comment (
        newline %std_internal_newline
        | ws
        | (nonnewline - ws) @comment
    )* :>> '"""' @comment;

    regular_string = '"' ([^"]{2}) @code (
        newline %std_internal_newline
        | ws
        | [^\r\n\f\t "\\] @code
        | '\\' nonnewline @code
    )* '"';

    line_comment = ';' @comment nonnewline*;
    comment = line_comment | docstring;
    empty_string = '"'{2} [^"] @{fhold;} @code;
    char_string = '"' [^"] '"' @code;
    string = empty_string | char_string | regular_string;

    line := |*
        spaces => ls;
        comment;
        string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_lisp(const char *path, size_t size)
{
    init(path, size);
    %% write init;
    cs = lisp_en_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
