/*
 * File: terrain-ga.cpp
 *
 * Author: Ryan L. Saunders & Mike Ong
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */


// Import function prototypes and class definitions
#include "terrain-ga.hpp"
#include "terrain-operations.hpp"

// Import random number generator
#include <inca/math/generator/RandomUniform>

// Import STL algorithms for working on vectors
#include <algorithm>
#include <functional>

using namespace inca::math;
using namespace terrainosaurus;
typedef TerrainChromosome::Gene             Gene;
typedef TerrainFitnessMeasure               FitnessMeasure;
//typedef TerrainChromosome::FitnessMeasure   FitnessMeasure;

// Some fixed parameters for the GA
#define POPULATION_SIZE     5
#define EVOLUTION_CYCLES    5
#define MAX_CROSSOVER_WIDTH 4


// This function runs the entire GA for a particular level of detail
Heightfield terrainosaurus::refineHeightfield(const TerrainSample::LOD & pattern,
                                              const MapRasterization::LOD & map) {
    std::cout << "GA heightfield refinement for "
              << map.levelOfDetail() << std::endl;


    // Step 1: generate an initial population of the specified size
    Population population(POPULATION_SIZE);


    // Step 2: run the GA for a certain number of evolution cycles
    scalar_t mutationProbability  = 0.1f;   // Odds of being mutated
    scalar_t crossoverProbability = 0.2f;   // Odds of being crossed
    scalar_t mutationRatio  = 0.5f;         // % of genes to mutate
    scalar_t crossoverRatio = 0.5f;         // % of genes to cross
    scalar_t selectionRatio = 0.75f;        // % of chromosomes to keep
    scalar_t eliteRatio     = 0.2f;         // % of 'best' chromosomes to keep
    RandomUniform<scalar_t>  randomFraction(0, 1);
    RandomUniform<IndexType> randomChromosome(0, population.size() - 1);
    IndexType mostFit;
    for (int cycle = 0; cycle < EVOLUTION_CYCLES; ++cycle) {
        // Step 1:
        // Create any new chromosomes we need to fill out our population.
        // Initially, this means creating a full complement of chromosomes,
        // but in later cycles, we only replace chromosomes that were
        // "marked for death" at the end of the last cycle.
        initializePopulation(population, pattern, map);

        // Cross genetic material between chromosomes in the population
        if (crossoverProbability > 0) {
            for (IndexType i = 0; i < population.size(); ++i)
                if (randomFraction() < crossoverProbability)
                    cross(population[i],
                          population[randomChromosome()],
                          crossoverRatio);
        }

        // Mutate genes within the chromosomes themselves
        if (mutationProbability > 0) {
            for (IndexType i = 0; i < population.size(); ++i)
                if (randomFraction() < crossoverProbability)
                    mutate(population[i], mutationRatio);
        }

        // Analyze the results of this evolution cycle. The fitness measure
        // is calculated for each chromosome. 'selectionRatio' percent of the
        // chromosomes will be kept for the next cycle (with 'eliteRatio'
        // percent being the highest fitness chromosomes). The other chromosomes
        // will be marked to be recycled during the next cycle. Finally, it
        // returns the index of the most-fit chromosome.
        mostFit = analyzePopulation(population, selectionRatio, eliteRatio);
    }

    std::cout << "Done executing GA" << std::endl;
    std::cout << "Fitness of the strongest chromosome is "
              << population[mostFit].fitness().overall() << std::endl;
    Heightfield hf;
    renderChromosome(hf, population[mostFit]);

    return hf;
}


// This function creates the starting population for the GA
void terrainosaurus::initializePopulation(Population & population,
                                          const TerrainSample::LOD & pattern,
                                          const MapRasterization::LOD & map) {
    population.resize(POPULATION_SIZE);
    for (int i = 0; i < population.size(); ++i)
        if (! population[i].isAlive())
            initializeChromosome(population[i], pattern, map);
}


