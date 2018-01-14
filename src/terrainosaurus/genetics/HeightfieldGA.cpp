/** -*- C++ -*-
 *
 * \file    HeightfieldGA.cpp
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

// Import class definition
#include "HeightfieldGA.hpp"
using namespace inca;
using namespace inca::math;

// TODO: This file is huge. How can it be made more comprehensible?


namespace terrainosaurus {
    // Forward declarations of operator classes
    class BasicInitializationOperator;
    class RandomSourceDataInitializationOperator;
    class PatternCloneInitializationOperator;

    class RectangularRegionCrossoverOperator;

    class ResetTransformMutationOperator;
    class VerticalOffsetMutationOperator;
    class VerticalScaleMutationOperator;
    class VerticalRotationMutationOperator;
    class HorizontalTranslationMutationOperator;
    class HorizontalJitterMutationOperator;
    class RandomSourceDataMutationOperator;
    class ConformMutationOperator;

    class GeneCompatibilityFitnessOperator;
    class RegionSimilarityFitnessOperator;
}

#include <terrainosaurus/genetics/terrain-operations.hpp>

#include <terrainosaurus/TerrainosaurusApplication.hpp>
using namespace terrainosaurus;


// HACK: is there a better way to do this?? Maybe something that could be
// integrated cleanly into the GeneticAlgorithm class?
// Since the index of operators can change, based on the order in which they
// are added to the GA, we build an catalog of the ones we care about (so we can
// adjust their PMF later, without having to hard-code their indices).
#define MUTATION_OPERATOR_CATALOG_SIZE  3
#define ROTATION_OPERATOR               0
#define OFFSET_OPERATOR                 1
#define SCALE_OPERATOR                  2

IndexType mutationOperatorCatalog[MUTATION_OPERATOR_CATALOG_SIZE];


/**
 * The BasicInitializationOperator implements functionality common to many or
 * all of the various chromosome initialization operators. It is not intended
 * to be instantiated directly.
 */
class terrainosaurus::BasicInitializationOperator
        : public HeightfieldGA::InitializationOperator {
protected:
    // Non-public constructor
    explicit BasicInitializationOperator() { }

    // Access to the owning GA and properties for the currnt LOD
    HeightfieldGA & owner() {
        return static_cast<HeightfieldGA &>(InitializationOperator::owner());
    }
    HeightfieldGA const & owner() const {
        return static_cast<HeightfieldGA const &>(InitializationOperator::owner());
    }
    TerrainLOD currentLOD() const { return owner().currentLOD(); }
    TerrainSample::LOD & terrainSample() {
        HeightfieldGA & ga = owner();
        return (*ga.terrainSample())[ga.currentLOD()];
    }
    TerrainSample::LOD const & terrainSample() const {
        HeightfieldGA const & ga = owner();
        return (*ga.terrainSample())[ga.currentLOD()];
    }
    TerrainSample::LOD & patternSample() {
        HeightfieldGA & ga = owner();
        return (*ga.patternSample())[ga.currentLOD()];
    }
    TerrainSample::LOD const & patternSample() const {
        HeightfieldGA const & ga = owner();
        return (*ga.patternSample())[ga.currentLOD()];
    }

    // What are the correct dimensions for a chromsome at the current LOD
    Dimension geneGridSizes() const {
        Dimension hfSize(patternSample().sizes());
        SizeType spacing = blendPatchSpacing(currentLOD());
        Dimension sizes = hfSize / spacing + Dimension(1);
        if (hfSize[0] % spacing != 0) sizes[0]++;   // Round up # of genes
        if (hfSize[1] % spacing != 0) sizes[1]++;
        return sizes;
    }
    
    // Initialize the chromosome with the correct map, pattern LOD & sizes
    void operator()(Chromosome & c) {
        c.setPatternSample(owner().patternSample());
        c.setScratchSample(owner().terrainSample());
        c.setLevelOfDetail(currentLOD());
        c.resize(geneGridSizes());
    }
};


/**
 * The RandomSourceDataMutationOperator implements a simple mutation operator
 * that randomly picks a TerrainSample and coordinates within that sample
 * from among the example terrains belonging to the gene's TerrainType.
 */
