/*
 * File: TerrainosaurusComponent.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The TerrainosaurusComponent class contains basic utility functions
 *      frequently used by many of the parts of Terrainosaurus, such as
 *      access to global data objects.
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
    MapPtr map() const;                             // The map
    PlanarGridPtr grid() const;                     // The alignment grid
    MeshSelectionPtr persistentSelection() const;   // Selected objects
    MeshSelectionPtr transientSelection() const;    // Mouse-over'd objects
    TerrainLibraryPtr terrainLibrary() const;       // Terrain information
    TerrainTypePtr currentTerrainType() const;
};

#endif