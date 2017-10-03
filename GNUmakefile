include config.mk

CC ?= gcc
LD = $(CC)
CFLAGS ?= -g -O2
XCFLAGS = -std=c99
CTAGS ?= ctags
VGRIND ?= valgrind -q --error-exitcode=1 --leak-check=full
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
MANDIR ?= $(PREFIX)/share/man

RL_LANGS   = c css html lisp lua meson python sql xml
RL_PARSERS = $(addprefix parsers/, $(addsuffix .o, $(RL_LANGS)))
PARSERS    = $(RL_PARSERS) parsers/plain.o parsers/shell.o
HASHTABLES = extensions.o filenames.o

CWARNS = \
    -Wall -Wextra -Wformat-security -Wmissing-prototypes \
    -Wold-style-definition -Wwrite-strings -Wundef -Wshadow

$(RL_PARSERS): private CWARNS += \
    -Wno-unused-const-variable

$(HASHTABLES): private CWARNS += \
    -Wno-missing-field-initializers

all: tally
tally.o: languages.h parse.h
languages.o: languages.h parse.h
parse.o: parse.h
parsers/plain.o parsers/shell.o: languages.h parse.h
$(RL_PARSERS): languages.h parse.h parsers/prelude.h parsers/common.rl
$(HASHTABLES): languages.h

tally: tally.o languages.o parse.o $(HASHTABLES) $(PARSERS)
	$(E) LINK $@
	$(Q) $(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(E) CC $@
	$(Q) $(CC) $(XCFLAGS) $(CWARNS) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.c: %.gperf config.mk
	$(E) GPERF $@
	$(Q) $(GPERF) -L ANSI-C $< > $@

parsers/%.c: parsers/%.rl config.mk
	$(E) RAGEL $@
	$(Q) $(RAGEL) -o $@ $<

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

ifneq "$(findstring s,$(firstword -$(MAKEFLAGS)))$(filter -s,$(MAKEFLAGS))" ""
  # Make "-s" flag was used (silent build)
  Q = @
  E = @:
else ifeq "$(V)" "1"
  # "V=1" variable was set (verbose build)
  Q =
  E = @:
else
  # Normal build
  Q = @
  E = @printf ' %7s  %s\n'
endif
