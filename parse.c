#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "parse.h"

inline char *mmapfile(const char *path, size_t size) {
    assert(path);
    assert(size > 0);
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    char *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return addr;
}

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
