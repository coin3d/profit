## Process this file with automake to generate Makefile.in.

noinst_LTLIBRARIES = libnodes.la

## Remove any linklibraries detected by configure, as they are not
## needed in the submodules -- only in the toplevel library build.
LIBS = 

INCLUDES = -I$(top_srcdir)/include

## All source files used when building this submodule, including all
## headers.

libnodes_la_SOURCES = \
	color_palette.c \
	color_palette.h \
	face.c \
	face.h \
	group.c \
	group.h \
	header.c \
	header.h \
	instance_definition.c \
	instance_definition.h \
	instance_reference.c \
	instance_reference.h \
	material_node.c \
	material_node.h \
	material_palette.c \
	material_palette.h \
	matrix.c \
	matrix.h \
	morph_vertex_list.c \
	morph_vertex_list.h \
	object.c \
	object.h \
	obsolete_vertex_with_color.c \
	obsolete_vertex_with_color.h \
	obsolete_vertex_with_normal.c \
	obsolete_vertex_with_normal.h \
	pop_attribute.c \
	pop_attribute.h \
	pop_extension.c \
	pop_extension.h \
	pop_level.c \
	pop_level.h \
	pop_subface.c \
	pop_subface.h \
	push_attribute.c \
	push_attribute.h \
	push_extension.c \
	push_extension.h \
	push_level.c \
	push_level.h \
	push_subface.c \
	push_subface.h \
	texture_node.c \
	texture_node.h \
	vertex_list.c \
	vertex_list.h \
	vertex_palette.c \
	vertex_palette.h \
	vertex_with_color.c \
	vertex_with_color.h \
	vertex_with_normal.c \
	vertex_with_normal.h \
	vertex_with_normal_and_texture.c \
	vertex_with_normal_and_texture.h \
	vertex_with_texture.c \
	vertex_with_texture.h


## Installation information.

libnodesincdir = $(includedir)/profit/nodes

libnodesinc_HEADERS = \
	$(top_srcdir)/include/profit/nodes/color_palette.h \
	$(top_srcdir)/include/profit/nodes/face.h \
	$(top_srcdir)/include/profit/nodes/group.h \
	$(top_srcdir)/include/profit/nodes/header.h \
	$(top_srcdir)/include/profit/nodes/instance_definition.h \
	$(top_srcdir)/include/profit/nodes/instance_reference.h \
	$(top_srcdir)/include/profit/nodes/material_node.h \
	$(top_srcdir)/include/profit/nodes/material_palette.h \
	$(top_srcdir)/include/profit/nodes/matrix.h \
	$(top_srcdir)/include/profit/nodes/morph_vertex_list.h \
	$(top_srcdir)/include/profit/nodes/object.h \
	$(top_srcdir)/include/profit/nodes/obsolete_vertex_with_color.h \
	$(top_srcdir)/include/profit/nodes/obsolete_vertex_with_normal.h \
	$(top_srcdir)/include/profit/nodes/pop_attribute.h \
	$(top_srcdir)/include/profit/nodes/pop_extension.h \
	$(top_srcdir)/include/profit/nodes/pop_level.h \
	$(top_srcdir)/include/profit/nodes/pop_subface.h \
	$(top_srcdir)/include/profit/nodes/push_attribute.h \
	$(top_srcdir)/include/profit/nodes/push_extension.h \
	$(top_srcdir)/include/profit/nodes/push_level.h \
	$(top_srcdir)/include/profit/nodes/push_subface.h \
	$(top_srcdir)/include/profit/nodes/replicate.h \
	$(top_srcdir)/include/profit/nodes/texture_node.h \
	$(top_srcdir)/include/profit/nodes/vertex_list.h \
	$(top_srcdir)/include/profit/nodes/vertex_palette.h \
	$(top_srcdir)/include/profit/nodes/vertex_with_color.h \
	$(top_srcdir)/include/profit/nodes/vertex_with_normal.h \
	$(top_srcdir)/include/profit/nodes/vertex_with_normal_and_texture.h \
	$(top_srcdir)/include/profit/nodes/vertex_with_texture.h


## FIXME: this is a temporary workaround for a bug in Automake. The
## bug is fixed in the latest Automake CVS, but not in any "official"
## release (?). Last checked with Automake 1.4a. 19990912 mortene.

install-libnodesincHEADERS: $(libnodesinc_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libnodesincdir)
	@list='$(libnodesinc_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libnodesincdir)/$$f"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libnodesincdir)/$$f; \
	done

uninstall-libnodesincHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(libnodesinc_HEADERS)'; for p in $$list; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(libnodesincdir)/$$f"; \
	  rm -f $(DESTDIR)$(libnodesincdir)/$$f; \
	done
