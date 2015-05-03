#include "prelude.h"

%%{
    machine xml;
    write data;
    include common "common.rl";

    xml_comment =
        '<!--' @comment (
            newline %std_internal_newline
            |
            ws
            |
            (nonnewline - ws) @comment
        )* :>> '-->';

    xml_cdata_str =
        '<![CDATA[' @code (
            newline %std_internal_newline
            |
            ws
            |
            (nonnewline - ws) @code
        )* :>> ']]>';

    xml_sq_str = '\'' [^\r\n\f']* '\'' @code;
    xml_dq_str = '"' [^\r\n\f"]* '"' @code;
    xml_string = xml_sq_str | xml_dq_str | xml_cdata_str;

    xml_line := |*
        spaces => ls;
        xml_comment;
        xml_string;
        newline => std_newline;
        ^space => code;
    *|;

}%%

LineCount parse_xml(const char *path, size_t size) {
    init(path, size);
    %% write init;
    cs = xml_en_xml_line;
    %% write exec;
    process_last_line();
    deinit();
    return (LineCount){.code = ncode, .comment = ncomment, .blank = nblank};
}
