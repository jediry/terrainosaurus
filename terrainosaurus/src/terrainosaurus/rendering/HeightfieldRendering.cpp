/*
 * File: HeightfieldRendering.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *
 * Implementation:
 *      The storage order for primitives in the grid is a rather odd arrangement
 *      optimized for triangle strip rendering:
 *
 *      |
 *      | 11 13 15 17 19
 *      | 10 12 14 16 18
 *    Y |  1  3  5  7  9
 *      |  0  2  4  6  8
 *      |________________
 *               X
 *
 *      Notice every other pair of rows forms a contiguous triangle strip. Of
 *      course, non-contiguous strips require a little more index wizardry.
 */

// Import class definition
#include "HeightfieldRendering.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;

namespace terrainosaurus {
    typedef OpenGLRenderer Renderer;
}
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
}
HeightfieldRendering::HeightfieldRendering(const TerrainSample::LOD & ts,
                                           const MapRasterization::LOD & map)
        : _features(FEATURE_COUNT, false) {
    _features[AS_POLYGONS] = true;
    load(ts, map);
}


// Populate the primitive grid with values from a heightfield
void HeightfieldRendering::load(const TerrainSample::LOD & ts,
                                const MapRasterization::LOD & map) {
    // Make sure this is the right size
    this->resize(ts.sizes());
    ts.ensureAnalyzed();
    std::cerr << "Loading " << ts.levelOfDetail() << " with sizes " << ts.sizes()
              << " (" << ts.globalElevationStatistics().count() << " points " << std::endl;
    std::cerr << "Elevation range is " << ts.globalElevationStatistics().min()
              << " -> " << ts.globalElevationStatistics().max()
              << " and mean is " << ts.globalElevationStatistics().mean() << std::endl;
    Heightfield::ElementType meanHeight = ts.globalElevationStatistics().mean();
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
            Vector2D gradient2D = ts.gradient(px);
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
void HeightfieldRendering::operator()(terrainosaurus::Renderer & renderer) const {
//void HeightfieldRendering::operator()(int) const {
    Renderer::Rasterizer & rasterizer = renderer.rasterizer();

    rasterizer.setPrimitiveArray(*this);

    if (_features[AS_POLYGONS]) {
        if (_features[WITH_LIGHTING])
            rasterizer.setLightingEnabled(true);
        else
            rasterizer.setLightingEnabled(false);
        GL::glEnable(GL_COLOR_MATERIAL);

        // Render the grid as a huge triangle strip, two rows at a time
        int vertexCount = size(0) << 1;     // sizeX * 2 vertices per strip
        int pairedRows = (size(1) >> 1) << 1;   // Strip off any odd-numbered last row
        for (IndexType y = 0; y < size(1) - 1; y += 2) {
            // The first row is stored left-to-right in contiguous memory
            IndexType startVertex = y * size(0);
            rasterizer.renderPrimitive(TriangleStrip, startVertex, vertexCount);

            // The second row is made by jumping between vertices from two other
            // strips, but should only be done if we have data there
            if (y < size(1) - 2) {
                IndexType bottomVertex = startVertex + 1;
                IndexType topVertex    = startVertex + vertexCount;
                rasterizer.beginPrimitive(TriangleStrip);
                for (IndexType x = 0; x < size(0); ++x) {
                    rasterizer.vertexIndex(bottomVertex);
                    rasterizer.vertexIndex(topVertex);
                    bottomVertex += 2;
                    topVertex += 2;
                }
                rasterizer.endPrimitive();
    //            cerr << "last indices were " << (bottomVertex - 1) << " and " << (topVertex - 1) << std::endl;
            }
        }
    }

    if (_features[AS_WIREFRAME]) {
        Pixel px;
        bool increasing = true;

        rasterizer.setLightingEnabled(false);

        rasterizer.beginPrimitive(LineStrip);
        for (px[1] = 0; px[1] < size(1); ++px[1]) {
            if (increasing) {
                for (px[0] = 0; px[0] < size(0); ++px[0])
                    rasterizer.vertexIndex(indexOf(px));
            } else {
                for (px[0] = size(0) - 1; px[0] >= 0; --px[0])
                    rasterizer.vertexIndex(indexOf(px));
            }
            increasing = ! increasing;
        }
        rasterizer.endPrimitive();

        rasterizer.beginPrimitive(LineStrip);
        for (px[0] = 0; px[0] < size(0); ++px[0]) {
            if (increasing) {
                for (px[1] = 0; px[1] < size(1); ++px[1])
                    rasterizer.vertexIndex(indexOf(px));
            } else {
                for (px[1] = size(1) - 1; px[1] >= 0; --px[1])
                    rasterizer.vertexIndex(indexOf(px));
            }
            increasing = ! increasing;
        }
        rasterizer.endPrimitive();
    }

    if (_features[AS_POINTS]) {
        rasterizer.setLightingEnabled(false);

        if (! this->isOverallocated())
            // We can do most of this in one fell swoop
            rasterizer.renderPrimitive(Points, 0, this->size());
        else {
            // We have to do it in two parts

            // First, all but the last row
            rasterizer.renderPrimitive(Points, 0, this->size() - this->size(0));

            // Now do the last row separately
            rasterizer.beginPrimitive(Points);
            Pixel px;
            px[1] = size(1) - 1;
            for (px[0] = 0; px[0] < size(0); ++px[0])
                rasterizer.vertexIndex(indexOf(px));
            rasterizer.endPrimitive();
        }
    }
}
