/*
 * File: terrain-operations.cpp
 *
 * Author: Ryan L. Saunders & Mike Ong
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
#include <terrainosaurus/rendering/MapRasterization.hpp>

// Import raster operations
#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/statistic>
#include <inca/raster/operators/selection>
#include <inca/raster/operators/transformation>

using namespace inca::math;
using namespace inca::raster;
using namespace terrainosaurus;
using TerrainChromosome::Gene;


// This function runs the entire GA
void terrainosaurus::generateTerrain(Heightfield & hf, MapConstPtr m,
                                     const Block & region, IndexType lod) {
    scalar_t resolution = MapExplorer::instance().terrainLibrary()->resolution(lod);
//    Dimension d(SizeType(region.size(0) * resolution),
//                SizeType(region.size(1) * resolution));
    Dimension d(m->terrainLibrary->terrainType(1)->samples[0]->elevation(0).sizes());
    cerr << "Generating terrain of size " << d[0] << "x" << d[1] << endl;

#if 0
    // Make sure we have elevation data in-memory for the terrain types
    // we are going to need
    cerr << "Preloading in-use TerrainTypes:" << endl;
    TerrainLibraryPtr library = map->terrainLibrary;
    std::vector<bool> inUse(library->size(), false);
    Map::face_const_iterator r, rEnd = m->faces().end();
    for (r = m->faces().begin(); r != rEnd; ++r)
        inUse[r->terrainType()->id()] = true;
    std::vector<bool>::iterator i;
    for (int i = 0; i < library->size(); ++i)
        if (inUse[i]) {
            TerrainTypePtr tt = library->terrainType(i);
            cerr << "\t" << tt->name << ": "
                 << tt->samples.size() << "samples" << endl;
            tt->ensureLoaded();
        }
#endif

    // Have the rendering system determine region membership at each LOD
    MapRasterization raster(m);//, region);
    TerrainChromosome c;
    TerrainTypeConstPtr tt = m->terrainLibrary->terrainType(1);
    tt->samples[0]->ensureAnalyzed();
    initializeChromosome(c, 0, tt->samples[0], raster);
    TerrainChromosome dd = c;
    for (int it = 0; it < 20; ++it) {
        std::cerr << "Beginning naive GA iteration " << it << std::endl;
        for (int i = 0; i < c.size(0); ++i) {
            std::cerr << "Row " << i << std::endl;
            for (int j = 0; j < c.size(1); ++j) {
                TerrainChromosome::Gene g = createRandomGene(tt, 0);
                if (evaluateCompatibility(c, i, j, g) > evaluateCompatibility(c, i, j, c.gene(i, j)))
                    c.gene(i, j) = g;
            }
        }
        evaluateFitness(dd);
    }
    renderChromosome(hf, dd);
}


// This function makes a chromosome from a heightfield
void terrainosaurus::initializeChromosome(TerrainChromosome & c, IndexType lod,
                                          TerrainSampleConstPtr pattern,
                                          const MapRasterization & raster) {
    // Point the Chromosome to its pattern/LOD
    c._pattern = pattern;
    c._levelOfDetail = lod;

    // Figure out how many genes we want in each direction    
    Dimension size = Dimension(pattern->sizes(lod)) / TerrainChromosome::geneSpacing(lod);
    c.resize(size);

    // Populate the gene grid with random data
    Pixel idx;
    for (idx[0] = 0; idx[0] < size[0]; ++idx[0])
        for (idx[1] = 0; idx[1] < size[1]; ++idx[1])
            c.gene(idx) = createRandomGene(raster.terrainTypeAt(idx, lod), lod);

    cerr << "Initialized chromosome with " << size[0] << "x" << size[1] << " genes\n";
}


// This function creates a random Gene given a terrain type and LOD
TerrainChromosome::Gene
terrainosaurus::createRandomGene(TerrainTypeConstPtr tt, IndexType lod) {

    // The random number generators we'll use
    RandomUniform<IndexType> randomIndex;
    RandomUniform<scalar_t>  randomScalar;

    // The Gene we're initializing
    TerrainChromosome::Gene g;

    // First, pick a random sample from the requested TerrainType
    g.terrainType = tt;
    randomIndex.max = tt->samples.size() - 1;
    g.sourceSample = tt->samples[randomIndex()];

    // Radius and alpha mask at this LOD
    g.levelOfDetail = lod;
    IndexType radius = IndexType(TerrainChromosome::geneRadius(lod));
    g.mask = TerrainChromosome::geneMask(lod);

    // Pick a random X,Y coordinate pair within the safe region of that sample
    Dimension size  = Dimension(g.sourceSample->sizes(lod));
    randomIndex.min = radius;
    randomIndex.max = size[0] - radius; g.sourceCoordinates[0] = randomIndex();
    randomIndex.max = size[1] - radius; g.sourceCoordinates[1] = randomIndex();

    // Pick a random transformation
    randomScalar.max = PI<scalar_t>();  g.rotation  = randomScalar();
    randomScalar.max = scalar_t(10);    g.offset    = randomScalar();
    randomScalar.min = scalar_t(0.2);
    randomScalar.max = scalar_t(5);     g.scale     = randomScalar();

    // Pick a random jitter around the target coordinates
//    randomIndex.min = IndexType(-(1 - TerrainChromosome::geneOverlapFactor(lod)) * radius);
    randomIndex.min = IndexType(0);
    randomIndex.max = IndexType( 2 * (1 - TerrainChromosome::geneOverlapFactor(lod)) * radius);
    g.targetJitter[0] = randomIndex();
    g.targetJitter[1] = randomIndex();
    
    return g;
}


/*
void terrainosaurus::incrementLevelOfDetail(TerrainChromosome & dst,
                                            const TerrainChromosome & src) {
    // Turn the src TerrainChromosome into a heightfield
    Heightfield hf;
    renderChromosome(hf, src);

    // Scale up to the size of the next map LOD
    rescale<Bicubic>(hf, src.map->rasterization(src.levelOfDetail + 1).dimensions());

    // Generate a higher LOD TerrainChromosome by processing the scaled image
    initializeChromosome(dst, hf);
}*/


