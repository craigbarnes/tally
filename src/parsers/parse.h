#ifndef PARSERS_PARSE_H
#define PARSERS_PARSE_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t code;
    uint64_t comment;
    uint64_t blank;
} LineCount;

typedef LineCount (*Parser)(const char *text, size_t len);

LineCount parse_c(const char*, size_t);
LineCount parse_config(const char*, size_t);
LineCount parse_css(const char*, size_t);
LineCount parse_html(const char*, size_t);
LineCount parse_lisp(const char*, size_t);
LineCount parse_lua(const char*, size_t);
LineCount parse_meson(const char*, size_t);
LineCount parse_plain(const char*, size_t);
LineCount parse_python(const char*, size_t);
LineCount parse_shell(const char*, size_t);
LineCount parse_sql(const char*, size_t);
LineCount parse_xml(const char*, size_t);

#endif
