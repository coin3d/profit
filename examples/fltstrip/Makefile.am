## Process this file with automake to generate Makefile.in.

DEFAULT_INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/include

noinst_PROGRAMS = fltstrip
fltstrip_SOURCES = fltstrip.c

if BUILD_WITH_MSVC
fltstrip_LDFLAGS = -L$(top_builddir)/src -lprofit@PROFIT_MAJOR_VERSION@@SUFFIX@
else
fltstrip_LDADD = $(top_builddir)/src/libprofit.la
endif

