// Import class definitions
#include "TerrainChromosome.hpp"
using namespace terrainosaurus;

// Import raster operators
#include <inca/raster/operators/statistic>
#include <inca/raster/operators/select>
#include <inca/raster/generators/gaussian>
using namespace inca::raster;


/*---------------------------------------------------------------------------*
 | TerrainChromosome constructors
 *---------------------------------------------------------------------------*/
// Constructor
TerrainChromosome::TerrainChromosome()
    : _alive(false) { }

// Copy constructor
TerrainChromosome::TerrainChromosome(const TerrainChromosome & tc) {
    // We're alive only if he is
    _alive = tc.isAlive();

    // Make a grid of genes the right size and claim each of them
    resize(tc.sizes(), false);

    // Copy pointers to data objects
    if (tc._pattern)
        setPattern(tc.pattern());
    if (tc._map)
        setMap(tc.map());

    // Now copy tc's gene data
    genes() = tc.genes();
}

// Notify each gene of its place in the world
void TerrainChromosome::claimGenes() {
    // Inform each Gene of its parent, lod, and coordinates
    Pixel idx;
    TerrainLOD lod = levelOfDetail();
    for (idx[0] = 0; idx[0] < size(0); ++idx[0])
        for (idx[1] = 0; idx[1] < size(0); ++idx[1])
            gene(idx).claim(this, lod, idx);
}


/*---------------------------------------------------------------------------*
 | Event listener (hacked in until ??)
 *---------------------------------------------------------------------------*/
void TerrainChromosome::addGAListener(GAListenerPtr p) {
    _listener = p;
}
void TerrainChromosome::removeGAListener(GAListenerPtr p) {
    _listener.reset();
}

// XXX HACK ought to be protected
void TerrainChromosome::fireAdvanced(int timestep) {
    if (_listener)
        _listener->advanced(timestep);
}
void TerrainChromosome::fireMutated(Pixel idx, int timestep) {
    if (_listener)
        _listener->mutated(gene(idx), GAEvent(GAEvent::Mutation, timestep));
}
void TerrainChromosome::fireCrossed(Pixel idx, int timestep) {
    if (_listener)
        _listener->crossed(gene(idx), GAEvent(GAEvent::Crossover, timestep));
}


/*---------------------------------------------------------------------------*
 | Gene functions
 *---------------------------------------------------------------------------*/
void TerrainChromosome::resize(const Dimension &sz,
                               bool preserveContents) {
    genes().setSizes(sz, preserveContents); // Become the new size
    claimGenes();                           // Notify the newcomers
}
void TerrainChromosome::resize(SizeType si, SizeType sj,
                               bool preserveContents) {
    resize(Dimension(si, sj), preserveContents);
}

// Access to the multivariate fitness measure of the chromosome
TerrainChromosome::ChromosomeFitnessMeasure &
TerrainChromosome::fitness() {
    return _fitness;
}
const TerrainChromosome::ChromosomeFitnessMeasure &
TerrainChromosome::fitness() const {
    return _fitness;
}

// Access to the multivariate fitness measure of the chromosome
TerrainChromosome::RegionFitnessMeasure &
TerrainChromosome::regionFitness(IDType regionID) {
    return _regionFitnesses[regionID];
}
const TerrainChromosome::RegionFitnessMeasure &
TerrainChromosome::regionFitness(IDType regionID) const {
    return _regionFitnesses[regionID];
}

// What level of detail are we?
TerrainLOD TerrainChromosome::levelOfDetail() const {
    return _lod;
}
void TerrainChromosome::setLevelOfDetail(TerrainLOD lod) {
    if (lod != _lod) {
        _lod = lod;
        claimGenes();       // Go tell it on the mountain...
    }                       // that our L-O-D has changed
}

// Access to the heightfield properties of the chromosome
const Heightfield::SizeArray & TerrainChromosome::heightfieldSizes() const {
    return map().sizes();
}


// Access to the pattern sample we're trying to match
const TerrainSample::LOD & TerrainChromosome::pattern() const {
    return (*_pattern)[levelOfDetail()];
}
void TerrainChromosome::setPattern(const TerrainSample::LOD & p) {
    if (&p != NULL) {
        _pattern = p.getObject();
        setLevelOfDetail(p.levelOfDetail());
    } else {
        _pattern.reset();
    }
}

// Access to the rasterized map
const MapRasterization::LOD & TerrainChromosome::map() const {
    return (*_map)[levelOfDetail()];
}
void TerrainChromosome::setMap(const MapRasterization::LOD & m) {
    if (&m != NULL) {
        _map = m.getObject();
        setLevelOfDetail(m.levelOfDetail());
        _regionFitnesses.resize(m.regionCount());
    } else {
        _map.reset();
        _regionFitnesses.resize(0);
    }
}

// Life and death kinda stuff
bool TerrainChromosome::isAlive() const {
    return _alive;
}
void TerrainChromosome::setAlive(bool alive) {
    _alive = alive;
}


/*****************************************************************************
 * Gene class corresponding to TerrainChromosome
 *****************************************************************************/
