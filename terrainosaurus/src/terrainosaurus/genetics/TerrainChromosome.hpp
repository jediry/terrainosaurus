/*
 * File: TerrainChromosome.hpp
 *
 * Author: Ryan L. Saunders & Mike Ong
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file contains the data structures used in the heightfield
 *      construction genetic algorithm.
 *
 *      HeightfieldChromosomes are the individual entities that interact
 *      during the genetic algorithm. A TerrainChromosome contains the
 *      instructions for generating a complete heightfield at a particular
 *      level-of-detail using the samples in a TerrainLibrary.
 *
 *      Genes are the fundamental building blocks of the  genetic algorithm.
 *      A Gene represents a "chunk" of terrain data taken from a particular
 *      terrain sample, at a particular level of detail. The data may
 *      additionally have an affine transformation applied to it, encoded as
 *      a vertical scale and offset and a rotation.
 */

#ifndef TERRAINOSAURUS_GENETICS_TERRAIN_CHROMOSOME
#define TERRAINOSAURUS_GENETICS_TERRAIN_CHROMOSOME

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class ChromosomeFitnessMeasure;
    class RegionFitnessMeasure;
    class GeneCompatibilityMeasure;
    class GeneShape;
    struct GAEvent;
    class GAListener;
    class TerrainChromosome;

    // Pointer typedefs
    typedef shared_ptr<GAListener>       GAListenerPtr;
    typedef shared_ptr<GAListener const> GAListenerConstPtr;
};


// Import container definitions
#include <vector>
#include <inca/util/MultiArray>

// Import terrain data structures
#include <terrainosaurus/data/TerrainLibrary.hpp>
#include <terrainosaurus/data/TerrainType.hpp>
#include <terrainosaurus/data/TerrainSample.hpp>
#include <terrainosaurus/data/MapRasterization.hpp>


// This macro makes it easy to create named accessor functions in the fitness
// measure classes
#define NAMED_ACCESSOR(NAME, INDEX)                                         \
    scalar_t & NAME()       { return (*this)[INDEX]; }                      \
    scalar_t   NAME() const { return (*this)[INDEX]; }


// The multivariate fitness measure for a TerrainChromosome
class terrainosaurus::ChromosomeFitnessMeasure : public inca::Array<scalar_t, 2> {
public:
    // Base class
    typedef inca::Array<scalar_t, 2>    Superclass;

    // Default constructor
    ChromosomeFitnessMeasure() : Superclass(scalar_t(0)) { }

    // If this is used in a scalar context, we return the overall fitness. E.g.:
    //      scalar_t overallFitness = fitnessMeasure;
    operator scalar_t() const { return overall(); }
    operator scalar_t &()     { return overall(); }

    // Aggregate fitness accessors
    NAMED_ACCESSOR(overall,               0);
    NAMED_ACCESSOR(normalized,            1);
};

class terrainosaurus::RegionFitnessMeasure : public inca::Array<scalar_t, 5> {
public:
    // Base class
    typedef inca::Array<scalar_t, 5>    Superclass;

    // Default constructor
    RegionFitnessMeasure() : Superclass(scalar_t(0)) { }

    // If this is used in a scalar context, we return the overall fitness. E.g.:
    //      scalar_t overallFitness = fitnessMeasure;
    operator scalar_t() const { return overall(); }
    operator scalar_t &()     { return overall(); }

    // Aggregate fitness accessors
    NAMED_ACCESSOR(overall,               0);
    NAMED_ACCESSOR(normalized,            1);

    // Component fitness accessors
    NAMED_ACCESSOR(elevationRMS,          2);
    NAMED_ACCESSOR(gradientMagnitudeRMS,  3);
    NAMED_ACCESSOR(gradientAngleRMS,      4);
};


class terrainosaurus::GeneCompatibilityMeasure : public inca::Array<scalar_t, 5> {
public:
    // Base class
    typedef inca::Array<scalar_t, 5>    Superclass;

    // Default constructor
    GeneCompatibilityMeasure() : Superclass(scalar_t(0)) { }

    // If this is used in a scalar context, we return the overall fitness. E.g.:
    //      scalar_t overallFitness = fitnessMeasure;
    operator scalar_t() const { return overall(); }
    operator scalar_t &()     { return overall(); }

    // Aggregate compatibility accessors
    NAMED_ACCESSOR(overall,               0);
    NAMED_ACCESSOR(normalized,            1);

    // Component compatibility accessors
    NAMED_ACCESSOR(elevation,             2);
    NAMED_ACCESSOR(angle,                 3);
    NAMED_ACCESSOR(slope,                 4);
};


class terrainosaurus::GeneShape {
public:
    // The height and gradient in one segment of the shape
    struct Cell {
        scalar_t h, gx, gy;
    };

    // The grid of cells making up this Shape
    typedef inca::MultiArray<Cell, 2> CellGrid;

    // How many Cells along each axis
    static const int segments = 3;

    // Constructor
    explicit GeneShape() : _grid(segments, segments) { }

    // Cell accessors
    Cell & operator()(IndexType i, IndexType j) {
        return _grid(i, j);
    }
    const Cell & operator()(IndexType i, IndexType j) const {
        return _grid(i, j);
    }

protected:
    CellGrid _grid; // Our NxN grid of cells
};


// Clean up the preprocessor namespace
#undef NAMED_ACCESSOR

// Event class for sending to GA listeners
struct terrainosaurus::GAEvent {
    // Enumeration of the different event types
    enum Type { None, Mutation, Crossover };

    // Default constructor
    explicit GAEvent() : type(None), timestep(0) { }

    // Initializing constructor
    explicit GAEvent(Type t, int ts) : type(t), timestep(ts) { }

    Type type;      // What sort of event is this?
    int timestep;   // When did it happen?
};


/*****************************************************************************
 * The chromosome for the terrain-construction algorithm
 *****************************************************************************/
class terrainosaurus::TerrainChromosome {
/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // Forward declaration of the "Gene" inner class, which represents a
    // small patch of terrain.
    class Gene;

    // Multivariate fitness & compatibility measures
    typedef terrainosaurus::ChromosomeFitnessMeasure    ChromosomeFitnessMeasure;
    typedef terrainosaurus::RegionFitnessMeasure        RegionFitnessMeasure;
    typedef terrainosaurus::GeneCompatibilityMeasure    GeneCompatibilityMeasure;

    // Two dimensional grid of Genes
    typedef inca::MultiArray<Gene, 2>   GeneGrid;
    typedef GeneGrid::SizeArray         SizeArray;
    typedef GeneGrid::IndexArray        IndexArray;


/*---------------------------------------------------------------------------*
 | Constructors & initialization functions
 *---------------------------------------------------------------------------*/
public:
    // Constructor
    TerrainChromosome();
    TerrainChromosome(const TerrainChromosome & tc);

protected:
    void claimGenes();


/*---------------------------------------------------------------------------*
 | Event listener (hacked in until ??)
 *---------------------------------------------------------------------------*/
public:
    void addGAListener(GAListenerPtr p);
    void removeGAListener(GAListenerPtr p);

    // XXX HACK ought to be protected
    void fireAdvanced(int timestep);
    void fireMutated(Pixel idx, int timestep);
    void fireCrossed(Pixel idx, int timestep);

protected:
    GAListenerPtr _listener;


/*---------------------------------------------------------------------------*
 | Gene grid accessor & mutator functions
 *---------------------------------------------------------------------------*/
public:
    // Direct access to the GeneGrid object
          GeneGrid & genes()       { return _genes; }
    const GeneGrid & genes() const { return _genes; }

    // Access to individual genes, using the function call operator (...)
          Gene & operator()(IndexType i, IndexType j)       { return gene(i, j); }
    const Gene & operator()(IndexType i, IndexType j) const { return gene(i, j); }
    template <class IndexList>
    Gene & operator()(const IndexList & idx) { return gene(idx); }
    template <class IndexList>
    const Gene & operator()(const IndexList & idx) const { return gene(idx); }

    // Access to individual genes, using the gene(...) function
          Gene & gene(IndexType i, IndexType j)       { return _genes(i, j); }
    const Gene & gene(IndexType i, IndexType j) const { return _genes(i, j); }
    template <class IndexList>
    Gene & gene(const IndexList & idx) {
        return _genes(idx);
    }
    template <class IndexList>
    const Gene & gene(const IndexList & idx) const {
        return _genes(idx);
    }

    // Size accessors
    SizeType size() const { return _genes.size(); }
    SizeType size(IndexType d) const { return _genes.size(d); }
    const SizeArray & sizes() const { return _genes.sizes(); }

    // Resize the grid of genes, specifying whether or not to preserve the
    // current contents. If preservation is requested, then any indices which
    // are valid in both the old and new dimensions will be preserved. Genes
    // that are created as a result of the resize are left uninitialized.
    void resize(const Dimension &sz, bool preserveContents = false);
    void resize(SizeType si, SizeType sj, bool preserveContents = false);
    template <class Collection>
        void resize(const Collection &sz, bool preserveContents = false) {
            Dimension d(sz);
            resize(d, preserveContents);
        }


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
public:
    // Fitness measurement accessors: whole HF and each region
          ChromosomeFitnessMeasure & fitness();
    const ChromosomeFitnessMeasure & fitness() const;
          RegionFitnessMeasure & regionFitness(IDType regionID);
    const RegionFitnessMeasure & regionFitness(IDType regionID) const;


    // What level of detail are we?
    TerrainLOD levelOfDetail() const;
    void setLevelOfDetail(TerrainLOD lod);

    // Heightfield property accessors
    const Heightfield::SizeArray & heightfieldSizes() const;

