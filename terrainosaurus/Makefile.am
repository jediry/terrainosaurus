## Makefile.am -- Process this file with automake to generate Makefile.in

## All of the source code lives in the src directory
SUBDIRS = src

## If we're using ac_tools' automatic source-tracking capabilities, this
## make target is useful for forcing a rebuild of the source catalog (say,
## if we added or removed source files).
# run 'make catalog' to rebuild the automatic source file catalog
catalog :
	$(top_srcdir)/ac_tools/make_src_catalog.sh \
	    $(top_srcdir)/src > $(top_srcdir)/src_catalog
