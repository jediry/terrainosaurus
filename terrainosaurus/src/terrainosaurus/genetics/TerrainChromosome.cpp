// Import class definitions
#include "TerrainChromosome.hpp"
using namespace terrainosaurus;
using TerrainChromosome::Gene;

// Import raster operators
#include <inca/raster/operators/transformation>
using namespace inca::raster;


/*---------------------------------------------------------------------------*
 | Static LOD-dependent query functions
 *---------------------------------------------------------------------------*/
bool TerrainChromosome::staticInitialized = false;
std::vector<scalar_t>       TerrainChromosome::geneRadii;
std::vector<GrayscaleImage> TerrainChromosome::geneMasks;

void TerrainChromosome::initializeStatic() {
    geneRadii.push_back(16);

    // Generate an appropriately-sized alpha mask
    int lod = 0;
    int maskSize = 2 * int(geneRadius(lod));
    geneMasks.push_back(GrayscaleImage(maskSize, maskSize));
    GrayscaleImage & mask = geneMasks[0];
    for (int i = 0; i < mask.size(0); ++i)
        for (int j = 0; j < mask.size(1); ++j) {
            scalar_t dx, dy;
            dx = abs(i - mask.size(0) / 2) / geneRadius(lod);
            dy = abs(j - mask.size(1) / 2) / geneRadius(lod);
            if (dx < 0) dx = 0;
            if (dy < 0) dy = 0;
#if 0
            // Frustum
            if (dx < mask.size(0) / 4)  dx = 1.0;
            else                        dx = (mask.size(0) / 2 - dx) * 2;
            if (dy < mask.size(1) / 4)  dy = 1.0;
            else                        dy = (mask.size(1) / 2 - dy) * 2;
            mask(i, j) = dx * dy;
#elif 0
            // r^1 falloff
            mask(i, j) = scalar_t(1) - sqrt(dx * dx + dy * dy);
            if (mask(i, j) < 0) mask(i, j) = 0;
#elif 1
            // r^2 falloff
            mask(i, j) = scalar_t(1) - (dx*dx + dy*dy);
            if (mask(i, j) < 0) mask(i, j) = 0;
#elif 0
            // Product (?)
            mask(i, j) = dx * dy;
#else
            // Constant
            mask(i, j) = 1.0f;
#endif
        }
//    mask = linearMap(mask, Array<scalar_t, 2>(0.0f, 1.0f));
    cerr << "Range of mask is " << range(mask) << endl;

    staticInitialized = true;
}

scalar_t TerrainChromosome::geneRadius(IndexType lod) {
    return geneRadii[0];
}
scalar_t TerrainChromosome::geneOverlapFactor(IndexType lod) {
    return scalar_t(0.5);
}
SizeType TerrainChromosome::geneSpacing(IndexType lod) {
    return SizeType(2 * geneRadius(lod) * (1 - geneOverlapFactor(lod)));
}
GrayscaleImage * TerrainChromosome::geneMask(IndexType lod) {
    return & geneMasks[0];
}


/*---------------------------------------------------------------------------*
 | TerrainChromosome functions
 *---------------------------------------------------------------------------*/
// Constructor
TerrainChromosome::TerrainChromosome() {
    if (! staticInitialized)
        initializeStatic();
}

//TerrainChromosome::TerrainChromosome(const Dimension &pixels, IndexType lod) {
    // Make sure static initialization has been performed
//    if (! staticInitialized)
//        initializeStatic();

//    _levelOfDetail = lod;

//    resize(x, y);
//}

void TerrainChromosome::resize(const Dimension &sz,
                               bool preserveContents) {
    genes().resize(sz, preserveContents); // Become the new size

    // Inform each Gene of its parent and coordinates
    Pixel idx;
    for (idx[0] = 0; idx[0] < sz[0]; ++idx[0])
        for (idx[1] = 0; idx[1] < sz[1]; ++idx[1])
            gene(idx).claim(this, idx);
}
void TerrainChromosome::resize(SizeType sx, SizeType sy,
                               bool preserveContents) {
    resize(Dimension(sx, sy), preserveContents);
}


/*---------------------------------------------------------------------------*
 | Gene functions
 *---------------------------------------------------------------------------*/
// Function called by chromosome to claim ownership of the gene
void Gene::claim(TerrainChromosome * p, const Pixel & idx) {
    _parent = p;
    _indices = idx;
}

// Assignment operator
Gene & Gene::operator=(const Gene &g) {
    // Source data
    terrainType         = g.terrainType;
    sourceSample        = g.sourceSample;
    sourceCoordinates   = g.sourceCoordinates;

    // Transformation data
    rotation    = g.rotation;
    scale       = g.scale;
    offset      = g.offset;

    // Trimming alpha mask
    mask = g.mask;

    // Target data
    targetJitter = g.targetJitter;
    targetCoordinates = indices() * geneSpacing(levelOfDetail()) + targetJitter;

    return *this;
}

