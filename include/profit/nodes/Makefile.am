# **************************************************************************

libprofitnodeincdir = $(includedir)/profit/nodes
libprofitnodeinc_HEADERS = \
	color_palette.h \
	external_reference.h \
	face.h \
	group.h \
	header.h \
	instance_definition.h \
	instance_reference.h \
	level_of_detail.h \
	material.h \
	material_palette.h \
	matrix.h \
	morph_vertex_list.h \
	object.h \
	obsolete_vertex_with_color.h \
	obsolete_vertex_with_normal.h \
	pop_attribute.h \
	pop_extension.h \
	pop_level.h \
	pop_subface.h \
	push_attribute.h \
	push_extension.h \
	push_level.h \
	push_subface.h \
	replicate.h \
	texture.h \
	vertex_list.h \
	vertex_palette.h \
	vertex_with_color.h \
	vertex_with_normal.h \
	vertex_with_normal_and_texture.h \
	vertex_with_texture.h

# **************************************************************************
## FIXME: this is a temporary workaround for a bug in Automake. The
## bug is fixed in the latest Automake CVS, but not in any "official"
## release (?). Last checked with Automake 1.4a. 19990912 mortene.

install-libprofitnodeincHEADERS: $(libprofitnodeinc_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libprofitnodeincdir)
	@list='$(libprofitnodeinc_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libprofitnodeincdir)/$$f"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libprofitnodeincdir)/$$f; \
	done

uninstall-libprofitnodeincHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(libprofitnodeinc_HEADERS)'; for p in $$list; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(libprofitnodeincdir)/$$f"; \
	  rm -f $(DESTDIR)$(libprofitnodeincdir)/$$f; \
	done

# EOF **********************************************************************
