/*
 * File: MapExplorer.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Description:
 */

// Initial GUI parameters
#define WINDOW_TITLE  "Terrainosaurus Map Explorer"
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480
#define FULL_SCREEN   false


// Import class definition
#include "MapExplorer.hpp"


// Import IOstream operators for data objects
#include "io/terrainosaurus-iostream.hpp"
#include <fstream>

// Import file-I/O exception definitions
#include <inca/io/FileExceptions.hpp>

#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/statistic>
#include <inca/raster/operators/select>
#include <inca/raster/operators/gradient>
#include <inca/raster/operators/linear_map>
#include <inca/raster/operators/fourier>
//#include <inca/raster/operators/complex>
//#include <inca/raster/operators/blur>
#include <inca/raster/generators/gaussian>
#include <inca/raster/algorithms/flood_fill>

using namespace terrainosaurus;
using namespace inca::raster;
using namespace inca::world;
using namespace inca::ui;
using namespace inca::io;

#include <complex>

typedef MultiArrayRaster< std::complex<float>, 2> ComplexImage;
typedef MultiArrayRaster< inca::math::Vector<float, 2>, 2> GradientImage;

#define GL_HPP_IMPORT_GLUT
#include <inca/integration/opengl/GL.hpp>
#include <inca/ui.hpp>
#include <inca/util/Timer>

#include "ui/ImageWindow.hpp"
#include "ui/HackWindow.hpp"
#include "data/MapRasterization.hpp"
#include "genetics/TerrainChromosome.hpp"
#include "genetics/terrain-operations.hpp"
#include "genetics/terrain-ga.hpp"
using namespace terrainosaurus;

// Hacked in thing
struct InTriangle {
    // Constructor
    InTriangle(Pixel _p0, Pixel _p1, Pixel _p2)
        : p0(_p0), p1(_p1), p2(_p2) { }

    template <typename IndexList>
    bool operator()(const IndexList & indices) {
        Pixel p(indices);

        if (p0 == p || p1 == p || p2 == p)
            return true;

        int ref1, ref2;
        bool ref1Found = false, ref2Found = false;
        if (p0[1] == p[1]) {
            ref1 = p0[0];
            ref1Found = true;
        }
        if (p1[1] == p[1] && ! (ref1Found && ref2Found)) {
            if (! ref1Found) {
                ref1 = p1[0];
                ref1Found = true;
            } else {
                ref2 = p1[0];
                ref2Found = true;
            }
        }
        if (p2[1] == p[1] && ! (ref1Found && ref2Found)) {
            if (! ref1Found) {
                ref1 = p2[0];
                ref1Found = true;
            } else {
                ref2 = p2[0];
                ref2Found = true;
            }
        }
        if (! (ref1Found && ref2Found)
                && ((p0[1] < p[1] && p1[1] > p[1]) || (p0[1] > p[1] && p1[1] < p[1]))) {
            float t = float(p[1] - p0[1]) / (p1[1] - p0[1]);
            if (t < 0 || t > 1) {

            } else if (! ref1Found) {
                ref1 = int(t * (p1[0] - p0[0]) + p0[0]);
                ref1Found = true;
            } else {
                ref2 = int(t * (p1[0] - p0[0]) + p0[0]);
                ref2Found = true;
            }
        }
        if (! (ref1Found && ref2Found)
                && ((p2[1] < p[1] && p1[1] > p[1]) || (p2[1] > p[1] && p1[1] < p[1]))) {
            float t = float(p[1] - p2[1]) / (p1[1] - p2[1]);
            if (t < 0 || t > 1) {

            } else if (! ref1Found) {
                ref1 = int(t * (p1[0] - p2[0]) + p2[0]);
                ref1Found = true;
            } else {
                ref2 = int(t * (p1[0] - p2[0]) + p2[0]);
                ref2Found = true;
            }
        }
        if (! (ref1Found && ref2Found)
                && ((p2[1] < p[1] && p0[1] > p[1]) || (p2[1] > p[1] && p0[1] < p[1]))) {
            float t = float(p[1] - p0[1]) / (p2[1] - p0[1]);
            if (t < 0 || t > 1) {

            } else if (! ref1Found) {
                ref1 = int(t * (p2[0] - p0[0]) + p0[0]);
                ref1Found = true;
            } else {
                ref2 = int(t * (p2[0] - p0[0]) + p0[0]);
                ref2Found = true;
            }
        }

        if (! ref1Found || ! ref2Found) // Completely outside
            return false;
        else
            return p[0] >= std::min(ref1, ref2) && p[0] <= std::max(ref1, ref2);
    }

    Pixel p0, p1, p2;
};



typedef shared_ptr<ImageWindow> ImageWindowPtr;
ImageWindowPtr globalSourceWindow, globalSourceWindow2, globalResultWindow, globalDifferenceWindow;
GrayscaleImage originalImage, originalImage2;


