/*
 * File: TerrainosaurusComponent.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The TerrainosaurusComponent class centralizes access to data objects
 *      and global application state (e.g., the current map, terrain library,
 *      selection, terrain type...).
 */

#ifndef TERRAINOSAURUS_COMPONENT
#define TERRAINOSAURUS_COMPONENT

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainosaurusComponent;

    // Pointer typedefs
    typedef shared_ptr<TerrainosaurusComponent>       TerrainosaurusComponentPtr;
    typedef shared_ptr<TerrainosaurusComponent const> TerrainosaurusComponentConstPtr;
};


// Import data object definitions
#include "data/Map.hpp"
#include "data/MeshSelection.hpp"
#include "data/TerrainLibrary.hpp"
#include "PlanarGrid.hpp"

class terrainosaurus::TerrainosaurusComponent {
protected:      // These functions are inherited, not used externally
    // Global data structures
    MapPtr            map() const;                  // The map
    TerrainLibraryPtr terrainLibrary() const;       // Terrain information

    // User interaction objects
    PlanarGridPtr     grid() const;                 // The alignment grid
    MeshSelectionPtr  persistentSelection() const;  // Selected objects
    MeshSelectionPtr  transientSelection() const;   // Mouse-over'd objects

    // Application state
    TerrainTypePtr    currentTerrainType() const;   // Currently selected TT
};

#endif