// This function makes a chromosome from a heightfield
void terrainosaurus::initializeChromosome(TerrainChromosome & c,
                                          const TerrainSample::LOD & pattern,
                                          const MapRasterization::LOD & map) {
    // Make this chromosome alive
    c.setAlive(true);

    // Point the Chromosome to its pattern LOD
    c.setPattern(pattern);
    c.setMap(map);

    // Figure out how many genes we want in each direction
    Dimension hfSize(pattern.sizes());
    SizeType spacing = geneSpacing(c.levelOfDetail());
    Dimension size = hfSize / spacing;
    if (hfSize[0] % spacing != 0) size[0]++;    // Round up # of genes
    if (hfSize[1] % spacing != 0) size[1]++;
    c.resize(size);

    // Populate the gene grid with random data
    Pixel idx;
    for (idx[0] = 0; idx[0] < c.size(0); ++idx[0])
        for (idx[1] = 0; idx[1] < c.size(1); ++idx[1])
//            c.gene(idx) = createGene(pattern, idx * spacing);
            c.gene(idx) = createRandomGene(map.terrainType(idx));

    cerr << "Initialized chromosome with " << c.size(0) << "x" << c.size(1) << " genes\n";
}


// Select a fraction of the population to keep for the next evolution
// cycle and replace all the others with new indiviuals.
IndexType terrainosaurus::analyzePopulation(Population & population,
                                            float selectRatio,
                                            float eliteRatio) {
    // Calculate the fitness of every chromosome in the population
    calculateFitness(population);

    // Figure out how many individuals we're talking about here
    int numberOfElites = int(population.size() * eliteRatio  + 0.5f);
    int numberToSelect = int(population.size() * selectRatio + 0.5f);
    if (numberToSelect < numberOfElites)
        numberToSelect = numberOfElites;

    // Sort the population in decreasing order by overall fitness
    FitnessMap popFit = getFitnessMap(population);

    // Select which to keep and kill
    if (numberToSelect < population.size()) {
        // We have to decide which ones we're going to keep
        int numberSelected = 0;
        std::vector<bool> selected(population.size(), false);

        std::cerr << "Selecting " << numberOfElites << " elites and "
                << (numberToSelect - numberOfElites) << " non-elites "
                    "from a population of size " << population.size() << '\n';

        // First, we have to handle the 'elites' -- take the top n%
        for (int i = 0; i < numberOfElites; ++i) {
            std::cerr << "\tElite:    \t" << popFit[i].first << "\tfitness("
                      << popFit[i].second << ")\n";
            IndexType index = popFit[i].first;
            selected[index] = true;
            numberSelected++;
        }

        // Now, we randomly choose the remaining amount to select from among
        // the non-elites.
        RandomUniform<scalar_t> randomFraction(0.0f, 1.0f);
        CumulativeFitness cumulativeFitness = getCumulativeFitness(population);
        int attempts = 0;
        while (numberSelected < numberToSelect) {
            // Select an individual from the old population to carry over
            // into the new one. We might have to try multiple times in
            // case we accidentally select one we've already picked.
            IndexType index;
            do {
                // Generate a random value between 0 and 1, which is a
                // point in the cumulative probability distribution. We
                // will select the individual chromosome corresponding
                // to that point. The result of this is that chromosomes
                // with higher fitness have a greater chance of being
                // picked, since they occupy a larger proportion of the
                // cumulative probability distribution.
                scalar_t pickPoint = randomFraction();
                index = cumulativeFitness.indexFor(pickPoint);
                attempts++;
            } while (selected[index]);  // Keep trying 'til we get a new one

            // Mark this one as 'kept'
            std::cerr << "\tNon-elite:\t" << index << "\tfitness("
                    << population[index].fitness() << ")\n";
            selected[index] = true;
            numberSelected++;
        }
        std::cerr << "Selecting non-elites took " << attempts << " attempts\n";

        // OK. We know which ones to keep. Let's make some new ones to
        // fill out the population
        for (int i = 0; i < population.size(); ++i)
            if (! selected[i]) {
                std::cerr << "'Reeducating' chromosome " << i << "\n";
                population[i].setAlive(false);
            }

    } else {
        std::cerr << "You asked me to select the entire population?? "
                     "Well, ok...it's your program...\n";
    }

    // Return the index of the most fit guy
    return popFit[0].first;
}


