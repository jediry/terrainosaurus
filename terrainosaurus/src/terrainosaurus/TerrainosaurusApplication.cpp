/**
 * \file    TerrainosaurusApplication.cpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the member functions belonging to the
 *      TerrainosaurusApplication class, defined in
 *      TerrainosaurusApplication.hpp.
 */

// Import class definition
#include "TerrainosaurusApplication.hpp"
using namespace terrainosaurus;


// Import I/O class definitions
#include <fstream>
#include <terrainosaurus/io/terrainosaurus-iostream.hpp>
#include <inca/io/FileExceptions.hpp>
using namespace inca::io;


/*---------------------------------------------------------------------------*
 | Access to the singleton Application instance
 *---------------------------------------------------------------------------*/
TerrainosaurusApplication & TerrainosaurusApplication::instance() {
    return dynamic_cast<TerrainosaurusApplication &>(Application::instance());
}


/*---------------------------------------------------------------------------*
 | Application setup functions
 *---------------------------------------------------------------------------*/
void TerrainosaurusApplication::setup(int &argc, char **argv) {
    // Seed the random number generator
    srand(getSystemClocks());

    // See if the user gave us any useful filenames
    string arg, ext;
    while (argc > 1) {
        arg = shift(argc, argv);
        ext = arg.substr(arg.length() - 4);
        if (ext == ".dem")          _terrainFilenames.push_back(arg);
        else if (ext == ".dem")     _libraryFilenames.push_back(arg);
        else if (ext == ".map")     _mapFilenames.push_back(arg);
        else
            exit(1, "Unrecognized argument \"" + arg + "\"");
    }


    // HACK If something wasn't specifed on the command-line, choose a default
//    if (_mapFilenames.size() == 0)
//        _mapFilenames.push_back("/home/jediry/Documents/Development/Media/terrainosaurus/data/test.map");
    if (_libraryFilenames.size() == 0)
        _libraryFilenames.push_back("/home/jediry/Documents/Development/Media/terrainosaurus/data/test.ttl");
}

// Put together our user interface
void TerrainosaurusApplication::constructInterface() {
    // Keep track of how many windows we create here
    int windowCount = 0;

    // Load each of the terrain libraries on the command-line
    for (IndexType i = 0; i < _libraryFilenames.size(); ++i)
        try {
            loadTerrainLibrary(_libraryFilenames[i]);
        } catch (inca::StreamException & e) {
            INCA_ERROR("[" << _libraryFilenames[i] << "]: " << e)
        }

    // Create windows for each map on the command-line
    for (IndexType i = 0; i < _mapFilenames.size(); ++i)
        try {
            createMapEditorWindow(_mapFilenames[i]);
            ++windowCount;
        } catch (inca::StreamException & e) {
            INCA_ERROR("[" << _mapFilenames[i] << "]: " << e)
        }

    // Create windows for each terrain on the command-line
    for (IndexType i = 0; i < _terrainFilenames.size(); ++i)
        try {
//            createMapEditorWindow(_terrainFilenames[i]);
            ++windowCount;
        } catch (inca::StreamException & e) {
            INCA_ERROR("[" << _terrainFilenames[i] << "]: " << e)
        }

    // XXX
    MapRasterization::LOD::IDMap idx(inca::Array<int, 2>(300, 300));
    fill(idx, 1);
    MapRasterizationPtr mr(new MapRasterization(idx, LOD_30m, _lastTerrainLibrary));
    createGeneticsWindow(mr);
    ++windowCount;

    // Make sure we did something with all this effort
    if (windowCount == 0)
        exit(1, "No windows created...exiting");
}


/*---------------------------------------------------------------------------*
 | Window management functions
 *---------------------------------------------------------------------------*/
MapEditorWindowPtr
TerrainosaurusApplication::createMapEditorWindow(const std::string & filename) {
    MapPtr map = loadMap(filename);
    return createMapEditorWindow(map);
}
MapEditorWindowPtr
TerrainosaurusApplication::createMapEditorWindow(MapPtr map) {
    MapEditorWindowPtr win(new MapEditorWindow(map));
    registerWindow(win);
    return win;
}


GeneticsWindowPtr
TerrainosaurusApplication::createGeneticsWindow(MapRasterizationPtr mr) {
    GeneticsWindowPtr win(new GeneticsWindow(mr));
    registerWindow(win);
    return win;
}


MapPtr TerrainosaurusApplication::loadMap(const std::string & filename) {
    INCA_INFO("[" << filename << "]: loading map")

    // Try to open the file and scream if we fail
    std::ifstream file(filename.c_str());
    if (! file) {
        FileAccessException e(filename);
        e << "Unable to read map file [" << filename
            << "]: does it exist?";
        throw e;
    }

    // Create a new Map object and initialize it
    MapPtr map(new Map());
    map->terrainLibrary = _lastTerrainLibrary;
    file >> *map;
    file.close();

    return map;
}


void TerrainosaurusApplication::storeMap(MapConstPtr map,
                                         const std::string & filename) {
    INCA_INFO("[" << filename << "]: storing map")

    // Try to open the file and scream if we fail
    std::ofstream file(filename.c_str());
    if (! file) {
        FileAccessException e(filename);
        e << "Unable to write map file [" << filename
          << "]: check directory/file permissions";
        throw e;
    }

    // Write the map out to the file
    file << *map;
    file.close();
}


// Throws inca::io::FileAccessException if the file cannot be opened for reading
// Throws inca::io::FileFormatException if the file is syntactically or semantically invalid
TerrainLibraryPtr
TerrainosaurusApplication::loadTerrainLibrary(const std::string & filename) {
    INCA_INFO("[" << filename << "]: loading terrain library")

    // Try to open the file and scream if we fail
    std::ifstream file(filename.c_str());
    if (! file) {
        FileAccessException e(filename);
        e << "Unable to read terrain library file [" << filename
          << "]: does it exist?";
        throw e;
    }

    // Create a new TerrainLibrary object and initialize it
    TerrainLibraryPtr lib(new TerrainLibrary());
    file >> *lib;
    file.close();

    // HACK
    _lastTerrainLibrary = lib;

    return lib;
}

// Throws inca::io::FileAccessException if the file cannot be opened for writing
void TerrainosaurusApplication::storeTerrainLibrary(TerrainLibraryConstPtr lib,
                                                    const std::string & filename) {
    INCA_INFO("[" << filename << "]: storing terrain library")

    // Try to open the file and scream if we fail
    std::ofstream file(filename.c_str());
    if (! file) {
        FileAccessException e(filename);
        e << "Unable to write terrain library file [" << filename
            << "]: check directory/file permissions";
        throw e;
    }

    // Write the library out to the file
    file << *lib;
    file.close();
}

// This macro expands to a main() function that instantiates the application
// and launches it
APPLICATION_MAIN(terrainosaurus::TerrainosaurusApplication);
