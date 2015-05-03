CWARNS = -Wall -Wextra -pedantic -Wno-missing-field-initializers
CFLAGS = -g -O2 -std=c99 $(CWARNS)
GPERF  = gperf
RAGEL  = ragel
CTAGS  = ctags
VGRIND = valgrind -q --error-exitcode=1 --leak-check=full
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

RL_LANGS   = c sql xml
RL_PARSERS = $(addprefix parsers/, $(addsuffix .o, $(RL_LANGS)))
PARSERS    = $(RL_PARSERS) parsers/plain.o parsers/shell.o

tally: tally.o languages.o parse.o extensions.o filenames.o $(PARSERS)
tally.o: languages.h parse.h
languages.o: languages.h parse.h
parse.o: parse.h
extensions.o filenames.o: languages.h
parsers/plain.o parsers/shell.o: languages.h parse.h
$(RL_PARSERS): languages.h parse.h parsers/prelude.h parsers/common.rl

%.c: %.gperf
	$(GPERF) -L ANSI-C $< > $@

parsers/%.c: parsers/%.rl
	$(RAGEL) -o $@ $<

tags: tally.c parse.[ch] languages.[ch]
	$(CTAGS) -f $@ $^

install: tally
	mkdir -p $(DESTDIR)$(BINDIR)
	install -p -m 0755 tally $(DESTDIR)$(BINDIR)/tally

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/tally

check: tally
	$(VGRIND) ./tally > /dev/null
	$(VGRIND) ./tally -d > /dev/null

clean:
	$(RM) tally *.o parsers/*.o


.PHONY: install uninstall check clean
.DELETE_ON_ERROR:
