/*
 * File: TerrainChunk.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The TerrainChunk class represents a single level of detail (LOD) for
 *      a piece of terrain.
 */

#ifndef TERRAINOSAURUS_TERRAIN_CHUNK
#define TERRAINOSAURUS_TERRAIN_CHUNK

// Import Inca library configuration
#include <inca/inca-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainChunk;
    
    // Pointer typedefs
    typedef shared_ptr<TerrainChunk>       TerrainChunkPtr;
    typedef shared_ptr<TerrainChunk const> TerrainChunkConstPtr;
};

// Import the bounding volume definition
#include "QuadPrism"


class terrainosaurus::TerrainChunk {
public:
    // Type definitions
    typedef inca::quadtree<TerrainChunkPtr>     ChunkTree;
    typedef inca::math::QuadPrism<double, 3>    QuadPrism;

    // Default constructor
    TerrainChunk();
    
    // Constructor taking a filename describing the terrain patch
    TerrainChunk(const string &file);

    // Accessor functions
    index_t level() const { return _level; }
    const QuadPrism & boundingVolume() const { return _boundingVolume; }
    double worldSpaceError() const { return _worldSpaceError; }
 
protected:
    size_t _level;              // What level in the LOD tree are we?
    float _worldSpaceError;     // The maximum world-space error in the HF
//    TriangleStrip triangles;    // The triangles visible at this LOD
    QuadPrism _boundingVolume;  // A bounding, axis-aligned, rectangular shape
};

#endif