void terrainosaurus::renderChromosome(Heightfield & hf,
                                      const TerrainChromosome & c) {
    // Augment the heightfield with an alpha channel
    Heightfield sum; 

    // Make the output heightfield the correct size
//    hf.resize(c.map->rasterization(c.levelOfDetail).dimensions());
    hf.resize(c.heightfieldSizes());
    sum.resize(hf.sizes());

    // Start from empty
    hf = 0.0f;
    sum = 0.0f;

    // Splat each gene into the augmented heightfield
    for (int i = 0; i < c.size(0); ++i)
        for (int j = 0; j < c.size(1); ++j)
            renderGene(hf, sum, c.gene(i, j));
//    renderGene(hf, sum, c.gene(10, 10));
//    std::cerr << "Mean gradient is " << gradient(c.gene(10, 10)) << std::endl;
//    std::cerr << "ElevationRange around target is " << range(c.gene(10, 10)) << std::endl;

    // XXX HACK
    scalar_t m_hf = mean(hf),
             m_s = mean(sum),
             mn = m_hf / m_s;

    // Divide out the alpha channel to produce a blended heightfield
    hf /= sum;

    for (int i = 0; i < hf.size(0); ++i)
        for (int j = 0; j < hf.size(1); ++j)
            if (sum(i, j) == scalar_t(0))
                hf(i, j) = mn;
}

void terrainosaurus::renderGene(Heightfield & hf,
                                Heightfield & sum,
                                const TerrainChromosome::Gene & g) {
    // Select the source region and apply the gene's transformation to it
    Dimension size(g.mask->sizes());
    Pixel stS = g.sourceCoordinates - size / 2,
          edS = stS + size,
          stT = g.targetCoordinates - size / 2,
          edT = stT + size;

    // Add the transformed, masked pixels to the HF and the mask itself to sum
    select(hf, stT, edT)  += *(g.mask) * select(
                                            linearMap(
                                                rotate(g.sourceSample->elevation(g.levelOfDetail), g.rotation),
                                                g.scale, g.offset
                                            ),
                                            stS, edS);
    select(sum, stT, edT) += *(g.mask);
#if 0
    cerr << "ElevationRange of mask: " << range(*g.mask) << endl;
    cerr << "RMS of mask: " << rms(*g.mask) << endl;
    cerr << "ElevationRange of gene: " << range(*(g.mask) * select(g.sourceSample->heightfield(g.levelOfDetail), stS, edS)) << endl;
    cerr << "ElevationRange of output: " << range(select(hf, stT, edT)) << endl;
    cerr << "ElevationRange of sum: " << range(select(sum, stT, edT)) << endl;
    cerr << "Rendering gene from " << g.sourceCoordinates << " to " << g.targetCoordinates << endl;
#endif
}


