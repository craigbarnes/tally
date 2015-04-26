CWARNS = -Wall -Wextra -pedantic -Wno-missing-field-initializers
CFLAGS = -g -O2 -std=c99 $(CWARNS)
GPERF  = gperf

all: ctally

ctally: ctally.o extensions.o filenames.o
ctally.o extensions.o hashbangs.o filenames.o: languages.h
ctally.o: names.h

%.c: %.gperf
	$(GPERF) -L ANSI-C $< > $@

clean:
	$(RM) ctally *.o


.PHONY: all clean
.DELETE_ON_ERROR:
