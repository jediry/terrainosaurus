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


class terrainosaurus::TerrainLibrary {
/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    typedef vector<TerrainTypePtr>              TerrainTypeList;
    typedef vector< vector<TerrainSeamPtr> >    TerrainSeamMatrix;


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    // Default constructor with optional number of TerrainTypes
    explicit TerrainLibrary(size_t n = 0);


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
public:
    // How many TerrainTypes are in the library
    size_t size() const;

    // Look up a TerrainType by name
    index_t indexOf(const string &name) const;

    // Access to the TerrainType objects
    const TerrainTypeList & terrainTypes() const;
    TerrainTypeConstPtr terrainType(index_t tt) const;
    TerrainTypePtr      terrainType(index_t tt);
    TerrainTypeConstPtr terrainType(const string &tt) const;
    TerrainTypePtr      terrainType(const string &tt);
      
    // Access to the TerrainSeam objects
    // The version accepting TT pointers allows NULL as a synonym for the
    // zeroth TT, which is always the "Void" TT
    const TerrainSeamMatrix & terrainSeams() const;
    TerrainSeamConstPtr terrainSeam(index_t tt1, index_t tt2) const;
    TerrainSeamPtr      terrainSeam(index_t tt1, index_t tt2);
    TerrainSeamConstPtr terrainSeam(const string &tt1, const string &tt2) const;
    TerrainSeamPtr      terrainSeam(const string &tt1, const string &tt2);
    TerrainSeamConstPtr terrainSeam(TerrainTypeConstPtr tt1,
                                    TerrainTypeConstPtr tt2) const;
    TerrainSeamPtr      terrainSeam(TerrainTypeConstPtr tt1,
                                    TerrainTypeConstPtr tt2);

    // Add a new TerrainType (creating new TerrainSeam combinations)
    TerrainTypePtr addTerrainType(const string &name = "");
    TerrainTypePtr addTerrainType(TerrainTypePtr tt);

protected:
    TerrainTypeList     types;
    TerrainSeamMatrix   seams;
};

#endif