class terrainosaurus::RandomSourceDataMutationOperator
        : public HeightfieldGA::MutationOperator {
public:
    void operator()(Gene & g) {
        // Pick some sample data from the gene's TT
        g.setTerrainSample(g.terrainType().randomTerrainSample());

        // Pick a random X,Y coordinate pair within the safe region of that sample
        Dimension sampleSizes(g.terrainSample().sizes());
        SizeType radius = SizeType(blendFalloffRadius(g.levelOfDetail()));
        Pixel center(randomInt(radius, sampleSizes[0] - radius),
                     randomInt(radius, sampleSizes[1] - radius));
        g.setSourceCenter(center);
    }

protected:
    RandomUniform<int> randomInt;
};


/**
 * The RandomSourceDataInitializationOperator implements a simple
 * initialization operator that populates the chromosome with random data from
 * the TerrainLibrary.
 */
class terrainosaurus::RandomSourceDataInitializationOperator
        : public terrainosaurus::BasicInitializationOperator {
public:
    void operator()(Chromosome & c) {
        // Set the basic properties of the chromosome
        BasicInitializationOperator::operator()(c);

        // Populate the gene grid with random data
        const MapRasterization::LOD & mr = patternSample().mapRasterization();
        Pixel idx;
        for (idx[0] = 0; idx[0] < c.size(0); ++idx[0])
            for (idx[1] = 0; idx[1] < c.size(1); ++idx[1]) {
                Gene & g = c(idx);       

                // Figure out what TerrainType this should be
                g.setTerrainType(mr.terrainType(g.targetCenter()));
                INCA_DEBUG("TerrainType at " << g.targetCenter()
                           << " is " << g.terrainType().name())
                
                // Pick some random sample data from the TT
                _pickRandomSourceData(g);

                // Reset the transformation parameters
                g.reset();
            }

        INCA_DEBUG("Initialized chromosome with "
                << c.size(0) << "x" << c.size(1) << " genes")
    }
    
protected:
    RandomSourceDataMutationOperator _pickRandomSourceData;
};


/**
 * The PatternCloneInitializationOperator implements a simple
 * initialization operator that initializes the chromosome to exactly
 * represent the pattern heightfield.
 */
class terrainosaurus::PatternCloneInitializationOperator
        : public terrainosaurus::BasicInitializationOperator {
public:
    void operator()(Chromosome & c) {
        // Set the basic properties of the chromosome
        BasicInitializationOperator::operator()(c);

        // Populate the gene grid with genes representing the pattern TS
        const TerrainSample::LOD & ps = patternSample();
        Pixel idx;
        for (idx[0] = 0; idx[0] < c.size(0); ++idx[0])
            for (idx[1] = 0; idx[1] < c.size(1); ++idx[1]) {
                Gene & g = c(idx);

                // Set the pattern as the source terrain sample, and eliminate
                // any transformation
                g.setTerrainSample(ps);
                g.reset();
            }

        INCA_DEBUG("Clone-initialized chromosome with "
                   << c.size(0) << "x" << c.size(1)
                   << " genes from " << ps.name())
    }
};


/**
 * The RectangularRegionCrossoverOperator implements a simple crossover operator
 * that swaps small rectangular clusters of genes from one chromosome with the
 * matching genes (i.e., with the same indices) from the other chromosome.
 *
 * The percentage of genes that are swapped is random but, on average, will
 * approximate the value of the crossoverRatio() property, which is a scalar
 * in the range [0..1]. Note that the maximum amount of crossover occurs with
 * crossoverRatio() == 0.5, since values above this are symmetric with those
 * below 0.5 (e.g. 0.4 and 0.6 will both result in each chromosome having
 * 40% from one chromosome and 60% from the other).
 */
