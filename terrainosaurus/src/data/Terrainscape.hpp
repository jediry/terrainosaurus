/*
 * File: Terrainscape.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The Terrainscape class is the top-level terrain object. It comprises
 *      a rectangular grid of TerrainPatches and coordinates the updating and
 *      rendering of individual patches, as well as the management of which
 *      TerrainChunks for each TerrainPatch are resident in memory.
 */

#ifndef TERRAINOSAURUS_TERRAINSCAPE
#define TERRAINOSAURUS_TERRAINSCAPE

// Import Inca library configuration
#include <inca/inca-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class Terrainscape;
    
    // Pointer typedefs
    typedef shared_ptr<Terrainscape>       TerrainscapePtr;
    typedef shared_ptr<Terrainscape const> TerrainscapeConstPtr;
};

// Import superclass definition
#include <inca/world.hpp>


class terrainosaurus::Terrainscape : public inca::world::Object {
public:
    // Recalculate which LODs should be in-memory
    void update(inca::world::CameraPtr camera, double time);

    const TerrainChunk & operator()(int col, int row) const;

protected:
    TerrainPatchGrid patches;   // The rectangular grid of terrain patches
    
};

#endif