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


// Import application class
#include <terrainosaurus/MapExplorer.hpp>

// Import function prototypes and class definitions
#include "terrain-operations.hpp"
#include "terrain-ga.hpp"

// Import raster operations
#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/statistic>
#include <inca/raster/operators/select>
#include <inca/raster/operators/linear_map>
#include <inca/raster/operators/rotate>

using namespace inca::math;
using namespace inca::raster;
using namespace terrainosaurus;


// Generate a heightfield corresponding to the rasterized chunk of map
// specified by 'map', by repeatedly scaling up low-resolution data and
// applying the GA to refine it, up to 'targetLOD'.
TerrainSamplePtr terrainosaurus::generateTerrain(MapRasterizationConstPtr map,
                                                 TerrainLOD startLOD,
                                                 TerrainLOD targetLOD) {
    Timer<float, false> total, loading, pass, generation, refinement;
    total.start();

    // Preload all of the TerrainTypes we'll be using, for every LOD we'll be
    // using.
    loading.start();
    std::cerr << "Preloading terrain sample data" << std::endl;
    TerrainLibraryConstPtr tl = map->terrainLibrary();
    const MapRasterization::LOD & maxMap = (*map)[targetLOD];
    for (IndexType i = 0; i < maxMap.regionCount(); ++i)
        maxMap.regionTerrainType(i).object().ensureLoaded(startLOD, targetLOD);
    loading.stop();


    std::stringstream report;
    report << "Terrain generation at " << (int(targetLOD - startLOD) + 1) << " LODs\n";
    report << "Loading terrain data took " << loading() << " seconds\n";
    report << "\t-------------------------------------------------------------\n";
    report << "\tLOD\t"
           << std::setw(15) << "gen. time (s)"
           << std::setw(15) << "ref. time (s)"
           << std::setw(15) << "total time (s)"
           << std::setw(15) << "target size"
           << std::setw(15) << "actual size"
           << std::endl;

    // We're trying to build a TerrainSample containing all the LODs of our terrain
    TerrainSamplePtr result(new TerrainSample());

    // Run the GA for every LOD from the coarsest up to the requested
    for (TerrainLOD lod = startLOD; lod <= targetLOD; ++lod) {
        pass.reset();
        pass.start();

        // Create the low-rez pattern we want the GA to refine
        generation.reset();
        generation.start();
        if (lod == startLOD) {      // Build something crappy from scratch
            const int borderWidth = int(samplesPerMeterForLOD(lod) * 1000);
            (*result)[lod].createFromRaster(naiveBlend((*map)[lod], borderWidth));
        } else {                                // Just scale up the previous LOD
            (*result)[lod].resampleFromLOD(lod - 1);
        }
        generation.stop();

        // Now, make a better version at this LOD using the GA
        refinement.reset();
        refinement.start();
        if (lod != startLOD) {
            (*result)[lod].createFromRaster(refineHeightfield((*result)[lod],
                                                              (*map)[lod]));
        }
        refinement.stop();
        pass.stop();

        // Report on what we did
        report << "\t[" << lod << "]:\t"
               << std::setw(15) << generation()
               << std::setw(15) << refinement()
               << std::setw(15) << pass()
               << std::setw(15) << (*map)[lod].sizes().stringifyElements("x")
               << std::setw(15) << (*result)[lod].sizes().stringifyElements("x")
               << std::endl;
    }
    total.stop();

    report << "\t-------------------------------------------------------------\n"
              "\tTotal elapsed time: " << total() << " seconds\n";

    // Return the finished product
    std::cerr << report.str();
    return result;
}


Heightfield terrainosaurus::naiveBlend(const MapRasterization::LOD & map,
                                       int borderWidth) {
    Heightfield result(map.sizes());
    fill(result, 0.0f);

    // Blend in a random chunk for each region
    RandomUniform<IndexType> randomIndex;
    for (IndexType i = 0; i < map.regionCount(); ++i) {
        GrayscaleImage mask = map.regionMask(i, borderWidth);

        // Choose a sample from the appropriate TerrainType
        const TerrainType::LOD & tt = map.terrainType(map.regionSeed(i));
        const TerrainSample::LOD & ts = tt.randomTerrainSample();

        // Choose a random starting point in the sample
        Pixel maxStart(ts.size(0) - mask.size(0),
                       ts.size(1) - mask.size(1));
        if (maxStart[0] < 0 || maxStart[1] < 0) {
            std::cerr << "Argh! TerrainSample " << ts.index()
                      << " (sizes " << ts.sizes() << ") of TerrainType "
                      << tt.name() << " is too small to supply data for "
                      << "region " << i << " with sizes " << mask.sizes()
                      << std::endl;
            continue;
        }
        Pixel start;
        if (maxStart[0] > 0)
            start[0] = randomIndex(0, ts.size(0) - mask.size(0));
        else
            start[0] = 0;
        if (maxStart[1] > 0)
            start[1] = randomIndex(0, ts.size(1) - mask.size(1));
        else
            start[1] = 0;

        // Blend in this chunk
        std::cerr << "Blending in region " << i << " from sample "
                  << ts.index() << " of TerrainType " << tt.name()
                  << " from " << start << std::endl;
        result += mask * selectBS(ts.elevations(), start, mask.sizes());
    }
    return result;
}


void terrainosaurus::renderChromosome(Heightfield & hf,
                                      const TerrainChromosome & c) {
    // Augment the heightfield with an alpha channel
    Heightfield sum;

    // Make the output heightfield the correct size
    hf.setSizes(c.heightfieldSizes());
    sum.setSizes(hf.sizes());

    // Start from empty
    fill(hf, 0.0f);
    fill(sum, 0.0f);

    // Splat each gene into the augmented heightfield
    for (int i = 0; i < c.size(0); ++i)
        for (int j = 0; j < c.size(1); ++j)
            renderGene(hf, sum, c.gene(i, j));

    // Divide out the alpha channel to produce a blended heightfield
    hf /= sum;

    // Go back and clean up any NaN's that appeared due to divide-by-zero
//    scalar_t m_hf = mean(hf),
//             m_s = mean(sum),
//             mn = m_hf / m_s;
//    for (int i = 0; i < hf.size(0); ++i)
//        for (int j = 0; j < hf.size(1); ++j)
//            if (sum(i, j) == scalar_t(0))
//                hf(i, j) = mn;
}

void terrainosaurus::renderGene(Heightfield & hf,
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
    selectBE(hf, stT, edT)  += mask * selectBE(
                                            linear_map(
                                                rotate(sample.elevations(), g.rotation()),
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
const Vector2D & terrainosaurus::elevationRange(const TerrainChromosome & c,
                                                IndexType i, IndexType j) {
    return c.pattern().localElevationRange(c(i, j).targetCenter());
}

// The slope range (min, max) across a slot (i, j) in a Chromosome
const Vector2D & terrainosaurus::slopeRange(const TerrainChromosome & c,
                                            IndexType i, IndexType j) {
    return c.pattern().localSlopeRange(c(i, j).targetCenter());
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
Vector2D terrainosaurus::elevationRange(const TerrainChromosome::Gene & g) {
    Vector2D range = g.terrainSample().localElevationRange(g.sourceCenter());
    scalar_t mean = g.terrainSample().localElevationMean(g.sourceCenter());
    return range * g.scale() + Vector2D(g.offset() + mean * (1 - g.scale()));
}

// The slope range [min, max] across the gene (including transformations)
Vector2D terrainosaurus::slopeRange(const TerrainChromosome::Gene & g) {
    Vector2D result = g.terrainSample().localSlopeRange(g.sourceCenter());
    return result * g.scale();
}