// This function creates a Gene at a particular point in the source sample
TerrainChromosome::Gene
terrainosaurus::createGene(const TerrainSample::LOD & ts, const Pixel & p) {

    // The Gene we're initializing
    TerrainChromosome::Gene g;

    // Set the X,Y source coordinates in the sample
    g.setTerrainSample(ts);
    g.setSourceCenter(p);

    // Use a identity transformation
    g.setRotation(scalar_t(0));
    g.setOffset(scalar_t(0));
    g.setScale(scalar_t(1));

    // Use no jitter around the target point
    g.setJitter(Pixel(0, 0));

    return g;
}


// This function creates a random Gene given a terrain type and LOD
TerrainChromosome::Gene
terrainosaurus::createRandomGene(const TerrainType::LOD & tt) {

    // The random number generators we'll use
    RandomUniform<IndexType> randomIndex;
    RandomUniform<scalar_t>  randomScalar;

    // The Gene we're initializing
    TerrainChromosome::Gene g;

    // First, pick a random sample from the requested TerrainType
    g.setTerrainSample(tt.randomTerrainSample());

    // Pick a random X,Y coordinate pair within the safe region of that sample
    Dimension sampleSizes(g.terrainSample().sizes());
    SizeType radius = SizeType(geneRadius(tt.levelOfDetail()));
    g.setSourceCenter(Pixel(randomIndex(radius, sampleSizes[0] - radius),
                            randomIndex(radius, sampleSizes[1] - radius)));

    // Pick a random transformation
    g.setRotation(randomScalar(scalar_t(0), PI<scalar_t>()));
//    g.setOffset(randomScalar(scalar_t(0), scalar_t(10)));
    g.setOffset(scalar_t(0));
    g.setScale(scalar_t(1));

    // Pick a random jitter around the target coordinates
    int jitterMax = int(2 * (1 - geneOverlapFactor(tt.levelOfDetail())) * radius);
    g.setJitter(Pixel(randomIndex(0, jitterMax),
                      randomIndex(0, jitterMax)));
    g.setJitter(Pixel(0, 0));

    return g;
}


// Calculate the fitness for each individual in the population, then
// calculate the normalized fitness for each individual (such
// that the whole population's normalized fitness sums to 1.0) and return
// a vector containing the cumulative fitness distribution.
void terrainosaurus::calculateFitness(Population & population) {
    // Evaluate the fitness for each individual and accumulate statistics
    PopulationStatistics popStats;
    for (int i = 0; i < population.size(); ++i) {
        calculateFitness(population[i]);            // Calculate this guy's fitness
        popStats(population[i].fitness());          // Incorporate into the stats
    }
    popStats.done();

    // Print the results of this cycle
    std::cout << "Population fitness statistics:\n"
                 "\tIndividuals " << population.size() << "\n"
                 "\tTotal:      " << popStats.sum() << "\n"
                 "\tMinimum:    " << popStats.min() << "\n"
                 "\tMaximum:    " << popStats.max() << "\n"
                 "\tMean:       " << popStats.mean() << "\n"
                 "\tVariance:   " << popStats.variance() << std::endl;

    // Calculate the normalized fitness for each individual
    for (int i = 0; i < population.size(); ++i)
        population[i].fitness().normalized()
            = (population[i].fitness().overall() - popStats.min()) / popStats.max();

    // Print the results
    std::cout << "Population normalized fitness:\n";
    for (int i = 0; i < population.size(); ++i)
        std::cout << "[" << i << "]: " << population[i].fitness().normalized() << "\n";
}

