# **************************************************************************
# Process this file with automake to generate Makefile.in.

if BUILD_WITH_MSVC
noinst_LIBRARIES = util.lst
else
noinst_LTLIBRARIES = libutil.la
endif

DEFAULT_INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/include
LIBS = 

SourceFiles = \
	array.c \
	bfile.c \
	mempool.c

# **************************************************************************

libutil_la_SOURCES = $(SourceFiles)
libutil_la_CFLAGS = $(PROFIT_CFLAGS)

# **************************************************************************

util.lst: Makefile $(util_lst_OBJECTS)
	@echo "Linking $@..."; \
	rm -f $@; \
	touch $@; \
	for i in $(util_lst_OBJECTS); do \
	  echo $$i >>$@; \
	done

# EOF **********************************************************************
