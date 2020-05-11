#pragma once
#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "parse.h"
#include "../macros.h"

static inline char *mmapfile(const char *path, size_t size)
{
    assert(path);
    assert(size > 0);
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    char *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return addr;
}

#define init(path, size) \
    char *buffer_start = mmapfile(path, size); \
    size_t buffer_size = size; \
    char *line_start = NULL; \
    bool whole_line_comment = false; \
    bool line_contains_code = false; \
    uint64_t ncode = 0; \
    uint64_t ncomment = 0; \
    uint64_t nblank = 0; \
    UNUSED uint64_t ncode_saved = 0; \
    UNUSED uint64_t nblank_saved = 0; \
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

DISABLE_WARNING("-Wimplicit-fallthrough")
DISABLE_WARNING("-Wunused-const-variable")
