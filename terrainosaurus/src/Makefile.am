## Makefile.am -- Process this file with automake to generate Makefile.in

###############################################################################
##
## Build configuration variables
##
###############################################################################
# Final output binaries go here
distdir=$(top_srcdir)/dist


###############################################################################
##
## Source file definitions
##
###############################################################################
## Include the source file catalog
include $(top_srcdir)/src_catalog


dist_PROGRAMS = map_editor

map_editor_SOURCES = $(src)



###############################################################################
##
## Special build target definitions
##
###############################################################################
# Make the Makefile depend on the src catalog
Makefile.in : $(top_srcdir)/src_catalog

# Rebuild the src catalog
catalog :
	( cd $(top_srcdir) ; make catalog )

# Reset the dependency information that make uses
clean-deps :
	for file in .deps/*; do echo > $$file ; done
