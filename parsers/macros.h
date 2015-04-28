#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NEWLINE -1
#define INTERNAL_NL -2

#define ls { \
    if (!line_start) \
        line_start = ts; \
}

#define code { \
    if (!line_contains_code && !line_start) \
        line_start = ts; \
    line_contains_code = true; \
}

#define std_internal_newline(lang) { \
    if (callback && p > line_start) { \
        if (line_contains_code) { \
            callback(lang, CODE, userdata); \
        } else if (whole_line_comment) { \
            callback(lang, COMMENT, userdata); \
        } else { \
            callback(lang, BLANK, userdata); \
        } \
    } \
    whole_line_comment = false; \
    line_contains_code = false; \
    line_start = p; \
}

#define std_newline(lang) { \
    if (callback && te > line_start) { \
        if (line_contains_code) \
            callback(lang, CODE, userdata); \
        else if (whole_line_comment) \
            callback(lang, COMMENT, userdata); \
        else \
            callback(lang, BLANK, userdata); \
    } \
    whole_line_comment = false; \
    line_contains_code = false; \
    line_start = 0; \
}

#define process_last_line(lang) {\
    if ((whole_line_comment || line_contains_code) && callback) { \
        if (line_contains_code) \
            callback(lang, CODE, userdata); \
        else if (whole_line_comment) \
            callback(lang, COMMENT, userdata); \
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
}
