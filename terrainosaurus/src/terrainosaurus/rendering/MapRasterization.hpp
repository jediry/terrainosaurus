/*
 * File: MapRasterization.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_MAP_RASTERIZATION
#define TERRAINOSAURUS_MAP_RASTERIZATION

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapRasterization;

    // Pointer typedefs
    typedef shared_ptr<MapRasterization>       MapRasterizationPtr;
    typedef shared_ptr<MapRasterization const> MapRasterizationConstPtr;
};

// Import the Map class
#include <terrainosaurus/data/Map.hpp>


class terrainosaurus::MapRasterization {
/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    // Default constructor, optionally initializing name
    explicit MapRasterization(MapConstPtr m);

protected:
    MapConstPtr             _map;               // The map we're rasterizing
    TerrainLibraryConstPtr  _terrainLibrary;    // The associated terrain lib.


/*---------------------------------------------------------------------------*
 | (Re)generation of rasterization
 *---------------------------------------------------------------------------*/
public:
    void rasterize();


/*---------------------------------------------------------------------------*
 | TerrainType query functions
 *---------------------------------------------------------------------------*/
public:
    TerrainTypeConstPtr terrainTypeAt(Pixel p, IndexType lod) const;
};

#endif
