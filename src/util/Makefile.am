## Process this file with automake to generate Makefile.in.

noinst_LTLIBRARIES = libutil.la

## Remove any linklibraries detected by configure, as they are not
## needed in the submodules -- only in the toplevel library build.
LIBS = 

INCLUDES = -I$(top_srcdir)/include

## All source files used when building this submodule, including all
## headers.

libutil_la_SOURCES = \
	array.c \
	array.h \
	bfile.c \
	bfile.h \
	mempool.c \
	mempool.h


## Installation information.

libutilincdir = $(includedir)/profit/util

libutilinc_HEADERS = \
	$(top_srcdir)/include/profit/util/array.h \
	$(top_srcdir)/include/profit/util/bfile.h \
	$(top_srcdir)/include/profit/util/mempool.h


## FIXME: this is a temporary workaround for a bug in Automake. The
## bug is fixed in the latest Automake CVS, but not in any "official"
## release (?). Last checked with Automake 1.4a. 19990912 mortene.

install-libutilincHEADERS: $(libutilinc_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libutilincdir)
	@list='$(libutilinc_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libutilincdir)/$$f"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libutilincdir)/$$f; \
	done

uninstall-libutilincHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(libutilinc_HEADERS)'; for p in $$list; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(libutilincdir)/$$f"; \
	  rm -f $(DESTDIR)$(libutilincdir)/$$f; \
	done