class terrainosaurus::RectangularRegionCrossoverOperator
        : public HeightfieldGA::CrossoverOperator {
public:
    // Constructors
    explicit RectangularRegionCrossoverOperator() {
        const TerrainosaurusApplication & app = TerrainosaurusApplication::instance();
        _crossoverRatio = app.heightfieldGACrossoverRatio();
        _regionWidth = app.heightfieldGAMaxCrossoverWidth();
    }
    explicit RectangularRegionCrossoverOperator(scalar_t r, int w)
        : _crossoverRatio(r), _regionWidth(w) { }

    // Properties
    Scalar crossoverRatio() const    { return _crossoverRatio; }
    void setCrossoverRatio(scalar_t r) { _crossoverRatio = r; }
    int regionWidth() const    { return _regionWidth; }
    void setRegionWidth(int w) { _regionWidth = w; }

    // Function call operator
    void operator()(Chromosome & c1, Chromosome & c2) {
        // Make sure the two chromosomes are the same size
        if (c1.sizes() != c2.sizes()) {
            GeneticAlgorithmException e;
            e << typeid(this).name() << ": Cannot cross differently-sized "
                 "chromosomes -- c1(" << c1.sizes() << "), c2(" << c2.sizes() << ')';
            throw e;
        }

        // The random number generator we'll use throughout
        RandomUniform<int> randomInt;

        // Decide how many chunks we'll swap (between 0 and all of them)
        int size = c1.size();
        int meanChunkSize = regionWidth() * regionWidth() / 4;
        int num = randomInt(0, int(size * crossoverRatio() / meanChunkSize));
//        INCA_DEBUG("Mean chunk size is " << meanChunkSize)
//        INCA_DEBUG("max num is " << (size * crossoverRatio() / meanChunkSize))
//        INCA_DEBUG("Nium is " << num)

        // For each chunk that we intend to swap...
        Pixel start, end, idx;
        Dimension regionSize;
        int crossed = 0;
        for (int k = 0; k < num; ++k) {
            // Pick a random starting X,Y point...
            start[0] = randomInt(0, c1.size(0) - 1);
            start[1] = randomInt(0, c1.size(1) - 1);

            //...and rectangular region size...
            regionSize[0] = randomInt(0, regionWidth());
            regionSize[1] = randomInt(0, regionWidth());

            //...then calculate the ending X,Y point...
            end[0] = std::min(start[0] + regionSize[0], c1.size(0) - 1);
            end[1] = std::min(start[1] + regionSize[1], c1.size(1) - 1);

            //...and swap everything in between
            for (idx[0] = start[0]; idx[0] <= end[0]; ++idx[0])
                for (idx[1] = start[1]; idx[1] <= end[1]; ++idx[1]) {
                    swap(c1(idx), c2(idx));
                    ++crossed;
                }
        }
//        INCA_DEBUG("Crossed " << crossed << " of " << size << " genes\n"
//                   "Target percentage: " << int(crossoverRatio() * 100) << "\n"
//                   "Actual percentage: " << int(crossed * 100.0f / size))
    }

protected:
    scalar_t _crossoverRatio;
    int      _regionWidth;
};


class terrainosaurus::ResetTransformMutationOperator
        : public HeightfieldGA::MutationOperator {
public:
    void operator()(Gene & g) {
        g.reset();
    }
};


class terrainosaurus::VerticalOffsetMutationOperator
        : public HeightfieldGA::MutationOperator {
public:
    explicit VerticalOffsetMutationOperator(Scalar maxP)
        : _maxPercentChange(maxP) { }

    void operator()(Gene & g) {
        Scalar range = terrainTypeElevationRange(g);
        Scalar mu    = patternElevationMean(g),
               sigma = std::sqrt(terrainTypeElevationVariance(g)),
               current = elevationMean(g),
               maxChange = _maxPercentChange * range;
        Scalar target = randomScalar(mu, sigma),
               change = target - current;
        if (std::abs(change) > maxChange)
            if (target < current)   change = -maxChange;
            else                    change = +maxChange;

        g.setOffset(g.offset() + change);
    }

protected:
    Scalar _maxPercentChange;
    RandomGaussian<Scalar> randomScalar;
};


