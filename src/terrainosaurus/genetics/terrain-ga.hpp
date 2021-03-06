/*
 * File: terrain-ga.hpp
 *
 * Author: Ryan L. Saunders & Mike Ong
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file declares the functions that make up the 3D terrain
 *      generation genetic algorithm. Auxillary helper functions can
 *      be found in terrain-operations.hpp.
 *
 *      The evaluateFitness(...) function examines a chromosome to determine
 *      its fitness according to a variety of metrics, and returns an overall
 *      fitness value as a floating-point number between 0 and 1 (inclusive).
 *
 *      The evaluateCompatibility(...) function examines a pair of genes to
 *      determine how well they match up (i.e., could one replace the other
 *      without damaging the terrain?). This is used initially to populate
 *      a higher-LOD chromosome from a lower-LOD chromosome, and also within
 *      an iteration of the GA to do mutation and crossover of chromosome
 *      data.
 */

#ifndef TERRAINOSAURUS_GENETICS_TERRAIN_GA
#define TERRAINOSAURUS_GENETICS_TERRAIN_GA

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// Import container definitions
#include <vector>

// Import statistical object definitions
#include <inca/math/statistics/Statistics>
#include <inca/math/statistics/ProbabilityMass>

// Import Map, TerrainLibrary, and terrain GA data structures
#include <terrainosaurus/data/Map.hpp>
#include <terrainosaurus/data/TerrainLibrary.hpp>
#include "TerrainChromosome.hpp"

// Genetic algorithm functions for building a heightfield
namespace terrainosaurus {
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


    // Declare types for the population of TerrainChromosomes and their fitness
    // statistics. These make the later function declarations shorter and
    // more readable.
    typedef std::vector<TerrainChromosome>                  Population;
    typedef inca::math::Statistics<scalar_t, true>          PopulationStatistics;
    typedef inca::math::ProbabilityMass<scalar_t>           PMF;
    typedef std::vector< std::pair<IndexType, scalar_t> >   FitnessMap;


    // Generate a single LOD by applying a GA to create a high-resolution
    // heightfield similar to the lower-resolution pattern. This is the
    // 'main' function that controlls the GA.
    Heightfield refineHeightfield(const TerrainSample::LOD & pattern,
                                  const MapRasterization::LOD & map);


/*---------------------------------------------------------------------------*
 | Initialization functions
 *---------------------------------------------------------------------------*/
    // For every slot in 'p' that is not already occupied by a living
    // chromosome, create a new chromosome there. At the first evolution
    // cycle, this initializes the whole population. In later cycles it only
    // replaces those chromosomes that were killed off in the previous cycle.
    void replenishPopulation(Population & p,
                             const TerrainSample::LOD & pattern,
                             const MapRasterization::LOD & map);

    // Select a fraction of the population to keep for the next evolution
    // cycle and "kill off" all the others (they will be replaced with new
    // chromosomes at the beginning of the next cycle). The 'selectionRatio'
    // parameter specifies the proportion of the population that will be kept,
    // chosen at random. The 'eliteRatio' parameter specifies the proportion
    // of "best" individuals who will be kept.
    //
    // Note that 'selectionRatio' includes 'eliteRatio'. For example, with
    //      selectRatio = 0.75 and
    //      eliteRatio  = 0.2
    // only 75% of the population will be kept, not 95%. If 'eliteRatio'
    // is larger than 'selectRatio', then 'selectRatio' will be ignored and
    // only the top 'eliteRatio' fraction will be kept.
    //
    // Finally, the function returns the index of the most fit individual
    void prunePopulation(Population & p, float selectRatio, float eliteRatio);

    // Fill a Chromosome by finding a set of Genes that are compatible with
    // the data in 'pattern'.
    void createChromosome(TerrainChromosome & c,
                          const TerrainSample::LOD & pattern,
                          const MapRasterization::LOD & map);

    // Initialize a Chromosome to accurately represent 'sample'. That is, each
    // gene points to the location it would write back to, with no
    // transformation applied.
    void createChromosome(TerrainChromosome & c,
                          const TerrainSample::LOD & sample);

//     // Create a random Gene by chosing a random location within a terrain
//     // sample of the given terrain type and LOD
//     TerrainChromosome::Gene createRandomGene(const TerrainType::LOD & tt);
//
//     // Create a gene at a particular point in a particular sample
//     TerrainChromosome::Gene createGene(const TerrainSample::LOD & ts, const Pixel & p);


/*---------------------------------------------------------------------------*
 | Fitness calculation functions
 *---------------------------------------------------------------------------*/

    // Calculate the fitness for each individual in the population, then
    // calculate the normalized fitness for each individual (such
    // that the whole population's normalized fitness sums to 1.0).
    // It returns the index of the most fit chromosome in the population.
    IndexType calculateFitness(Population & population);

    // Calculate the fitness measure for an entire chromosome.
    void calculateFitness(TerrainChromosome & c);

    // Calculate the fitness measure for a single region
    void calculateRegionFitness(TerrainChromosome & c, IDType regionID);

    // Calculate the compatibility measure for a single gene
    void calculateCompatibility(TerrainChromosome::Gene & g);

    // Create a vector of index/fitness pairs. This is useful for running
    // sorting algorithms based on fitness, since the sorting can be done
    // on this vector, without copying the chromosomes themselves.
    FitnessMap getFitnessMap(const Population & population);

    // Create a probability mass function (valid over [0,1]) from the
    // overall fitnesses of the chromosomes in the population. This PMF
    // may be used to randomly pick chromosomes, giving a probabilistic
    // preference to the more fit chromosomes.
    PMF getFitnessPMF(const Population & population);

    // Calculate a value in [0, 1] representing the compatibility of a Gene
    // with a slot in a Chromosome
    scalar_t evaluateCompatibility(const TerrainChromosome & c,
                                   IndexType i, IndexType j,
                                   const TerrainChromosome::Gene & g);


/*---------------------------------------------------------------------------*
 | Crossover and mutation functions
 *---------------------------------------------------------------------------*/

    // Exchange approximately 'ratio' percent of total genetic material
    // between two chromosomes.
    void cross(TerrainChromosome & c1,TerrainChromosome & c2, float ratio);

    // Alter approximately 'ratio' percent of the genes in a single
    // chromosome. The mutation that occurs at each gene is random, with
    // a probabilistic preference for those operations that would improve the
    // gene's compatibility with its surrounding environment.
    void mutate(TerrainChromosome & c, float ratio);

    // Effectively replace the gene with a new one of the specified TerrainType,
    // with a randomized transformation.
    void randomize(TerrainChromosome::Gene & g, const TerrainType::LOD & tt);

    // Make a gene's source and target coordinates identical, with no
    // transformation. This can be used to construct a chromosome that exactly
    // represents a heightfield.
    void reset(TerrainChromosome::Gene & g);

    // Offset the gene vertically from its current mean value
    void offset(TerrainChromosome::Gene & g, scalar_t min, scalar_t max);

    // Scale the gene vertically around its mean value
    void scale(TerrainChromosome::Gene & g, scalar_t min, scalar_t max);

    // Rotate the gene around its center
    void rotate(TerrainChromosome::Gene & g, scalar_t min, scalar_t max);

    // Translate the location of the source coordinates of the gene
    void translate(TerrainChromosome::Gene & g, const Offset & min,
                                                const Offset & max);

    // Jitter the location of the target coordinates of the gene
    void jitter(TerrainChromosome::Gene & g, const Offset & min,
                                             const Offset & max);

    // Swirl the gene around its center
//    void swirl(TerrainChromosome::Gene & g);
};

#endif
