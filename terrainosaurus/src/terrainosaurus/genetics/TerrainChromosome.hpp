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
    class TerrainChromosome;
    
    // LOD-dependent query functions
    scalar_t geneRadius(IndexType lod);
}


// Import container definitions
#include <inca/util/MultiArray>

// Import the Terrainosaurus map we're generating from
#include <terrainosaurus/data/Map.hpp>


// The chromosome for the terrain-construction algorithm
class terrainosaurus::TerrainChromosome {
/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // Forward declaration of corresponding "Gene" inner class,
    // representing a small patch of terrain
    class Gene;

    // Two dimensional grid of Genes
    typedef inca::MultiArray<Gene, 2> GeneGrid;


/*---------------------------------------------------------------------------*
 | Static query functions
 *---------------------------------------------------------------------------*/
public:
    static void initializeStatic();
    static scalar_t         geneRadius(IndexType lod);
    static scalar_t         geneOverlapFactor(IndexType lod);
    static SizeType         geneSpacing(IndexType lod);
    static GrayscaleImage * geneMask(IndexType lod);

private:
    static bool staticInitialized;
    static std::vector<scalar_t>        geneRadii;
    static std::vector<GrayscaleImage>  geneMasks;


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    // Constructor
    TerrainChromosome();
//    TerrainChromosome(const Dimension &pixels, IndexType lod);

    // Resize the grid of genes, specifying whether or not to preserve the
    // current contents. If preservation is requested, then any indices which
    // are valid in both the old and new dimensions will be preserved. Genes
    // that are created as a result of the resize are left uninitialized.
    void resize(const Dimension &sz, bool preserveContents = false);
    void resize(SizeType sx, SizeType sy, bool preserveContents = false);


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
public:
    // Gene grid accessors
    SizeType size() const { return _genes.size(); }
    SizeType size(IndexType d) const { return _genes.size(d); }
    const GeneGrid::SizeArray & sizes() const { return _genes.sizes(); }
          GeneGrid & genes()       { return _genes; }
    const GeneGrid & genes() const { return _genes; }
          Gene & gene(int x, int y)       { return _genes(x, y); }
    const Gene & gene(int x, int y) const { return _genes(x, y); }
    template <class IndexList>       Gene & gene(const IndexList &idx)       { return _genes(idx); }
    template <class IndexList> const Gene & gene(const IndexList &idx) const { return _genes(idx); }

    // Level of detail accessors
    IndexType levelOfDetail() const { return _levelOfDetail; }

//protected:
    // The contents of this Chromosome
    IndexType _levelOfDetail;   // What LOD is this chromosome?
    GeneGrid  _genes;           // The genes making up this chromosome
    Dimension _heightfieldSize; // How big should the heightfield be?
};


// The gene for the terrain-construction algorithm
class terrainosaurus::TerrainChromosome::Gene {
/*---------------------------------------------------------------------------*
 | Connections to chromosome
 *---------------------------------------------------------------------------*/
protected:
    // We give our parent class permission to call the following function
    friend TerrainChromosome;

    // This function is called by the parent TerrainChromosome to claim
    // ownership and to inform it of its X,Y coordinates within the grid
    void claim(TerrainChromosome * p, const Pixel & idx);

public:
    // Chromosome relationship accessors
    TerrainChromosome & parent() const { return *_parent; }
    const Pixel & indices() const { return _indices; }

    IndexType levelOfDetail() const { return parent().levelOfDetail(); }

protected:
    // Link to the parent Chromosome, and position within parent
    TerrainChromosome *     _parent;        // Daddy!
    Pixel                   _indices;       // My genetic coordinates


/*---------------------------------------------------------------------------*
 | Data fields & assignment operator
 *---------------------------------------------------------------------------*/
public:
    // Assignment operator (preserves parent pointer and indices, but
    // copies data fields)
    Gene & operator=(const Gene &g);


    // Source data
    TerrainTypeConstPtr   terrainType;      // What sort of terrain is this?
    TerrainSampleConstPtr sourceSample;     // Where do we get our data from?
    Pixel               sourceCoordinates;  // Source X,Y center coordinates

    // Transformation to apply to source data
    scalar_t            rotation;           // Horizontal rotation in radians
    scalar_t            scale;              // Vertical scale factor
    scalar_t            offset;             // Vertical offset amount

    // Output alpha mask (trimming shape & blending)
    GrayscaleImage *    mask;               // Alpha mask to trim with

    // Target (destination) data
    Pixel               targetJitter;       // How much to offset from the
                                            // target coordinates
    Pixel               targetCoordinates;  // Where does it go?
};

#if 0
// XXX Hack -- these should be folded into MultiArray in a more general form
namespace terrainosaurus {
    typedef TerrainChromosome::GeneGrid::Iterator GeneIterator;
    void copy(GeneIterator srcStart, GeneIterator srcEnd,
              GeneIterator dstStart, GeneIterator dstEnd) {
        bool srcHoriz, dstHoriz;
        if (srcStart
}
#endif

#endif
