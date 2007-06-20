/*
 * File: terrain-operations.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */


// Import function prototypes and class definitions
#include "terrain-operations.hpp"

// Import raster operations
#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/statistic>
#include <inca/raster/operators/select>
#include <inca/raster/operators/linear_map>
#include <inca/raster/operators/rotate>

// Import Timer definition
#include <inca/util/Timer>

using namespace inca;
using namespace inca::math;
using namespace inca::raster;
using namespace terrainosaurus;


void terrainosaurus::naiveBlend(TerrainSample::LOD & tsl, int borderWidth) {
    const MapRasterization::LOD & map = tsl.mapRasterization();

    Heightfield elevations(map.sizes());

#if 0
    // If there is only one region, then this is cinch-y
    if (tsl.regionCount() == 1) {
        const TerrainType::LOD & tt = map.terrainType(0, 0);
        const TerrainSample::LOD & ex = tt.randomTerrainSample();
        elevations =

    
    
    // Otherwise, we have to blend 'n' stuff
    } else {
#endif
        Heightfield maskBlend(map.sizes());
        inca::raster::fill(maskBlend, 0.0f);
        
        // Start with an empty elevation raster
        inca::raster::fill(elevations, 0.0f);

        // Blend in a random chunk for each region in the map
        RandomUniform<IndexType> randomIndex;
        for (IDType rID = 0; rID < IDType(map.regionCount()); ++rID) {
            GrayscaleImage mask = map.regionMask(rID, borderWidth);
            maskBlend += mask;

            // Choose a sample from the appropriate TerrainType
            const TerrainType::LOD & tt = map.terrainType(map.regionSeed(rID));
            const TerrainSample::LOD & ex = tt.randomTerrainSample();

            // Choose a random starting point in the sample
            // FIXME ???
            Pixel maxStart;
            Pixel exSz(ex.sizes());
            Pixel mkSz(mask.sizes());
            maxStart[0] = ex.size(0) - mask.size(0);
            maxStart[1] = ex.size(1) - mask.size(1);
            INCA_DEBUG("Max is " << maxStart)
            INCA_DEBUG("Ex is " << exSz)
            INCA_DEBUG("Mask is " << mkSz)
            if (maxStart[0] < 0 || maxStart[1] < 0) {
                INCA_DEBUG("Argh! TerrainSample " << ex.index()
                        << " (sizes " << ex.size(0) << "-" << ex.size(1) << ") of TerrainType "
                        << tt.name() << " is too small to supply data for "
                        << "region " << rID << " with sizes " << mask.size(0) << "-" << mask.size(1)
                        << " (" << maxStart << ")")
                continue;
            }
            Pixel start;
            if (maxStart[0] > 0)
                start[0] = randomIndex(0, maxStart[0]);
            else
                start[0] = 0;
            if (maxStart[1] > 0)
                start[1] = randomIndex(0, maxStart[1]);
            else
                start[1] = 0;

            // Blend in this chunk
            INCA_DEBUG("Blending in region " << rID << " from sample "
                    << ex.index() << " of TerrainType " << tt.name()
                    << " from " << start)
            elevations += mask * selectBS(ex.elevations(), start, mask.sizes());
//            GrayscaleImage ones(mask.sizes());
//            fill(ones, 1.0f);
//            elevations += mask * select(ones, mask.sizes());
        }
//    }
    fill(elevations, 0.0f);

    // Initialize the elevations in the TerrainSample
    tsl.createFromRaster(elevations);
//    tsl.createFromRaster(maskBlend);
}


void terrainosaurus::renderChromosome(TerrainSample::LOD & tsl,
                                      const TerrainChromosome & c) {
    // Augment the heightfield with an alpha channel
    Heightfield elevations, sum;

    // Make the heightfields the correct size
    elevations.setSizes(c.heightfieldSizes());
    sum.setSizes(c.heightfieldSizes());

    // Start from empty
    fill(elevations, 0.0f);
    fill(sum, 0.0f);

    // Splat each gene into the augmented heightfield
    for (int i = 0; i < c.size(0); ++i)
        for (int j = 0; j < c.size(1); ++j)
            renderGene(elevations, sum, c.gene(i, j));

    // Divide out the alpha channel to produce a blended heightfield
    elevations /= sum;

//    // Go back and clean up any NaN's that appeared due to divide-by-zero
//    scalar_t m_hf = mean(hf),
//             m_s = mean(sum),
//             mn = m_hf / m_s;
//    for (int i = 0; i < hf.size(0); ++i)
//        for (int j = 0; j < hf.size(1); ++j)
//            if (sum(i, j) == scalar_t(0))
//                hf(i, j) = mn;

    tsl.createFromRaster(elevations);
}

void terrainosaurus::renderGene(Heightfield & elevations,
                                Heightfield & sum,
                                const TerrainChromosome::Gene & g) {
    // Select the source region and apply the gene's transformation to it
    const GrayscaleImage & mask = gaussianMask(g.levelOfDetail());
    Dimension size(mask.sizes());
    Pixel stS = g.sourceCenter() - size / 2,
          edS = stS + size,
          stT = g.targetCenter() - size / 2,
          edT = stT + size;

    // Add the transformed, masked pixels to the HF and the mask itself to sum
    const TerrainSample::LOD & sample = g.terrainSample();
    scalar_t mean = sample.localElevationMean(g.sourceCenter());
    selectBE(elevations, stT, edT) += mask *
            selectBE(
                linear_map(
                    rotate(sample.elevations(), g.rotation(), g.sourceCenter()),
                    g.scale(), g.offset() + mean * (1 - g.scale())
                ),
                stS, edS);
    selectBE(sum, stT, edT) += mask;

#if 0
    cerr << "ElevationRange of mask: " << range(*g.mask) << endl;
    cerr << "RMS of mask: " << rms(*g.mask) << endl;
    cerr << "ElevationRange of gene: " << range(*(g.mask) * selectBE(g.sourceSample->heightfield(g.levelOfDetail), stS, edS)) << endl;
    cerr << "ElevationRange of output: " << range(selectBE(hf, stT, edT)) << endl;
    cerr << "ElevationRange of sum: " << range(selectBE(sum, stT, edT)) << endl;
    cerr << "Rendering gene from " << g.sourceCoordinates << " to " << g.targetCoordinates << endl;
#endif
}

// Compute the aggregate fitness of a TerrainLibrary LOD
scalar_t terrainosaurus::terrainLibraryFitness(const TerrainLibrary::LOD & tl,
                                               bool print) {
    TerrainLOD lod = tl.levelOfDetail();

    if (print) {
        INCA_INFO("")
        INCA_INFO("================================================")
        INCA_INFO("Evaluating fitness of TL(" << lod << ")")
    }

    // Compute the average fitness of all terrain types
    scalar_t libraryFitness = 0;
    for (IDType ttid = 1; ttid < IDType(tl.size()); ++ttid)
        libraryFitness += terrainTypeFitness(tl.terrainType(ttid), print);
    libraryFitness /= (tl.size() - 1);

    if (print) {
        INCA_INFO("Fitness of TL(" << lod << ") is " << libraryFitness)
        INCA_INFO("================================================")
        INCA_INFO("")
    }
    
    return libraryFitness;
}

// Compute the aggregate fitness of a TerrainType LOD
scalar_t terrainosaurus::terrainTypeFitness(const TerrainType::LOD & tt,
                                            bool print) {
    if (print) {
        INCA_INFO("")
        INCA_INFO("  ++++++++++++++++++++++++++++++++++++++++++++")
        INCA_INFO("  Evaluating fitness of TT(" << tt.name() << ")")
    }

    // Compute the average fitness of all terrain samples
    scalar_t typeFitness = 0;
    for (IDType tsid = 0; tsid < IDType(tt.size()); ++tsid)
        typeFitness += terrainSampleFitness(tt.terrainSample(tsid), print);
    typeFitness /= tt.size();

    if (print) {
        INCA_INFO("  Fitness for TT " << tt.name() << " is " << typeFitness)
        INCA_INFO("  ++++++++++++++++++++++++++++++++++++++++++++")
        INCA_INFO("")
    }
    
    return typeFitness;
}

// Compute the aggregate fitness of a TerrainSample LOD
scalar_t terrainosaurus::terrainSampleFitness(const TerrainSample::LOD & ts,
                                              bool print) {
    if (print) {
        INCA_INFO("")
        INCA_INFO("    ----------------------------------------")
    }

    // Compute the average fitness of all regions, weighted by area
    scalar_t sampleFitness = 0;
    for (IDType rid = 0; rid < IDType(ts.regionCount()); ++rid) {
        RegionSimilarityMeasure fitness = terrainRegionSimilarity(ts, rid, print);
        sampleFitness += fitness.overall() * ts.regionArea(rid);
    }
    sampleFitness /= ts.size();

    if (print) {
        INCA_INFO("    Fitness for TS " << ts.name() << " is " << sampleFitness)
        INCA_INFO("    ----------------------------------------")
        INCA_INFO("")
    }
    
    return sampleFitness;
}


// Compute the similarity between a region within a TerrainSample
// and a reference TerrainType
RegionSimilarityMeasure
terrainosaurus::terrainRegionSimilarity(const TerrainSample::LOD & ts,
                                        IDType regionID, bool print) {
    // The multi-valued fitness measure we're calculating
    RegionSimilarityMeasure values, ref, fitness;

    // Find the measurements for the reference TT
    const TerrainType::LOD & tt = ts.regionTerrainType(regionID);

    // Compute aggregate region fitness
    fitness.elevation()    = tt.elevationDistribution().match(ts.regionElevationStatistics(regionID));
    fitness.slope()        = tt.slopeDistribution().match(ts.regionSlopeStatistics(regionID));
    fitness.edgeLength()   = tt.edgeLengthDistribution().match(ts.regionEdgeLengthStatistics(regionID));
    fitness.edgeScale()    = tt.edgeScaleDistribution().match(ts.regionEdgeScaleStatistics(regionID));
    fitness.edgeStrength() = tt.edgeStrengthDistribution().match(ts.regionEdgeStrengthStatistics(regionID));
    fitness.overall() = tt.elevationWeight() * fitness.elevation()
                      + tt.slopeWeight() * fitness.slope();

    fitness.overall() += tt.edgeLengthWeight() * (std::isnan(fitness.edgeLength())
                                                    ? 0.5f
                                                    : fitness.edgeLength());
    fitness.overall() += tt.edgeScaleWeight() * (std::isnan(fitness.edgeScale())
                                                    ? 0.5f
                                                    : fitness.edgeScale());
    fitness.overall() += tt.edgeStrengthWeight() * (std::isnan(fitness.edgeStrength())
                                                    ? 0.5f
                                                    : fitness.edgeStrength());
        
    // Barf out the fitness thingy
    if (print) {
        INCA_INFO("      Region " << regionID
                  << "  fitness(" << fitness.overall() << ')'
                  << "  area(" << ts.regionArea(regionID) << ')')
        INCA_INFO("        Elevation     " << fitness.elevation())
        INCA_INFO("        Slope         " << fitness.slope())
        INCA_INFO("        Edge Length   " << fitness.edgeLength())
        INCA_INFO("        Edge Scale    " << fitness.edgeScale())
        INCA_INFO("        Edge Strength " << fitness.edgeStrength())
    }

    return fitness;
}


// The mean elevation across a slot (i, j) in a Chromosome
scalar_t terrainosaurus::elevationMean(const TerrainChromosome & c,
                                       IndexType i, IndexType j) {
    return c.pattern().localElevationMean(c(i, j).targetCenter());
}

// The mean gradient across a slot (i, j) in a Chromosome
const Vector2D & terrainosaurus::gradientMean(const TerrainChromosome & c,
                                              IndexType i, IndexType j) {
    return c.pattern().localGradientMean(c(i, j).targetCenter());
}

// The elevation range (min, max) across a slot (i, j) in a Chromosome
const Vector2D & terrainosaurus::elevationLimits(const TerrainChromosome & c,
                                                IndexType i, IndexType j) {
    return c.pattern().localElevationLimits(c(i, j).targetCenter());
}

// The slope range (min, max) across a slot (i, j) in a Chromosome
const Vector2D & terrainosaurus::slopeLimits(const TerrainChromosome & c,
                                            IndexType i, IndexType j) {
    return c.pattern().localSlopeLimits(c(i, j).targetCenter());
}

// The elevation range across a slot (i, j) in a Chromosome
scalar_t terrainosaurus::elevationRange(const TerrainChromosome & c,
                                        IndexType i, IndexType j) {
    const Vector2D & limits = elevationLimits(c, i, j);
    return limits[1] - limits[0];
}

// The slope range across a slot (i, j) in a Chromosome
scalar_t terrainosaurus::slopeRange(const TerrainChromosome & c,
                                    IndexType i, IndexType j) {
    const Vector2D & limits = slopeLimits(c, i, j);
    return limits[1] - limits[0];
}

scalar_t terrainosaurus::patternElevationMean(const TerrainChromosome::Gene & g) {
    return g.parent().pattern().localElevationMean(g.targetCenter());
}
const Vector2D & terrainosaurus::patternGradientMean(const TerrainChromosome::Gene & g) {
    return g.parent().pattern().localGradientMean(g.targetCenter());
}
const Vector2D & terrainosaurus::patternElevationLimits(const TerrainChromosome::Gene & g) {
    return g.parent().pattern().localElevationLimits(g.targetCenter());
}
const Vector2D & terrainosaurus::patternSlopeLimits(const TerrainChromosome::Gene & g) {
    return g.parent().pattern().localSlopeLimits(g.targetCenter());
}
scalar_t terrainosaurus::patternElevationRange(const TerrainChromosome::Gene & g) {
    const Vector2D & limits = patternElevationLimits(g);
    return limits[1] - limits[0];
}
scalar_t terrainosaurus::patternSlopeRange(const TerrainChromosome::Gene & g) {
    const Vector2D & limits = patternSlopeLimits(g);
    return limits[1] - limits[0];
}


// The mean elevation across the gene (including transformations)
scalar_t terrainosaurus::elevationMean(const TerrainChromosome::Gene & g) {
    scalar_t mean = g.terrainSample().localElevationMean(g.sourceCenter());
    return mean + g.offset();
}

// The mean gradient across the gene (including transformations)
Vector2D terrainosaurus::gradientMean(const TerrainChromosome::Gene & g) {
    Vector2D result = g.terrainSample().localGradientMean(g.sourceCenter());
    return math::rotate(result, g.rotation()) * g.scale();
}

// The elevation range [min, max] across the gene (including transformations)
Vector2D terrainosaurus::elevationLimits(const TerrainChromosome::Gene & g) {
    Vector2D range = g.terrainSample().localElevationLimits(g.sourceCenter());
    scalar_t mean = g.terrainSample().localElevationMean(g.sourceCenter());
    return range * g.scale() + Vector2D(g.offset() + mean * (1 - g.scale()));
}

// The slope range [min, max] across the gene (including transformations)
Vector2D terrainosaurus::slopeLimits(const TerrainChromosome::Gene & g) {
    Vector2D result = g.terrainSample().localSlopeLimits(g.sourceCenter());
    return result * g.scale();
}

scalar_t terrainosaurus::elevationRange(const TerrainChromosome::Gene & g) {
    Vector2D limits = elevationLimits(g);
    return limits[1] - limits[0];
}
scalar_t terrainosaurus::slopeRange(const TerrainChromosome::Gene & g) {
    Vector2D limits = slopeLimits(g);
    return limits[1] - limits[0];
}


scalar_t terrainosaurus::terrainTypeElevationMean(const TerrainChromosome::Gene & g) {
    return g.terrainSample().globalElevationStatistics().mean();
}
scalar_t terrainosaurus::terrainTypeSlopeMean(const TerrainChromosome::Gene & g) {
    return g.terrainSample().globalSlopeStatistics().mean();
}
Vector2D terrainosaurus::terrainTypeElevationLimits(const TerrainChromosome::Gene & g) {
    return Vector2D(g.terrainSample().globalElevationStatistics().min(),
                    g.terrainSample().globalElevationStatistics().max());
}
Vector2D terrainosaurus::terrainTypeSlopeLimits(const TerrainChromosome::Gene & g) {
    return Vector2D(g.terrainSample().globalSlopeStatistics().min(),
                    g.terrainSample().globalSlopeStatistics().max());
}
scalar_t terrainosaurus::terrainTypeElevationRange(const TerrainChromosome::Gene & g) {
    Vector2D limits = terrainTypeElevationLimits(g);
    return limits[1] - limits[0];
}
scalar_t terrainosaurus::terrainTypeSlopeRange(const TerrainChromosome::Gene & g) {
    Vector2D limits = terrainTypeSlopeLimits(g);
    return limits[1] - limits[0];
}


scalar_t terrainosaurus::terrainTypeElevationVariance(const TerrainChromosome::Gene & g) {
    scalar_t range = g.terrainSample().globalElevationStatistics().range();
    return range * range / 8;
}
scalar_t terrainosaurus::terrainTypeSlopeVariance(const TerrainChromosome::Gene & g) {
    return g.terrainSample().globalSlopeStatistics().range() / 4;
}
scalar_t terrainosaurus::terrainTypeAngleVariance(const TerrainChromosome::Gene & g) {
    return inca::math::PI<scalar_t>() / 4;
}
