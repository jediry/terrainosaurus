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
    Dimension d(m->terrainLibrary->terrainType(1)->samples[0]->heightfield(0).sizes());
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
    m->terrainLibrary->terrainType(1)->samples[0]->ensureLoaded();
    TerrainChromosome::initializeStatic();
    TerrainChromosome c;
    initializeChromosome(c, 0, m->terrainLibrary->terrainType(1)->samples[0]->heightfield(0), raster);
    TerrainChromosome dd = c;
//    evaluateFitness(c);
    renderChromosome(hf, dd);
    std::cerr << "Range of result is " << range(hf) << endl;
//    raster.rasterize(d);
//    hf.resize(d);
}


// This function makes a chromosome from a heightfield
void terrainosaurus::initializeChromosome(TerrainChromosome & c, IndexType lod,
                                          const Heightfield & pattern,
                                          const MapRasterization & raster) {
    // Figure out how many genes we want in each direction    
    Dimension size = Dimension(pattern.sizes()) / TerrainChromosome::geneSpacing(lod);
    c.resize(size);
    c._levelOfDetail = lod;
    c._heightfieldSize = Dimension(pattern.sizes());

    // Populate the gene grid with random data
    Pixel idx;
    for (idx[0] = 0; idx[0] < size[0]; ++idx[0])
        for (idx[1] = 0; idx[1] < size[1]; ++idx[1])
            c.gene(idx) = createRandomGene(raster.terrainTypeAt(idx, lod), lod);

    cerr << "Initialized chromosome with " << size[0] << "x" << size[1] << " genes\n";
}


// This function creates a random Gene given a terrain type and LOD
Gene terrainosaurus::createRandomGene(TerrainTypeConstPtr tt, IndexType lod) {

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
    IndexType radius = IndexType(TerrainChromosome::geneRadius(lod));
    g.mask = TerrainChromosome::geneMask(lod);

    // Pick a random X,Y coordinate pair within the safe region of that sample
    Dimension size  = Dimension(g.sourceSample->size(lod));
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
    hf.resize(c._heightfieldSize);
    sum.resize(hf.sizes());

    // Start from empty
    hf = 0.0f;
    sum = 0.0f;

    // Splat each gene into the augmented heightfield
//    for (int i = 0; i < c.size(0); ++i)
//        for (int j = 0; j < c.size(1); ++j)
//            renderGene(hf, sum, c.gene(i, j));
    renderGene(hf, sum, c.gene(10, 10));
    std::cerr << "Mean gradient is " << gradient(c.gene(10, 10)) << std::endl;
    std::cerr << "Range around target is " << range(c.gene(10, 10)) << std::endl;

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
                                                rotate(g.sourceSample->heightfield(g.levelOfDetail()), g.rotation),
                                                g.scale, g.offset
                                            ),
                                            stS, edS);
    select(sum, stT, edT) += *(g.mask);
#if 0
    cerr << "Range of mask: " << range(*g.mask) << endl;
    cerr << "RMS of mask: " << rms(*g.mask) << endl;
    cerr << "Range of gene: " << range(*(g.mask) * select(g.sourceSample->heightfield(g.levelOfDetail()), stS, edS)) << endl;
    cerr << "Range of output: " << range(select(hf, stT, edT)) << endl;
    cerr << "Range of sum: " << range(select(sum, stT, edT)) << endl;
    cerr << "Rendering gene from " << g.sourceCoordinates << " to " << g.targetCoordinates << endl;
#endif
}


// This function determines how "good" a chromosome is
float terrainosaurus::evaluateFitness(const TerrainChromosome & c) {
    // First, turn the TerrainChromosome back into a heightfield
    Heightfield hf;
    renderChromosome(hf, c);

    cerr << "Size of generated heightfield is " << Dimension(hf.sizes()) << endl;
    Heightfield & orig = const_cast<Heightfield &>(c.gene(0,0).sourceSample->heightfield(c.levelOfDetail()));
//    Heightfield diff = hf - orig;
    Dimension size(math::min(orig.size(0), hf.size(0)), math::min(orig.size(1), hf.size(1)));
    scalar_t rtms = 0.0f;
    for (int i = 0; i < size[0]; ++i)
        for (int j = 0; j < size[1]; ++j)
            rtms += (hf(i,j) - orig(i,j)) * (hf(i,j) - orig(i,j));
    rtms = sqrt(rtms / size(0) * size(1));
    cerr << "RMS value was " << rtms << endl;
    cerr << "Functor measured it as " << rms(orig - hf) << endl;

#if 0
    Heightfield::Iterator i = diff.begin();
    scalar_t rms = 0.0f;
    for (i = diff.begin(); i != diff.end(); ++i)
        rms += *i * *i;
    rms = sqrt(rms / diff.size());
    cerr << "RMS value was " << rms << endl;
#endif
    // Calculate the fitness of each individual terrain region
    // TODO: per-region fitness

    // Calculate the fitness of each individual terrain seam
    // TODO: per-seam fitness

    // Calculate global fitness estimates
    // TODO: global fitness

    // Determine the aggregate fitness estimate
    // TODO: aggregate fitness

    
    return 1.0f;
}


// This function determines how good a match two genes are
float terrainosaurus::evaluateCompatibility(const TerrainChromosome::Gene & g1,
                                            const TerrainChromosome::Gene & g2) {
    return 1.0f;
}


// This function calculates the average gradient across the gene
Vector2D terrainosaurus::gradient(const TerrainChromosome::Gene & g) {
    Vector2D result = g.sourceSample->averageGradient(g.levelOfDetail())(g.sourceCoordinates);
    if (! effectivelyZero(g.rotation))
        result = inca::math::rotate(result, g.rotation);
    if (! effectivelyEqual(scalar_t(1), g.scale))
        result *= g.scale;
    return result;
}

// This function calculates the absolute height range across the gene
Vector2D terrainosaurus::range(const TerrainChromosome::Gene & g) {
    Vector2D result = g.sourceSample->localRange(g.levelOfDetail())(g.sourceCoordinates);
    if (! effectivelyEqual(scalar_t(1), g.scale))
        result *= g.scale;
    return result;
}

