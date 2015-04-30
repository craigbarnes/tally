#pragma once
#include <stdbool.h>

#define process_last_line {\
    if (line_contains_code) { \
        ncode++; \
    } else if (whole_line_comment) { \
        ncomment++; \
    } \
}

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
char *line_start; // The beginning of the current line in the buffer
int entity; // State variable for the current entity being matched
bool whole_line_comment;
bool line_contains_code;

unsigned long long int ncode, ncomment, nblank;

#define init { \
    p = buffer; \
    pe = buffer + length; \
    eof = pe; \
    \
    buffer_start = buffer; \
    whole_line_comment = false; \
    line_contains_code = false; \
    line_start = 0; \
    entity = 0; \
    \
    ncode = 0ULL; \
    ncomment = 0ULL; \
    nblank = 0ULL; \
}
