/*
 * File: TerrainType.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_TERRAIN_TYPE
#define TERRAINOSAURUS_TERRAIN_TYPE

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// Import LOD template declaration
#include "TerrainLOD.hpp"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainType;
    template <> class LOD<TerrainType>;  // Specialization of LOD template

    // Pointer typedefs
    typedef shared_ptr<TerrainType>       TerrainTypePtr;
    typedef shared_ptr<TerrainType const> TerrainTypeConstPtr;
};


// Import associated data classes
#include "TerrainLibrary.hpp"
#include "TerrainSample.hpp"

// Import statistical analysis tools
#include <inca/math/statistics/DistributionMatcher>


/*****************************************************************************
 * LOD specialization for TerrainType
 *****************************************************************************/
template <>
class terrainosaurus::LOD<terrainosaurus::TerrainType>
    : public LODBase<TerrainType> {
/*---------------------------------------------------------------------------*
 | Type declarations
 *---------------------------------------------------------------------------*/
public:
    typedef inca::math::DistributionMatcher<scalar_t> DistributionMatcher;


/*---------------------------------------------------------------------------*
 | Constructors & fundmental properties
 *---------------------------------------------------------------------------*/
public:
    // Constructors
    explicit LOD();
    explicit LOD(TerrainTypePtr tt, TerrainLOD lod);

    // Access to related LOD objects
          LOD<TerrainLibrary> & terrainLibrary();
    const LOD<TerrainLibrary> & terrainLibrary() const;
          LOD<TerrainSample> & terrainSample(IndexType i);
    const LOD<TerrainSample> & terrainSample(IndexType i) const;
          LOD<TerrainSample> & randomTerrainSample();
    const LOD<TerrainSample> & randomTerrainSample() const;


/*---------------------------------------------------------------------------*
 | Loading & analysis
 *---------------------------------------------------------------------------*/
public:
    // Analysis of constitutent TerrainSamples' statistical properties
    void analyze();

    // Lazy loading & analysis mechanism
    void ensureLoaded()   const;
    void ensureAnalyzed() const;
    void ensureStudied()  const;


/*---------------------------------------------------------------------------*
 | Statistical distribution matching
 *---------------------------------------------------------------------------*/
public:
    const DistributionMatcher & elevationDistribution() const;
    const DistributionMatcher & slopeDistribution() const;
    const DistributionMatcher & edgeLengthDistribution() const;
    const DistributionMatcher & edgeScaleDistribution() const;
    const DistributionMatcher & edgeStrengthDistribution() const;

    scalar_t elevationWeight() const;
    scalar_t slopeWeight() const;
    scalar_t edgeLengthWeight() const;
    scalar_t edgeScaleWeight() const;
    scalar_t edgeStrengthWeight() const;

    scalar_t agreement() const;

protected:
    DistributionMatcher _elevationDistribution,
                        _slopeDistribution,
                        _edgeLengthDistribution,
                        _edgeScaleDistribution,
                        _edgeStrengthDistribution;

    scalar_t _elevationWeight,
             _slopeWeight,
             _edgeLengthWeight,
             _edgeScaleWeight,
             _edgeStrengthWeight;

    scalar_t _agreement;
    

/*---------------------------------------------------------------------------*
 | Access to parent TerrainType properties
 *---------------------------------------------------------------------------*/
public:
    SizeType size() const;
    IDType terrainTypeID() const;
    const std::string & name() const;
    const Color & color() const;
};


/*****************************************************************************
 * TerrainType class for representing a logical terrain class
 *****************************************************************************/
class terrainosaurus::TerrainType
        : public MultiResolutionObject<TerrainType> {
/*---------------------------------------------------------------------------*
 | Type & constant definitions
 *---------------------------------------------------------------------------*/
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(TerrainType);


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    // Default constructor, optionally initializing name
    explicit TerrainType(TerrainLibraryPtr lib,
                         IDType eyeDee, const std::string &nm = "");


/*---------------------------------------------------------------------------*
 | Property accessor functions
 *---------------------------------------------------------------------------*/
public:
    // Unique TerrainType ID
    IDType terrainTypeID() const;
    void setTerrainTypeID(IDType id);

    // TerrainType name
    const std::string & name() const;
    void setName(const std::string & n);

    // Representative color
    const Color & color() const;
    void setColor(const Color & c);

    // Associated TerrainLibrary
    TerrainLibraryPtr      terrainLibrary();
    TerrainLibraryConstPtr terrainLibrary() const;
    void setTerrainLibrary(TerrainLibraryPtr tl);

    // List of associated TerrainSamples
    SizeType size() const;
    TerrainSamplePtr      terrainSample(IndexType i);
    TerrainSampleConstPtr terrainSample(IndexType i) const;
    TerrainSamplePtr      randomTerrainSample();
    TerrainSampleConstPtr randomTerrainSample() const;
    void addTerrainSample(TerrainSamplePtr ts);

protected:
    IDType                          _terrainTypeID;
    std::string                     _name;
    Color                           _color;
    std::vector<TerrainSamplePtr>   _terrainSamples;
    TerrainLibraryPtr               _terrainLibrary;
};

#endif
