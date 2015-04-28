#include <stddef.h>
#include "languages.h"

typedef enum {CODE, COMMENT, BLANK} LineType;
typedef void (*ParserCallback)(Language, LineType, void*);
typedef void (*ParserFunc)(char*, size_t, ParserCallback, void*);

ParserFunc lookup_parser(Language);
void parse_c(char*, size_t, ParserCallback, void*);
void parse_cheader(char*, size_t, ParserCallback, void*);
void parse_cpp(char*, size_t, ParserCallback, void*);
void parse_cppheader(char*, size_t, ParserCallback, void*);
void parse_csharp(char*, size_t, ParserCallback, void*);
void parse_vala(char*, size_t, ParserCallback, void*);
