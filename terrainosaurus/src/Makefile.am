## Makefile.am -- Process this file with automake to generate Makefile.in

## Incorporate automatic source file tracking mechanism
include $(top_srcdir)/ac_tools/automake/src-catalog.rules

## Incorporate manually-invoked rules for regenerating and cleaning things
include $(top_srcdir)/ac_tools/automake/manual.rules

## FIXME: create ANTLR .g rule to build .o's (or maybe .a) from a .g file directly
## FIXME: change make_src_catalog to keep 'terrainosaurus_src' separate from 'terrainosaurus_src_recursive' or something

# Import the auto-generated source catalog
include $(top_srcdir)/config/src-catalog

## Program definitions
bin_PROGRAMS = map_explorer verify_dem view_dem
map_explorer_SOURCES = $(terrainosaurus_data_src) $(terrainosaurus_genetics_src) \
		       $(terrainosaurus_io_src) \
		       $(terrainosaurus_rendering_src) \
		       terrainosaurus/ui/HackWindow.cpp \
		       terrainosaurus/ui/GeneticsWindow.cpp \
		       terrainosaurus/ui/MapEditorWindow.cpp \
		       terrainosaurus/MapExplorer.cpp \
		       terrainosaurus/TerrainosaurusComponent.cpp

verify_dem_SOURCES = terrainosaurus/io/DEMInterpreter.cpp test/verify_dem.cpp

view_dem_SOURCES = terrainosaurus/TerrainViewer.cpp $(terrainosaurus_data_src) \
		       terrainosaurus/io/DEMInterpreter.cpp \
		       terrainosaurus/rendering/HeightfieldRendering.cpp \
		       terrainosaurus/ui/HackWindow.cpp

check_PROGRAMS = analyze_dem
analyze_dem_SOURCES = $(terrainosaurus_data_src) terrainosaurus/io/DEMInterpreter.cpp \
		      test/analyze_dem.cpp
