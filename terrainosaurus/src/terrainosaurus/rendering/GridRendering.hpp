/*
 * File: GridRendering.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      FIXME: This class needs a different name. This is sorta just a hack until I figure out the "right" way to do this.
 */

#ifndef TERRAINOSAURUS_RENDERING_GRID
#define TERRAINOSAURUS_RENDERING_GRID

// Import Inca library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class GridRendering;
};

// FIXME!!!
#include <inca/rendering.hpp>
// Renderer type
typedef inca::rendering::OpenGLRenderer Renderer;

// Import data object definitions
#include <terrainosaurus/PlanarGrid.hpp>

// Import container definitions
class terrainosaurus::GridRendering {
public:
    // Constructors
    GridRendering();
    GridRendering(const PlanarGrid & g);

    void load(const PlanarGrid & g);

    // Set/toggle features and properties
    void setDefaults();
    bool toggle(const std::string & feature);
    void setScalar(const std::string & name, scalar_arg_t s);
    void setColor(const std::string & name, const Color & c);

    // Render function call operator
    void operator()(Renderer & renderer) const;

protected:
    PlanarGrid *            _grid;
    std::vector<bool>       _features;
    std::vector<Color>      _colors;
    std::vector<scalar_t>   _scalars;
};

#endif
