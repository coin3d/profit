## Process this file with automake to generate Makefile.in.

DEFAULT_INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/include

bin_PROGRAMS = flt2vrml

flt2vrml_SOURCES = \
	flt2vrml.c \
	array.c \
	array.h \
	bsptree.c \
	bsptree.h

if BUILD_WITH_MSVC
flt2vrml_LDFLAGS = -L$(top_builddir)/src -lprofit@PROFIT_MAJOR_VERSION@@SUFFIX@
else
flt2vrml_LDADD = $(top_builddir)/src/libprofit.la -lm
endif

