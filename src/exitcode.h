#ifndef EXITCODE_H
#define EXITCODE_H

// Semantic exit codes, as defined by BSD sysexits(3)
enum {
    EX_OK = 0, // Exited normally
    EX_USAGE = 64, // Command line usage error
    EX_NOINPUT = 66, // Input file missing or not readable
    EX_SOFTWARE = 70, // Internal software error
    EX_IOERR = 74, // Input/output error
};

#endif
