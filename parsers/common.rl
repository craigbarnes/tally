%%{
machine common;

ws = [\t ];
spaces = [\t ]+;
newline = ('\r\n' | '\n' | '\f' | '\r' when { p+1 < pe && *(p+1) != '\n' });
escaped_newline = '\\' newline;
nonnewline = any - [\r\n\f];
nonprintable_char = cntrl - [\r\n\f];

sq_str = '\'' [^']* '\'';
dq_str = '"' [^"]* '"';
sq_str_with_escapes = '\'' ([^'\\] | '\\' any)* '\'';
dq_str_with_escapes = '"' ([^"\\] | '\\' any)* '"';

dec_num = digit+;
hex_num = 0 [xX] [a-fA-F0-9]+;
oct_num = 0 [0-7]+;
integer = [+\-]? (hex_num | oct_num | dec_num);
float = [+\-]? ((digit* '.' digit+) | (digit+ '.' digit*) | digit+)
        [eE] [+\-]? digit+;

action ls {
    if (!line_start) {
        line_start = ts;
    }
}

action zcode {
    if (!line_contains_code && !line_start) {
        line_start = ts;
    }
    line_contains_code = true;
}

action comment {
    if (!line_contains_code) {
        whole_line_comment = true;
        if (!line_start) {
            line_start = ts;
        }
    }
}

}%%