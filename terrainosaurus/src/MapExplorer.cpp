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

// Import terrain-specialized renderer
#include "TerrainosaurusRenderer.hpp"

// Import widget definitions
#include "ui/MapExplorerWidget.hpp"
#include "ui/CameraNavigationWidget.hpp"
#include "ui/MapEditWidget.hpp"
#include "ui/AddFaceWidget.hpp"
#include "ui/AddVertexWidget.hpp"
#include "ui/ModifyPropertyWidget.hpp"
#include "ui/TranslateWidget.hpp"
#include "ui/RotateWidget.hpp"
#include "ui/ScaleWidget.hpp"


// Import IOstream operators for data objects
#include "io/terrainosaurus-iostream.hpp"

// Import file-I/O exception definitions
#include <inca/io/FileExceptions.hpp>

using namespace terrainosaurus;
using namespace inca::world;
using namespace inca::ui;
using namespace inca::io;


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
}


// Put together our user interface
void MapExplorer::constructInterface() {
    ///////////////////////////////////////////////////////////////////////////
    // Step 1: construct shared global objects
    OrthographicCameraPtr camera2D(new OrthographicCamera());
        camera2D->viewWidth = 15.0;
        camera2D->viewHeight = 15.0;
        camera2D->nearClip = -1.0;
        camera2D->transform->position = Transform::Point(0.0, 0.0, 100.0);
    TerrainosaurusRendererPtr renderer(new TerrainosaurusRenderer());


    ///////////////////////////////////////////////////////////////////////////
    // Step 2: build the widget stack

    // The MapExplorerWidget handles application events, draws the UI and
    // chooses between major application modes (editing, walk-thru, etc.)
    MapExplorerWidgetPtr explorer(new MapExplorerWidget("Map Explorer"));

    // The 2D map editing mode widget stack
    CameraNavigationWidgetPtr navigate2D(new CameraNavigationWidget("2D Navigation"));
    MultiplexorWidgetPtr toolSelect2D(new MultiplexorWidget("2D Tool Select"));
    MapEditWidgetPtr modifyProperty2D(new ModifyPropertyWidget("Modify Property"));
    MapEditWidgetPtr addFace2D (new AddFaceWidget("Add Face"));
    MapEditWidgetPtr addVertex2D (new AddVertexWidget("Add Vertex"));
    MapEditWidgetPtr translate2D (new TranslateWidget("Translate"));
    MapEditWidgetPtr rotate2D    (new RotateWidget("Rotate"));
    MapEditWidgetPtr scale2D     (new ScaleWidget("Scale"));
        explorer->addWidget(navigate2D);
        explorer->toolSelect2D = toolSelect2D;
            navigate2D->widget = toolSelect2D;
            navigate2D->camera = camera2D;  // Control the 2D camera
            navigate2D->enableLook = false;     // Disable capabilities
            navigate2D->enableDolly = false;    // that don't make sense
            navigate2D->enableYaw = false;      // in two dimensions
            navigate2D->enablePitch = false;
            navigate2D->rollScale = 3.1415962 / 150.0;  // Tweak the controls
            navigate2D->zoomScale = 1.001;
                toolSelect2D->addWidget(modifyProperty2D);
                toolSelect2D->addWidget(addFace2D);
                toolSelect2D->addWidget(addVertex2D);
 //               toolSelect2D->addWidget(translate2D);
 //               toolSelect2D->addWidget(rotate2D);
 //               toolSelect2D->addWidget(scale2D);

    // Start with the draw tool
 //   toolSelect2D->selectWidget("Map Topology");

    // XXX Pass the renderer to everyone. This is ugly
    explorer->renderer = renderer;
    navigate2D->renderer = renderer;
    modifyProperty2D->renderer = renderer;
    addFace2D->renderer = renderer;
    addVertex2D->renderer = renderer;
    translate2D->renderer = renderer;
    rotate2D->renderer = renderer;
    scale2D->renderer = renderer;

    // Now, make a window to hold it all
    window = createWindow(explorer, WINDOW_TITLE);
    window->setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (FULL_SCREEN)    window->setFullScreen(true);
    else                window->centerOnScreen();
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
        ifstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e.os() << "Unable to read map file [" << filename
                << "]: does it exist?";
            throw e;
        }

        // Create a new Map object and initialize it
        MapPtr newMap(new Map());
        newMap->terrainLibrary = _terrainLibrary;
        file >> *newMap;
        setMap(newMap);

        cerr << "[" << filename << "]: loading map complete\n";

    } catch (const stream_exception &e) {
        cerr << "[" << filename << "]: " << e.message() << endl;
    }
}

void MapExplorer::storeMap(const string &filename) const {
    try {
        cerr << "[" << filename << "]: storing map\n";

        // Try to open the file and scream if we fail
        std::ofstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e.os() << "Unable to write map file [" << filename
                << "]: check directory/file permissions";
            throw e;
        }

        // Write the map out to the file
        file << *map();

        cerr << "[" << filename << "]: storing map complete\n";

    } catch (const stream_exception &e) {
        cerr << "[" << filename << "]: " << e.message() << endl;
    }
}

// I/O operations for terrain libraries
void MapExplorer::createTerrainLibrary() {
    // Create a new TerrainLibrary object
    TerrainLibraryPtr tl(new TerrainLibrary(3));

    // Create some sample terrain types
    tl->terrainType(0)->name = "Empty space";
    tl->terrainType(0)->color = Color(0.0f, 0.0f, 0.0f, 1.0f);
    tl->terrainType(1)->name = "Grassy Knoll";
    tl->terrainType(1)->color = Color(0.0f, 0.5f, 0.2f, 1.0f);
    tl->terrainType(2)->name = "Desert";
    tl->terrainType(2)->color = Color(0.5f, 0.5f, 0.0f, 1.0f);

    setTerrainLibrary(tl);  // Set it!
}

// Throws inca::io::FileAccessException if the file cannot be opened for reading
// Throws inca::io::FileFormatException if the file is syntactically or semantically invalid
void MapExplorer::loadTerrainLibrary(const string &filename) {
    try {
        cerr << "[" << filename << "]: loading terrain library\n";

        // Try to open the file and scream if we fail
        ifstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e.os() << "Unable to read terrain library file [" << filename
                << "]: does it exist?";
            throw e;
        }
        
        // Create a new TerrainLibrary object and initialize it
        TerrainLibraryPtr newLib(new TerrainLibrary());
        file >> *newLib;
        setTerrainLibrary(newLib);

        cerr << "[" << filename << "]: loading terrain library complete\n";

    } catch (const stream_exception &e) {
        cerr << "[" << filename << "]: " << e.message() << endl;
    }
}

// Throws inca::io::FileAccessException if the file cannot be opened for writing
void MapExplorer::storeTerrainLibrary(const string &filename) const {
    try {
        cerr << "[" << filename << "]: storing terrain library\n";

        // Try to open the file and scream if we fail
        ofstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e.os() << "Unable to write terrain library file [" << filename
                << "]: check directory/file permissions";
            throw e;
        }

        // Write the library out to the file
        file << *terrainLibrary();

        cerr << "[" << filename << "]: storing terrain library complete\n";
    } catch (const stream_exception &e) {
        cerr << "[" << filename << "]: " << e.message() << endl;
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
        index_t index = (tl->size() >= 2) ? 1 : 0;
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
