/*
 * File: ChromosomeRendering.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "ChromosomeRendering.hpp"

// Import genetics functions
#include <terrainosaurus/genetics/terrain-operations.hpp>
using namespace terrainosaurus;

// XXX
#define GL_HPP_IMPORT_GLUT 1
#include <inca/integration/opengl/GL.hpp>

// Import raster processing functions
#include <inca/raster/algorithms/fill>
#include <inca/raster/operators/gradient>
#include <inca/raster/operators/statistic>
#include <inca/raster/operators/select>
#include <inca/raster/operators/arithmetic>
using namespace inca::raster;
using namespace inca::rendering;


// Features that we support
#define FEATURE_COUNT   4
#define AS_WIREFRAME    0
#define AS_POINTS       1
#define AS_POLYGONS     2
#define WITH_LIGHTING   3

// Constants
#define EFFECT_EXPIRE_TIMESTEPS 10


// Default constructor
ChromosomeRendering::ChromosomeRendering()
        : _chromosome(NULL), _features(FEATURE_COUNT, false),
          _heightfieldDirty(false), _colormapDirty(false),
          _timestep(0) {
    _features[AS_POLYGONS] = true;
    _features[WITH_LIGHTING] = true;
}

// Initializing constructor
ChromosomeRendering::ChromosomeRendering(TerrainChromosome & c)
        : _chromosome(NULL), _features(FEATURE_COUNT, false),
          _heightfieldDirty(false), _colormapDirty(false),
          _timestep(0) {
    _features[AS_POLYGONS] = true;
    _features[WITH_LIGHTING] = true;
    setChromosome(c);
}


// Chromosome accessor functions
TerrainChromosome & ChromosomeRendering::chromosome() {
    return *_chromosome;
}
const TerrainChromosome & ChromosomeRendering::chromosome() const {
    return *_chromosome;
}
void ChromosomeRendering::setChromosome(TerrainChromosome & c) {
    _chromosome = &c;

    if (_chromosome) {
        const MapRasterization::LOD & m = chromosome().map();

        // Resize the heightfield to the dimensions of the new chromosome
        _renderHF.resize(m.sizes());

        // Precalculate the X,Y coordinates for each vertex
        Heightfield::ElementType gridScale =
                metersPerSampleForLOD(chromosome().levelOfDetail());
        Pixel px, idx;
        for (px[1] = m.base(1), idx[1] = 0; px[1] <= m.extent(1); ++px[1], ++idx[1])
            for (px[0] = m.base(0), idx[0] = 0; px[0] <= m.extent(0); ++px[0], ++idx[0])
                _renderHF.vertex(idx) = Point3D(
                        (px[0] - m.size(0) / 2) * gridScale,
                        (px[1] - m.size(1) / 2) * gridScale,
                        Heightfield::ElementType(0));

        // Resize and initialize the state-tracking grids
        _events.setSizes(chromosome().sizes());
        _selections.setSizes(chromosome().sizes());

        fill(_events, GAEvent());
        fill(_selections, false);

        for (px[1] = _events.base(1); px[1] <= _events.extent(1); px[1] += 2)
            for (px[0] = _events.base(0); px[0] <= _events.extent(0); px[0] += 2)
                _events(px) = GAEvent(GAEvent::Mutation, 0);
        _heightfieldDirty = true;
        _colormapDirty = true;

    } else {
        // Deallocate all raster & primitive memory
        _renderHF.resize(0, 0);
        _events.setSizes(0, 0);
        _selections.setSizes(0, 0);
    }
}


// GA listener functions
void ChromosomeRendering::advanced(int timestep) {
    _timestep = timestep;
    _colormapDirty = true;
}
void ChromosomeRendering::mutated(const TerrainChromosome::Gene & g, GAEvent e) {
    _events(g.indices()) = e;
    _heightfieldDirty = true;
    _colormapDirty = true;
}
void ChromosomeRendering::crossed(const TerrainChromosome::Gene & g, GAEvent e) {
    _events(g.indices()) = e;
    _heightfieldDirty = true;
    _colormapDirty = true;
}


bool ChromosomeRendering::toggle(const std::string & feature) {
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
void ChromosomeRendering::operator()(ChromosomeRendering::Renderer & renderer) const {
    if (! this->_chromosome)    // Nothing to do...
        return;

    // Ensure the heightfield and colormap are current
    if (_heightfieldDirty)
        _rebuildHeightfield();
    if (_colormapDirty)
        _rebuildColormap();


    Renderer::Rasterizer & rasterizer = renderer.rasterizer();

    if (_features[AS_POLYGONS]) {
        if (_features[WITH_LIGHTING]) {
            rasterizer.setLightingEnabled(true);
            GL::glEnable(GL_COLOR_MATERIAL);
        } else {
            rasterizer.setLightingEnabled(true);
        }

        GL::glEnable(GL_POLYGON_OFFSET_FILL);
        rasterizer.setPolygonOffset(1.5f);

        _renderHF(renderer, Solid());
    }

    if (_features[AS_WIREFRAME]) {
        rasterizer.setLightingEnabled(false);
        rasterizer.setLineSmoothingEnabled(true);
        rasterizer.setAlphaBlendingEnabled(true);

        _renderHF(renderer, Wireframe());
    }

    if (_features[AS_POINTS]) {
        rasterizer.setLightingEnabled(false);

        _renderHF(renderer, Vertices());
    }
}


// Recalculate the geometry of the heightfield (elevation & gradient)
void ChromosomeRendering::_rebuildHeightfield() const {
    INCA_DEBUG("Rebuilding heightfield")

    // Calculate elevations and gradient of the heightfield from the chromosome
    renderChromosome(_heightfield, chromosome());
    _normals = gradient(_heightfield);

    // Calculate how to transform it to the right relative proportions and
    // height for viewing
    const MapRasterization::LOD & m = chromosome().map();
    Heightfield::ElementType meanHeight = mean(_heightfield);
    Pixel px, idx;
    int nanCount = 0, goodCount = 0;
    for (px[1] = m.base(1), idx[1] = 0; px[1] <= m.extent(1); ++px[1], ++idx[1])
        for (px[0] = m.base(0), idx[0] = 0; px[0] <= m.extent(0); ++px[0], ++idx[0]) {
            scalar_t h = _heightfield(px);
            Vector2D n = _normals(px);
            if (std::isnan(h))      nanCount++;
            else                    goodCount++;
            _renderHF.vertex(idx)[2] = h - meanHeight;
            _renderHF.normal(idx) = normalize(Vector3D(n[0], n[1], scalar_t(1)));
        }
    std::cerr << nanCount << " NaNs and " << goodCount << " good values\n";
    std::cerr << "Mean height is " << meanHeight << endl;

    // All is well
    _heightfieldDirty = false;
}


// Recalculate the geometry of the heightfield (elevation & gradient)
void ChromosomeRendering::_rebuildColormap() const {
    INCA_DEBUG("Recoloring heightfield");

    const TerrainChromosome & c = chromosome();
    const MapRasterization::LOD & m = c.map();
    int winSize = windowSize(c.levelOfDetail());

    GrayscaleImage mask = sphericalMask(c.levelOfDetail());
//    mask /= mask((winSize - 1) / 2, (winSize - 1) / 2);

    _colormap.setBounds(m.bounds());

    // Initially, color the terrain according to the terrain types
    Pixel px;
    for (px[1] = m.base(1); px[1] <= m.extent(1); ++px[1])
        for (px[0] = m.base(0); px[0] <= m.extent(0); ++px[0])
            _colormap(px) = m.terrainType(px).color();

    // Blend in the mutations and crossovers
    Pixel idx;
    for (idx[1] = 0; idx[1] < c.size(1); ++idx[1])
        for (idx[0] = 0; idx[0] < c.size(0); ++idx[0]) {
            const TerrainChromosome::Gene & g = c(idx);
            GAEvent e = _events(idx);
            break; // XXX HACK

            // Ignore non-affected genes
            if (e.type == GAEvent::None) {

            // Clear expired events
            } else if (_timestep - e.timestep > EFFECT_EXPIRE_TIMESTEPS) {
                _events(idx) = GAEvent();

            // Blend in the effects of still-recent operations
            } else {
                // XXX Replace with ReturnType-augmented raster ops
                // Also need channel operators...maybe some sort of bind/unbind syntax...
                Color hilightColor;
                switch (e.type) {
                    case GAEvent::Mutation:
                        hilightColor = Color(1.0f, 0.2f, 0.2f, 1.0f);
                        break;
                    case GAEvent::Crossover:
                        hilightColor = Color(0.2f, 0.2f, 1.0f, 1.0f);
                        break;
                }
                ColorImage effect = select(constant<Color, 2>(hilightColor),
                                           mask.sizes());
                for (px[1] = 0; px[1] < mask.size(1); ++px[1])
                    for (px[0] = 0; px[0] < mask.size(1); ++px[0])
                        effect(px)[3] = mask(px);
                selectCS(_colormap, g.targetCenter(), mask.sizes()) %= effect;
            }
        }

    // Copy the colors into the primitive grid
    for (px[1] = _colormap.base(1), idx[1] = 0; px[1] <= _colormap.extent(1); ++px[1], ++idx[1])
        for (px[0] = _colormap.base(0), idx[0] = 0; px[0] <= _colormap.extent(0); ++px[0], ++idx[0])
            _renderHF.color(idx) = _colormap(px);

    // All is well
    _colormapDirty = false;
}
