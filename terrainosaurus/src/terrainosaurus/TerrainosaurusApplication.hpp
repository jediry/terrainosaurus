/**
 * \file    TerrainosaurusApplication.hpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The TerrainosaurusApplication class represents the application itself,
 *      and is responsible for processing command-line arguments and for
 *      creating and coordinating the various windows and other structures
 *      that do the real work.
 */

#ifndef TERRAINOSAURUS_APPLICATION
#define TERRAINOSAURUS_APPLICATION

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the fluid simulation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainosaurusApplication;
};


// Import superclass definition
#include <inca/ui.hpp>

// Import window and data object definitions
#include <terrainosaurus/ui/MapEditorWindow.hpp>
#include <terrainosaurus/ui/GeneticsWindow.hpp>
#include <terrainosaurus/data/TerrainLibrary.hpp>
#include <terrainosaurus/data/Map.hpp>
#include <terrainosaurus/data/MapRasterization.hpp>


// Import container definitions
#include <vector>


// TerrainosaurusApplication is an instance of an Application, using the
// GUI toolkit specified in terrainosaurus-common.h
class terrainosaurus::TerrainosaurusApplication
        : public APPLICATION(GUI_TOOLKIT) {
/*---------------------------------------------------------------------------*
 | Access to the singleton Application instance
 *---------------------------------------------------------------------------*/
public:
    static TerrainosaurusApplication & instance();


/*---------------------------------------------------------------------------*
 | Application setup functions
 *---------------------------------------------------------------------------*/
public:
    // Get command-line arguments and set up the terrainscape
    void setup(int &argc, char **argv);

    // Put together our user interface
    void constructInterface();

protected:
    // Filenames to load, given on the commandline
    std::vector<std::string>    _terrainFilenames,  // .dem heightfield files
                                _libraryFilenames,  // .ttl terrain lib files
                                _mapFilenames;      // .map files


/*---------------------------------------------------------------------------*
 | Window management functions
 *---------------------------------------------------------------------------*/
public:
    MapEditorWindowPtr createMapEditorWindow(const std::string & filename);
    MapEditorWindowPtr createMapEditorWindow(MapPtr map);
    GeneticsWindowPtr  createGeneticsWindow(MapRasterizationPtr mr);

protected:
    // Existing windows
    std::vector<MapEditorWindowPtr> _mapEditorWindows;
    std::vector<GeneticsWindowPtr>  _geneticsWindows;


/*---------------------------------------------------------------------------*
 | Data object management functions
 *---------------------------------------------------------------------------*/
public:
    TerrainLibraryPtr loadTerrainLibrary(const std::string & filename);
    void storeTerrainLibrary(TerrainLibraryConstPtr lib,
                             const std::string & filename);
    MapPtr loadMap(const std::string & filename);
    void storeMap(MapConstPtr map, const std::string & filename);

protected:
    // HACK
    TerrainLibraryPtr _lastTerrainLibrary;
};

#endif
