/*
 * File: TerrainLibrary.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_TERRAIN_LIBRARY
#define TERRAINOSAURUS_TERRAIN_LIBRARY

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// Import LOD template declaration
#include "TerrainLOD.hpp"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainLibrary;
    template <> class LOD<TerrainLibrary>;  // Specialization of LOD template

    // Pointer typedefs
    typedef shared_ptr<TerrainLibrary>       TerrainLibraryPtr;
    typedef shared_ptr<TerrainLibrary const> TerrainLibraryConstPtr;
};

// Import sub-object definitions
#include "TerrainType.hpp"
#include "TerrainSeam.hpp"

// Import statistical analysis tools
#include <inca/math/statistics/DistributionMatcher>


/*****************************************************************************
 * LOD specialization for TerrainLibrary
 *****************************************************************************/
template <>
class terrainosaurus::LOD<terrainosaurus::TerrainLibrary>
    : public LODBase<TerrainLibrary> {
/*---------------------------------------------------------------------------*
 | Type & constant definitions
 *---------------------------------------------------------------------------*/
public:
    typedef inca::raster::MultiArrayRaster<IndexType, 2>    IndexMap;
    typedef inca::raster::MultiArrayRaster<scalar_t, 2>     DistanceMap;
    typedef IndexMap::SizeArray                             SizeArray;
    typedef inca::math::DistributionMatcher<scalar_t>       DistributionMatcher;
    typedef DistributionMatcher::ScalarArray                VarianceArray;


/*---------------------------------------------------------------------------*
 | Constructors & fundmental properties
 *---------------------------------------------------------------------------*/
public:
    // Constructors
    explicit LOD();
    explicit LOD(TerrainLibraryPtr tl, TerrainLOD lod);

    // Access to related LOD objects
          LOD<TerrainType> & terrainType(IndexType i);
    const LOD<TerrainType> & terrainType(IndexType i) const;
          LOD<TerrainType> & terrainType(const std::string & name);
    const LOD<TerrainType> & terrainType(const std::string & name) const;
          LOD<TerrainType> & randomTerrainType();
    const LOD<TerrainType> & randomTerrainType() const;


/*---------------------------------------------------------------------------*
 | Loading & analysis
 *---------------------------------------------------------------------------*/
public:
    void analyze();

    // Lazy loading and analysis mechanism
    void ensureLoaded()   const;
    void ensureAnalyzed() const;
    void ensureStudied()  const;


/*---------------------------------------------------------------------------*
 | Statistical distribution matching
 *---------------------------------------------------------------------------*/
public:
    // Aggregation of the distributions for *everything* in the library
    const DistributionMatcher & elevationDistribution()    const;
    const DistributionMatcher & slopeDistribution()        const;
    const DistributionMatcher & edgeLengthDistribution()   const;
    const DistributionMatcher & edgeScaleDistribution()    const;
    const DistributionMatcher & edgeStrengthDistribution() const;

    // Default variances (used for singular TT's)
    const VarianceArray & defaultElevationVariances()    const;
    const VarianceArray & defaultSlopeVariances()        const;
    const VarianceArray & defaultEdgeLengthVariances()   const;
    const VarianceArray & defaultEdgeScaleVariances()    const;
    const VarianceArray & defaultEdgeStrengthVariances() const;

    // Variances for matching every parameter across the whole lib. HACK
    const VarianceArray & libraryElevationVariances()    const;
    const VarianceArray & librarySlopeVariances()        const;
    const VarianceArray & libraryEdgeLengthVariances()   const;
    const VarianceArray & libraryEdgeScaleVariances()    const;
    const VarianceArray & libraryEdgeStrengthVariances() const;

protected:
    DistributionMatcher _elevationDistribution,
                        _slopeDistribution,
                        _edgeLengthDistribution,
                        _edgeScaleDistribution,
                        _edgeStrengthDistribution;

    VarianceArray _defaultElevationVariances,
                  _defaultSlopeVariances,
                  _defaultEdgeLengthVariances,
                  _defaultEdgeScaleVariances,
                  _defaultEdgeStrengthVariances;

    VarianceArray _libraryElevationVariances,
                  _librarySlopeVariances,
                  _libraryEdgeLengthVariances,
                  _libraryEdgeScaleVariances,
                  _libraryEdgeStrengthVariances;
                  

/*---------------------------------------------------------------------------*
 | Access to parent TerrainLibrary's properties
 *---------------------------------------------------------------------------*/
public:
    SizeType size() const;
    TerrainSeamPtr      terrainSeam(IndexType tt1, IndexType tt2);
    TerrainSeamConstPtr terrainSeam(IndexType tt1, IndexType tt2) const;
    TerrainSeamPtr      terrainSeam(const std::string & tt1,
                                    const std::string & tt2);
    TerrainSeamConstPtr terrainSeam(const std::string & tt1,
                                    const std::string & tt2) const;
};


/*****************************************************************************
 * TerrainLibrary containing all the TerrainType definitions
 *****************************************************************************/
class terrainosaurus::TerrainLibrary
    : public MultiResolutionObject<TerrainLibrary> {
/*---------------------------------------------------------------------------*
 | Type & cosntant definitions
 *---------------------------------------------------------------------------*/
public:
    typedef std::vector<TerrainTypePtr>                 TerrainTypeList;
    typedef std::vector< std::vector<TerrainSeamPtr> >  TerrainSeamMatrix;


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    // Default constructor with optional number of TerrainTypes
    explicit TerrainLibrary(SizeType n = 0);


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
public:
    // How many TerrainTypes are in the library
    SizeType size() const;

    // Look up a TerrainType by name
    IndexType indexOf(const std::string &name) const;

    // Access to the TerrainType objects
    const TerrainTypeList & terrainTypes() const;
    TerrainTypePtr      terrainType(IndexType tt);
    TerrainTypeConstPtr terrainType(IndexType tt) const;
    TerrainTypePtr      terrainType(const std::string &tt);
    TerrainTypeConstPtr terrainType(const std::string &tt) const;
    TerrainTypePtr      randomTerrainType();
    TerrainTypeConstPtr randomTerrainType() const;

    // Access to the TerrainSeam objects
    // The version accepting TT pointers allows NULL as a synonym for the
    // zeroth TT, which is always the "Void" TT
    const TerrainSeamMatrix & terrainSeams() const;
    TerrainSeamConstPtr terrainSeam(IndexType tt1, IndexType tt2) const;
    TerrainSeamPtr      terrainSeam(IndexType tt1, IndexType tt2);
    TerrainSeamConstPtr terrainSeam(const std::string &tt1,
                                    const std::string &tt2) const;
    TerrainSeamPtr      terrainSeam(const std::string &tt1,
                                    const std::string &tt2);
    TerrainSeamConstPtr terrainSeam(TerrainTypeConstPtr tt1,
                                    TerrainTypeConstPtr tt2) const;
    TerrainSeamPtr      terrainSeam(TerrainTypeConstPtr tt1,
                                    TerrainTypeConstPtr tt2);

    // Add a new TerrainType (creating new TerrainSeam combinations)
    TerrainTypePtr addTerrainType(const std::string &name = "");
    TerrainTypePtr addTerrainType(TerrainTypePtr tt);

protected:
    TerrainTypeList     _terrainTypes;
    TerrainSeamMatrix   _terrainSeams;
};


#endif
