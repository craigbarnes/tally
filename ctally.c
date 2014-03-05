#define _GNU_SOURCE
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pair.h"

// TODO: Make naming more consistent
const struct pair *findtype(register const char*, register unsigned int);
const struct pair *names(register const char*, register unsigned int);

static const char *findext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) {
        return "";
    }
    return dot + 1;
}

static const char *gettype(const char *filename, int base) {
    const char *basename = filename + base;
    const char *ext = findext(basename);
    const struct pair *ret;

    ret = findtype(ext, strlen(ext));
    if (ret)
        return ret->value;

    ret = names(basename, strlen(basename));
    if (ret)
        return ret->value;

    return NULL;
}

static int cb(const char *f, const struct stat *s, int t, struct FTW *ftw) {
    (void)s; // Unused
    const char *basename = f + ftw->base;
    const char *ext = findext(basename);
    const bool ignored = (basename[0] == '.' && ftw->level > 0);

    if (t == FTW_F && !ignored) {
        printf("%-16s | %-8s |", basename, ext);
        const char *lang = gettype(f, ftw->base);
        if (lang) {
            printf(" %s", lang);
        }
        printf("\n");
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
