CFLAGS = -g -O2 -std=c99 -pedantic -Wall
GPERF  = gperf

all: ctally

ctally: ctally.o findtype.o hashbang.o names.o
findtype.o hashbang.o names.o ctally.o: pair.h

%.c: %.gperf
	$(GPERF) -L ANSI-C $< > $@

clean:
	$(RM) ctally *.o


.PHONY: all clean
.DELETE_ON_ERROR:
