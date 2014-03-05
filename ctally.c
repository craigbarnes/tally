#define _GNU_SOURCE
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pair.h"

static const char *findext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) {
        return "";
    }
    return dot + 1;
}

static int cb(const char *f, const struct stat *s, int t, struct FTW *ftw) {
    (void)s; // Unused
    const char *basename = f + ftw->base;
    const char *ext = findext(basename);
    const bool ignored = (basename[0] == '.' && ftw->level > 0);

    if (t == FTW_F && !ignored) {
        printf("%-16s | %-8s |", basename, ext);
        const struct pair *ret = findtype(ext, strlen(ext));
        if (ret) {
            printf(" %s", ret->value);
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
