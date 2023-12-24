try-run = $(if $(shell $(1) >/dev/null 2>&1 && echo 1),$(2),$(3))
cc-option = $(call try-run,$(CC) $(1) -Werror -c -o /dev/null mk/x.c,$(1),$(2))

MAKEFLAGS += -r
NPROC = $(or $(shell sh mk/nproc.sh), 1)
MAKE_S = $(findstring s,$(firstword -$(MAKEFLAGS)))

ifneq "$(MAKE_S)" ""
  # Make "-s" flag was used (silent build)
  LOG = :
  Q = @
else ifeq "$(V)" "1"
  # "V=1" variable was set (verbose build)
  LOG = :
  Q =
else
  # Normal build
  LOG = printf ' %7s  %s\n'
  Q = @
endif

E = @$(LOG)
