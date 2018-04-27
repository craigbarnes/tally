CC ?= gcc
CFLAGS ?= -g -O2
XCFLAGS = -std=c99
VALGRIND ?= valgrind -q --error-exitcode=1 --leak-check=full

LANGS_RL = c css html lisp lua meson python sql xml
PARSERS_RL = $(foreach L, $(LANGS_RL), build/parsers/$L.o)
PARSERS = $(PARSERS_RL) build/parsers/plain.o build/parsers/shell.o
HASHTABLES = build/extensions.o build/filenames.o

CWARNS = \
    -Wall -Wextra -Wformat=2 -Wmissing-prototypes \
    -Wold-style-definition -Wwrite-strings -Wundef -Wshadow

ifdef WERROR
  CWARNS += -Werror
endif

$(PARSERS_RL): XCFLAGS += -Iparsers
$(PARSERS_RL): CWARNS += -Wno-unused-const-variable
$(HASHTABLES): CWARNS += -Wno-missing-field-initializers

build/tally.o: languages.h parse.h
build/languages.o: languages.h parse.h
build/parse.o: parse.h
build/parsers/plain.o build/parsers/shell.o: languages.h parse.h
$(PARSERS_RL): languages.h parse.h parsers/prelude.h parsers/common.rl
$(HASHTABLES): languages.h

all: tally

tally: build/tally.o build/languages.o build/parse.o $(HASHTABLES) $(PARSERS)
	$(E) LINK $@
	$(Q) $(CC) $(LDFLAGS) -o $@ $^

$(PARSERS_RL) $(HASHTABLES): build/%.o: build/%.c
	$(E) CC $@
	$(Q) $(CC) $(XCFLAGS) $(CWARNS) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

build/%.o: %.c | build/
	$(E) CC $@
	$(Q) $(CC) $(XCFLAGS) $(CWARNS) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

build/%.c: %.gperf config.mk | build/
	$(E) GPERF $@
	$(Q) $(GPERF) -L ANSI-C $< > $@

build/parsers/%.c: parsers/%.rl config.mk | build/parsers/
	$(E) RAGEL $@
	$(Q) $(RAGEL) -o $@ $<

config.mk: configure
	$(Q) test -f '$@' || ./configure

build/ build/parsers/:
	$(Q) mkdir -p $@

check: all
	$(VALGRIND) ./tally > /dev/null
	$(VALGRIND) ./tally -d > /dev/null
	$(VALGRIND) ./tally -i > /dev/null


CLEANFILES += tally
CLEANDIRS += build/
.PHONY: all check

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
