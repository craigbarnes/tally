#define _XOPEN_SOURCE 500 // For nftw(3)
#define _GNU_SOURCE // For FTW_ACTIONRETVAL
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include "languages.h"
#include "names.h"

const LanguageHashSlot *lookup_language_by_extension(const char*, unsigned int);
const LanguageHashSlot *lookup_language_by_filename(const char*, unsigned int);

static inline const char *file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) {
        return NULL;
    }
    return dot + 1;
}

static Language detect_language(const char *filename, int base) {
    const char *const basename = filename + base;
    const char *const ext = file_extension(basename);
    const LanguageHashSlot *slot = NULL;

    if (ext && (slot = lookup_language_by_extension(ext, strlen(ext)))) {
        return slot->language;
    }

    if ((slot = lookup_language_by_filename(basename, strlen(basename)))) {
        return slot->language;
    }

    return UNKNOWN;
}

static int detect(const char *f, const struct stat *s, int t, struct FTW *w) {
    (void)s; // Unused
    const char *basename = f + w->base;
    const bool ignored = (basename[0] == '.' && w->level > 0);

    if (t == FTW_F && !ignored) {
        const char *path = (f[0] == '.' && f[1] == '/') ? f + 2 : f;
        Language lang = detect_language(f, w->base);
        printf("%-20s  %s\n", path, lookup_language_name(lang));
    }

    if (t == FTW_D && ignored) {
        return FTW_SKIP_SUBTREE;
    }

    return FTW_CONTINUE;
}

// TODO: Implement line counting, summarized by language
static int summary(const char *f, const struct stat *s, int t, struct FTW *w) {
    (void)s;
    const char *basename = f + w->base;
    const bool ignored = (basename[0] == '.' && w->level > 0);

    if (t == FTW_F && !ignored) {
        puts(f);
    }

    if (t == FTW_D && ignored) {
        return FTW_SKIP_SUBTREE;
    }

    return FTW_CONTINUE;
}

int main(int argc, char *argv[]) {
    int (*cb)(const char*, const struct stat*, int, struct FTW*) = detect;

    for (int opt; (opt = getopt(argc, argv, "ds")) != -1; ) {
        switch (opt) {
        case 'd':
            cb = detect;
            break;
        case 's':
            cb = summary;
            break;
        case '?':
            exit(EXIT_FAILURE);
        default:
            fprintf(stderr, "Error: unhandled option: -%c\n", opt);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        const char *path = argv[i];
        if (access(path, R_OK) != 0) {
            perror(path);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        const char *path = argv[i];
        if (nftw(path, cb, 20, FTW_PHYS | FTW_ACTIONRETVAL) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) {
        if (nftw(".", cb, 20, FTW_PHYS | FTW_ACTIONRETVAL) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
