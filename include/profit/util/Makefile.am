# **************************************************************************

libprofitutilincdir = $(includedir)/profit/util
libprofitutilinc_HEADERS = \
        array.h \
        bfile.h \
        mempool.h

# **************************************************************************
## FIXME: this is a temporary workaround for a bug in Automake. The
## bug is fixed in the latest Automake CVS, but not in any "official"
## release (?). Last checked with Automake 1.4a. 19990912 mortene.

install-libprofitutilincHEADERS: $(libprofitutilinc_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libprofitutilincdir)
	@list='$(libprofitutilinc_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libprofitutilincdir)/$$f"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libprofitutilincdir)/$$f; \
	done

uninstall-libprofitutilincHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(libprofitutilinc_HEADERS)'; for p in $$list; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(libprofitutilincdir)/$$f"; \
	  rm -f $(DESTDIR)$(libprofitutilincdir)/$$f; \
	done

# EOF **********************************************************************
