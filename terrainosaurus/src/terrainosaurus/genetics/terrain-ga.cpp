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

// Import random number generators
#include <inca/math/generator/RandomUniform>
#include <inca/math/generator/RandomGaussian>

// Import STL algorithms for working on vectors
#include <algorithm>
#include <functional>

using namespace inca::math;
using namespace terrainosaurus;
typedef TerrainChromosome::Gene             Gene;

#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/statistic>

// GLOBAL timestep
int timestep;

// HACK -- should be in math
namespace std {
    inline terrainosaurus::Vector2D pow(const terrainosaurus::Vector2D & v, float f) {
        return terrainosaurus::Vector2D(std::pow(v[0], f), std::pow(v[1], f));
    }
}

// GA global parameters, which may be set from the TTL file
int      terrainosaurus::POPULATION_SIZE        = 5;
int      terrainosaurus::EVOLUTION_CYCLES       = 5;
scalar_t terrainosaurus::SELECTION_RATIO        = 0.75f;
scalar_t terrainosaurus::ELITE_RATIO            = 0.2f;
scalar_t terrainosaurus::MUTATION_PROBABILITY   = 0.1f;
scalar_t terrainosaurus::MUTATION_RATIO         = 0.5f;
scalar_t terrainosaurus::CROSSOVER_PROBABILITY  = 0.2f;
scalar_t terrainosaurus::CROSSOVER_RATIO        = 0.5f;

// Crossover/mutation operator limits
int      terrainosaurus::MAX_CROSSOVER_WIDTH    = 4;
int      terrainosaurus::MAX_JITTER_PIXELS      = 0;
scalar_t terrainosaurus::MAX_SCALE_FACTOR       = 1.1f;
scalar_t terrainosaurus::MAX_OFFSET_AMOUNT      = 50.0f;


// This function runs the entire GA for a particular level of detail
Heightfield terrainosaurus::refineHeightfield(const TerrainSample::LOD & pattern,
                                              const MapRasterization::LOD & map) {
    std::cout << "GA heightfield refinement for "
              << map.levelOfDetail() << std::endl;

    // The set of chromosomes we're evolving (initially all 'dead')
    Population population(POPULATION_SIZE);

    timestep = 0;

    RandomUniform<scalar_t>  randomFraction(0, 1);
    RandomUniform<IndexType> randomChromosome(0, population.size() - 1);
    for (int cycle = 0; cycle < EVOLUTION_CYCLES; ++cycle) {
        // Step 1:
        // Create any new chromosomes we need to fill out our population.
        // Initially, this means creating a full complement of chromosomes,
        // but in later cycles, we only replace chromosomes that were
        // "marked for death" at the end of the last cycle.
        replenishPopulation(population, pattern, map);

        // Step 2:
        // Evaluate the fitness of each chromosome. Select 'selectionRatio'
        // percent of the chromosomes to continue to the next cycle and kill
        // off the rest (the top 'eliteRatio' percent are guaranteed to be
        // kept).
        prunePopulation(population, SELECTION_RATIO, ELITE_RATIO);

        // Step 3:
        // Cross genetic material between chromosomes in the population.
        // Approximately 2*'crossoverProbability' percent of the population
        // will be crossed, with at most 'crossoverRatio' percent of the genes
        // exchanged between them.
        if (CROSSOVER_PROBABILITY > 0) {
            for (IndexType i = 0; i < population.size(); ++i)
                // XXX This may cross with a 'dead' chromosome...but I'm lazy
                if (population[i].isAlive() && randomFraction() < CROSSOVER_PROBABILITY)
                    cross(population[i],
                          population[randomChromosome()],
                          CROSSOVER_RATIO);
        }

        // Step 4:
        // Mutate genes within the chromosomes themselves. Approximately
        // 'mutationProbability' percent of the population will be mutated,
        // with at most 'mutationRatio' percent of the genes being affected.
        if (MUTATION_PROBABILITY > 0) {
            for (IndexType i = 0; i < population.size(); ++i)
                if (population[i].isAlive() && randomFraction() < MUTATION_PROBABILITY)
                    mutate(population[i], MUTATION_RATIO);
        }
    }

    // Step 5:
    // Recalculate the fitness of the population and pick the most fit
    // chromosome. Render this chromosome to a heightfield.
    IndexType mostFit = calculateFitness(population);
    Heightfield hf;
    renderChromosome(hf, population[mostFit]);

    std::cout << "Done executing GA" << std::endl;
    std::cout << "Fitness of the strongest chromosome is " << mostFit << " with "
              << population[mostFit].fitness().overall() << std::endl;

    return hf;
}


