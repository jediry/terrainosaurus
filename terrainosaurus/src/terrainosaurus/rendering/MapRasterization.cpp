/*
 * File: MapRasterization.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "MapRasterization.hpp"
using namespace terrainosaurus;


// Constructor
MapRasterization::MapRasterization(MapConstPtr m)
    : _map(m), _terrainLibrary(m->terrainLibrary()) { }


void MapRasterization::rasterize() {

}


TerrainTypeConstPtr
MapRasterization::terrainTypeAt(Pixel p, IndexType lod) const {
    return _terrainLibrary->terrainType(1);
} 
