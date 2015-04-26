CWARNS = -Wall -Wextra -pedantic -Wno-missing-field-initializers
CFLAGS = -g -O2 -std=c99 $(CWARNS)
GPERF  = gperf
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

all: tally

tally: tally.o extensions.o filenames.o
tally.o extensions.o hashbangs.o filenames.o: languages.h
tally.o: names.h

%.c: %.gperf
	$(GPERF) -L ANSI-C $< > $@

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	install -p -m 0755 tally $(DESTDIR)$(BINDIR)/tally

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/tally

clean:
	$(RM) tally *.o


.PHONY: all clean
.DELETE_ON_ERROR:
