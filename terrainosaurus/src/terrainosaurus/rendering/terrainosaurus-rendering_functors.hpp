/*
 * File: terrainosaurus-rendering_functors.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file defines the function objects for rendering Terrainosaurus
 *      objects using the Inca rendering subsystem.
 *
 *      The RenderTerrainPatch class renders a TerrainPatch at the appropriate
 *      level of detail.
 */

#ifndef TERRAINOSAURUS_RENDERING_FUNCTORS
#define TERRAINOSAURUS_RENDERING_FUNCTORS

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class RenderTerrainPatch;
};


// Import superclass and object definitions
//#include <inca/rendering/RenderFunctor??>
#include "../data/TerrainPatch.hpp"


class terrainosaurus::RenderTerrainPatch {
public:
    // Constructor
    explicit RenderTerrainPatch(TerrainPatchConstPtr tp) : _patch(tp) { }

    // Rendering function
    void operator()(RendererConstPtr renderer, CameraConstPtr camera);

protected:
    TerrainPatchConstPtr _patch;
};

#endif