// This function determines how "good" a chromosome is
void terrainosaurus::calculateFitness(TerrainChromosome & c) {
    // First, turn the TerrainChromosome back into a heightfield
    RandomUniform<scalar_t> randomValue(0.0f, 1.0f);
    c.fitness().overall() = randomValue();
//    Heightfield hf;
//    renderChromosome(hf, c);

//    Heightfield & orig = const_cast<Heightfield &>(c.gene(0,0).sourceSample->elevation(c.levelOfDetail()));
//    const Heightfield & orig = c.gene(0,0).sourceSample.elevations();

    // Calculate the fitness of each individual terrain region
    // TODO: per-region fitness

    // Calculate the fitness of each individual terrain seam
    // TODO: per-seam fitness

    // Calculate global fitness estimates
    // TODO: global fitness

    // Determine the aggregate fitness estimate
    // TODO: aggregate fitness

/*    FitnessMeasure fitness;
    float _fitness = 1.0f - std::log10(1.0f + rms(orig - hf)) / 5.0f;
    std::cerr << "evaluateFitness(" << hf.sizes() << "): " << _fitness << endl;
    if (_fitness < 0.0f) {
        _fitness = 0.0f;
        std::cerr << "    returning 0.0f\n";
    }
    return fitness;*/
}

// This comparison functor is used by the following function to
// sort the FitnessMap vector by decreasing fitness.
struct FitnessMapGreaterThan {
    bool operator()(const FitnessMap::value_type & f1,
                    const FitnessMap::value_type & f2) const {
        return f1.second > f2.second;
    }
};

// Create a vector of index/fitness pairs, sorted in . This is useful for running
// sorting algorithms based on fitness, since the sorting can be done
// on this vector, without copying the chromosomes themselves.
FitnessMap terrainosaurus::getFitnessMap(const Population & population) {
    FitnessMap fitMap(population.size());
    for (int i = 0; i < population.size(); ++i)
        fitMap[i] = FitnessMap::value_type(i, population[i].fitness().overall());
    std::sort(fitMap.begin(), fitMap.end(), FitnessMapGreaterThan());
    return fitMap;
}


// Create a vector of scalar values between 0.0 and 1.0, representing
// the normalized, cumulative fitness distribution.
CumulativeFitness terrainosaurus::getCumulativeFitness(const Population & population) {
    CumulativeFitness cumFit;
    for (int i = 0; i < population.size(); ++i)
        cumFit.add(population[i].fitness().overall());
    return cumFit;
}


// This function determines how good a match two genes are
scalar_t terrainosaurus::evaluateCompatibility(const TerrainChromosome & c,
                                               IndexType i, IndexType j,
                                               const TerrainChromosome::Gene & g) {
    Vector2D gGrad = gradientMean(g),
             cGrad = gradientMean(c, i, j);
    Vector2D gSRange = slopeRange(g),
             cSRange = slopeRange(c, i, j);
    scalar_t diffMag   = abs(magnitude(gGrad) - magnitude(cGrad)),
             diffAngle = angle(gGrad, cGrad),
             diffMean  = abs(elevationMean(g) - elevationMean(c, i, j));
    Vector2D diffRange = abs(elevationRange(g) - elevationRange(c, i, j));
    scalar_t diffSlope = ( gSRange[0] < cSRange[0]
                            ? cSRange[0] - gSRange[0] : scalar_t(0) )
                       + ( gSRange[1] > cSRange[1]
                            ? gSRange[1] - cSRange[1] : scalar_t(0) );
    scalar_t maxMag = magnitude(c.pattern().globalGradientStatistics().mean()) * 20,
             maxAngle = PI<scalar_t>(),
             maxElevation = c.pattern().globalElevationStatistics().max(),
             slopeRange = cSRange[1] - cSRange[0];
    scalar_t compatibility =  scalar_t(1) - (diffMag / maxMag
                                           + diffAngle / maxAngle
                                           + diffMean / maxElevation
                                           + diffRange[0] / maxElevation
                                           + diffRange[1] / maxElevation,
                                           + diffSlope / slopeRange) / 6;
#if 0
    std::cerr << "Differences:\n"
              << "\tgradient magnitude: " << diffMag << "\n"
              << "\tgradient angle:     " << diffAngle << "\n"
              << "\tmean elevation:     " << diffMean << "\n"
              << "\televation range:    " << diffRange << "\n"
              << "\tslope range:        " << diffSlope << "\n";
    std::cerr << "Reference maxima:\n"
              << "\tgradient magnitude: " << maxMag << "\n"
              << "\tgradient angle:     " << maxAngle << "\n"
              << "\tslope range:        " << slopeRange << "\n"
              << "\televation:          " << maxElevation << "\n";
    std::cerr << "Aggregate compatibility: " << compatibility << endl;
#endif
    return compatibility;
}


