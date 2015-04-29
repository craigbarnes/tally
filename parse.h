#pragma once
#include <stdio.h>

typedef unsigned long long int u64;
typedef struct {u64 code, comment, blank;} LineCount;
typedef LineCount (*Parser)(const char*, size_t);

LineCount parse_c(const char*, size_t);
LineCount parse_plain(const char*, size_t);
LineCount parse_shell(const char*, size_t);

char *mmapfile(const char *path, size_t size);
FILE *xfopen(const char *path);
char first_nonspace_char(const char *str, size_t len);
