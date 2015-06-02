CWARNS = -Wall -Wextra
CFLAGS = -g -O2 -std=c99 $(CWARNS)
GPERF  = gperf
RAGEL  = ragel
CTAGS  = ctags
VGRIND = valgrind -q --error-exitcode=1 --leak-check=full
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man

RL_LANGS   = c css html lisp lua python sql xml
RL_PARSERS = $(addprefix parsers/, $(addsuffix .o, $(RL_LANGS)))
PARSERS    = $(RL_PARSERS) parsers/plain.o parsers/shell.o

tally: tally.o languages.o parse.o extensions.o filenames.o $(PARSERS)
tally.o: languages.h parse.h
languages.o: languages.h parse.h
parse.o: parse.h
extensions.o filenames.o: languages.h
parsers/plain.o parsers/shell.o: languages.h parse.h
$(RL_PARSERS): languages.h parse.h parsers/prelude.h parsers/common.rl

$(RL_PARSERS): CWARNS += -Wno-unused-const-variable
extensions.o filenames.o: CWARNS += -Wno-missing-field-initializers
extensions.o filenames.o: CWARNS += -Wno-static-in-inline

%.c: %.gperf
	$(GPERF) -L ANSI-C $< > $@

parsers/%.c: parsers/%.rl
	$(RAGEL) -o $@ $<

tags: tally.c parse.[ch] languages.[ch]
	$(CTAGS) -f $@ $^

install: tally | $(DESTDIR)$(BINDIR)/ $(DESTDIR)$(MANDIR)/man1/
	install -p -m 0755 tally '$(DESTDIR)$(BINDIR)/tally'
	install -p -m 0644 tally.1 '$(DESTDIR)$(MANDIR)/man1/tally.1'

uninstall:
	$(RM) '$(DESTDIR)$(BINDIR)/tally'
	$(RM) '$(DESTDIR)$(MANDIR)/man1/tally.1'

$(DESTDIR)$(BINDIR)/ $(DESTDIR)$(MANDIR)/man1/:
	mkdir -p '$@'

check: tally
	$(VGRIND) ./tally > /dev/null
	$(VGRIND) ./tally -d > /dev/null

clean:
	$(RM) tally *.o parsers/*.o


.PHONY: install uninstall check clean
.DELETE_ON_ERROR:
