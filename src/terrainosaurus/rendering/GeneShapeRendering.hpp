/*
 * File: GeneShapeRendering.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      FIXME: This class needs a different name. This is sorta just a hack until I figure out the "right" way to do this.
 */

#ifndef TERRAINOSAURUS_RENDERING_GENE_SHAPE
#define TERRAINOSAURUS_RENDERING_GENE_SHAPE

// Import Inca library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class GeneShapeRendering;
};

// Import rendering primitive definitions
#include <inca/rendering.hpp>
#include <inca/rendering/immediate-mode/PrimitiveGrid>

// Import data object definitions
#include <terrainosaurus/data/TerrainSample.hpp>
#include <terrainosaurus/data/MapRasterization.hpp>

// Import container definitions
#include <vector>

class terrainosaurus::GeneShapeRendering
    : public rendering::PrimitiveGrid<Point3D,
                                      Vector3D,
                                      Nothing,
                                      Color> {
public:
    typedef inca::rendering::PrimitiveGrid<Point3D, Vector3D,
                                           Nothing, Color> Superclass;
    typedef inca::rendering::OpenGLRenderer Renderer;

    // Default constructor
    GeneShapeRendering();

    // Initializing constructor
    GeneShapeRendering(const GeneShape & g);

    // Initialize the grid with values from the heightfield
    void load(const GeneShape & g);

    bool toggle(const std::string & feature);

    // Render function call operator
    void operator()(Renderer & renderer) const;

protected:
    std::vector<bool> _features;
};

#endif

