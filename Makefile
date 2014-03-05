CFLAGS = -g -O2 -Wall

all: ctally

ctally: ctally.o findtype.o hashbang.o names.o

ctally.o: ctally.c pair.h
findtype.o: findtype.c pair.h
hashbang.o: hashbang.c pair.h
names.o: names.c pair.h

%.c: %.gperf
	gperf -ItTC -L ANSI-C -N $* $< > $@

clean:
	$(RM) ctally ctally.o findtype.[co] hashbang.[co] names.[co]


.PHONY: clean
.SECONDARY: findtype.c hashbang.c names.c
.DELETE_ON_ERROR:
