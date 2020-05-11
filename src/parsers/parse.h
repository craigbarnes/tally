#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct {uint64_t code, comment, blank;} LineCount;
typedef LineCount (*Parser)(const char*, size_t);

LineCount parse_c(const char*, size_t);
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
