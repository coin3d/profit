## Process this file with automake to generate Makefile.in.

lib_LTLIBRARIES = libprofit.la

libprofit_la_LIBADD = util/libutil.la nodes/libnodes.la

SUBDIRS = util nodes .

INCLUDES = -I$(top_srcdir)/include

## All source files from the base directory used when building the
## library, including all headers.

libprofit_la_SOURCES = \
	basic_funcs.c \
	basic_funcs.h \
	basic_types.h \
	callback.h \
	debug.c \
	debug.h \
	material.h \
	messages.c \
	messages.h \
	model.c \
	model.h \
	node.c \
	node.h \
	nodeinfo.c \
	nodeinfo.h \
	profit.c \
	profit.h \
	state.c \
	state.h \
	texture.h \
	types.h \
	vertex.c \
	vertex.h

## Installation information.

libprofitincdir = $(includedir)/profit

libprofit_la_LDFLAGS = \
        -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
        -release $(LT_RELEASE)

libprofitinc_HEADERS = \
	$(top_srcdir)/include/profit/basic_funcs.h \
	$(top_srcdir)/include/profit/basic_types.h \
	$(top_srcdir)/include/profit/callback.h \
	$(top_srcdir)/include/profit/debug.h \
	$(top_srcdir)/include/profit/material.h \
	$(top_srcdir)/include/profit/messages.h \
	$(top_srcdir)/include/profit/model.h \
	$(top_srcdir)/include/profit/node.h \
	$(top_srcdir)/include/profit/nodeinfo.h \
	$(top_srcdir)/include/profit/profit.h \
	$(top_srcdir)/include/profit/state.h \
	$(top_srcdir)/include/profit/texture.h \
	$(top_srcdir)/include/profit/types.h \
	$(top_srcdir)/include/profit/vertex.h

## FIXME: this is a temporary workaround for a bug in Automake. The
## bug is fixed in the latest Automake CVS, but not in any "official"
## release (?). Last checked with Automake 1.4a. 19990912 mortene.

install-libprofitincHEADERS: $(libprofitinc_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libprofitincdir)
	@list='$(libprofitinc_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libprofitincdir)/$$f"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libprofitincdir)/$$f; \
	done

uninstall-libprofitincHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(libprofitinc_HEADERS)'; for p in $$list; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(libprofitincdir)/$$f"; \
	  rm -f $(DESTDIR)$(libprofitincdir)/$$f; \
	done
