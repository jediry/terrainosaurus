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

// Import TerrainType class
#include "TerrainType.hpp"


class terrainosaurus::TerrainSample {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(TerrainSample);


/*---------------------------------------------------------------------------*
 | Constructors & assignment operator
 *---------------------------------------------------------------------------*/
public:
    // Constructors
    explicit TerrainSample(const std::string & file,
                           TerrainTypePtr tt = TerrainTypePtr());
    explicit TerrainSample(const Heightfield & hf,
                           TerrainTypePtr tt = TerrainTypePtr());

    // Assignment operator overload
    TerrainSample & operator=(const TerrainSample &ts);


/*---------------------------------------------------------------------------*
 | Loading/analysis (lazily performed)
 *---------------------------------------------------------------------------*/
public:
    // Lazy evaluation status
    bool loaded() const;        // Do we have heightfield data?
    bool analyzed() const;      // Have we analyzed it?

    // Functions to force evalutation
    void ensureLoaded() const;      // If we've not loaded, do it now
    void ensureAnalyzed() const;    // If we've not analyzed, do it now

protected:
    mutable bool _loaded;           // Have we loaded this?
    mutable bool _analyzed;         // Have we analyzed it yet?


/*---------------------------------------------------------------------------*
 | Property accessor functions
 *---------------------------------------------------------------------------*/
public:
    // Properties
    ro_property(std::string, filename, "");             // The data file
    ro_property(IndexType, levelsOfDetail, 1);          // How many LODs?
    ro_ptr_property(TerrainType, terrainType, NULL);    // What do I belong to?

    // LOD property accessors
    const Heightfield::SizeArray & sizes(IndexType lod) const;

    // Per-cell heightfield property accessors
    const Heightfield & elevation(IndexType lod) const;
    const VectorMap &   gradient(IndexType lod) const;

    // Local (windowed) heightfield property accessors
    const Heightfield & localElevationMean(IndexType lod) const;
    const VectorMap &   localGradientMean(IndexType lod) const;
    const VectorMap &   localElevationRange(IndexType lod) const;
    const VectorMap &   localSlopeRange(IndexType lod) const;

    // Global heightfield property accessors
    const Heightfield::ElementType & globalElevationMean(IndexType lod) const;
    const VectorMap::ElementType &   globalGradientMean(IndexType lod) const;
    const VectorMap::ElementType &   globalElevationRange(IndexType lod) const;
    const VectorMap::ElementType &   globalSlopeRange(IndexType lod) const;

protected:
    // Per-cell heightfield properties
    mutable std::vector<Heightfield>    _elevations;
    mutable std::vector<VectorMap>      _gradients;

    // Local (windowed) heightfield properties
    mutable std::vector<Heightfield>    _localElevationMeans;
    mutable std::vector<VectorMap>      _localGradientMeans;
    mutable std::vector<VectorMap>      _localElevationRanges;
    mutable std::vector<VectorMap>      _localSlopeRanges;

    // Global heightfield properties
    mutable std::vector<Heightfield::ElementType>   _globalElevationMeans;
    mutable std::vector<VectorMap::ElementType>     _globalGradientMeans;
    mutable std::vector<VectorMap::ElementType>     _globalElevationRanges;
    mutable std::vector<VectorMap::ElementType>     _globalSlopeRanges;
};

#endif
