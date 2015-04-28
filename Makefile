CWARNS = -Wall -Wextra -pedantic -Wno-missing-field-initializers
CFLAGS = -g -O2 -std=c99 $(CWARNS)
GPERF  = gperf
RAGEL  = ragel
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

all: tally

tally: tally.o parse.o extensions.o filenames.o parsers/c.o
tally.o: languages.h parse.h names.h
parse.o: languages.h parse.h
extensions.o filenames.o: languages.h
parsers/c.o: languages.h parse.h parsers/macros.h parsers/common.rl

%.c: %.gperf
	$(GPERF) -L ANSI-C $< > $@

parsers/%.c: parsers/%.rl
	$(RAGEL) -o $@ $<

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	install -p -m 0755 tally $(DESTDIR)$(BINDIR)/tally

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/tally

clean:
	$(RM) tally *.o parsers/*.o


.PHONY: all clean
.DELETE_ON_ERROR:
