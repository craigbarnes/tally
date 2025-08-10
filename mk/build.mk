CC ?= gcc
CFLAGS ?= -g -O2
GPERF = gperf
RAGEL = ragel
RAGEL_FLAGS = -G2

parsers = c config css html lisp lua meson plain python shell sql xml
ragel_objects := $(foreach p, $(parsers), build/parsers/$(p).o)
gperf_objects := build/extensions.o build/filenames.o
main_objects := build/tally.o build/languages.o
all_objects := $(main_objects) $(gperf_objects) $(ragel_objects)

ifeq "$(USE_SANITIZER)" "1"
  ifneq "" "$(CC_SANITIZER_FLAGS)"
    $(all_objects): BASIC_CFLAGS += $(CC_SANITIZER_FLAGS)
    tally: BASIC_LDFLAGS += $(CC_SANITIZER_FLAGS)
  endif
  DEBUG = 3
else
  DEBUG ?= 0
endif

ifeq "$(DEBUG)" "0"
  BASIC_CFLAGS += $(CC_NO_UNWIND_TABLES)
else
  BASIC_CPPFLAGS += -DDEBUG='$(DEBUG)'
endif

ifeq "$(WERROR)" "1"
  BASIC_CFLAGS += -Werror
endif

OPTCHECK = mk/optcheck.sh $(if $(MAKE_S),-s)

# If "make install*" with no other named targets
ifeq "" "$(filter-out install%,$(or $(MAKECMDGOALS),all))"
  # Only generate files with $(OPTCHECK) if they don't already exist
  OPTCHECK += -R
endif

ifndef NO_DEPS
  -include $(patsubst %.o, %.mk, $(all_objects))
endif

$(gperf_objects): BASIC_CPPFLAGS += -I src
$(ragel_objects): BASIC_CPPFLAGS += -I src/parsers

CFLAGS_ALL = $(CPPFLAGS) $(CFLAGS) $(BASIC_CPPFLAGS) $(BASIC_CFLAGS)
LDFLAGS_ALL = $(CFLAGS) $(LDFLAGS) $(BASIC_LDFLAGS)

all: tally

tally: $(all_objects) build/all.ldflags
	$(E) LINK $@
	$(Q) $(CC) $(LDFLAGS_ALL) -o $@ $(filter %.o, $^)

$(ragel_objects) $(gperf_objects): build/%.o: build/%.c build/all.cflags
	$(E) CC $@
	$(Q) $(CC) $(CFLAGS_ALL) $(CC_DEPFLAGS) -c -o $@ $<

build/%.o: src/%.c build/all.cflags | build/
	$(E) CC $@
	$(Q) $(CC) $(CFLAGS_ALL) $(CC_DEPFLAGS) -c -o $@ $<

build/all.ldflags: FORCE | build/
	@$(OPTCHECK) '$(CC) $(LDFLAGS_ALL)' $@

build/all.cflags: FORCE | build/
	@$(OPTCHECK) '$(CC) $(CFLAGS_ALL)' $@

build/all.ragelflags: FORCE | build/
	@$(OPTCHECK) '$(RAGEL) $(RAGEL_FLAGS)' $@

build/%.c: src/%.gperf | build/
	$(E) GPERF $@
	$(Q) $(GPERF) -L ANSI-C $< > $@

build/parsers/%.c: src/parsers/%.rl src/parsers/common.rl build/all.ragelflags | build/parsers/
	$(E) RAGEL $@
	$(Q) $(RAGEL) $(RAGEL_FLAGS) -o $@ $<

build/gen/compiler.mk: mk/compiler.sh | build/gen/
	$(E) GEN $@
	$(Q) mk/compiler.sh '$(CC)' '$(logfile)' >$@

build/ build/parsers/ build/gen/:
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
    NPROC := $(or $(shell sh mk/nproc.sh), 1)
    MAKEFLAGS += -j$(NPROC)
  endif
endif