/*---------------------------------------------------------------------------*
 | Connections to TerrainChromosome
 *---------------------------------------------------------------------------*/
// Function called by chromosome to claim ownership of the gene
void TerrainChromosome::Gene::claim(TerrainChromosome * p,
                                    TerrainLOD lod,
                                    const Pixel & idx) {
    _parent = p;
    _levelOfDetail = lod;
    _indices = idx;
}

// Access to the parent TerrainChromosome
TerrainChromosome & TerrainChromosome::Gene::parent() {
    return *_parent;
}
const TerrainChromosome & TerrainChromosome::Gene::parent() const {
    return *_parent;
}

// Where in our parent's grid are we?
const Pixel & TerrainChromosome::Gene::indices() const { return _indices; }


/*---------------------------------------------------------------------------*
 | Source heightfield & terrain-type data
 *---------------------------------------------------------------------------*/
// Chromosome level of detail
TerrainLOD TerrainChromosome::Gene::levelOfDetail() const { return _levelOfDetail; }

// What TerrainType and TerrainSample do we represent?
const TerrainType::LOD & TerrainChromosome::Gene::terrainType() const {
    return terrainSample().terrainType();
}
const TerrainSample::LOD & TerrainChromosome::Gene::terrainSample() const {
    return *_terrainSample;
}
void TerrainChromosome::Gene::setTerrainSample(const TerrainSample::LOD & ts) {
    _terrainSample = & ts;
}

// How well do we match our pattern geometry?
TerrainChromosome::GeneCompatibilityMeasure &
TerrainChromosome::Gene::compatibility() {
    return _compatibility;
}
const TerrainChromosome::GeneCompatibilityMeasure &
TerrainChromosome::Gene::compatibility() const {
    return _compatibility;
}


/*---------------------------------------------------------------------------*
 | Data fields
 *---------------------------------------------------------------------------*/
// Assignment operator (only copies data fields)
TerrainChromosome::Gene &
TerrainChromosome::Gene::operator=(const TerrainChromosome::Gene &g) {
    // Source data
    setTerrainSample(g.terrainSample());

    // Transformation data
    setSourceCenter(g.sourceCenter());
    setRotation(g.rotation());
    setScale(g.scale());
    setOffset(g.offset());
    setJitter(g.jitter());

    return *this;
}

// The pixel blending mask we're using to splat this gene.
const GrayscaleImage & TerrainChromosome::Gene::mask() const {
    return gaussianMask(levelOfDetail());
}

// The pixel indices (within the source sample) of the center of our data
const Pixel & TerrainChromosome::Gene::sourceCenter() const {
    return _sourceCenter;
}
void TerrainChromosome::Gene::setSourceCenter(const Pixel & p) {
    _sourceCenter = p;
}

// The pixel indices (within the resulting, generated heightfield) where
// this Gene will center its data. This field cannot be set directly, but
// is derived from the gene's indices() and jitter().
const Pixel & TerrainChromosome::Gene::targetCenter() const {
    return _targetCenter;
}

// A scalar amount, in radians, by which to rotate the elevation data.
scalar_t TerrainChromosome::Gene::rotation() const {
    return _rotation;
}
void TerrainChromosome::Gene::setRotation(scalar_arg_t r) {
    _rotation = r;
}


// A scalar factor by which to scale the elevation data around its local
// mean. In other words, the mean will remain the same, but the range will
// increase or decrease.
scalar_t TerrainChromosome::Gene::scale() const {
    return _scale;
}
void TerrainChromosome::Gene::setScale(scalar_arg_t s) {
    if (s > scalar_t(0))    _scale = s;
    else                    _scale = scalar_t(1);
}

// A scalar amount by which to offset the elevation data from its local
// mean. The elevation range is not changed by this.
scalar_t TerrainChromosome::Gene::offset() const {
    return _offset;
}
void TerrainChromosome::Gene::setOffset(scalar_arg_t o) {
    _offset = o;
}

// An amount in pixels by which to jitter the gene's target center-point.
const Offset & TerrainChromosome::Gene::jitter() const {
    return _jitter;
}
void TerrainChromosome::Gene::setJitter(const Offset & j) {
    _jitter = j;
    _targetCenter  = indices() * blendPatchSpacing(levelOfDetail()) + j;
}


/*****************************************************************************
 * Free gene query functions
 *****************************************************************************/
// int terrainosaurus::geneSpacing(TerrainLOD lod) {
//     return int(2 * blendRadius(lod) * (1 - blendOverlapFactor(lod)));
// }
//scalar_t terrainosaurus::geneRadius(TerrainLOD lod) {
//    return windowSize(lod);
//}
//scalar_t terrainosaurus::geneOverlapFactor(TerrainLOD lod) {
//    return scalar_t(0.5);
//}

void terrainosaurus::swap(TerrainChromosome::Gene & g1,
                          TerrainChromosome::Gene & g2) {
    TerrainChromosome::Gene temp = g1;
    g1 = g2;
    g2 = temp;
}
