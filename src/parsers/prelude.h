#ifndef PARSERS_PRELUDE_H
#define PARSERS_PRELUDE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "parse.h"
#include "../macros.h"

#define init(buffer_start, size) \
    const char *line_start = NULL; \
    bool whole_line_comment = false; \
    bool line_contains_code = false; \
    uint64_t ncode = 0; \
    uint64_t ncomment = 0; \
    uint64_t nblank = 0; \
    UNUSED uint64_t ncode_saved = 0; \
    UNUSED uint64_t nblank_saved = 0; \
    const char *p = buffer_start; \
    const char *pe = buffer_start + size; \
    const char *eof = pe; \
    int cs; \
    int act; \
    const char *ts; \
    const char *te; \
    (void)act; // Silence unused-but-set-variable warnings

#define process_last_line() \
    if (line_contains_code) { \
        ncode++; \
    } else if (whole_line_comment) { \
        ncomment++; \
    }

DISABLE_WARNING("-Wimplicit-fallthrough")
DISABLE_WARNING("-Wunused-const-variable")

#endif
