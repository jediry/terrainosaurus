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
 
// HACK: This could be better integrated w/ HF rendering, for an increase in
// performance (regen less of the grid), not to mention less code

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "ChromosomeRendering.hpp"

// Import genetics functions
#include <terrainosaurus/genetics/terrain-operations.hpp>
using namespace terrainosaurus;

// XXX
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
#define NORMALS         3

// Constants
#define EFFECT_EXPIRE_TIMESTEPS 10


// Default constructor
ChromosomeRendering::ChromosomeRendering()
        : _chromosome(NULL) { }

// Initializing constructor
ChromosomeRendering::ChromosomeRendering(TerrainChromosome & c)
        : _chromosome(NULL) {
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
        TerrainSample::LOD & ss = c.scratch();
        renderChromosome(ss, c);
        load(ss);
    }
}


// Rendering functor
void ChromosomeRendering::operator()(ChromosomeRendering::Renderer & renderer) const {
    if (! this->_chromosome)    // Nothing to do...
        return;

    // Render the HF
    TerrainSampleRendering::operator()(renderer);
}
