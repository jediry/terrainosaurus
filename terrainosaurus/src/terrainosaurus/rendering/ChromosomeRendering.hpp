/*
 * File: ChromosomeRendering.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      FIXME: This class needs a different name. This is sorta just a hack until I figure out the "right" way to do this.
 */

#ifndef TERRAINOSAURUS_RENDERING_CHROMOSOME
#define TERRAINOSAURUS_RENDERING_CHROMOSOME

// Import Inca library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class ChromosomeRendering;
};

// Import rendering primitive definitions
#include <inca/rendering.hpp>
#include <inca/rendering/immediate-mode/PrimitiveGrid>

// Import GA classes and functions
#include <terrainosaurus/genetics/TerrainChromosome.hpp>
#include <terrainosaurus/genetics/terrain-ga.hpp>

// Import container definitions
#include <vector>


class terrainosaurus::ChromosomeRendering : public GAListener {
public:
    typedef inca::rendering::OpenGLRenderer Renderer;
    typedef inca::rendering::PrimitiveGrid<Point3D,
                                           Vector3D,
                                           Nothing,
                                           Color>       HeightfieldRendering;
    typedef inca::raster::MultiArrayRaster<GAEvent, 2>  EventGrid;
    typedef inca::raster::MultiArrayRaster<bool, 2>     SelectionGrid;


/*---------------------------------------------------------------------------*
 | Constructors & data accessors
 *---------------------------------------------------------------------------*/
public:
    // Default constructor
    ChromosomeRendering();

    // Initializing constructor
    ChromosomeRendering(TerrainChromosome & tc);

    // Chromosome accessor functions
          TerrainChromosome & chromosome();
    const TerrainChromosome & chromosome() const;
    void setChromosome(TerrainChromosome & tc);

protected:
    TerrainChromosome * _chromosome;


/*---------------------------------------------------------------------------*
 | GA and selection listener functions
 *---------------------------------------------------------------------------*/
public:
    void advanced(int timestep);
    void crossed(const TerrainChromosome::Gene & g, GAEvent e);
    void mutated(const TerrainChromosome::Gene & g, GAEvent e);

    void focused(const TerrainChromosome::Gene & g);
    void selected(const TerrainChromosome::Gene & g);

protected:
    mutable EventGrid       _events;
    mutable SelectionGrid   _selections;
    mutable int             _timestep;

/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    // Rendering feature toggles
    bool toggle(const std::string & feature);

    // Render function call operator
    void operator()(Renderer & renderer) const;

protected:
    mutable Heightfield             _heightfield;   // The raster heightfield
    mutable VectorMap               _normals;       // Normal vectors at each point
    mutable ColorImage              _colormap;      // Colors at each point

    mutable HeightfieldRendering    _renderHF;  // The mesh for the heightfield
    std::vector<bool>               _features;  // Rendering option toggles

    // Rendering helper functions
    void _rebuildHeightfield() const;
    void _rebuildColormap() const;

    mutable bool _heightfieldDirty;
    mutable bool _colormapDirty;

//    HeightfieldRendering renderGene;
//    SelectionSet<int> activeGenes, consideredGenes;
};

#endif

