# **************************************************************************

SUBDIRS = util nodes

libprofitincdir = $(includedir)/profit
libprofitinc_HEADERS = \
        basics.h \
        callback.h \
        debug.h \
        material.h \
        messages.h \
        model.h \
        node.h \
        nodeinfo.h \
        profit.h \
        state.h \
        texture.h \
        vertex.h

# **************************************************************************
## FIXME: this is a temporary workaround for a bug in Automake. The
## bug is fixed in the latest Automake CVS, but not in any "official"
## release (?). Last checked with Automake 1.4a. 19990912 mortene.

install-libprofitincHEADERS: $(libprofitinc_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libprofitincdir)
	@list='$(libprofitinc_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libprofitincdir)/$$f"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libprofitincdir)/$$f; \
	done

uninstall-libprofitincHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(libprofitinc_HEADERS)'; for p in $$list; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(libprofitincdir)/$$f"; \
	  rm -f $(DESTDIR)$(libprofitincdir)/$$f; \
	done

# EOF **********************************************************************