/*---------------------------------------------------------------------------*
 | Access to the singleton Application instance
 *---------------------------------------------------------------------------*/
MapExplorer & MapExplorer::instance() {
    return dynamic_cast<MapExplorer &>(Application::instance());
}


/*---------------------------------------------------------------------------*
 | Application setup functions
 *---------------------------------------------------------------------------*/
// Get command-line arguments and set up the terrainscape
void MapExplorer::setup(int &argc, char **argv) {
    // Seed the random number generator
    srand(getSystemClocks());

    // Create the selection objects
    _persistentSelection.reset(new MeshSelection());
    _transientSelection.reset(new MeshSelection());
    _persistentSelection->setElementType(MeshSelection::Edges);
    _transientSelection->setElementType(MeshSelection::Edges);

    // See if the user gave us any useful filenames
    string arg, ext;
    while (argc > 1) {
        arg = shift(argc, argv);
        ext = arg.substr(arg.length() - 4);
        if (ext == ".ttl")                  // Load the terrain from here
            loadTerrainLibrary(arg);
        else if (ext == ".map")             // Load the map from here
            loadMap(arg);
        else
            exit(1, "Unrecognized argument \"" + arg + "\"");
    }

    // If something wasn't specifed on the command line, choose a default
    if (! _map)                 createMap();
    if (! _terrainLibrary)      createTerrainLibrary();

    // Create the alignment grid
    _grid.reset(new PlanarGrid());
        grid()->minorTickSpacing = 0.5;

    MapRasterizationPtr mr(new MapRasterization(terrainLibrary()));
    MapRasterization::LOD::IDMap idx(300, 300);
    fill(idx, 1);
//    select(idx, SizeArray(225, 225)) = select(constant<int, 2>(2), SizeArray(225, 225));
//    flood_fill(idx, Pixel(90, 36), InTriangle(Pixel(30,30), Pixel(90, 30), Pixel(120, 240)), constant<int, 2>(1));
//    flood_fill(idx, Pixel(210, 135), InTriangle(Pixel(225,90), Pixel(225, 180), Pixel(180, 150)), constant<int, 2>(1));
    (*mr)[LOD_30m].createFromRaster(idx);

//    TerrainSamplePtr test = terrainLibrary()->terrainType(1)->terrainSample(0);
//    TerrainSample::LOD & tsl = (*test)[LOD_30m];
//    tsl.ensureLoaded();
//    tsl.storeToFile("test30.cache");
//    tsl.loadFromFile("test30.cache");

#if 1
    matchSample = terrainLibrary()->terrainType(1)->terrainSample(0);
    TerrainSamplePtr result = generateTerrain(mr, LOD_810m, LOD_90m);

    WindowPtr hw(new HackWindow(result, mr, LOD_90m, "Generated Terrain"));
    registerWindow(hw);
#else
//    WindowPtr iw(new ImageWindow(idx, "TT Map"));
//    registerWindow(iw);
//    WindowPtr mw(new ImageWindow((*mr)[LOD_30m].boundaryDistances(), "Boundary Distances"));
//    registerWindow(mw);
//    WindowPtr mw2(new ImageWindow((*mr)[LOD_30m].regionIDs(), "Region IDs"));
//    registerWindow(mw2);
//    WindowPtr mm(new ImageWindow((*mr)[LOD_30m].regionMask(0, 5), "Region Mask"));
//    registerWindow(mm);
//    WindowPtr bhf(new ImageWindow(naiveBlend((*mr)[LOD_30m], 50), "Blend"));
//    registerWindow(bhf);
//    WindowPtr w(new TerrainSampleWindow(result, LOD_30m, "Generated Terrain"));
//    registerWindow(w);
#endif

//    Heightfield hf, diff;
//    diff = originalImage - hf;
//    globalOriginalWindow->loadImage(originalImage);
//    globalDifferenceWindow->loadImage(diff);
//    globalResultWindow->loadImage(hf);
}



// XXX Temporary function until the regular rendering architecture is
// completed and we can use the editor again.
void MapExplorer::constructInterface() {

    // Always display the heightfield as an image
/*    globalSourceWindow.reset(new ImageWindow(originalImage, "Original"));
    registerWindow(globalSourceWindow);
    globalResultWindow.reset(new ImageWindow(GrayscaleImage(100, 100), "Derived"));
    registerWindow(globalResultWindow);
    globalDifferenceWindow.reset(new ImageWindow(GrayscaleImage(100, 100), "Difference"));
    registerWindow(globalDifferenceWindow);*/
}


/*---------------------------------------------------------------------------*
 | I/O functions
 *---------------------------------------------------------------------------*/
