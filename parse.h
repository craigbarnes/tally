#include <stddef.h>
#include "languages.h"

typedef unsigned long long int u64;
typedef struct {u64 code, comment, blank;} LineCount;
typedef LineCount (*ParserFunc)(char*, size_t);

ParserFunc lookup_parser(Language);
LineCount parse_c(char*, size_t);