class terrainosaurus::VerticalScaleMutationOperator
        : public HeightfieldGA::MutationOperator {
public:
    explicit VerticalScaleMutationOperator(Scalar maxP)
        : _maxPercentChange(maxP) { }

    void operator()(Gene & g) {
        Scalar range = terrainTypeSlopeRange(g);
        Vector2D targetGrad = patternGradientMean(g),
                 currentGrad = gradientMean(g);
        Scalar mu    = magnitude(targetGrad),
               sigma = std::sqrt(terrainTypeSlopeVariance(g)),
               current = magnitude(currentGrad),
               maxChange = _maxPercentChange * range;
        Scalar target = randomScalar(mu, sigma),
               change = target - current;
        if (std::abs(change) > maxChange)
            if (target < current)   change = -maxChange;
            else                    change = +maxChange;

        g.setScale(g.scale() + change);
    }

protected:
    Scalar _maxPercentChange;
    RandomGaussian<Scalar> randomScalar;
};


class terrainosaurus::VerticalRotationMutationOperator
        : public HeightfieldGA::MutationOperator {
public:
    explicit VerticalRotationMutationOperator(Scalar maxP)
        : _maxPercentChange(maxP) { }

    void operator()(Gene & g) {
        Scalar range = terrainTypeSlopeRange(g);
        Vector2D targetGrad = patternGradientMean(g),
                 currentGrad = gradientMean(g);
        Scalar mu    = 0,
               sigma = std::sqrt(terrainTypeAngleVariance(g)),
               current = signedAngle(currentGrad, targetGrad),
               maxChange = _maxPercentChange * range;
        Scalar target = randomScalar(mu, sigma),
               change = target - current;
        if (std::abs(change) > maxChange)
            if (target < current)   change = -maxChange;
            else                    change = +maxChange;

        g.setRotation(g.rotation() + change);
    }

protected:
    Scalar _maxPercentChange;
    RandomGaussian<Scalar> randomScalar;
};


class terrainosaurus::HorizontalTranslationMutationOperator
        : public HeightfieldGA::MutationOperator {
public:
    void operator()(Gene & g) {
        Offset min, max;
        g.setSourceCenter(g.sourceCenter()
                               + Offset(randomDifference(min[0], max[0]),
                                        randomDifference(min[1], max[1])));
    }

protected:
    RandomUniform<DifferenceType> randomDifference;
};


class terrainosaurus::HorizontalJitterMutationOperator
        : public HeightfieldGA::MutationOperator {
public:
    void operator()(Gene & g) {
        Offset min, max;
        g.setJitter(g.jitter() + Offset(randomDifference(min[0], max[0]),
                                        randomDifference(min[1], max[1])));
    }

protected:
    RandomUniform<DifferenceType> randomDifference;
};


class terrainosaurus::ConformMutationOperator
        : public HeightfieldGA::MutationOperator {
public:
    void operator()(Gene & g) {
        scalar_t myRange = elevationRange(g);
        scalar_t changeMean  = patternElevationMean(g)  - elevationMean(g);
        scalar_t changeScale = (myRange == scalar_t(0))
                                ? scalar_t(1)
                                : patternElevationRange(g) / myRange;
        scalar_t changeAngle = signedAngle(patternGradientMean(g),
                                           gradientMean(g));
        g.setOffset(g.offset() + changeMean);
//        g.setScale(g.scale() * changeScale);
//        g.setRotation(g.rotation() + changeAngle);
    }
};


// HACK This should be given another home
template <typename Scalar>
Scalar gauss_project(Scalar m, Scalar v, Scalar x) {
    Scalar diff = x - m;
    return std::exp(-(diff * diff) / (2 * v));
}


/**
 * The GeneCompatibilityFitnessOperator implements a fitness operator returning
 * the unweighted average of the compatibility of each gene with its
 * corresponding "chunk" of the pattern heightfield.
 */
