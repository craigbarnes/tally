#define _GNU_SOURCE
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "languages.h"
#include "names.h"

const struct pair *lookup_language_by_extension(const char*, unsigned int);
const struct pair *lookup_language_by_filename(const char*, unsigned int);

static inline const char *file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) {
        return NULL;
    }
    return dot + 1;
}

static Language detect_language(const char *filename, int base) {
    const char *basename = filename + base;
    const char *ext = file_extension(basename);
    const struct pair *ret;

    if (ext) {
        ret = lookup_language_by_extension(ext, strlen(ext));
        if (ret)
            return ret->language;
    }

    ret = lookup_language_by_filename(basename, strlen(basename));
    if (ret)
        return ret->language;

    return 0;
}

static int cb(const char *f, const struct stat *s, int t, struct FTW *ftw) {
    (void)s; // Unused
    const char *basename = f + ftw->base;
    const bool ignored = (basename[0] == '.' && ftw->level > 0);

    if (t == FTW_F && !ignored) {
        Language lang = detect_language(f, ftw->base);
        printf("%-16s  %s\n", basename, lookup_language_name(lang));
    }

    if (t == FTW_D && ignored) {
        return FTW_SKIP_SUBTREE; // TODO: Replace with something portable
    }

    return FTW_CONTINUE;
}

int main(int argc, char *argv[]) {
    const int flags = FTW_PHYS | FTW_ACTIONRETVAL;

    if (nftw((argc < 2) ? "." : argv[1], cb, 20, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
