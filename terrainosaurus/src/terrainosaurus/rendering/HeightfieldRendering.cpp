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
//    ts.ensureAnalyzed();
//    std::cerr << "Loading " << ts.levelOfDetail() << " with sizes " << ts.sizes()
//              << " (" << ts.globalElevationStatistics().count() << " points " << std::endl;
//    std::cerr << "Elevation range is " << ts.globalElevationStatistics().min()
//              << " -> " << ts.globalElevationStatistics().max()
//              << " and mean is " << ts.globalElevationStatistics().mean() << std::endl;
//    Heightfield::ElementType meanHeight = ts.globalElevationStatistics().mean();
    VectorMap grad = gradient(ts.elevations(), metersPerSampleForLOD(ts.levelOfDetail()));
    Heightfield::ElementType meanHeight = mean(ts.elevations());
    Heightfield::ElementType horizontalScale = metersPerSampleForLOD(ts.levelOfDetail());
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
            p.color() = map.terrainType(px).color();
        }
    std::cerr << nanCount << " NaNs and " << goodCount << " good values\n";
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
    std::cerr << feature << (_features.at(index) ? " on" : " off") << std::endl;
    return _features.at(index);
}


// Rendering functor
void HeightfieldRendering::operator()(HeightfieldRendering::Renderer & renderer) const {
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