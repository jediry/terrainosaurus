/*
 * File: TerrainViewer.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Description:
 */

#include <terrainosaurus/terrainosaurus-common.h>

// Initial GUI parameters
#define WINDOW_TITLE  "Terrainosaurus DEM File Viewer"
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480
#define FULL_SCREEN   false


#define GL_HPP_IMPORT_GLUT
#include <inca/integration/opengl/GL.hpp>
#include <inca/ui.hpp>

#include "ui/HackWindow.hpp"
#include "data/MapRasterization.hpp"
//#include "genetics/TerrainChromosome.hpp"
//#include "genetics/terrain-operations.hpp"
//#include "genetics/terrain-ga.hpp"

namespace terrainosaurus {
    // Forward declarations
    class TerrainViewer;
}


class terrainosaurus::TerrainViewer : public APPLICATION(GUI_TOOLKIT) {
    // Get command-line arguments and set up the terrainscape
    void setup(int &argc, char **argv) {
        // See if the user gave us any useful filenames
        string arg, ext, demFile;
        while (argc > 1) {
            arg = shift(argc, argv);
            ext = arg.substr(arg.length() - 4);
            if (ext == ".dem")
                filename = arg;
            else
                exit(1, "Unrecognized argument \"" + arg + "\"");
        }

        _terrainLibrary.reset(new TerrainLibrary());
        TerrainTypePtr tt = _terrainLibrary->addTerrainType("Sample");
        tt->setColor(Color(0.2f, 0.8f, 0.4f, 1.0f));

        _sample.reset(new TerrainSample(filename));
//        (*_sample)[LOD_30m].ensureStudied();
        tt->addTerrainSample(_sample);

        _map.reset(new MapRasterization(_terrainLibrary));
        MapRasterization::LOD::IDMap idx((*_sample)[LOD_30m].sizes());
        fill(idx, 1);
        (*_map)[LOD_30m].createFromRaster(idx);

        (*_map)[LOD_30m].terrainType(10, 10).object().setColor(Color(0.2f, 0.8f, 0.4f, 1.0f));
        cerr << "Color is " << (*_map)[LOD_30m].terrainType(10, 10).color() << endl;
    }

    // Create a window
    void constructInterface() {
        inca::ui::WindowPtr hw(new HackWindow(_sample, _map, LOD_30m, WINDOW_TITLE));
        registerWindow(hw);
    }

protected:
    MapRasterizationPtr _map;
    TerrainLibraryPtr   _terrainLibrary;
    TerrainSamplePtr    _sample;
    std::string filename;
};


// This macro expands to a main() function that instantiates the application
// and launches it
APPLICATION_MAIN(terrainosaurus::TerrainViewer);
