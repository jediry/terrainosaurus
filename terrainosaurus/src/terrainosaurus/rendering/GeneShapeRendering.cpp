/*
 * File: GeneShapeRendering.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "GeneShapeRendering.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::raster;

#include <inca/integration/opengl/GL.hpp>

// Features that we support
#define FEATURE_COUNT   4
#define AS_WIREFRAME    0
#define AS_POINTS       1
#define AS_POLYGONS     2
#define WITH_LIGHTING   3


// Functor constructors
GeneShapeRendering::GeneShapeRendering()
        : Superclass(GeneShape::segments, GeneShape::segments),
          _features(FEATURE_COUNT, false) {
    _features[AS_POLYGONS] = true;
    _features[WITH_LIGHTING] = true;
}
GeneShapeRendering::GeneShapeRendering(const GeneShape & g)
        : Superclass(GeneShape::segments, GeneShape::segments),
          _features(FEATURE_COUNT, false) {
    _features[AS_POLYGONS] = true;
    _features[WITH_LIGHTING] = true;
    load(g);
}


// Populate the primitive grid with values from a heightfield
void GeneShapeRendering::load(const GeneShape & g) {
    // Figure out the 
}


// Feature toggle functions
bool GeneShapeRendering::toggle(const std::string & feature) {
    IndexType index;
    if (feature == "Wireframe")
        index = AS_WIREFRAME;
    else if (feature == "Points")
        index = AS_POINTS;
    else if (feature == "Polygons")
        index = AS_POLYGONS;
    else if (feature == "Lighting")
        index = WITH_LIGHTING;
    else
        index = -1;
    _features.at(index) = ! _features.at(index);
    std::cerr << feature << (_features.at(index) ? " on" : " off") << std::endl;
    return _features.at(index);
}


// Rendering functor
void GeneShapeRendering::operator()(GeneShapeRendering::Renderer & renderer) const {
    Renderer::Rasterizer & rasterizer = renderer.rasterizer();

    if (_features[AS_POLYGONS]) {
        if (_features[WITH_LIGHTING]) {
            rasterizer.setLightingEnabled(true);
            GL::glEnable(GL_COLOR_MATERIAL);
        } else {
            rasterizer.setLightingEnabled(false);
        }

        GL::glEnable(GL_POLYGON_OFFSET_FILL);
        rasterizer.setPolygonOffset(1.5f);

        static_cast<Superclass const &>(*this)(renderer, inca::rendering::Solid());
    }

    if (_features[AS_WIREFRAME]) {
        Pixel px;
        bool increasing = true;

        rasterizer.setLightingEnabled(false);
        rasterizer.setLineSmoothingEnabled(true);
        rasterizer.setAlphaBlendingEnabled(true);

        static_cast<Superclass const &>(*this)(renderer, inca::rendering::Wireframe());
    }

    if (_features[AS_POINTS]) {
        rasterizer.setLightingEnabled(false);

        static_cast<Superclass const &>(*this)(renderer, inca::rendering::Vertices());
    }
}
