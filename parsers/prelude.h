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
    u64 ncode = 0ULL; \
    u64 ncomment = 0ULL; \
    u64 nblank = 0ULL; \
    char *p = buffer_start; \
    char *pe = buffer_start + size; \
    char *eof = pe; \
    int cs; \
    int act; \
    char *ts; \
    char *te;

#define deinit() \
    munmap(buffer_start, buffer_size)

#define process_last_line() \
    if (line_contains_code) { \
        ncode++; \
    } else if (whole_line_comment) { \
        ncomment++; \
    }
