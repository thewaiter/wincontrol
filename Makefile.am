MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/images

filesdir = $(datadir)
files_DATA = module.desktop e-module-wincontrol.edj

EXTRA_DIST = module.desktop.in \
	     e_modules-wincontrol.spec.in \
	     e-module-wincontrol.edc \
	     images/module_icon.png

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf e-module-wincontrol.edj module.desktop e_modules-wincontrol.spec *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
