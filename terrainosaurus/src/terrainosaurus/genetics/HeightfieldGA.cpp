/**
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
using namespace terrainosaurus;
using namespace inca;
using namespace inca::math;


namespace terrainosaurus {
    // Forward declarations of operator classes
    class RandomSourceDataInitializationOperator;

    class RectangularRegionCrossoverOperator;

    class ResetTransformMutationOperator;
    class VerticalOffsetMutationOperator;
    class VerticalScaleMutationOperator;
    class VerticalRotationMutationOperator;
    class HorizontalTranslationMutationOperator;
    class HorizontalJitterMutationOperator;
    class ConformMutationOperator;

    class GeneCompatibilityFitnessOperator;
    class RegionCharacteristicsFitnessOperator;
}

// HACK-age
#if 1
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

#else
#define POPULATION_SIZE        5
#define EVOLUTION_CYCLES       5
#define SELECTION_RATIO        0.75f
#define ELITE_RATIO            0.2f
#define MUTATION_PROBABILITY   0.1f
#define MUTATION_RATIO         0.5f
#define CROSSOVER_PROBABILITY  0.2f
#define CROSSOVER_RATIO        0.5f

#define MAX_CROSSOVER_WIDTH    4
#define MAX_JITTER_PIXELS      0
#define MAX_SCALE_FACTOR       1.1f
#define MAX_OFFSET_AMOUNT      50.0f
#endif


#include <terrainosaurus/genetics/terrain-operations.hpp>


/**
 * The RandomSourceDataInitializationOperator implements a simple
 * initialization operator that populates the chromosome with random data from
 * the TerrainLibrary.
 */
class terrainosaurus::RandomSourceDataInitializationOperator
        : public HeightfieldGA::InitializationOperator {
public:
    void operator()(Chromosome & c) {
        // Get the LOD and the MapRasterization from the GA
        HeightfieldGA & ga = static_cast<HeightfieldGA &>(owner());
        TerrainLOD lod = ga.currentLOD();
        MapRasterization::LOD & mr = (*ga.mapRasterization())[lod];
        TerrainSample::LOD & ts = (*ga.terrainSample())[lod];

        // Tell the chromosome about them
        c.setPattern(ts);
        c.setMap(mr);
        c.setLevelOfDetail(lod);

        // Figure out how many genes we want in each direction
        Dimension hfSize(mr.sizes());
        SizeType spacing = blendPatchSpacing(lod);
        Dimension size = hfSize / spacing + Dimension(1);
        if (hfSize[0] % spacing != 0) size[0]++;    // Round up # of genes
        if (hfSize[1] % spacing != 0) size[1]++;
        c.resize(size);

        // Populate the gene grid with random data
        Pixel idx;
        for (idx[0] = 0; idx[0] < c.size(0); ++idx[0])
            for (idx[1] = 0; idx[1] < c.size(1); ++idx[1]) {
                Gene & g = c(idx);

                // Figure out what TerrainType this should be and pick some sample data
                const TerrainType::LOD & tt = mr.terrainType(g.targetCenter());
                g.setTerrainSample(tt.randomTerrainSample());
//                g.setTerrainSample(tt.terrainSample(0));

                // Reset the transformation parameters
                g.reset();

                // Pick a random X,Y coordinate pair within the safe region of that sample
                Dimension sampleSizes(g.terrainSample().sizes());
                SizeType radius = SizeType(blendFalloffRadius(tt.levelOfDetail()));
                Pixel center(randomInt(radius, sampleSizes[0] - radius),
                             randomInt(radius, sampleSizes[1] - radius));
                g.setSourceCenter(center);
            }

        INCA_DEBUG("Initialized chromosome with "
                << c.size(0) << "x" << c.size(1) << " genes")
    }

protected:
    RandomUniform<int> randomInt;
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
    // Constructor
    explicit RectangularRegionCrossoverOperator(scalar_t r = CROSSOVER_RATIO,
                                                int w = MAX_CROSSOVER_WIDTH)
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
        scalar_t changeMean = patternElevationMean(g) - elevationMean(g);
        scalar_t changeScale = patternElevationRange(g) / elevationRange(g);
        scalar_t changeAngle = signedAngle(patternGradientMean(g),
                                           gradientMean(g));
        g.setOffset(g.offset() + changeMean);
        if (! isnan(changeScale))
            g.setScale(g.scale() * changeScale);
        g.setRotation(g.rotation() + changeAngle);
        g.setRotation(0.0f);
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

                if (isnan(g.compatibility())) {
                    INCA_DEBUG("Compatibility for " << px)
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
        return compat / c.genes().size();
    }
};


/**
 * The RegionCharacteristicsFitnessOperator implements a fitness operator
 * returning the area-weighted average of region fitnesses. The fitness of a
 * region is determined by evaluating the terrain characteristics for the
 * region and comparing them to those of its terrain type.
 */
class terrainosaurus::RegionCharacteristicsFitnessOperator
        : public HeightfieldGA::FitnessOperator {
public:
    Scalar operator()(Chromosome & c) {
        Heightfield hf;
        renderChromosome(hf, c);
        TerrainSamplePtr ts(new TerrainSample(hf, c.levelOfDetail()));
        const MapRasterization::LOD & map = c.map();
        const TerrainSample::LOD & candidate = (*ts)[c.levelOfDetail()];
        const TerrainSample::LOD & reference = c(0, 0).terrainSample();

        // Analyze the candidate HF, then compare values to the reference
//        candidate.ensureAnalyzed();
        Scalar fitness = 0;
        for (IDType regionID = 0; regionID < map.regionCount(); ++regionID) {
            Scalar regFit  = _regionFitness(regionID);
            Scalar regArea = map.regionArea(regionID);
            fitness += regFit / regArea;
        }
        return fitness;
    }

protected:
    Scalar _regionFitness(IDType regionID) {
        Scalar fitness = 0;

        return fitness;
    }
};


