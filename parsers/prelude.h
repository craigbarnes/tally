#pragma once
#include <stdbool.h>
#include <sys/mman.h>
#include "../parse.h"

#define init(path, size) \
    char *buffer_start = mmapfile(path, size); \
    size_t buffer_size = size; \
    char *line_start = NULL; \
    bool whole_line_comment = false; \
    bool line_contains_code = false; \
    uint64_t ncode = 0; \
    uint64_t ncomment = 0; \
    uint64_t nblank = 0; \
    char *p = buffer_start; \
    char *pe = buffer_start + size; \
    char *eof = pe; \
    int cs; \
    int act; \
    char *ts; \
    char *te; \
    (void)act; // Silence unused-but-set-variable warnings

#define deinit() \
    munmap(buffer_start, buffer_size)

#define process_last_line() \
    if (line_contains_code) { \
        ncode++; \
    } else if (whole_line_comment) { \
        ncomment++; \
    }
