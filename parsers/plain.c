#define _XOPEN_SOURCE 700 // For getline(3)
#include <stdio.h>
#include <stdlib.h>
#include "../parse.h"

LineCount parse_plain(const char *path, size_t size) {
    (void)size;
    FILE *stream = xfopen(path);
    uint64_t code = 0, blank = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stream)) != -1) {
        if (first_nonspace_char(line, read) == '\0') {
            blank += 1;
        } else {
            code += 1;
        }
    }
    free(line);
    fclose(stream);
    return (LineCount){.code = code, .comment = 0, .blank = blank};
}
