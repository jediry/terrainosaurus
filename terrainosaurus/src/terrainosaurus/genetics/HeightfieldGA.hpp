/**
 * \file    HeightfieldGA.hpp
 *
 * \author  Ryan L. Saunders & Mike Ong
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the heightfield-generation genetic algorithm
 *      using the Inca GA framework.
 */

#ifndef TERRAINOSAURUS_GENETICS_HEIGHTFIELD_GA
#define TERRAINOSAURUS_GENETICS_HEIGHTFIELD_GA

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class HeightfieldGA;
}


class terrainosaurus::HeightfieldGA
        : public inca::GeneticAlgorithm<TerrainChromosome, float> {
public:
    // Constructor
    explicit HeightfieldGA();

    // Destructor
    ~HeightfieldGA();


};

#endif
