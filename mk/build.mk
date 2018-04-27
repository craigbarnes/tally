CC ?= gcc
LD = $(CC)
CFLAGS ?= -g -O2
XCFLAGS = -std=c99
VGRIND ?= valgrind -q --error-exitcode=1 --leak-check=full

RL_LANGS   = c css html lisp lua meson python sql xml
RL_PARSERS = $(addprefix parsers/, $(addsuffix .o, $(RL_LANGS)))
PARSERS    = $(RL_PARSERS) parsers/plain.o parsers/shell.o
HASHTABLES = extensions.o filenames.o

CWARNS = \
    -Wall -Wextra -Wformat=2 -Wmissing-prototypes \
    -Wold-style-definition -Wwrite-strings -Wundef -Wshadow

ifdef WERROR
  CWARNS += -Werror
endif

$(RL_PARSERS): private CWARNS += \
    -Wno-unused-const-variable

$(HASHTABLES): private CWARNS += \
    -Wno-missing-field-initializers

tally.o: languages.h parse.h
languages.o: languages.h parse.h
parse.o: parse.h
parsers/plain.o parsers/shell.o: languages.h parse.h
$(RL_PARSERS): languages.h parse.h parsers/prelude.h parsers/common.rl
$(HASHTABLES): languages.h

all: tally

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

check: all
	$(VGRIND) ./tally > /dev/null
	$(VGRIND) ./tally -d > /dev/null


.PHONY: all check
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