// This function fills any slots left by dead chromosomes with new ones. It
// is used both to create the initial population and to repopulate at the
// beginning of each later cycle.
void terrainosaurus::replenishPopulation(Population & population,
                                         const TerrainSample::LOD & pattern,
                                         const MapRasterization::LOD & map) {
    population.resize(POPULATION_SIZE);
    for (int i = 0; i < population.size(); ++i)
        if (! population[i].isAlive())
            createChromosome(population[i], pattern, map);
}


// Select a fraction of the population to keep for the next evolution
// cycle and kill off all the others.
void terrainosaurus::prunePopulation(Population & population,
                                     float selectionRatio,
                                     float eliteRatio) {
    // Calculate the fitness of every chromosome in the population
    calculateFitness(population);

    // Figure out how many individuals we're talking about here
    int numberOfElites = int(population.size() * eliteRatio  + 0.5f);
    int numberToSelect = int(population.size() * selectionRatio + 0.5f);
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
        PMF pmf = getFitnessPMF(population);
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
                index = pmf.indexFor(pickPoint);
                attempts++;
            } while (selected[index]);  // Keep trying 'til we get a new one

            // Mark this one as 'kept'
            std::cerr << "\tNon-elite:\t" << index << "\tfitness("
                    << population[index].fitness() << ")\n";
            selected[index] = true;
            numberSelected++;
        }
        std::cerr << "Selecting non-elites took " << attempts << " attempts\n";

        // OK. We know which ones to keep. Let's kill off the rest.
        for (int i = 0; i < population.size(); ++i)
            if (! selected[i]) {
                std::cerr << "'Reeducating' chromosome " << i << "\n";
                population[i].setAlive(false);
            }

    } else {
        std::cerr << "You asked me to select the entire population?? "
                     "Well, ok...it's your program...\n";
    }
}


// This function makes a chromosome from a heightfield
void terrainosaurus::createChromosome(TerrainChromosome & c,
                                      const TerrainSample::LOD & pattern,
                                      const MapRasterization::LOD & map) {
    // Make this chromosome alive
    c.setAlive(true);

    // Point the Chromosome to its pattern LOD & terrain-type map
    c.setPattern(pattern);
    c.setMap(map);

    // Figure out how many genes we want in each direction
    Dimension hfSize(pattern.sizes());
    SizeType spacing = blendPatchSpacing(pattern.levelOfDetail());
    Dimension size = hfSize / spacing + Dimension(1);
    if (hfSize[0] % spacing != 0) size[0]++;    // Round up # of genes
    if (hfSize[1] % spacing != 0) size[1]++;
    c.resize(size);

    // Populate the gene grid with random data
    Pixel idx;
    for (idx[0] = 0; idx[0] < c.size(0); ++idx[0])
        for (idx[1] = 0; idx[1] < c.size(1); ++idx[1])
            randomize(c(idx), map.terrainType(c(idx).targetCenter()));

    cerr << "Initialized chromosome with " << c.size(0) << "x" << c.size(1) << " genes\n";
}


