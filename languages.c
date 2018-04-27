#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "languages.h"

static inline const char *file_extension(const char *const filename) {
    const char *const dot = strrchr(filename, '.');
    if(!dot || dot == filename) {
        return NULL;
    }
    return dot + 1;
}

Language detect_language(const char *path, int base, int level, size_t size) {
    if (size == 0 || access(path, R_OK) != 0) {
        return IGNORED;
    }

    const char *const basename = path + base;

    if (basename[0] == '.' && level > 0) {
        return IGNORED;
    }

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
