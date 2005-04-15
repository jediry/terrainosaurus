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

    // GA global parameters, which may be set from the TTL file
    extern int      POPULATION_SIZE;
    extern int      EVOLUTION_CYCLES;
    extern scalar_t SELECTION_RATIO;
    extern scalar_t ELITE_RATIO;
    extern scalar_t MUTATION_PROBABILITY;
    extern scalar_t MUTATION_RATIO;
    extern scalar_t CROSSOVER_PROBABILITY;
    extern scalar_t CROSSOVER_RATIO;

    // Crossover/mutation operator limits
    extern int      MAX_CROSSOVER_WIDTH;
    extern int      MAX_JITTER_PIXELS;
    extern scalar_t MAX_SCALE_FACTOR;
    extern scalar_t MAX_OFFSET_AMOUNT;
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
    explicit HeightfieldGA(MapRasterizationPtr mr = MapRasterizationPtr(),
                           TerrainSamplePtr ts = TerrainSamplePtr());

    // Destructor
    ~HeightfieldGA();


    // The MapRasterization that we're trying to follow
    MapRasterizationPtr      mapRasterization();
    MapRasterizationConstPtr mapRasterization() const;
    void setMapRasterization(MapRasterizationPtr mr);

    // The TerrainSample that we're building
    TerrainSamplePtr      terrainSample();
    TerrainSampleConstPtr terrainSample() const;
    void setTerrainSample(TerrainSamplePtr ts);

    // Current LOD accessor. Since the algorithm runs from coarse to fine,
    // only LODs coarser than this are valid in the TerrainSample
    TerrainLOD currentLOD() const;

    // Whether the GA is running
    bool running() const;

    // Run the GA, and return the generated TS
    TerrainSamplePtr run(MapRasterizationPtr mr,
                         TerrainLOD startLOD, TerrainLOD targetLOD);
    TerrainSamplePtr run(TerrainLOD startLOD, TerrainLOD targetLOD);

    // Change the PMF based on gene compatibility
    const PMF & mutationOperatorPMF(const Gene & g) const;

protected:
    MapRasterizationPtr _mapRasterization;
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
