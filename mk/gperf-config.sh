#!/bin/sh
set -eu

abort() {
    echo "Error: $1" >&2
    exit 1
}

version_min() {
    test -n "$1" -a -n "$2" || abort "version_min called with too few arguments"
    printf "$1\n$2" | sort -t. -n -k1,1 -k2,2 -k3,3 -k4,4 | head -n1
}

GPERF="$1"
GPERF_V=$("$GPERF" -v | sed -n '1{s/^GNU gperf \([0-9]\+\.[0-9.]\+\).*/\1/p}')
test -n "$GPERF_V" || abort "Unable to parse gperf version string"

if test "$(version_min $GPERF_V 3.1)" != 3.1; then
    echo '-DLENGTH_TYPE=unsigned'
fi
