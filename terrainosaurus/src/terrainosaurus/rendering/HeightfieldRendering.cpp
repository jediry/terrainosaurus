/*
 * File: HeightfieldRendering.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "HeightfieldRendering.hpp"
#include <inca/raster/operators/statistic>
#include <inca/raster/operators/gradient>
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
HeightfieldRendering::HeightfieldRendering() : _features(FEATURE_COUNT, false) {
    _features[AS_POLYGONS] = true;
    _features[WITH_LIGHTING] = true;
}
HeightfieldRendering::HeightfieldRendering(const TerrainSample::LOD & ts,
                                           const MapRasterization::LOD & map)
        : _features(FEATURE_COUNT, false) {
    _features[AS_POLYGONS] = true;
    _features[WITH_LIGHTING] = true;
    load(ts, map);
}


// Populate the primitive grid with values from a heightfield
void HeightfieldRendering::load(const TerrainSample::LOD & ts,
                                const MapRasterization::LOD & map) {
    // Make sure this is the right size
    this->resize(ts.sizes());

    // Figure out the mean height and the horizontal resolution
    Heightfield::ElementType meanHeight, horizontalScale;
    horizontalScale = metersPerSampleForLOD(ts.levelOfDetail());
    if (ts.analyzed())  meanHeight = ts.globalElevationStatistics().mean();
    else                meanHeight = mean(ts.elevations());

    VectorMap grad = gradient(ts.elevations(), Vector3D(horizontalScale));

    Pixel px;
    int nanCount = 0, goodCount = 0;
    for (px[1] = ts.base(1); px[1] <= ts.extent(1); ++px[1])
        for (px[0] = ts.base(0); px[0] <= ts.extent(0); ++px[0]) {
            if (std::isnan(ts.elevation(px)))
                nanCount++;
            else
                goodCount++;
            Primitive p = this->primitive(px[0] - ts.base(0),
                                          px[1] - ts.base(1));
            p.vertex() = Point3D((px[0] - ts.size(0) / 2) * horizontalScale,
                                 (px[1] - ts.size(1) / 2) * horizontalScale,
                                 ts.elevation(px) - meanHeight);
            Vector2D gradient2D = grad(px);
            p.normal() = normalize(Vector3D(gradient2D[0], gradient2D[1], scalar_t(1)));
//            p.normal() = Vector3D(scalar_t(0), scalar_t(0), scalar_t(1));
            p.color() = map.terrainType(px).color();
        }
    INCA_DEBUG(nanCount << " NaNs and " << goodCount << " good values")
}


// Feature toggle functions
bool HeightfieldRendering::toggle(const std::string & feature) {
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
    INCA_INFO(feature << (_features.at(index) ? " on" : " off"))
    return _features.at(index);
}


// Rendering functor
void HeightfieldRendering::operator()(HeightfieldRendering::Renderer & renderer) const {
    Renderer::Rasterizer & rasterizer = renderer.rasterizer();

    if (_features[AS_POLYGONS]) {
        if (_features[WITH_LIGHTING]) {
            // Enable lighting and shading
//            Renderer::LightingUnit & light0 = renderer.lightingUnit(0);
//            light0.setPosition(Point3D(0.0f, 0.0f, 200.0f));
//            light0.setDiffuseColor(Color(0.6f, 0.6f, 0.6f, 1.0f));
//            light0.setEnabled(true);

//            Renderer::LightingUnit & light1 = renderer.lightingUnit(1);
//            light1.setPosition(Point3D(1000.0f, 0.0f, 1000.0f));
//            light1.setDiffuseColor(Color(0.2f, 0.2f, 0.2f, 1.0f));
//            light1.setEnabled(true);

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