void terrainosaurus::createChromosome(TerrainChromosome & c,
                                      const TerrainSample::LOD & sample) {
    // Create a very boring map
    TerrainLibraryPtr tl = const_cast<TerrainSample::LOD &>(sample).getObject()
                            ->terrainType()->terrainLibrary();
    MapRasterizationPtr map(new MapRasterization(tl));
    MapRasterization::LOD::IDMap terrainTypes(sample.bounds());
    fill(terrainTypes, sample.terrainType().terrainTypeID());
    (*map)[sample.levelOfDetail()].createFromRaster(terrainTypes);

    // Make this chromosome alive
    c.setAlive(true);

    // Point the Chromosome to its pattern LOD & terrain-type map
    c.setPattern(sample);
    c.setMap((*map)[sample.levelOfDetail()]);

    // Figure out how many genes we want in each direction
    Dimension hfSize(sample.sizes());
    SizeType spacing = blendPatchSpacing(sample.levelOfDetail());
    Dimension size = hfSize / spacing;
    if (hfSize[0] % spacing != 0) size[0]++;    // Round up # of genes
    if (hfSize[1] % spacing != 0) size[1]++;
    c.resize(size);

    // Populate the gene grid with the sample data
    Pixel idx;
    for (idx[0] = 0; idx[0] < c.size(0); ++idx[0])
        for (idx[1] = 0; idx[1] < c.size(1); ++idx[1]) {
            c(idx).setTerrainSample(sample);
            reset(c(idx));
        }

    cerr << "Initialized chromosome with " << c.size(0) << "x" << c.size(1) << " genes\n";
}

#if 0
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


#endif

