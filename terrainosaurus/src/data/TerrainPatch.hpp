/*
 * File: TerrainPatch.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The TerrainPatch class represents a rectangular piece of terrain that
 *      can be viewed at multiple levels of detail (LODs). The various LODs
 *      are stored using a quadtree of TerrainChunks, most of which will not
 *      be in memory at the same time.
 */

#ifndef TERRAINOSAURUS_TERRAIN_PATCH
#define TERRAINOSAURUS_TERRAIN_PATCH

// Import Inca library configuration
#include <inca/inca-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainPatch;
    
    // Pointer typedefs
    typedef shared_ptr<TerrainPatch>       TerrainPatchPtr;
    typedef shared_ptr<TerrainPatch const> TerrainPatchConstPtr;
};

// Import sub-object definition
#include "TerrainChunk.hpp"


class terrainosaurus::TerrainPatch {
public:
    // Container typedefs
    typedef inca::quadtree<TerrainChunkPtr> ChunkTree;

    // Default constructor
    TerrainPatch();
    
    // Constructor taking a filename describing the terrain patch
    TerrainPatch(const string &file);

protected:
    string filename;    // The filename of describing the terrain chunk
    ChunkTree lodTree;  // The quadtree of terrain chunks
};

#endif