## Makefile.am -- Process this file with automake to generate Makefile.in

###############################################################################
##
## Automatic source file tracking stuff
##
###############################################################################
## Include the source file catalog
include $(top_srcdir)/src_catalog

# Make the Makefile depend on the src catalog
Makefile.in : Makefile.am $(top_srcdir)/src_catalog

# Rebuild the src catalog
catalog :
	( cd $(top_srcdir) ; make catalog )


###############################################################################
##
## Source file definitions
##
###############################################################################


bin_PROGRAMS = map_editor

map_editor_SOURCES = GLUTMapEditor.cpp Map.cpp MapEditorWidget.cpp
map_editor_LDADD = -L/home/jediry/Documents/Development/Media/libinca/src \
		   -linca -lGL -lGLU -lglut



###############################################################################
##
## Special build target definitions
##
###############################################################################
# Reset the dependency information that make uses
clean-deps :
	for file in .deps/*; do echo > $$file ; done