// Calculate the fitness for each individual in the population, then
// calculate the normalized fitness for each individual (such
// that the whole population's normalized fitness sums to 1.0) and return
// a vector containing the cumulative fitness distribution.
// It returns the index of the most fit chromosome in the population.
IndexType terrainosaurus::calculateFitness(Population & population) {
    // Evaluate the fitness for each individual and accumulate statistics
    PopulationStatistics popStats;
    IndexType mostFit = -1;
    scalar_t maxFitness = 0;
    for (int i = 0; i < population.size(); ++i) {
        calculateFitness(population[i]);            // Calculate this guy's fitness
        popStats(population[i].fitness());          // Incorporate into the stats
        if (population[i].fitness().overall() > maxFitness) {   // Update 'max'
            maxFitness = population[i].fitness();
            mostFit = i;
        }
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

    // Calculate the normalized fitness for each individual (s.t. sum(pop) == 1.0)
    for (int i = 0; i < population.size(); ++i)
        population[i].fitness().normalized() = population[i].fitness() / popStats.sum();

    // Print the results
    std::cout << "Population normalized fitness:\n";
    for (int i = 0; i < population.size(); ++i)
        std::cout << "[" << i << "]: " << population[i].fitness().normalized() << "\n";

    return mostFit;
}

#include <inca/raster/generators/gaussian>
// Helper function for fitness evaluation
scalar_t normalCurveProject(scalar_t mean, scalar_t variance, scalar_t value) {
    scalar_t stddev(std::sqrt(variance));
    scalar_t projection = std::exp(-(value-mean) * (value-mean)) / (2 * variance);
//    std::cerr << "gaussian of (" << mean << ", " << variance << ", " << value << ") is " << projection << std::endl;
    return projection;
}


// This function determines how "good" a chromosome is
void terrainosaurus::calculateFitness(TerrainChromosome & c) {
    // First, turn the TerrainChromosome back into a heightfield
    Heightfield hf;
    renderChromosome(hf, c);

    // Do analysis of it
    TerrainSamplePtr ts(new TerrainSample(hf, c.levelOfDetail()));
    ts->ensureAnalyzed(c.levelOfDetail());

    // Calculate the area-weighted mean fitness of each terrain region
    scalar_t rf = 0;
    for (IDType i = 0; i < c.map().regionCount(); ++i) {
        calculateRegionFitness(c, i);
        rf += c.regionFitness(i).normalized();
    }

    // Calculate mean gene compatibility
    scalar_t gc = 0;
    Pixel idx;
    for (idx[1] = 0; idx[1] < c.size(1); ++idx[1])
        for (idx[0] = 0; idx[0] < c.size(0); ++idx[0]) {
            calculateCompatibility(c(idx));
            gc += c(idx).compatibility().overall();
        }
    gc /= c.size();

    // Calculate the RMS difference from the pattern (mostly for amusement)
    scalar_t rmsDiff = rms(c.pattern().elevations() - hf);

    // The aggregate fitness estimate is a weighted average of these
    scalar_t alpha = 0.5f;
    c.fitness().overall() = alpha * gc + (1 - alpha) * rf;

    std::cout << "Fitness evaluation:\n"
              << "\tArea-averaged region fitnesses: " << rf << '\n'
              << "\tAveraged gene compatibilities:  " << gc << '\n'
              << "\tRMS difference from pattern:    " << rmsDiff << '\n'
              << "\tAggregate fitness:              " << c.fitness().overall() << std::endl;
}


// Calculate the fitness measure for a single region
void terrainosaurus::calculateRegionFitness(TerrainChromosome & c,
                                            IDType regionID) {
    RegionFitnessMeasure & rf = c.regionFitness(regionID);
    const MapRasterization::LOD & m = c.map();

    // Calculate the bare fitness measure
    rf.overall() = 1.0f;

    // Calculate the area-normalized quantity
    rf.normalized() = rf.overall() * m.regionArea(regionID) / m.size();
}


// Calculate the compatibility measure for a single gene
void terrainosaurus::calculateCompatibility(TerrainChromosome::Gene & g) {
    const TerrainChromosome & c = g.parent();
    IndexType i = g.indices()[0];
    IndexType j = g.indices()[1];
    GeneCompatibilityMeasure & gc = g.compatibility();

    // Calculate the bare compatibility measure
    Vector2D gGrad = gradientMean(g),
             cGrad = gradientMean(c, i, j),
             xAxis(1.0f, 0.0f);
    scalar_t ce = normalCurveProject(elevationMean(c, i, j), 150.0f, elevationMean(g));
    scalar_t cs = normalCurveProject(magnitude(cGrad), 30.0f, magnitude(gGrad));
    scalar_t ca = normalCurveProject(signedAngle(cGrad, xAxis), PI<scalar_t>() / 3, signedAngle(gGrad, xAxis));
    gc.overall() = (ce + cs + ca) / 3;
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


// Create a probability mass function (valid over [0,1]) from the
// overall fitnesses of the chromosomes in the population. This PMF
// may be used to randomly pick chromosomes, giving a probabilistic
// preference to the more fit chromosomes.
PMF terrainosaurus::getFitnessPMF(const Population & population) {
    PMF pmf;
    for (int i = 0; i < population.size(); ++i)
        pmf.add(population[i].fitness().overall());
    return pmf;
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

    // Make sure both chromosomes are 'living'
    if (! c1.isAlive())
        std::cerr << "Ack! C1 is not alive!!!!\n";
    if (! c2.isAlive())
        std::cerr << "Awww...C2 is not alive.\n";

    // The random number generator we'll use throughout
    RandomUniform<int> randomInt;

    // Decide how many chunks we'll swap (between 0 and all of them)
    int size = c1.size();
    int meanChunkSize = MAX_CROSSOVER_WIDTH * MAX_CROSSOVER_WIDTH / 4;
    int num = randomInt(0, int(size * ratio / meanChunkSize));

    // For each chunk that we intend to swap...
    Pixel start, end, idx;
    Dimension regionSize;
    int crossed = 0;
    for (int k = 0; k < num; ++k) {
        timestep++;

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
            for (idx[1] = start[1]; idx[1] <= end[1]; ++idx[1]) {
                swap(c1(idx), c2(idx));
                c1.fireCrossed(idx, timestep);
                c2.fireCrossed(idx, timestep);
                ++crossed;
            }
    }
    std::cerr << "Crossed " << crossed << " of " << c1.size() << " genes\n";
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
            timestep++;

            // Decide whether or not to mutate this gene
            if(randomFraction() <= ratio) {
                // OK, we're gonna do it. Decide how.
                scalar_t f = randomFraction();

                if (f < 0.1f)
                    randomize(c(idx), c(idx).terrainType());
                else if (f < 0.3f) {
                    scalar_t a = canonicalAngle(gradientMean(c, idx[0], idx[1]), Vector2D(1.0f, 0.0f)) - c(idx).rotation();
                    rotate(c(idx), a - 0.2f, a + 0.2f);
                } else if (f < 0.5f) {
                    scalar_t m = elevationMean(c, idx[0], idx[1]);
                    offset(c(idx), m - 30.0f, m + 30.0f);
                } else if (f < 0.7f)
                    scale(c(idx), 0.9f, 1.1f);
                else
                    translate(c(idx), Offset(-1, -1), Offset(1, 1));

                c.fireMutated(idx, timestep);
            }
}


// This gene operator effectively generates a new gene, with the same
// TerrainType as the old one.
void terrainosaurus::randomize(Gene & g, const TerrainType::LOD & tt) {
    // The random number generators we'll use
    RandomUniform<IndexType> randomIndex;
    RandomUniform<scalar_t>  randomScalar;

    // First, pick a random sample from the requested TerrainType
//    g.setTerrainSample(tt.randomTerrainSample());
    g.setTerrainSample(tt.terrainSample(0));

    // Pick a random X,Y coordinate pair within the safe region of that sample
    Dimension sampleSizes(g.terrainSample().sizes());
    SizeType radius = SizeType(blendFalloffRadius(tt.levelOfDetail()));
    g.setSourceCenter(Pixel(sampleSizes[0] / 2, sampleSizes[1] / 2));
    translate(g, Offset(-sampleSizes[0] / 2 + radius, -sampleSizes[1] / 2 + radius),
                 Offset( sampleSizes[0] / 2 - radius,  sampleSizes[1] / 2 - radius));

    // Pick (bounded) random values for the transformation parameters
    g.setRotation(0);       //rotate(g, 0, 2 * PI<scalar_t>());
    g.setOffset(0);         offset(g, -MAX_OFFSET_AMOUNT, MAX_OFFSET_AMOUNT);
    g.setScale(1);          scale(g, 1 / MAX_SCALE_FACTOR, MAX_SCALE_FACTOR);
    g.setJitter(Offset(0)); jitter(g, Offset(-MAX_JITTER_PIXELS), Offset(MAX_JITTER_PIXELS));
}


// This gene operator effectively generates a new gene, with the same
// TerrainType as the old one.
void terrainosaurus::reset(Gene & g) {
    // Set all transformation parameters to defaults
    g.setRotation(0);
    g.setOffset(0);
    g.setScale(1);
    g.setJitter(Offset(0));

    // Set source coordinates equal to target coordinates
    g.setSourceCenter(g.targetCenter());
}

// This gene operator offsets the gene by a random amount between 'min' and 'max'
void terrainosaurus::offset(Gene & g, scalar_t min, scalar_t max) {
    RandomUniform<scalar_t> randomScalar;
    g.setOffset(g.offset() + randomScalar(min, max));
}

// This gene operator vertically scales the gene by a random factor between
// 'min' and 'max'.
void terrainosaurus::scale(Gene & g, scalar_t min, scalar_t max) {
    RandomUniform<scalar_t> randomScalar;
    g.setScale(g.scale() * randomScalar(min, max));
}

// This gene operator rotates the gene by a random angle (in radians) between
// 'min' and 'max'.
void terrainosaurus::rotate(Gene & g, scalar_t min, scalar_t max) {
    RandomUniform<scalar_t> randomScalar;
    g.setRotation(g.rotation() + randomScalar(min, max));
}

// This gene operator moves the source coordinates for the gene by a random
// vector between 'min' and 'max'.
void terrainosaurus::translate(Gene & g, const Offset & min,
                                         const Offset & max) {
    RandomUniform<IndexType> randomIndex;
    g.setSourceCenter(g.sourceCenter()
                            + Offset(randomIndex(min[0], max[0]),
                                     randomIndex(min[1], max[1])));
}

// This gene operator moves the target coordinates for the gene by a random
// vector between 'min' and 'max'.
void terrainosaurus::jitter(Gene & g, const Offset & min,
                                      const Offset & max) {
    RandomUniform<IndexType> randomIndex;
    g.setJitter(g.jitter() + Offset(randomIndex(min[0], max[0]),
                                    randomIndex(min[1], max[1])));
}