// I/O operations for map objects
void MapExplorer::createMap() {
    // Make a new map
    setMap(MapPtr(new Map()));
}

void MapExplorer::loadMap(const string &filename) {
    try {
        cerr << "[" << filename << "]: loading map\n";

        // Try to open the file and scream if we fail
        std::ifstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e << "Unable to read map file [" << filename
              << "]: does it exist?";
            throw e;
        }

        // Create a new Map object and initialize it
        MapPtr newMap(new Map());
        newMap->terrainLibrary = _terrainLibrary;
        file >> *newMap;
        setMap(newMap);

        cerr << "[" << filename << "]: loading map complete\n";

    } catch (const StreamException &e) {
        cerr << "[" << filename << "]: " << e << endl;
    }
}

void MapExplorer::storeMap(const string &filename) const {
    try {
        cerr << "[" << filename << "]: storing map\n";

        // Try to open the file and scream if we fail
        std::ofstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e << "Unable to write map file [" << filename
              << "]: check directory/file permissions";
            throw e;
        }

        // Write the map out to the file
        file << *map();

        cerr << "[" << filename << "]: storing map complete\n";

    } catch (const StreamException &e) {
        cerr << "[" << filename << "]: " << e << endl;
    }
}


// I/O operations for terrain libraries
void MapExplorer::createTerrainLibrary() {
    // Create a new TerrainLibrary object
    TerrainLibraryPtr tl(new TerrainLibrary(3));

    // Create some sample terrain types
    tl->terrainType(0)->setName("Empty space");
    tl->terrainType(0)->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
    tl->terrainType(1)->setName("Grassy Knoll");
    tl->terrainType(1)->setColor(Color(0.0f, 0.5f, 0.2f, 1.0f));
    tl->terrainType(2)->setName("Desert");
    tl->terrainType(2)->setColor(Color(0.5f, 0.5f, 0.0f, 1.0f));

    setTerrainLibrary(tl);  // Set it!
}

// Throws inca::io::FileAccessException if the file cannot be opened for reading
// Throws inca::io::FileFormatException if the file is syntactically or semantically invalid
void MapExplorer::loadTerrainLibrary(const string &filename) {
    try {
        cerr << "[" << filename << "]: loading terrain library\n";

        // Try to open the file and scream if we fail
        std::ifstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e << "Unable to read terrain library file [" << filename
              << "]: does it exist?";
            throw e;
        }

        // Create a new TerrainLibrary object and initialize it
        TerrainLibraryPtr newLib(new TerrainLibrary());
        file >> *newLib;
        setTerrainLibrary(newLib);

        cerr << "[" << filename << "]: loading terrain library complete\n";

    } catch (const StreamException &e) {
        cerr << "[" << filename << "]: " << e << endl;
    }
}

// Throws inca::io::FileAccessException if the file cannot be opened for writing
void MapExplorer::storeTerrainLibrary(const string &filename) const {
    try {
        cerr << "[" << filename << "]: storing terrain library\n";

        // Try to open the file and scream if we fail
        std::ofstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e << "Unable to write terrain library file [" << filename
              << "]: check directory/file permissions";
            throw e;
        }

        // Write the library out to the file
        file << *terrainLibrary();

        cerr << "[" << filename << "]: storing terrain library complete\n";
    } catch (const StreamException &e) {
        cerr << "[" << filename << "]: " << e << endl;
    }
}


/*---------------------------------------------------------------------------*
 | Data and UI components
 *---------------------------------------------------------------------------*/
void MapExplorer::setMap(MapPtr m) {
    // Now, we point to this map, not that one
    _map = m;

    // Attach the terrain library, if there is one
    if (_terrainLibrary) _map->terrainLibrary = _terrainLibrary;

    // Now, attach this new map to each of the selections
    _persistentSelection->setMap(map());
    _transientSelection->setMap(map());
}

void MapExplorer::setTerrainLibrary(TerrainLibraryPtr tl) {
    // Now, we point to this library, not that one
    _terrainLibrary = tl;

    // Attach it to the map, if there is one
    if (_map) _map->terrainLibrary = _terrainLibrary;

    // Ensure that the current TT belongs to this library
    if (tl == NULL || tl->size() == 0)
        // Crud. There ARE no TTs
        _currentTerrainType = TerrainTypePtr();
    else {
        // Pick the first TerrainType (if possible), since the zeroth one
        // a special TT intended only for empty space
        IndexType index = (tl->size() >= 2) ? 1 : 0;
        _currentTerrainType = _terrainLibrary->terrainType(index);
    }
}

void MapExplorer::setCurrentTerrainType(TerrainTypePtr tt) {
    // This becomes our selected TT
    _currentTerrainType = tt;
}


// This macro expands to a main() function that instantiates the application
// and launches it
APPLICATION_MAIN(terrainosaurus::MapExplorer);
