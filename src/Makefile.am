# **************************************************************************
# Process this file with automake to generate Makefile.in.

if BUILD_WITH_MSVC
lib_LIBRARIES = profit@PROFIT_MAJOR_VERSION@@SUFFIX@.lib
else
lib_LTLIBRARIES = libprofit.la
endif

SUBDIRS = util nodes

DEFAULT_INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/include

SourceFiles = \
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

libprofit_la_CFLAGS = $(PROFIT_CFLAGS)

libprofit_la_LDFLAGS = \
	-no-undefined \
        -version-info $(PROFIT_LIBTOOL_VERSION_INFO)

libprofit_la_LIBADD = \
	util/libutil.la \
	nodes/libnodes.la

libprofit_la_SOURCES = $(SourceFiles)

profit@PROFIT_MAJOR_VERSION@@SUFFIX@_lib_SOURCES = $(SourceFiles)
profit@PROFIT_MAJOR_VERSION@@SUFFIX@_lib_CFLAGS = $(PROFIT_CFLAGS)
profit@PROFIT_MAJOR_VERSION@@SUFFIX@_lib_LIBADD = \
	util/util.lst \
	nodes/nodes.lst

# **************************************************************************

profit@PROFIT_MAJOR_VERSION@@SUFFIX@.lib: Makefile $(profit_lib_OBJECTS) $(profit_lib_DEPENDENCIES)
	@basename="profit@PROFIT_MAJOR_VERSION@@SUFFIX@"; \
	rm -f "$$basename.dll" "$$basename.lib"; \
	echo "Linking $$basename.lib..."; \
	echo $(CC) $(LDFLAGS) $(LIBFLAGS) \
	  $(profit@PROFIT_MAJOR_VERSION@@SUFFIX@_lib_OBJECTS) \
	  $(profit@PROFIT_MAJOR_VERSION@@SUFFIX@_lib_LIBADD) $(LIBS); \
	$(CC) $(LDFLAGS) $(LIBFLAGS) \
	  $(profit@PROFIT_MAJOR_VERSION@@SUFFIX@_lib_OBJECTS) \
	  $(profit@PROFIT_MAJOR_VERSION@@SUFFIX@_lib_LIBADD) $(LIBS);

# **************************************************************************

if BUILD_WITH_MSVC
install-exec-local: profit@PROFIT_MAJOR_VERSION@@SUFFIX@.lib
	@basename="profit@PROFIT_MAJOR_VERSION@"; \
	for ext in dll pdb; do \
	  file="$$basename.$$ext"; \
	  if test -s "$$file"; then \
	    $(mkinstalldirs) "$(DESTDIR)$(bindir)"; \
	    echo " $(INSTALL_PROGRAM)" "$$file" "$(DESTDIR)$(bindir)/$$file"; \
	    $(INSTALL_PROGRAM) "$$file" "$(DESTDIR)$(bindir)/$$file"; \
	  fi \
	done;
else
install-exec-local:
endif

install-data-local:
	@basename="profit@PROFIT_MAJOR_VERSION@@SUFFIX@"; \
	for ext in lib; do \
	  file="$$basename.$$ext"; \
	  if test -s "$$file"; then \
	    $(mkinstalldirs) "$(DESTDIR)$(libdir)"; \
	    echo " $(INSTALL_DATA)" "$$file" "$(DESTDIR)$(libdir)/$$file"; \
	    $(INSTALL_DATA) "$$file" "$(DESTDIR)$(libdir)/$$file"; \
	  fi \
	done;

uninstall-local:
	@basename="profit@PROFIT_MAJOR_VERSION@@SUFFIX@"; \
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
