/** -*- C++ -*-
 *
 * \file    BoundaryGA.hpp
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
    struct BoundaryGene;
    class BoundaryChromosome;
    class BoundaryGA;
}

// Import genetic algorithm framework
#include <inca/util/GeneticAlgorithm>


// The Gene type
struct terrainosaurus::BoundaryGene {
    scalar_t relativeAngle;     // Relative angle between segments
    scalar_t absoluteAngle;     // Absolute angle to X axis
};


// The Chromosome type
class terrainosaurus::BoundaryChromosome : public std::vector<BoundaryGene> {
public:
    // The required Gene typedef
    typedef BoundaryGene Gene;
};


class terrainosaurus::BoundaryGA
        : public inca::GeneticAlgorithm<BoundaryChromosome, float> {
public:
    typedef inca::GeneticAlgorithm<BoundaryChromosome, float>    Superclass;

    // Constructor
    explicit BoundaryGA();
    
    // Additional properties...
    scalar_t maxAbsoluteAngle() const;
    void setMaxAbsoluteAngle(scalar_arg_t m);
    scalar_t smoothness() const;
    void setSmoothness(scalar_arg_t s);
    int segmentCount() const;
    void setSegmentCount(int c);
    
    // Post-processing operation, ensuring the max absolute angle constraint
    // is upheld. By default, it enforces this constraint across the whole
    // chromosome, but it can skip part of the chromosome if that prefix is
    // known to be valid.
    void enforceMaxAbsoluteAngle(Chromosome & c, IndexType start = 0) const;
    
    // Run the GA
    void run(int segments, Scalar smoothness);
    
protected:
    scalar_t _maxAbsoluteAngle;
    scalar_t _smoothness;
    int      _segmentCount;
};

#endif
