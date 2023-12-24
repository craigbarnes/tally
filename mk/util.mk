CFILE := mk/basic.c
try-run = $(if $(shell $(1) >/dev/null 2>&1 && echo 1),$(2),$(3))
cc-option = $(call try-run,$(CC) $(1) -Werror -c -o /dev/null $(CFILE),$(1),$(2))

# Note: this doesn't work reliably in make 3.81, due to a bug, but
# we already check for GNU Make 4.0+ in mk/compat.mk.
# See also: https://blog.jgc.org/2016/07/lazy-gnu-make-variables.html
make-lazy = $(eval $1 = $$(eval $1 := $(value $(1)))$$($1))

NPROC = $(or $(shell sh mk/nproc.sh), 1)
$(call make-lazy,NPROC)

MAKEFLAGS += -r
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
