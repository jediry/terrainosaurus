/*
 * File: MapExplorer.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */
 
#ifndef TERRAINOSAURUS_MAP_EXPLORER
#define TERRAINOSAURUS_MAP_EXPLORER

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the fluid simulation engine
namespace terrainosaurus {
    // Forward declarations
    class MapExplorer;
    
    // Pointer typedefs
    typedef shared_ptr<MapExplorer>       MapExplorerPtr;
    typedef shared_ptr<MapExplorer const> MapExplorerConstPtr;
};


// Import superclass definition
#include <inca/ui.hpp>

// Import data object definitions
#include "TerrainosaurusComponent.hpp"


// MapExplorer is an instance of an Application, using the specified toolkit
class terrainosaurus::MapExplorer : public APPLICATION(GUI_TOOLKIT) {
/*---------------------------------------------------------------------------*
 | Access to the singleton Application instance
 *---------------------------------------------------------------------------*/
public:
    static MapExplorer & instance();


/*---------------------------------------------------------------------------*
 | Application setup functions
 *---------------------------------------------------------------------------*/
public:
    // Get command-line arguments and set up the terrainscape
    void setup(int & argc, char ** argv);

    // Put together our user interface
    void constructInterface();


/*---------------------------------------------------------------------------*
 | I/O functions
 *---------------------------------------------------------------------------*/
public:
    // Simple (non I/O) empty-object creation functions
    void createMap();
    void createTerrainLibrary();

    // Load-from-file functions
    //      throws inca::io::FileAccessException if the file cannot be read
    //      throws inca::io::FileFormatException if a parsing error occurs
    void loadMap(const std::string & filename);
    void storeMap(const std::string & filename) const;

    // Store-to-file functions
    //      throws inca::io::FileAccessException if the file cannot be written
    void loadTerrainLibrary(const std::string & filename);
    void storeTerrainLibrary(const std::string & filename) const;


/*---------------------------------------------------------------------------*
 | Data and UI components
 *---------------------------------------------------------------------------*/
public:
    // Global data structures
    MapPtr map() const { return _map; }
    TerrainLibraryPtr terrainLibrary() const { return _terrainLibrary; }

    // User interaction objects
    PlanarGridPtr grid() const { return _grid; }
    MeshSelectionPtr persistentSelection() const { return _persistentSelection; }
    MeshSelectionPtr transientSelection() const { return _transientSelection; }

    // Application state
    TerrainTypePtr currentTerrainType() const { return _currentTerrainType; }
    void setCurrentTerrainType(TerrainTypePtr tt);

protected:
    // Global data objects (may only be set internally)
    void setMap(MapPtr m);
    void setTerrainLibrary(TerrainLibraryPtr tl);

    // Global data objects
    MapPtr              _map;
    TerrainLibraryPtr   _terrainLibrary;

    // UI objects and properties
    PlanarGridPtr       _grid;
    MeshSelectionPtr    _persistentSelection, _transientSelection;
    TerrainTypePtr      _currentTerrainType;
    Block               _processingBlock;
    Dimension           _resolution;

    // UI components
    inca::ui::WindowPtr window;
};

#endif
