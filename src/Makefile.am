# **************************************************************************
# Process this file with automake to generate Makefile.in.

SUBDIRS = util nodes
INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = libprofit.la

libprofit_la_CFLAGS = \
	$(PROFIT_CFLAGS)

libprofit_la_LDFLAGS = \
	-no-undefined \
        -version-info $(PROFIT_LIBTOOL_VERSION_INFO)

libprofit_la_LIBADD = \
	util/libutil.la \
	nodes/libnodes.la

libprofit_la_SOURCES = \
	basics.c \
	debug.c \
	messages.c \
	model.c \
	node.c \
	nodeinfo.c \
	profit.c \
	state.c \
	vertex.c \
	cruft.c

# **************************************************************************
# Custom Rule for Linking Windows Library (LIB/DLL)

profit@PROFIT_MAJOR_VERSION@.lib: Makefile $(libprofit_la_OBJECTS) $(libprofit_la_DEPENDENCIES)
	@basename="profit@PROFIT_MAJOR_VERSION@"; \
	rm -f "$$basename.dll" "$$basename.lib"; \
	if @BUILD_WITH_MSVC@; then \
	  echo "Linking $$basename.lib..."; \
	  echo $(CC) $(LDFLAGS) $(LIBFLAGS) \
	    `echo $(libprofit_la_OBJECTS) | sed 's%\.lo%.obj%g'` \
	    $(libprofit_la_LIBADD) $(LIBS); \
	  $(CC) $(LDFLAGS) $(LIBFLAGS) \
	    `echo $(libprofit_la_OBJECTS) | sed 's%\.lo%.obj%g'` \
	    $(libprofit_la_LIBADD) $(LIBS); \
	else \
	  touch "$$basename.dll" "$$basename.lib"; \
	fi

all-local: profit@PROFIT_MAJOR_VERSION@.lib

# **************************************************************************

install-exec-local:
	@basename="profit@PROFIT_MAJOR_VERSION@"; \
	for ext in dll pdb; do \
	  file="$$basename.$$ext"; \
	  if test -s "$$file"; then \
	    $(mkinstalldirs) "$(DESTDIR)$(bindir)"; \
	    echo " $(INSTALL_PROGRAM)" "$$file" "$(DESTDIR)$(bindir)/$$file"; \
	    $(INSTALL_PROGRAM) "$$file" "$(DESTDIR)$(bindir)/$$file"; \
	  fi \
	done;

install-data-local:
	@basename="profit@PROFIT_MAJOR_VERSION@"; \
	for ext in lib; do \
	  file="$$basename.$$ext"; \
	  if test -s "$$file"; then \
	    $(mkinstalldirs) "$(DESTDIR)$(libdir)"; \
	    echo " $(INSTALL_DATA)" "$$file" "$(DESTDIR)$(libdir)/$$file"; \
	    $(INSTALL_DATA) "$$file" "$(DESTDIR)$(libdir)/$$file"; \
	  fi \
	done;

uninstall-local:
	@basename="profit@PROFIT_MAJOR_VERSION@"; \
	for file in \
	  "$(DESTDIR)$(bindir)/$$basename.dll" \
	  "$(DESTDIR)$(bindir)/$$basename.pdb" \
	  "$(DESTDIR)$(libdir)/$$basename.lib"; do \
	  if test -f "$$file"; then \
	    echo " rm -f" "$$file"; \
	    rm -f "$$file"; \
	  fi \
	done; \
	baseincdir="$(DESTDIR)$(includedir)/profit"; \
	for dir in \
	  "$$baseincdir/util" \
	  "$$baseincdir/nodes" \
	  "$$baseincdir"; do \
	  if test -d "$$dir"; then \
	    echo " rmdir" "$$dir"; \
	    rmdir "$$dir"; \
	  fi \
	done;

# EOF **********************************************************************
