#include <stdlib.h>
#include "parse.h"

inline FILE *xfopen(const char *path) {
    FILE *stream = fopen(path, "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    return stream;
}

inline char first_nonspace_char(const char *str, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char c = str[i];
        switch (c) {
        case ' ': case '\t': case '\f': case '\v': case '\r':
            break;
        case '\n':
            return '\0';
        default:
            return c;
        }
    }
    return '\0';
}
