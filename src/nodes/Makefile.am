# **************************************************************************
# Process this file with automake to generate Makefile.in.

if BUILD_WITH_MSVC
noinst_LIBRARIES = nodes.lst
else
noinst_LTLIBRARIES = libnodes.la
endif

DEFAULT_INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/include
LIBS = 

SourceFiles = \
	color_palette.c \
	face.c \
	group.c \
	header.c \
	instance_definition.c \
	instance_reference.c \
	material.c \
	material_palette.c \
	matrix.c \
	morph_vertex_list.c \
	object.c \
	obsolete_vertex_with_color.c \
	obsolete_vertex_with_normal.c \
	pop_attribute.c \
	pop_extension.c \
	pop_level.c \
	pop_subface.c \
	push_attribute.c \
	push_extension.c \
	push_level.c \
	push_subface.c \
	texture.c \
	vertex_list.c \
	vertex_palette.c \
	vertex_with_color.c \
	vertex_with_normal.c \
	vertex_with_normal_and_texture.c \
	vertex_with_texture.c

# **************************************************************************

libnodes_la_SOURCES = $(SourceFiles)
libnodes_la_CFLAGS = $(PROFIT_CFLAGS)

nodes.lst: Makefile $(nodes_lst_OBJECTS)
	echo "Linking $@..."; \
	rm -f $@; \
	touch $@; \
	for i in $(nodes_lst_OBJECTS); do \
	  echo $$i >>$@; \
	done

# EOF **********************************************************************