class terrainosaurus::GeneCompatibilityFitnessOperator
        : public HeightfieldGA::FitnessOperator {
public:
    Scalar operator()(Chromosome & c) {
        INCA_DEBUG("Evaluating gene compat for chromosome " << owner().indexOf(c))

        ConformMutationOperator conform;
        Scalar compat = 0;
        Pixel px;
        for (px[0] = 0; px[0] < c.size(0); ++px[0])
            for (px[1] = 0; px[1] < c.size(1); ++px[1]) {
                Gene & g = c(px);

                // XXX
                conform(g);

                Scalar targetElevation = patternElevationMean(g);
                Vector2D targetGradient = patternGradientMean(g);
                Scalar targetSlope = magnitude(targetGradient);
                Scalar targetAngle = 0;

                Scalar currentElevation = elevationMean(g);
                Vector2D currentGradient = gradientMean(g);
                Scalar currentSlope = magnitude(currentGradient);
                Scalar currentAngle = signedAngle(currentGradient, targetGradient);

                Scalar elevationVariance = terrainTypeElevationVariance(g);
                Scalar slopeVariance     = terrainTypeSlopeVariance(g);
                Scalar angleVariance     = terrainTypeAngleVariance(g);

                g.compatibility().elevation() = gauss_project(targetElevation,
                                                              elevationVariance,
                                                              currentElevation);
                g.compatibility().slope()     = gauss_project(targetSlope,
                                                              slopeVariance,
                                                              currentSlope);
                g.compatibility().angle()     = gauss_project(targetAngle,
                                                              angleVariance,
                                                              currentAngle);
                g.compatibility().overall() =
                    ( g.compatibility().elevation()
                    + g.compatibility().slope()
                    + g.compatibility().angle() ) / 3;

                if (std::isnan(g.compatibility())) {
                    INCA_DEBUG("Compatibility for " << px)
                    INCA_DEBUG("TS: " << g.terrainSample().name())
                    const TerrainSample::LOD::Stat & s = g.terrainSample().globalElevationStatistics();
                    INCA_DEBUG("Count: " << s.sampleSize())
                    INCA_DEBUG("Range is " << s.range())
                    INCA_DEBUG("Mean is " << s.mean())
                    INCA_DEBUG("M/M is " << s.min() << " - " << s.max())
                    INCA_DEBUG("Gradient is " << targetGradient << "   " << currentGradient)
                    INCA_DEBUG("E: <" << targetElevation << ", " << elevationVariance << "> "
                            << currentElevation << "  : " << g.compatibility().elevation())
                    INCA_DEBUG("S: <" << targetSlope << ", " << slopeVariance << "> "
                            << currentSlope << "  : " << g.compatibility().slope())
                    INCA_DEBUG("A: <" << targetAngle << ", " << angleVariance << "> "
                            << currentAngle << "  : " << g.compatibility().angle())
                    INCA_DEBUG("Overall: " << g.compatibility().overall())
                    INCA_DEBUG("Scale: " << g.scale())
                    INCA_DEBUG("Rotation: " << g.rotation())
                    ::exit(1);
                }

                compat += g.compatibility().overall();
            }
        c.fitness().compatibility() = compat / c.genes().size();
        return c.fitness().compatibility();
    }
};


/**
 * The RegionSimilarityFitnessOperator implements a fitness operator
 * returning the area-weighted average of region fitnesses. The fitness of a
 * region is determined by evaluating the terrain characteristics for the
 * region and comparing them to those of its terrain type.
 */
class terrainosaurus::RegionSimilarityFitnessOperator
        : public HeightfieldGA::FitnessOperator {
public:
    Scalar operator()(Chromosome & c) {
        INCA_DEBUG("Evaluating region fitnesses for chromosome " << owner().indexOf(c))

        // Retrieve the "scratch pad" TerrainSample for the Chromosome and its map
        TerrainSample::LOD    & terrain = c.scratch();
        MapRasterization::LOD & map     = terrain.mapRasterization();

        // Render the Chromosome to the scratch TS
        renderChromosome(terrain, c);
        c.setRegionCount(terrain.regionCount());

        // Compare each region's measured characteristics with its reference
        Scalar fitness = 0;
        for (IDType rID = 0; rID < IDType(map.regionCount()); ++rID) {
            // Find individual fitness components for this region
            Chromosome::RegionSimilarityMeasure & regFit = c.regionFitness(rID);
            regFit = terrainRegionSimilarity(terrain, rID, true);

            Scalar regArea = Scalar(map.regionArea(rID));
            fitness += regFit.overall() * regArea;
        }
        c.fitness().similarity() = fitness / map.size();
        return c.fitness().similarity();
    }
};