// This function trades approximately 'ratio' percent of the genes between
// the two chromosomes. Right now, genes are only swapped for the gene in the
// other // chromosome with the same indices (e.g., c1(5, 3) with c2(5, 3) ).
//
// Obviously, the maximum amount of crossover occurs with a ratio of 0.5, since
// values above 0.5 are more or less equivalent to the corresponding value
// under 0.5 (e.g., 0.4 and 0.6 will both result in each chromosome having
// 40% from one chromosome and 60% from the other).
//
// The number of genes to be swapped is randomly chosen, but proportional to
// the number of chromosomes and the 'ratio' parameter, which should be in
// the range [0..1].
void terrainosaurus::cross(TerrainChromosome & c1,
                           TerrainChromosome & c2,
                           float ratio) {
    // Make sure the two chromosomes are the same size
    TerrainChromosome::SizeArray chromoSizes = c1.sizes();
    if (c1.size(0) != c2.size(0) || c1.size(1) != c2.size(1)) {
        std::cerr << "crossover(): Refusing to cross differently "
                     "sized chromosomes -- c1(" << c1.sizes() << "), "
                     "c2(" << c2.sizes() << ")" << std::endl;
        return;
    }

    // The random number generator we'll use throughout
    RandomUniform<int> randomInt;

    // Decide how many chunks we'll swap (between 0 and all of them)
    int size = c1.size();
    int meanChunkSize = MAX_CROSSOVER_WIDTH * MAX_CROSSOVER_WIDTH / 4;
    int num = randomInt(0, int(size * ratio / meanChunkSize));

    // For each chunk that we intend to swap...
    Pixel start, end, idx;
    Dimension regionSize;
    for (int k = 0; k < num; ++k) {
        // Pick a random starting X,Y point...
        start[0] = randomInt(0, chromoSizes[0] - 1);
        start[1] = randomInt(0, chromoSizes[1] - 1);

        //...and rectangular region size...
        regionSize[0] = randomInt(0, MAX_CROSSOVER_WIDTH);
        regionSize[1] = randomInt(0, MAX_CROSSOVER_WIDTH);

        //...then calculate the ending X,Y point...
        end[0] = std::min(start[0] + regionSize[0], chromoSizes[0] - 1);
        end[1] = std::min(start[1] + regionSize[1], chromoSizes[1] - 1);

        //...and swap everything in between
        for (idx[0] = start[0]; idx[0] <= end[0]; ++idx[0])
            for (idx[1] = start[1]; idx[1] <= end[1]; ++idx[1])
                swap(c1(idx), c2(idx));
    }
}


// This function mutates approximately 'ratio' percent of the genes in a single
// chromosome. The actual mutation is done by one of the single-gene mutation
// operator functions, chosen somewhat at random (with preference given to
// operators that we think have a better chance of improving the gene's
// compatibility with its environment.
void terrainosaurus::mutate(TerrainChromosome & c, float ratio) {
    RandomUniform<scalar_t> randomFraction(0, 1);
    Pixel idx;
    for (idx[0] = 0; idx[0] < c.size(0); ++idx[0])
        for (idx[1] = 0; idx[1] < c.size(1); ++idx[1])
            // Decide whether or not to mutate this gene
            if(randomFraction() <= ratio) {
                // OK, we're gonna do it. Decide how.
                randomize(c(idx));
            }
}


// This gene operator effectively generates a new gene, with the same
// TerrainType as the old one.
void terrainosaurus::randomize(Gene & g) {

}