    // Access to the pattern and rasterized map
    const TerrainSample::LOD & pattern() const;
    void setPattern(const TerrainSample::LOD & p);
    const MapRasterization::LOD & map() const;
    void setMap(const MapRasterization::LOD & m);

    // Is this chromosome "alive" (part of the active population)?
    bool isAlive() const;
    void setAlive(bool alive);

protected:
    // The contents of this Chromosome
    TerrainSampleConstPtr       _pattern;   // What we're trying to match
    MapRasterizationConstPtr    _map;       // How the terrain is laid out
    TerrainLOD                  _lod;       // What level of detail are we?

    GeneGrid        _genes;     // The genes making up this chromosome
    bool            _alive;     // Is it allowed to go to the next cycle?
    std::vector<RegionFitnessMeasure>   _regionFitnesses;
    ChromosomeFitnessMeasure            _fitness;
};


// XXX OBSOLETE
// Listener interface for observing the progress of the GA
class terrainosaurus::GAListener {
public:
    virtual void advanced(int timestep) = 0;
    virtual void mutated(const TerrainChromosome::Gene & g, GAEvent e) = 0;
    virtual void crossed(const TerrainChromosome::Gene & g, GAEvent e) = 0;
};


/*****************************************************************************
 * The gene for the terrain-construction algorithm
 *****************************************************************************/
class terrainosaurus::TerrainChromosome::Gene {
/*---------------------------------------------------------------------------*
 | Connections to TerrainChromosome
 *---------------------------------------------------------------------------*/
public:
    // Chromosome relationship accessors
          TerrainChromosome & parent();
    const TerrainChromosome & parent() const;
    const Pixel & indices() const;

protected:
    // We give our parent class permission to call the following function
    friend class TerrainChromosome;

    // This function is called by the parent TerrainChromosome to claim
    // ownership and to inform it of its X,Y coordinates within the grid
    void claim(TerrainChromosome * p, TerrainLOD lod, const Pixel & idx);

    // Link to the parent Chromosome, and position within parent
    TerrainChromosome *     _parent;        // Daddy!
    Pixel                   _indices;       // My genetic coordinates


/*---------------------------------------------------------------------------*
 | Source heightfield & terrain-type data
 *---------------------------------------------------------------------------*/
public:
    // What LOD are we working with?
    TerrainLOD levelOfDetail() const;

    // What TerrainType and TerrainSample do we represent?
    const TerrainType::LOD   & terrainType() const;
    const TerrainSample::LOD & terrainSample() const;
    void setTerrainSample(const TerrainSample::LOD & ts);

    // How compatibile are we with our pattern geometry?
          GeneCompatibilityMeasure & compatibility();
    const GeneCompatibilityMeasure & compatibility() const;

protected:
    TerrainLOD                  _levelOfDetail;
    TerrainSample::LOD const *  _terrainSample;
    GeneCompatibilityMeasure    _compatibility;


/*---------------------------------------------------------------------------*
 | Data fields
 *---------------------------------------------------------------------------*/
public:
    // Assignment operator (preserves parent pointer and indices, but
    // copies data fields)
    Gene & operator=(const Gene & g);

    // The pixel blending mask we're using to splat this gene.
    const GrayscaleImage & mask() const;

    // Function to reset the transformation parameters
    void reset();

    // The pixel indices (within the source sample) of the center of our data
    const Pixel & sourceCenter() const;
    void setSourceCenter(const Pixel & p);

    // The pixel indices (within the resulting, generated heightfield) where
    // this Gene will center its data. This field cannot be set directly, but
    // is derived from the gene's indices() and jitter().
    const Pixel & targetCenter() const;

    // A scalar amount, in radians, by which to rotate the elevation data.
    scalar_t rotation() const;
    void setRotation(scalar_arg_t r);

    // A scalar factor by which to scale the elevation data around its local
    // mean. In other words, the mean will remain the same, but the range will
    // increase or decrease.
    scalar_t scale() const;
    void setScale(scalar_arg_t s);

    // A scalar amount by which to offset the elevation data from its local
    // mean. The elevation range is not changed by this.
    scalar_t offset() const;
    void setOffset(scalar_arg_t o);

    // An amount in pixels by which to jitter the gene's target center-point.
    const Offset & jitter() const;
    void setJitter(const Offset & j);

protected:
    // Transformation to apply to source data
    Pixel               _sourceCenter;  // Source X,Y center coordinates
    Pixel               _targetCenter;  // Where does it go? (This is derived
                                        // from jitter and indices)
    scalar_t            _rotation;      // Horizontal rotation in radians
    scalar_t            _scale;         // Vertical scale factor
    scalar_t            _offset;        // Vertical offset amount
    Offset              _jitter;        // How much to adjust the
                                        // target center
};


/*****************************************************************************
 * Free gene functions
 *****************************************************************************/
namespace terrainosaurus {
    // Function to swap two genes
    void swap(TerrainChromosome::Gene & g1, TerrainChromosome::Gene & g2);
}

#endif
