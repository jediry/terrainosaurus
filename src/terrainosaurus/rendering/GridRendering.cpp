/*
 * File: GridRendering.cpp
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

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "GridRendering.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;

namespace terrainosaurus {
    typedef OpenGLRenderer Renderer;
}
#include <inca/integration/opengl/GL.hpp>

// Features that we support
#define MAJOR_TICKS         0
#define MINOR_TICKS         1
#define FEATURE_COUNT       2

// Scalar values that can be set
#define MAJOR_TICK_WIDTH    0
#define MINOR_TICK_WIDTH    1
#define SCALAR_COUNT        2

// Colors that can be set
#define MAJOR_TICK_COLOR    0
#define MINOR_TICK_COLOR    1
#define COLOR_COUNT         2


// Functor constructors
GridRendering::GridRendering() {
    setDefaults();
}
GridRendering::GridRendering(const PlanarGrid & g) {
    setDefaults();
    load(g);
}


void GridRendering::setDefaults() {
    _features.resize(FEATURE_COUNT);
    _features.at(MAJOR_TICKS) = true;
    _features.at(MINOR_TICKS) = true;

    _colors.resize(COLOR_COUNT);
    _colors.at(MAJOR_TICK_COLOR) = Color(0.8f, 0.8f, 0.8f, 0.5f);
    _colors.at(MINOR_TICK_COLOR) = Color(0.8f, 0.8f, 0.8f, 0.3f);

    _scalars.resize(SCALAR_COUNT);
    _scalars.at(MAJOR_TICK_WIDTH) = 1.5f;
    _scalars.at(MINOR_TICK_WIDTH) = 1.0f;
}

// Acquire a reference to the grid we're rendering
void GridRendering::load(const PlanarGrid & g) {

}


// Feature toggle function
bool GridRendering::toggle(const std::string & feature) {
    IndexType index;
    if (feature == "MajorTicks")
        index = MAJOR_TICKS;
    else if (feature == "MinorTicks")
        index = MINOR_TICKS;
    else
        index = -1;
    _features.at(index) = ! _features.at(index);
    std::cerr << feature << (_features.at(index) ? " on" : " off") << std::endl;
    return _features.at(index);
}


// Scalar set function
void GridRendering::setScalar(const std::string & name, scalar_arg_t s) {
    IndexType index;
    if (name == "MajorTickWidth")
        index = MAJOR_TICK_WIDTH;
    else if (name == "MinorTickWidth")
        index = MINOR_TICK_WIDTH;
    else
        index = -1;
    _scalars.at(index) = s;
    std::cerr << name << " = " << _scalars.at(index) << std::endl;
}


// Color set function
void GridRendering::setColor(const std::string & name, const Color & c) {
    IndexType index;
    if (name == "MajorTickColor")
        index = MAJOR_TICK_COLOR;
    else if (name == "MinorTickColor")
        index = MINOR_TICK_COLOR;
    else
        index = -1;
    _colors.at(index) = c;
    std::cerr << name << " = " << _colors.at(index) << std::endl;
}


// Rendering functor
void GridRendering::operator()(terrainosaurus::Renderer & renderer) const {
    // Render a regular grid over the window
    Point2D ps, pe;
    Vector2D extents = _grid->extents();
    scalar_t halfX = extents[0] / 2.0f,
             halfY = extents[1] / 2.0f;
    scalar_t dx, dy,
             minorTickSpacing = _grid->minorTickSpacing(),
             majorTickSpacing = _grid->majorTickSpacing();

    // Get a reference to things we'll be using repeatedly
    Renderer::Rasterizer & rasterizer = renderer.rasterizer();

    // Render minor grid ticks
    if (minorTickSpacing > 0.0) {
        rasterizer.setLineWidth(_scalars[MINOR_TICK_WIDTH]);
        rasterizer.setCurrentColor(_colors[MINOR_TICK_COLOR]);
        dx = dy = minorTickSpacing;

        rasterizer.beginPrimitive(Lines);

            // Draw horizontal grid lines
            ps[0] = -halfX; pe[0] = halfX;
            for (scalar_t y = 0; y <= halfY; y += dy) {
                ps[1] = pe[1] = y;
                rasterizer.vertexAt(ps);
                rasterizer.vertexAt(pe);
                ps[1] = pe[1] = -y;
                rasterizer.vertexAt(ps);
                rasterizer.vertexAt(pe);
            }

            // Draw vertical grid lines
            ps[1] = -halfY; pe[1] = halfY;
            for (scalar_t x = 0; x <= halfX; x += dx) {
                ps[0] = pe[0] = x;
                rasterizer.vertexAt(ps);
                rasterizer.vertexAt(pe);
                ps[0] = pe[0] = -x;
                rasterizer.vertexAt(ps);
                rasterizer.vertexAt(pe);
            }

        rasterizer.endPrimitive();
    }

    // Render major grid ticks
    if (majorTickSpacing > 0.0) {
        rasterizer.setLineWidth(_scalars[MAJOR_TICK_WIDTH]);
        rasterizer.setCurrentColor(_colors[MAJOR_TICK_COLOR]);
        dx = dy = majorTickSpacing;

        rasterizer.beginPrimitive(Lines);

            // Draw horizontal grid lines
            ps[0] = -halfX; pe[0] = halfX;
            for (scalar_t y = 0; y <= halfY; y += dy) {
                ps[1] = pe[1] = y;
                rasterizer.vertexAt(ps);
                rasterizer.vertexAt(pe);
                ps[1] = pe[1] = -y;
                rasterizer.vertexAt(ps);
                rasterizer.vertexAt(pe);
            }

            // Draw vertical grid lines
            ps[1] = -halfY; pe[1] = halfY;
            for (scalar_t x = 0; x <= halfX; x += dx) {
                ps[0] = pe[0] = x;
                rasterizer.vertexAt(ps);
                rasterizer.vertexAt(pe);
                ps[0] = pe[0] = -x;
                rasterizer.vertexAt(ps);
                rasterizer.vertexAt(pe);
            }

        rasterizer.endPrimitive();
    }
}
