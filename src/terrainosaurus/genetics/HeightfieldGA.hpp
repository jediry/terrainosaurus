/** -*- C++ -*-
 *
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

// Import container and utility definitions
#include <vector>
#include <inca/util/Timer>

// Import genetic algorithm framework
#include <inca/util/GeneticAlgorithm>

// Import Chromosome definition
#include "TerrainChromosome.hpp"


class terrainosaurus::HeightfieldGA
        : public inca::GeneticAlgorithm<TerrainChromosome, float> {
public:
    typedef inca::Timer<float, false>   Timer;
    typedef std::vector<Timer>          TimerArray;
    typedef inca::GeneticAlgorithm<TerrainChromosome, float>    Superclass;


    // Constructor
    explicit HeightfieldGA(TerrainSamplePtr t = TerrainSamplePtr(),
                           TerrainSamplePtr p = TerrainSamplePtr());

    // Destructor
    ~HeightfieldGA();

    // The TerrainSample holding the terrain that we're building
    TerrainSamplePtr      terrainSample();
    TerrainSampleConstPtr terrainSample() const;
    void setTerrainSample(TerrainSamplePtr ts);

    // The TerrainSample holding that we're building
    TerrainSamplePtr      patternSample();
    TerrainSampleConstPtr patternSample() const;
    void setPatternSample(TerrainSamplePtr ts);

    // Current LOD accessor. Since the algorithm runs from coarse to fine,
    // only LODs coarser than this are valid in the TerrainSample
    TerrainLOD currentLOD() const;

    // Whether the GA is running
    bool running() const;

    // Run the GA, and return the generated TS
    void run(TerrainLOD targetLOD);
    void run(TerrainLOD startLOD, TerrainLOD targetLOD);

    // Change the initialization PMF based on the LOD
    const PMF & initializationOperatorPMF(const Chromosome & c) const;

    // Change the mutation PMF based on gene compatibility
    const PMF & mutationOperatorPMF(const Gene & g) const;
    
    // Modified fitness calculation function to cache fitness results in Chromosome
    Scalar calculateFitness(Chromosome & c);

    // XXX -- misc test function
    void test(TerrainLOD lod);
    TerrainSamplePtr redo(TerrainSamplePtr ts, TerrainLOD lod);

protected:
    TerrainSamplePtr    _patternSample;
    TerrainSamplePtr    _terrainSample;
    bool        _running;           // Whether we're currently doing anything
    TerrainLOD  _currentLOD;        // The LOD we're currently working on
    Timer       _totalTime,         // Time spent on the whole generation process
                _loadingTime;       // Time spent pre-loading the terrain library
    TimerArray  _lodTimes,          // Time spent on each LOD
                _setupTimes,        // Time spent setting up for the GA, per LOD
                _processingTimes;   // Time spent running the GA, per LOD
};

#endif
