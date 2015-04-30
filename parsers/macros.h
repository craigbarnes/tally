#pragma once
#include <stdbool.h>
#include <sys/mman.h>
#include "../parse.h"

int cs;
int act;
char *p;
char *pe;
char *eof;
char *ts;
char *te;
int stack[5];
int top;

char *buffer_start; // The buffer currently being parsed
size_t buffer_size; // The size of the current buffer
char *line_start; // The beginning of the current line in the buffer
bool whole_line_comment;
bool line_contains_code;

u64 ncode;
u64 ncomment;
u64 nblank;

static inline void init(const char *path, size_t size) {
    buffer_size = size;
    buffer_start = mmapfile(path, size);
    p = buffer_start;
    pe = buffer_start + size;
    eof = pe;
    line_start = NULL;
    whole_line_comment = false;
    line_contains_code = false;
    ncode = 0ULL;
    ncomment = 0ULL;
    nblank = 0ULL;
}

static inline void deinit() {
    munmap(buffer_start, buffer_size);
}

static inline void process_last_line() {
    if (line_contains_code) {
        ncode++;
    } else if (whole_line_comment) {
        ncomment++;
    }
}
