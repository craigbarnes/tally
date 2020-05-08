#define _XOPEN_SOURCE 700 // For getline(3)
#include <stdio.h>
#include <stdlib.h>
#include "../parse.h"

LineCount parse_shell(const char *path, size_t size)
{
    (void)size;
    FILE *stream = xfopen(path);
    uint64_t code = 0, comment = 0, blank = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stream)) != -1) {
        char c = first_nonspace_char(line, read);
        if (c == '\0') {
            blank += 1;
        } else if (c == '#') {
            comment += 1;
        } else {
            code += 1;
        }
    }
    free(line);
    fclose(stream);
    return (LineCount){.code = code, .comment = comment, .blank = blank};
}
