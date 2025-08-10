# Generate a log filename for $@, by removing its extension (if any)
# and appending ".log"
logfile = $(@:$(suffix $@)=.log)

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
