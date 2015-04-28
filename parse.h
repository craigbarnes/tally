#pragma once
#include <stddef.h>
#include "languages.h"

typedef unsigned long long int u64;
typedef struct {u64 code, comment, blank;} LineCount;
typedef LineCount (*ParserFunc)(const char*, size_t);

ParserFunc lookup_parser(Language);
LineCount parse_c(const char*, size_t);
LineCount parse_plain(const char*, size_t);