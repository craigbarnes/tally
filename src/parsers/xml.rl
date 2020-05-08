#include "prelude.h"

%%{
    machine xml;
    write data;
    include common "common.rl";

    comment =
        '<!--' @comment (
            newline %std_internal_newline
            | ws
            | (nonnewline - ws) @comment
        )* :>> '-->';

    cdata =
        '<![CDATA[' @code (
            newline %std_internal_newline
            | ws
            | (nonnewline - ws) @code
        )* :>> ']]>';

    sq_str = '\'' [^\r\n\f']* '\'' @code;
    dq_str = '"' [^\r\n\f"]* '"' @code;
    string = sq_str | dq_str | cdata;

    line := |*
        spaces => ls;
        comment;
        string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_xml(const char *path, size_t size)
{
    init(path, size);
    %% write init;
    cs = xml_en_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
