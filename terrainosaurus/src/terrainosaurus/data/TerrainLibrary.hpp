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

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainLibrary;
    
    // Pointer typedefs
    typedef shared_ptr<TerrainLibrary>       TerrainLibraryPtr;
    typedef shared_ptr<TerrainLibrary const> TerrainLibraryConstPtr;
};

// Import sub-object definitions
#include "TerrainType.hpp"
#include "TerrainSeam.hpp"

// Import container definitions
#include <vector>


class terrainosaurus::TerrainLibrary {
/*---------------------------------------------------------------------------*
 | Type definitions
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
 | LOD-dependent query functions
 *---------------------------------------------------------------------------*/
public:
    // Highest level of detail allowed
    IndexType maximumLevelOfDetail();

    // Heightfield resolution in pixels / meters at a specific LOD
    scalar_t resolution(IndexType lod);

    // Window size in pixels at a specific LOD
    int windowSize(IndexType lod);

protected:
    IndexType _maximumLevelOfDetail;
    std::vector<scalar_t> resolutions;
    std::vector<int>      windowSizes;


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
public:
    // How many TerrainTypes are in the library
    SizeType size() const;

    // Look up a TerrainType by name
    IndexType indexOf(const string &name) const;

    // Access to the TerrainType objects
    const TerrainTypeList & terrainTypes() const;
    TerrainTypeConstPtr terrainType(IndexType tt) const;
    TerrainTypePtr      terrainType(IndexType tt);
    TerrainTypeConstPtr terrainType(const string &tt) const;
    TerrainTypePtr      terrainType(const string &tt);
      
    // Access to the TerrainSeam objects
    // The version accepting TT pointers allows NULL as a synonym for the
    // zeroth TT, which is always the "Void" TT
    const TerrainSeamMatrix & terrainSeams() const;
    TerrainSeamConstPtr terrainSeam(IndexType tt1, IndexType tt2) const;
    TerrainSeamPtr      terrainSeam(IndexType tt1, IndexType tt2);
    TerrainSeamConstPtr terrainSeam(const string &tt1, const string &tt2) const;
    TerrainSeamPtr      terrainSeam(const string &tt1, const string &tt2);
    TerrainSeamConstPtr terrainSeam(TerrainTypeConstPtr tt1,
                                    TerrainTypeConstPtr tt2) const;
    TerrainSeamPtr      terrainSeam(TerrainTypeConstPtr tt1,
                                    TerrainTypeConstPtr tt2);

    // Add a new TerrainType (creating new TerrainSeam combinations)
    TerrainTypePtr addTerrainType(const string &name = "");
    TerrainTypePtr addTerrainType(TerrainTypePtr tt);

    // Force loading of a terrain type (meaning that we intend to use it)
    void ensureLoaded(IndexType tt) const;
    void ensureLoaded(const string &tt) const;

protected:
    TerrainTypeList     types;
    TerrainSeamMatrix   seams;
};

#endif