// Constructor
HeightfieldGA::HeightfieldGA(TerrainSamplePtr ts, TerrainSamplePtr ps) {
    // Not doing anything yet...
    _running = false;
    _currentLOD = TerrainLOD::minimum();

    // Set up the TerrainSample
    setTerrainSample(ts);
    setPatternSample(ps);

    // Set up the initialization operators
    addInitializationOperator(new RandomSourceDataInitializationOperator());
    addInitializationOperator(new PatternCloneInitializationOperator(), 0.0f);

    // Set up the crossover operators
    addCrossoverOperator(new RectangularRegionCrossoverOperator());

    // Set up the mutation operators (keeping track of the ones we care about)
//    addMutationOperator(new ResetTransformMutationOperator());
    mutationOperatorCatalog[OFFSET_OPERATOR] = mutationOperatorCount();
    addMutationOperator(new VerticalOffsetMutationOperator(1.5f));
    mutationOperatorCatalog[SCALE_OPERATOR] = mutationOperatorCount();
    addMutationOperator(new VerticalScaleMutationOperator(1.5f));
    mutationOperatorCatalog[ROTATION_OPERATOR] = mutationOperatorCount();
    addMutationOperator(new VerticalRotationMutationOperator(1.5f));
    addMutationOperator(new HorizontalTranslationMutationOperator());
    addMutationOperator(new RandomSourceDataMutationOperator());
//    addMutationOperator(new HorizontalJitterMutationOperator());

    // Set up the fitness calculation operators
    addFitnessOperator(new GeneCompatibilityFitnessOperator());
    addFitnessOperator(new RegionSimilarityFitnessOperator());

    // Set up the initial parameters for the GA
    const TerrainosaurusApplication & app = TerrainosaurusApplication::instance();
    setPopulationSize(app.heightfieldGAPopulationSize());
    setEvolutionCycles(app.heightfieldGAEvolutionCycles());
    setSelectionRatio(app.heightfieldGASelectionRatio());
    setElitismRatio(app.heightfieldGAElitismRatio());
    setCrossoverProbability(app.heightfieldGACrossoverProbability());
    setCrossoverRatio(app.heightfieldGACrossoverRatio());
    setMutationProbability(app.heightfieldGAMutationProbability());
    setMutationRatio(app.heightfieldGAMutationRatio());
}

// Destructor
HeightfieldGA::~HeightfieldGA() { }


// The TerrainSample holding the pattern we use to build
TerrainSamplePtr HeightfieldGA::patternSample() {
    return _patternSample;
}
TerrainSampleConstPtr HeightfieldGA::patternSample() const {
    return _patternSample;
}
void HeightfieldGA::setPatternSample(TerrainSamplePtr ps) {
    _patternSample = ps;
}

// The TerrainSample holding the terrain that we're building
TerrainSamplePtr HeightfieldGA::terrainSample() {
    return _terrainSample;
}
TerrainSampleConstPtr HeightfieldGA::terrainSample() const {
    return _terrainSample;
}
void HeightfieldGA::setTerrainSample(TerrainSamplePtr ts) {
    _terrainSample = ts;
}


// The LOD currently being worked on
TerrainLOD HeightfieldGA::currentLOD() const { return _currentLOD; }


// Whether we're currently doing anything
bool HeightfieldGA::running() const { return _running; }


