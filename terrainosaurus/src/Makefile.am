## Makefile.am -- Process this file with automake to generate Makefile.in

###############################################################################
##
## Automatic source file tracking stuff
##
###############################################################################

## Include the source file catalog
include $(top_srcdir)/src_catalog

# Make the Makefile depend on the src catalog
Makefile.in : Makefile.am src_catalog

# Rebuild the src catalog
catalog :
	#$(top_srcdir)/ac_tools/make_src_catalog.sh \
	#    $(top_srcdir)/src > $(top_srcdir)/src_catalog
	$(top_srcdir)/ac_tools/make_src_catalog.sh . > src_catalog

# Force a re-link of executables
relink :
	rm -f $(bin_PROGRAMS)
	make $(bin_PROGRAMS)


###############################################################################
##
## Source file definitions
##
###############################################################################
bin_PROGRAMS = map_explorer

map_explorer_SOURCES = MapExplorer.cpp MapExplorerWidget.cpp Map.cpp	\
		       TranslateWidget.cpp MapView.cpp
map_explorer_LDADD   = -linca -lglut


###############################################################################
##
## Special build target definitions
##
###############################################################################
# Reset the dependency information that make uses
clean-deps :
	for file in .deps/*; do echo > $$file ; done
