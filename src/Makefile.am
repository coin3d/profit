# **************************************************************************
# Process this file with automake to generate Makefile.in.

SUBDIRS = util nodes
INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = libprofit.la

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
# Custom Rules for Linking Windows DLL

# EOF **********************************************************************
