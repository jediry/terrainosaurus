/**
 * \file    SimilarityGA.hpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the heightfield-generation genetic algorithm
 *      using the Inca GA framework.
 */

#ifndef TERRAINOSAURUS_GENETICS_SIMILARITY_GA
#define TERRAINOSAURUS_GENETICS_SIMILARITY_GA

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class SimilarityChromosome;
    class SimilarityGene;
    class SimilarityGA;
}

// Import genetic algorithm framework
#include <inca/util/GeneticAlgorithm>

// Import the RegionFitness Measure definition
#include "TerrainChromosome.hpp"


class terrainosaurus::SimilarityGene {
public:
    int      index;
    scalar_t weight;
    scalar_t variance;
    int      magnitude;
};


class terrainosaurus::SimilarityChromosome
    : public inca::Array<SimilarityGene, RegionFitnessMeasure::submeasureCount> {
public:
    // Type declarations
    typedef SimilarityGene Gene;
    
    // Constructor
    explicit SimilarityChromosome();
    
    // Set the values used by the analysis function according to the contents
    // of this chromosome
    void setValues() const;
};


class terrainosaurus::SimilarityGA
        : public inca::GeneticAlgorithm<SimilarityChromosome, scalar_t> {
public:
    explicit SimilarityGA(const TerrainLibrary::LOD & tl);        
};

#endif