// This function determines how "good" a chromosome is
scalar_t terrainosaurus::evaluateFitness(const TerrainChromosome & c) {
    // First, turn the TerrainChromosome back into a heightfield
    Heightfield hf;
    renderChromosome(hf, c);

//    Heightfield & orig = const_cast<Heightfield &>(c.gene(0,0).sourceSample->elevation(c.levelOfDetail()));
    const Heightfield & orig = c.gene(0,0).sourceSample->elevation(c.levelOfDetail());

    // Calculate the fitness of each individual terrain region
    // TODO: per-region fitness

    // Calculate the fitness of each individual terrain seam
    // TODO: per-seam fitness

    // Calculate global fitness estimates
    // TODO: global fitness

    // Determine the aggregate fitness estimate
    // TODO: aggregate fitness

    float fitness = 1.0f - std::log10(1.0f + rms(orig - hf)) / 5.0f;
    std::cerr << "evaluateFitness(" << hf.sizes() << "): " << fitness << endl;
    if (fitness < 0.0f) {
        fitness = 0.0f;
        std::cerr << "    returning 0.0f\n";
    }
    return fitness;
}


// This function determines how good a match two genes are
scalar_t terrainosaurus::evaluateCompatibility(const TerrainChromosome & c,
                                               IndexType i, IndexType j,
                                               const TerrainChromosome::Gene & g) {
    IndexType lod = c.levelOfDetail();
    Vector2D gGrad = gradientMean(g),
             cGrad = gradientMean(c, i, j);
    scalar_t diffMag   = abs(magnitude(gGrad) - magnitude(cGrad)),
             diffAngle = angle(gGrad, cGrad),
             diffMean  = abs(elevationMean(g) - elevationMean(c, i, j));
    Vector2D diffRange = abs(elevationRange(g) - elevationRange(c, i, j));
    scalar_t maxMag = magnitude(c.pattern()->globalGradientMean(lod)) * 20,
             maxAngle = PI<scalar_t>(),
             maxElevation = c.pattern()->globalElevationRange(lod)[1];
    scalar_t compatibility =  scalar_t(1) - (diffMag / maxMag
                                           + diffAngle / maxAngle
                                           + diffMean / maxElevation
                                           + diffRange[0] / maxElevation
                                           + diffRange[1] / maxElevation) / 5;
#if 0
    std::cerr << "Differences:\n"
              << "\tgradient magnitude: " << diffMag << "\n"
              << "\tgradient angle:     " << diffAngle << "\n"
              << "\tmean elevation:     " << diffMean << "\n"
              << "\televation range:    " << diffRange << "\n";
    std::cerr << "Reference maxima:\n"
              << "\tgradient magnitude: " << maxMag << "\n"
              << "\tgradient angle:     " << maxAngle << "\n"
              << "\televation:          " << maxElevation << "\n";
    std::cerr << "Aggregate compatibility: " << compatibility << endl;
#endif
    return compatibility;
}


// This mean elevation across a slot (i, j) in a Chromosome
scalar_t terrainosaurus::elevationMean(const TerrainChromosome & c,
                                       IndexType i, IndexType j) {
    return c.pattern()->localElevationMean(c.levelOfDetail())(c.gene(i, j).targetCoordinates);
}

// This mean gradient across a slot (i, j) in a Chromosome
const Vector2D & terrainosaurus::gradientMean(const TerrainChromosome & c,
                                              IndexType i, IndexType j) {
    return c.pattern()->localGradientMean(c.levelOfDetail())(c.gene(i, j).targetCoordinates);
}

// This elevation range (min, max) across a slot (i, j) in a Chromosome
const Vector2D & terrainosaurus::elevationRange(const TerrainChromosome & c,
                                                IndexType i, IndexType j) {
    return c.pattern()->localElevationRange(c.levelOfDetail())(c.gene(i, j).targetCoordinates);
}


// The mean elevation across the gene (including transformations)
scalar_t terrainosaurus::elevationMean(const TerrainChromosome::Gene & g) {
    scalar_t result = g.sourceSample->localElevationMean(g.levelOfDetail)(g.sourceCoordinates);
    return result * g.scale + g.offset;
}

// The mean gradient across the gene (including transformations)
Vector2D terrainosaurus::gradientMean(const TerrainChromosome::Gene & g) {
    Vector2D result = g.sourceSample->localGradientMean(g.levelOfDetail)(g.sourceCoordinates);
    return math::rotate(result, g.rotation) * g.scale; 
}

// The elevation range [min, max] across the gene (including transformations)
Vector2D terrainosaurus::elevationRange(const TerrainChromosome::Gene & g) {
    Vector2D result = g.sourceSample->localElevationRange(g.levelOfDetail)(g.sourceCoordinates);
    return result * g.scale + Vector2D(g.offset);
}
