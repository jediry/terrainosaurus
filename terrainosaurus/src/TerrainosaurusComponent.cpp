/*
 * File: TerrainosaurusComponent.cpp
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

// Import class configuration
#include "TerrainosaurusComponent.hpp"

// Import Application class definition
#include "MapExplorer.hpp"
using namespace terrainosaurus;

// Access to the globally-shared map object
MapPtr TerrainosaurusComponent::map() const {
    return MapExplorer::instance().map();
}

// Access to the grid used to align stuff
PlanarGridPtr TerrainosaurusComponent::grid() const {
    return MapExplorer::instance().grid();
}

// Access to the set of selected map parts
MeshSelectionPtr TerrainosaurusComponent::persistentSelection() const {
    return MapExplorer::instance().persistentSelection();
}

// Access to the set of mouse-over'd or lasso-selected map parts
MeshSelectionPtr TerrainosaurusComponent::transientSelection() const {
    return MapExplorer::instance().transientSelection();
}

// Access to the repository of terrain type information
TerrainLibraryPtr TerrainosaurusComponent::terrainLibrary() const {
    return MapExplorer::instance().terrainLibrary();
}

// Access to the currently-selected TerrainType record
TerrainTypePtr TerrainosaurusComponent::currentTerrainType() const {
    return MapExplorer::instance().currentTerrainType();
}
