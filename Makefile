CFLAGS = -g -O2 -Wall

all: ctally

ctally: ctally.o findtype.o

ctally.o: ctally.c pair.h
findtype.o: findtype.c pair.h

%.c: %.gperf
	gperf -ItTC -L ANSI-C -N $* $< > $@

clean:
	$(RM) ctally ctally.o findtype.[co]


.PHONY: clean
.SECONDARY: findtype.c
.DELETE_ON_ERROR:
