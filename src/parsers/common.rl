%%{
machine common;

ws = [\t ];
spaces = [\t ]+;
newline = ('\r\n' | '\n' | '\f' | '\r' when { p+1 < pe && *(p+1) != '\n' });
escaped_newline = '\\' newline;
nonnewline = any - [\r\n\f];

action ls {
    if (!line_start) {
        line_start = ts;
    }
}

action code {
    if (!line_contains_code && !line_start) {
        line_start = ts;
    }
    line_contains_code = true;
}

action block {
    ncode_saved = ncode;
    nblank_saved = nblank;
    line_contains_code = true;
}

action block_end {
    line_contains_code = true;
}

action block_eof {
    ncode = ncode_saved + 1;
    nblank = nblank_saved;
}

action comment {
    if (!line_contains_code) {
        whole_line_comment = true;
        if (!line_start) {
            line_start = ts;
        }
    }
}

action std_internal_newline {
    if (p > line_start) {
        if (line_contains_code) {
            ncode++;
        } else if (whole_line_comment) {
            ncomment++;
        } else {
            nblank++;
        }
    }
    whole_line_comment = false;
    line_contains_code = false;
    line_start = p;
}

action std_newline {
    if (te > line_start) {
        if (line_contains_code) {
            ncode++;
        } else if (whole_line_comment) {
            ncomment++;
        } else {
            nblank++;
        }
    }
    whole_line_comment = false;
    line_contains_code = false;
    line_start = 0;
}

}%%