// Constructor
HeightfieldGA::HeightfieldGA(MapRasterizationPtr mr, TerrainSamplePtr ts ) {
    // Not doing anything yet...
    _running = false;
    _currentLOD = TerrainLOD::minimum();

    // Set up the MR and TS
    setMapRasterization(mr);
    setTerrainSample(ts);

    // Set up the initialization operators
    addInitializationOperator(new RandomSourceDataInitializationOperator());

    // Set up the crossover operators
    addCrossoverOperator(new RectangularRegionCrossoverOperator());

    // Set up the mutation operators
//    addMutationOperator(new ResetTransformMutationOperator());
    addMutationOperator(new VerticalOffsetMutationOperator(1.5f));
    addMutationOperator(new VerticalScaleMutationOperator(1.5f));
    addMutationOperator(new VerticalRotationMutationOperator(1.5f));
//    addMutationOperator(new HorizontalTranslationMutationOperator());
//    addMutationOperator(new HorizontalJitterMutationOperator());

    // Set up the fitness calculation operators
    addFitnessOperator(new GeneCompatibilityFitnessOperator());
    addFitnessOperator(new RegionCharacteristicsFitnessOperator(), 0);

    // Set up the initial parameters for the GA
    setPopulationSize(POPULATION_SIZE);
    setEvolutionCycles(EVOLUTION_CYCLES);
    setCrossoverProbability(CROSSOVER_PROBABILITY);
    setMutationProbability(MUTATION_PROBABILITY);
    setSelectionRatio(SELECTION_RATIO);
    setElitismRatio(ELITE_RATIO);
    setCrossoverRatio(CROSSOVER_RATIO);
    setMutationRatio(MUTATION_RATIO);
}

// Destructor
HeightfieldGA::~HeightfieldGA() { }


// The MapRasterization that we're trying to follow
MapRasterizationPtr HeightfieldGA::mapRasterization() {
    return _mapRasterization;
}
MapRasterizationConstPtr HeightfieldGA::mapRasterization() const {
    return _mapRasterization;
}
void HeightfieldGA::setMapRasterization(MapRasterizationPtr mr) {
    _mapRasterization = mr;
}

// The TerrainSample that we're building
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
TerrainSamplePtr HeightfieldGA::run(MapRasterizationPtr mr,
                                    TerrainLOD startLOD, TerrainLOD targetLOD) {
    setMapRasterization(mr);
    return run(startLOD, targetLOD);
}
TerrainSamplePtr HeightfieldGA::run(TerrainLOD startLOD, TerrainLOD targetLOD) {
    _running = true;

    try {
        // If we don't have a TerrainSample, we'll just make one
        if (! terrainSample())
            _terrainSample.reset(new TerrainSample());

        // If we don't have a map or a library, MAYDAY! MAYDAY!
        MapRasterizationPtr mr = mapRasterization();
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
        for (IndexType i = 0; i < (*mr)[targetLOD].regionCount(); ++i)
            (*mr)[targetLOD].regionTerrainType(i).object().ensureLoaded(startLOD, targetLOD);
        _loadingTime.stop();

        // Get the TerrainSample we're working on
        TerrainSamplePtr ts = terrainSample();

        // Run the GA for every LOD from the coarsest up to the requested
        for (_currentLOD = startLOD; _currentLOD <= targetLOD; ++_currentLOD) {
            _lodTimes[currentLOD()].start(true);

            // Create the low-rez pattern we want the GA to refine
            _setupTimes[currentLOD()].start(true);
            if (currentLOD() == TerrainLOD::minimum()) {    // Build something crappy from scratch
                const int borderWidth = int(samplesPerMeterForLOD(currentLOD()) * 1000);
                (*ts)[currentLOD()].createFromRaster(naiveBlend((*mr)[currentLOD()], borderWidth));
            } else {                                // Just scale up the previous LOD
                (*ts)[currentLOD()].resampleFromLOD(currentLOD() - 1);
            }
            _setupTimes[currentLOD()].stop();

            // Now, make a better version at this LOD using the GA
            _processingTimes[currentLOD()].start(true);
            if (currentLOD() != TerrainLOD::minimum()) {
                const Chromosome & best = Superclass::run();
                INCA_DEBUG("Map for 'Mr. Right' is " << &best.map())
                Heightfield hf;
                renderChromosome(hf, best);
                (*ts)[currentLOD()].createFromRaster(hf);
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
                                            << std::setw(15) << (*mapRasterization())[lod].sizes().stringifyElements("x")
                                            << std::setw(15) << (*terrainSample())[lod].sizes().stringifyElements("x"))
        INCA_INFO("-------------------------------------------------------------")
        INCA_INFO("Total elapsed time: " << _totalTime() << " seconds")

        _running = false;   // All done!

        // Return the finished product
        return ts;

    } catch (GeneticAlgorithmException & e) {
        _running = false;   // We're not running anymore...stuff blew up
        throw e;            // Re-throw for anyone who cares
    }
}

const HeightfieldGA::PMF & HeightfieldGA::mutationOperatorPMF(const Gene & g) const {

//    _mutationOperatorPMF.element(0) = (1 - g.compatibility().elevation()) / 3;
//    _mutationOperatorPMF.element(1) = (1 - g.compatibility().slope()) / 3;
//    _mutationOperatorPMF.element(2) = (1 - g.compatibility().angle()) / 3;
    return _mutationOperatorPMF;
}
