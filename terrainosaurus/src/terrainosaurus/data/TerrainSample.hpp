/*
 * File: TerrainSample.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      A TerrainSample represents a single, real-world sample of terrain
 *      data. In addition to the height data
 */

#ifndef TERRAINOSAURUS_TERRAIN_SAMPLE
#define TERRAINOSAURUS_TERRAIN_SAMPLE

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainSample;

    // Pointer typedefs
    typedef shared_ptr<TerrainSample>       TerrainSamplePtr;
    typedef shared_ptr<TerrainSample const> TerrainSampleConstPtr;
};


class terrainosaurus::TerrainSample {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(TerrainSample);


/*---------------------------------------------------------------------------*
 | Constructors, properties & accessors
 *---------------------------------------------------------------------------*/
public:
    // Constructor
    explicit TerrainSample(const std::string & file);

    // Assignment operator overload
    TerrainSample & operator=(const TerrainSample &ts);

    // Properties
    ro_property(std::string, filename, "");     // The data file
    ro_property(IndexType, levelsOfDetail, 0);  // How many LODs?

    // Raster data accessors
    const Heightfield::SizeArray & size(IndexType lod) const;
    const Heightfield & heightfield(IndexType lod) const;
    const VectorMap & gradient(IndexType lod) const;
    const VectorMap & averageGradient(IndexType lod) const;
    const VectorMap & localRange(IndexType lod) const;

protected:
    mutable bool loaded;           // Have we loaded this?
    mutable std::vector<Heightfield>    _heightfields;
    mutable std::vector<VectorMap>      _gradients;
    mutable std::vector<VectorMap>      _averageGradients;
    mutable std::vector<VectorMap>      _localRanges;


/*---------------------------------------------------------------------------*
 | Loading/processing (lazily performed)
 *---------------------------------------------------------------------------*/
public:
    void ensureLoaded() const;
};

#endif