// Functions to run the GA and generate a TerrainSample
void HeightfieldGA::run(TerrainLOD targetLOD) {
    run(currentLOD(), targetLOD);
}
void HeightfieldGA::run(TerrainLOD startLOD, TerrainLOD targetLOD) {
    _running = true;

    try {
        // If we don't have an output TerrainSample, we'll just make one
        if (! terrainSample())
            _terrainSample.reset(new TerrainSample());
        TerrainSamplePtr ts = _terrainSample;

        // If we don't have a pattern TerrainSample, a MapRasterization, or a
        // TerrainLibrary, MAYDAY! MAYDAY!
        TerrainSamplePtr ps = patternSample();
        if (! ps) {
            GeneticAlgorithmException e;
            e << "HeightfieldGA incorrectly configured...no pattern TerrainSample given";
            throw e;
        }
        MapRasterizationPtr mr = ps->mapRasterization();
        if (! mr) {
            GeneticAlgorithmException e;
            e << "HeightfieldGA incorrectly configured...no MapRasterization given";
            throw e;
        }
        TerrainLibraryConstPtr tl = mr->terrainLibrary();
        if (! tl) {
            GeneticAlgorithmException e;
            e << "HeightfieldGA incorrectly configured...no TerrainLibrary given";
            throw e;
        }

        // Set up for the right number of LODs
        _lodTimes.resize(int(targetLOD) + 1);
        _setupTimes.resize(int(targetLOD) + 1);
        _processingTimes.resize(int(targetLOD) + 1);

        // Reset and start timing
        _totalTime.start(true);

        // Preload all of the TerrainTypes we'll be using, for every LOD we'll be
        // using.
        _loadingTime.start(true);
        INCA_INFO("Preloading terrain sample data")
        for (IndexType i = 0; i < (*ps)[targetLOD].regionCount(); ++i)
            (*mr)[targetLOD].regionTerrainType(i).ensureStudied();
//            (*mr)[targetLOD].regionTerrainType(i).object().ensureStudied(startLOD, targetLOD);
        _loadingTime.stop();

        // Run the GA for every LOD from the coarsest up to the requested
        for (_currentLOD = startLOD; _currentLOD <= targetLOD; ++_currentLOD) {
            _lodTimes[currentLOD()].start(true);
            
            TerrainSample::LOD & pattern = (*ps)[currentLOD()];
            TerrainSample::LOD & terrain = (*ts)[currentLOD()];

            // Create the low-rez pattern we want the GA to refine
            _setupTimes[currentLOD()].start(true);
            if (currentLOD() != TerrainLOD::minimum()) {
                pattern.resampleFromLOD(currentLOD() - 1);
            }
            tl->ensureAnalyzed(currentLOD());
            _setupTimes[currentLOD()].stop();

            // Now, make a better version at this LOD using the GA
            _processingTimes[currentLOD()].start(true);
            if (currentLOD() != TerrainLOD::minimum()) {
                const Chromosome & best = Superclass::run();
                renderChromosome(terrain, best);
                pattern.createFromRaster(terrain.elevations());
            }
            _processingTimes[currentLOD()].stop();
            _lodTimes[currentLOD()].stop();

        }
        _totalTime.stop();

        // Report on what we did
        INCA_INFO("Terrain generation at " << (int(targetLOD - startLOD) + 1) << " LODs")
        INCA_INFO("Loading terrain data took " << _loadingTime() << " seconds")
        INCA_INFO("-------------------------------------------------------------")
        INCA_INFO("LOD\t" << std::setw(15) << "setup time (s)"
                          << std::setw(15) << "proc. time (s)"
                          << std::setw(15) << "total time (s)"
                          << std::setw(15) << "target size"
                          << std::setw(15) << "actual size")
        for (TerrainLOD lod = startLOD; lod <= targetLOD; ++lod)
            INCA_INFO("[" << lod << "]:\t" << std::setw(15) << _setupTimes[lod]()
                                           << std::setw(15) << _processingTimes[lod]()
                                           << std::setw(15) << _lodTimes[lod]()
                                           << std::setw(15) << (*patternSample())[lod].sizes().stringifyElements("x")
                                           << std::setw(15) << (*terrainSample())[lod].sizes().stringifyElements("x"))
        INCA_INFO("-------------------------------------------------------------")
        INCA_INFO("Total elapsed time: " << _totalTime() << " seconds")

        _running = false;   // All done!

    } catch (GeneticAlgorithmException & e) {
        _running = false;   // We're not running anymore...stuff blew up
        throw e;            // Re-throw for anyone who cares
    }
}

// The initialization operator PMF changes depending on which LOD we're working
// on.
const HeightfieldGA::PMF &
HeightfieldGA::initializationOperatorPMF(const Chromosome & c) const {
    return _initializationOperatorPMF;
}

