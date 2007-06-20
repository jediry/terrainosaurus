/*
 * File: TerrainSampleRendering.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_RENDERING_TERRAIN_SAMPLE
#define TERRAINOSAURUS_RENDERING_TERRAIN_SAMPLE

// Import Inca library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainSampleRendering;
};

// HACK! There's got to be a way to preserve renderer independence...
// Import rendering primitive definitions
#include <inca/rendering.hpp>
#include <inca/rendering/immediate-mode/PrimitiveGrid>

// Import data object definitions
#include <terrainosaurus/data/TerrainSample.hpp>
#include <terrainosaurus/data/MapRasterization.hpp>

// Import container definitions
#include <vector>

class terrainosaurus::TerrainSampleRendering
    : public inca::rendering::PrimitiveGrid<Point3D,
                                            Vector3D,
                                            Nothing,
                                            Color> {
/*---------------------------------------------------------------------------*
 | Type declarations
 *---------------------------------------------------------------------------*/
public:
    typedef inca::rendering::PrimitiveGrid<Point3D, Vector3D,
                                           Nothing, Color> Superclass;
    typedef inca::rendering::OpenGLRenderer Renderer;
    typedef Heightfield::ElementType scalar_t;
    typedef Heightfield::Region      Region;


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    // Default constructor
    TerrainSampleRendering();

    // Initializing constructor
    TerrainSampleRendering(const TerrainSample::LOD & tsl);


/*---------------------------------------------------------------------------*
 | Grid (re)construction functions
 *---------------------------------------------------------------------------*/
public:
    // Initialize the grid with values from the a TerrainSample::LOD
    void load(const TerrainSample::LOD & tsl);

protected:
    void _calculateScaleAndOffset(const TerrainSample::LOD & tsl);
    void _rebuildGeometry() { _rebuildGeometry(_elevations.bounds()); }
    void _rebuildColorMap() { _rebuildColorMap(_elevations.bounds()); }
    void _rebuildGeometry(const Region & r);
    void _rebuildColorMap(const Region & r);
    
    Heightfield _elevations;
    VectorMap   _gradients;
    ColorMap    _colors;
    
    bool _geometryDirty,
         _colorMapDirty,
         _setupComplete;

    Heightfield::ElementType _xAxisOffset, _xAxisScale,
                             _yAxisOffset, _yAxisScale,
                             _zAxisOffset, _zAxisScale;


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    // Rendering switches
    bool toggle(const std::string & feature);

    // Render function call operator
    void operator()(Renderer & renderer) const;

protected:
    std::vector<bool> _features;
    mutable std::vector<bool> _displayListValid;
    mutable int _displayListBase;
};

#endif
