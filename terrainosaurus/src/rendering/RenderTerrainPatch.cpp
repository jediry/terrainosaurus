/*
 * File: RenderTerrainPatch.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "terrainosaurus-rendering_functors.hpp"
using namespace inca::rendering;
using namespace terrainosaurus;


void RenderTerrainPatch::operator()(RendererPtr renderer,
                                    CameraConstPtr camera) {

    // Transform into the TerrainPatch's local coordinate space
    renderer->pushTransform(patch->transform);

        // For now, we'll just render a quadrilateral
        renderer->beginRenderImmediate(Quads);
            renderer->renderVertex(Point2D(0.0, 0.0));
            renderer->renderVertex(Point2D(1.0, 0.0));
            renderer->renderVertex(Point2D(1.0, 1.0));
            renderer->renderVertex(Point2D(0.0, 1.0));
        renderer->endRenderImmediate();

    // Step back out
    renderer->popTransform();
}
