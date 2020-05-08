CC ?= gcc
CFLAGS ?= -g -O2
XCFLAGS = -std=c99
GPERF = gperf
RAGEL = ragel
RAGEL_FLAGS = -G2

XCFLAGS += \
    -Wall -Wextra -Wformat=2 -Wmissing-prototypes \
    -Wold-style-definition -Wwrite-strings -Wundef -Wshadow

rl_parser_objects := $(addprefix build/parsers/, $(addsuffix .o, \
    c css html lisp lua meson plain python sql xml ))

parser_objects := $(rl_parser_objects) build/parsers/shell.o
gperf_objects := build/extensions.o build/filenames.o

all_objects := \
    build/tally.o build/languages.o build/parse.o \
    $(gperf_objects) \
    $(parser_objects)

ifdef WERROR
  XCFLAGS += -Werror
endif

# If "make install" with no other named targets
ifeq "" "$(filter-out install,$(or $(MAKECMDGOALS),all))"
  OPTCHECK = :
else
  OPTCHECK = SILENT_BUILD='$(MAKE_S)' mk/optcheck.sh
endif

ifndef NO_DEPS
  ifneq '' '$(call cc-option,-MMD -MP -MF /dev/null)'
    DEPFLAGS = -MMD -MP -MF $(patsubst %.o, %.mk, $@)
  else ifneq '' '$(call cc-option,-MD -MF /dev/null)'
    DEPFLAGS = -MD -MF $(patsubst %.o, %.mk, $@)
  endif
  -include $(patsubst %.o, %.mk, $(all_objects))
endif

GPERF_CFLAGS = $(shell mk/gperf-config.sh '$(GPERF)')
$(call make-lazy,GPERF_CFLAGS)
XCFLAGS += $(GPERF_CFLAGS)

$(rl_parser_objects): XCFLAGS += \
    -Iparsers \
    -Wno-unused-const-variable \
    -Wno-implicit-fallthrough

CFLAGS_ALL = $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)
LDFLAGS_ALL = $(CFLAGS) $(LDFLAGS) $(XLDFLAGS)

all: tally

tally: $(all_objects) build/all.ldflags
	$(E) LINK $@
	$(Q) $(CC) $(LDFLAGS_ALL) -o $@ $(filter %.o, $^)

$(rl_parser_objects) $(gperf_objects): build/%.o: build/%.c build/all.cflags
	$(E) CC $@
	$(Q) $(CC) $(CFLAGS_ALL) $(DEPFLAGS) -c -o $@ $<

build/%.o: %.c build/all.cflags | build/
	$(E) CC $@
	$(Q) $(CC) $(CFLAGS_ALL) $(DEPFLAGS) -c -o $@ $<

build/all.ldflags: FORCE | build/
	@$(OPTCHECK) '$(CC) $(LDFLAGS_ALL)' $@

build/all.cflags: FORCE | build/
	@$(OPTCHECK) '$(CC) $(CFLAGS_ALL)' $@

build/%.c: %.gperf | build/
	$(E) GPERF $@
	$(Q) $(GPERF) -L ANSI-C $< > $@

build/parsers/%.c: parsers/%.rl parsers/common.rl | build/parsers/
	$(E) RAGEL $@
	$(Q) $(RAGEL) $(RAGEL_FLAGS) -o $@ $<

build/ build/parsers/:
	$(Q) mkdir -p $@

check: all
	./tally > /dev/null
	./tally -d > /dev/null
	./tally -i > /dev/null


.PHONY: all check FORCE
CLEANFILES += tally
CLEANDIRS += build/
NON_PARALLEL_TARGETS += clean

ifeq "" "$(filter $(NON_PARALLEL_TARGETS), $(or $(MAKECMDGOALS),all))"
  ifeq "" "$(filter -j%, $(MAKEFLAGS))"
    MAKEFLAGS += -j$(NPROC)
  endif
endif