// The mutation operator PMF changes based on the compatibility heuristic for
// the current gene.
const HeightfieldGA::PMF &
HeightfieldGA::mutationOperatorPMF(const Gene & g) const {
    IndexType offsetOp = mutationOperatorCatalog[OFFSET_OPERATOR];
    IndexType scaleOp  = mutationOperatorCatalog[SCALE_OPERATOR];
    IndexType rotateOp = mutationOperatorCatalog[ROTATION_OPERATOR];
//    _mutationOperatorPMF.element(offsetOp) = (1 - g.compatibility().elevation()) / 3;
//    _mutationOperatorPMF.element(scaleOp)  = (1 - g.compatibility().slope()) / 3;
//    _mutationOperatorPMF.element(rotateOp) = (1 - g.compatibility().angle()) / 3;
#if 0
    INCA_DEBUG("Current gene compatibility is:\n"
               "\tE " << g.compatibility().elevation() <<
               "\t" << offsetOp << " " << _mutationOperatorPMF.probability(offsetOp) << "\n"
               "\tS " << g.compatibility().slope()     <<
               "\t" << scaleOp << " " << _mutationOperatorPMF.probability(scaleOp) << "\n"
               "\tA " << g.compatibility().angle()     <<
               "\t" << rotateOp << " " << _mutationOperatorPMF.probability(rotateOp))
    INCA_DEBUG("PMF is " << _mutationOperatorPMF)
    std::cerr << "Done!!" << std::endl;
#endif
    return _mutationOperatorPMF;
}


// Modified fitness calculation function to cache fitness results in Chromosome
HeightfieldGA::Scalar HeightfieldGA::calculateFitness(Chromosome & c) {
    c.fitness().overall() = Superclass::calculateFitness(c);
    return c.fitness().overall();
}


// XXX Junk function
void HeightfieldGA::test(TerrainLOD lod) {
    _currentLOD = lod;
    INCA_DEBUG("Testing fitness function for " << currentLOD())

    setPopulationSize(1);
    MapRasterizationPtr mr = _patternSample->mapRasterization();
    TerrainLibraryPtr tl = mr->terrainLibrary();
    Scalar libraryFitness = 0;

    // For every TerrainType...
    for (IDType ttid = 1; ttid < IDType(tl->size()); ++ttid) {
        TerrainTypePtr   tt = tl->terrainType(ttid);
        Scalar typeFitness = 0;
        
        // For every TerrainSample...
        for (IDType tsid = 0; tsid < IDType(tt->size()); ++tsid) {
            TerrainSamplePtr ts = tt->terrainSample(tsid);
            
            // Initialize the map to be of the right size and TT ID
            IDMap ids((*ts)[currentLOD()].sizes());
            raster::fill(ids, ttid);
            (*mr)[currentLOD()].createFromRaster(ids);
            ts->setMapRasterization(mr);
            
            // Make a chromosome from it, and then check its fitness
            setPatternSample(ts);
            Chromosome & c = chromosome(0);
            initializationOperator(1)(c);
            Scalar sampleFitness = fitnessOperator(1)(c);
            INCA_DEBUG("Fitness for TS " << ts->name() << " is " << sampleFitness)
            
            // Average it in to the self-fitness for this TerrainType
            typeFitness += sampleFitness;
        
            std::cerr << "\n\n------------------\n\n";
        }
        
        // Compute the mean fitness for this TerrainType
        typeFitness /= tt->size();
        INCA_DEBUG("Fitness for TT " << tt->name() << " is " << typeFitness)
        
        // Average it in to the self-fitness for the TerrainLibrary
        libraryFitness += typeFitness;
        
        std::cerr << "\n\n==================\n\n";
    }
    
    // Compute the mean fitness for the TerrainLibrary
    libraryFitness /= (tl->size() - 1);
    INCA_DEBUG("Fitness for TL is " << libraryFitness)
}

TerrainSamplePtr HeightfieldGA::redo(TerrainSamplePtr ts, TerrainLOD lod) {
    setPopulationSize(1);
    _currentLOD = lod;
    setPatternSample(ts);
    Chromosome & c = chromosome(0);
    initializationOperator(1)(c);
    TerrainSamplePtr rs(new TerrainSample());
    renderChromosome((*rs)[lod], c);
    (*rs)[lod].analyze();
    return rs;
}
