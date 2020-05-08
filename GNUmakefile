include mk/compat.mk
-include Config.mk
include mk/build.mk

prefix ?= /usr/local
bindir ?= $(prefix)/bin
mandir ?= $(prefix)/share/man
man1dir ?= $(mandir)/man1

INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644
RM = rm -f
CTAGS = ctags

install: all
	$(Q) $(INSTALL) -d -m755 '$(DESTDIR)$(bindir)'
	$(Q) $(INSTALL) -d -m755 '$(DESTDIR)$(man1dir)'
	$(E) INSTALL '$(DESTDIR)$(bindir)/tally'
	$(Q) $(INSTALL_PROGRAM) tally '$(DESTDIR)$(bindir)'
	$(E) INSTALL '$(DESTDIR)$(man1dir)/tally.1'
	$(Q) $(INSTALL_DATA) tally.1 '$(DESTDIR)$(man1dir)'

uninstall:
	$(RM) '$(DESTDIR)$(bindir)/tally'
	$(RM) '$(DESTDIR)$(man1dir)/tally.1'

tags: tally.c parse.[ch] languages.[ch]
	$(CTAGS) -f $@ $^

clean:
	$(RM) $(CLEANFILES)
	$(RM) -r $(CLEANDIRS)


.DEFAULT_GOAL = all
.PHONY: install uninstall clean
.DELETE_ON_ERROR:
