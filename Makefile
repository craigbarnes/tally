include config.mk

CWARNS ?= -Wall -Wextra
CFLAGS ?= -g -O2
XCFLAGS = -std=c99
CTAGS ?= ctags
VGRIND ?= valgrind -q --error-exitcode=1 --leak-check=full
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
MANDIR ?= $(PREFIX)/share/man

RL_LANGS   = c css html lisp lua python sql xml
RL_PARSERS = $(addprefix parsers/, $(addsuffix .o, $(RL_LANGS)))
PARSERS    = $(RL_PARSERS) parsers/plain.o parsers/shell.o
HASHTABLES = extensions.o filenames.o

$(RL_PARSERS): private CWARNS += \
    -Wno-unused-const-variable \
    -Wno-unused-but-set-variable

$(HASHTABLES): private CWARNS += \
    -Wno-missing-field-initializers

all: tally
tally: tally.o languages.o parse.o $(HASHTABLES) $(PARSERS)
tally.o: languages.h parse.h
languages.o: languages.h parse.h
parse.o: parse.h
parsers/plain.o parsers/shell.o: languages.h parse.h
$(RL_PARSERS): languages.h parse.h parsers/prelude.h parsers/common.rl
$(HASHTABLES): languages.h

%.o: %.c
	$(CC) $(XCFLAGS) $(CWARNS) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.c: %.gperf config.mk
	$(GPERF) -L ANSI-C $< > $@

parsers/%.c: parsers/%.rl config.mk
	$(RAGEL) -o $@ $<

config.mk: configure
	@test -f '$@' || ./configure

tags: tally.c parse.[ch] languages.[ch]
	$(CTAGS) -f $@ $^

install: all
	mkdir -p '$(DESTDIR)$(BINDIR)/' '$(DESTDIR)$(MANDIR)/man1/'
	install -p -m 0755 tally '$(DESTDIR)$(BINDIR)/tally'
	install -p -m 0644 tally.1 '$(DESTDIR)$(MANDIR)/man1/tally.1'

uninstall:
	$(RM) '$(DESTDIR)$(BINDIR)/tally'
	$(RM) '$(DESTDIR)$(MANDIR)/man1/tally.1'

check: export CWARNS += -Werror
check:
	$(MAKE) -B -j`nproc` CC=clang
	$(MAKE) -B -j`nproc` CC=gcc
	$(VGRIND) ./tally > /dev/null
	$(VGRIND) ./tally -d > /dev/null

clean:
	$(RM) tally *.o parsers/*.o


.PHONY: all install uninstall check clean
.DELETE_ON_ERROR:
