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

// Import superclass definition
#include "TerrainSampleRendering.hpp"

// Import GA classes and functions
#include <terrainosaurus/genetics/TerrainChromosome.hpp>
#include <inca/util/GeneticAlgorithm>

// Import container definitions
#include <vector>


class terrainosaurus::ChromosomeRendering : public TerrainSampleRendering {
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
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    // Render function call operator
    void operator()(Renderer & renderer) const;
};

#endif
