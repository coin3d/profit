## Process this file with automake to generate Makefile.in.

DEFAULT_INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/include

noinst_PROGRAMS = fltdump
fltdump_SOURCES = fltdump.c

if BUILD_WITH_MSVC
fltdump_LDFLAGS = -L$(top_builddir)/src -lprofit@PROFIT_MAJOR_VERSION@@SUFFIX@
else
fltdump_LDADD = $(top_builddir)/src/libprofit.la
endif
