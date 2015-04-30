CWARNS = -Wall -Wextra -pedantic -Wno-missing-field-initializers
CFLAGS = -g -O2 -std=c99 $(CWARNS)
GPERF  = gperf
RAGEL  = ragel
VGRIND = valgrind -q --error-exitcode=1 --leak-check=full
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

PARSERS= $(addprefix parsers/, c.o plain.o shell.o)

all: tally

tally: tally.o languages.o parse.o extensions.o filenames.o $(PARSERS)
tally.o: languages.h parse.h
languages.o: languages.h parse.h
parse.o: parse.h
extensions.o filenames.o: languages.h
parsers/c.o: languages.h parse.h parsers/prelude.h parsers/common.rl
parsers/plain.o parsers/shell.o: languages.h parse.h

%.c: %.gperf
	$(GPERF) -L ANSI-C $< > $@

parsers/%.c: parsers/%.rl
	$(RAGEL) -o $@ $<

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	install -p -m 0755 tally $(DESTDIR)$(BINDIR)/tally

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/tally

check: all
	$(VGRIND) ./tally > /dev/null
	$(VGRIND) ./tally -d > /dev/null

clean:
	$(RM) tally *.o parsers/*.o


.PHONY: all install uninstall check clean
.DELETE_ON_ERROR:
