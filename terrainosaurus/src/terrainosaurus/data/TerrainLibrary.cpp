/*
 * File: TerrainLibrary.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *
 * Implementation note:
 *      TerrainTypes are stored in a simple linear vector. TerrainSeams,
 *      on the other hand, are stored in a triangular "matrix" (a vector of
 *      vectors), with the TerrainSeam for a given combination of
 *      TerrainTypes being stored with the greater numeric id of the constituent
 *      types giving the major index and the lesser id giving the index within
 *      the vector at that major index:
 *
 *                   major
 *            |  0   1   2   3
 *    m     --+-----------------  'X' indicates NULL, since TerrainTypes
 *    i     0 |  X  0-1 0-2 0-3   don't have seams with themselves
 *    n     1 |      X  1-2 1-3
 *    o     2 |          X  2-3
 *    r     3 |              X
 */

// Import class definition
#include "TerrainLibrary.hpp"
using namespace terrainosaurus;

// Import random number generator
#include <inca/math/generator/RandomUniform>
using inca::math::RandomUniform;


/*****************************************************************************
 * LOD specialization for TerrainLibrary
 *****************************************************************************/
 // Default constructor
TerrainLibrary::LOD::LOD()
    : LODBase<TerrainLibrary>() { }

// Constructor linking back to TerrainLibrary
TerrainLibrary::LOD::LOD(TerrainLibraryPtr tl, TerrainLOD lod)
    : LODBase<TerrainLibrary>(tl, lod) { }


// Access to related LOD objects
TerrainType::LOD & TerrainLibrary::LOD::terrainType(IndexType index) {
    return (*object().terrainType(index))[levelOfDetail()];
}
const TerrainType::LOD & TerrainLibrary::LOD::terrainType(IndexType index) const {
    return (*object().terrainType(index))[levelOfDetail()];
}
TerrainType::LOD & TerrainLibrary::LOD::terrainType(const std::string & name) {
    return (*object().terrainType(name))[levelOfDetail()];
}
const TerrainType::LOD & TerrainLibrary::LOD::terrainType(const std::string & name) const {
    return (*object().terrainType(name))[levelOfDetail()];
}
TerrainType::LOD & TerrainLibrary::LOD::randomTerrainType() {
    return (*object().randomTerrainType())[levelOfDetail()];
}
const TerrainType::LOD & TerrainLibrary::LOD::randomTerrainType() const {
    return (*object().randomTerrainType())[levelOfDetail()];
}


/*---------------------------------------------------------------------------*
 | Loading & analysis
 *---------------------------------------------------------------------------*/
void TerrainLibrary::LOD::ensureLoaded() const {
    if (! loaded()) {
        for (IndexType i = 0; i < size(); ++i)
            terrainType(i).ensureLoaded();
        _loaded = true;
    }
}
void TerrainLibrary::LOD::ensureAnalyzed() const {
    if (! analyzed()) {
        for (IndexType i = 0; i < size(); ++i)
            terrainType(i).ensureAnalyzed();
        _analyzed = true;
    }
}


/*---------------------------------------------------------------------------*
 | Access to parent TerrainLibrary's properties
 *---------------------------------------------------------------------------*/
SizeType TerrainLibrary::LOD::size() const {
    return object().size();
}

TerrainSeamPtr TerrainLibrary::LOD::terrainSeam(IndexType tt1,
                                                IndexType tt2) {
    return object().terrainSeam(tt1, tt2);
}
TerrainSeamConstPtr TerrainLibrary::LOD::terrainSeam(IndexType tt1,
                                                     IndexType tt2) const {
    return object().terrainSeam(tt1, tt2);
}
TerrainSeamPtr TerrainLibrary::LOD::terrainSeam(const std::string & tt1,
                                                const std::string & tt2) {
    return object().terrainSeam(tt1, tt2);
}
TerrainSeamConstPtr TerrainLibrary::LOD::terrainSeam(const std::string & tt1,
                                                     const std::string & tt2) const {
    return object().terrainSeam(tt1, tt2);
}


/*****************************************************************************
 * TerrainLibrary containing all the TerrainType definitions
 *****************************************************************************/
/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
// Default constructor with optional number of TerrainTypes
TerrainLibrary::TerrainLibrary(SizeType n) { }


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
// Number of TerrainTypes in the library
SizeType TerrainLibrary::size() const { return terrainTypes().size(); }


// Look up a TerrainType by name
IndexType TerrainLibrary::indexOf(const std::string & name) const {
    const TerrainTypeList & ttl = terrainTypes();
    TerrainTypeList::const_iterator it;
    for (it = ttl.begin(); it != ttl.end(); ++it)
        if ((*it)->name() == name)
            return it - ttl.begin();
    return -1;
}


// Access to the TerrainType objects
const TerrainLibrary::TerrainTypeList & TerrainLibrary::terrainTypes() const {
    // If this is the first time this was called, we need to do a litle extra
    // initialization -- creating a "Void" TerrainType representing empty
    // space. We can't do this in the constructor because the
    // shared_from_this call in addTerrainType isn't valid in the
    // constructor. Sigh.
    static bool initialized = false;
    if (! initialized) {
        initialized = true;     // Have to do this FIRST, 'cause we call recursively
        const_cast<TerrainLibrary*>(this)->addTerrainType("Void");
    }
    return _terrainTypes;
}
TerrainTypePtr TerrainLibrary::terrainType(IndexType i) {
    if (i < 0 || i >= IndexType(terrainTypes().size()))
        return TerrainTypePtr();
    else
        return _terrainTypes[i];
}
TerrainTypeConstPtr TerrainLibrary::terrainType(IndexType i) const {
    if (i < 0 || i >= IndexType(terrainTypes().size()))
        return TerrainTypeConstPtr();
    else
        return _terrainTypes[i];
}
TerrainTypePtr TerrainLibrary::terrainType(const std::string &tt) {
    return terrainType(indexOf(tt));
}
TerrainTypeConstPtr TerrainLibrary::terrainType(const std::string &tt) const {
    return terrainType(indexOf(tt));
}
TerrainTypePtr TerrainLibrary::randomTerrainType() {
    RandomUniform<IndexType> randomIndex(0, terrainTypes().size() - 1);
    return _terrainTypes[randomIndex()];
}
TerrainTypeConstPtr TerrainLibrary::randomTerrainType() const {
    RandomUniform<IndexType> randomIndex(0, terrainTypes().size() - 1);
    return _terrainTypes[randomIndex()];
}


// Access to the TerrainSeam objects
const TerrainLibrary::TerrainSeamMatrix & TerrainLibrary::terrainSeams() const {
    return _terrainSeams;
}
TerrainSeamPtr TerrainLibrary::terrainSeam(IndexType tt1, IndexType tt2) {
    // We must make sure that the first index is the greater of the two,
    // since this is how the TerrainSeams are stored in the triangular
    // matrix described above.
    IndexType ttMajor = std::max(tt1, tt2),
              ttMinor = std::min(tt1, tt2);
    if (ttMinor < 0 || ttMajor >= IndexType(terrainTypes().size()))
        return TerrainSeamPtr();
    else
        return _terrainSeams[ttMajor][ttMinor];
}
TerrainSeamConstPtr TerrainLibrary::terrainSeam(IndexType tt1,
                                                IndexType tt2) const {
    // See previous comment
    IndexType ttMajor = std::max(tt1, tt2),
              ttMinor = std::min(tt1, tt2);
    if (ttMinor < 0 || ttMajor >= IndexType(terrainTypes().size()))
        return TerrainSeamConstPtr();
    else
        return _terrainSeams[ttMajor][ttMinor];
}
TerrainSeamPtr TerrainLibrary::terrainSeam(const std::string &tt1,
                                           const std::string &tt2) {
    return terrainSeam(indexOf(tt1), indexOf(tt2));
}
TerrainSeamConstPtr TerrainLibrary::terrainSeam(const std::string &tt1,
                                                const std::string &tt2) const {
    return terrainSeam(indexOf(tt1), indexOf(tt2));
}
TerrainSeamPtr TerrainLibrary::terrainSeam(TerrainTypeConstPtr tt1,
                                           TerrainTypeConstPtr tt2) {
    IDType id1 = (tt1 == NULL ? 0 : tt1->terrainTypeID());
    IDType id2 = (tt2 == NULL ? 0 : tt2->terrainTypeID());
    return terrainSeam(id1, id2);
}
TerrainSeamConstPtr TerrainLibrary::terrainSeam(TerrainTypeConstPtr tt1,
                                                TerrainTypeConstPtr tt2) const {
    IDType id1 = (tt1 == NULL ? 0 : tt1->terrainTypeID());
    IDType id2 = (tt2 == NULL ? 0 : tt2->terrainTypeID());
    return terrainSeam(id1, id2);
}


/*---------------------------------------------------------------------------*
 | Library modification functions
 *---------------------------------------------------------------------------*/
TerrainTypePtr TerrainLibrary::addTerrainType(const std::string & name) {
    INCA_INFO("Adding terrain type " << name)

    TerrainLibraryPtr self = shared_from_this();

    // Create a new TerrainType object...
    TerrainTypePtr tt(new TerrainType(self, _terrainTypes.size(), name));
    _terrainTypes.push_back(tt);

    // ...then create a TerrainSeam for each new combination.
    _terrainSeams.resize(_terrainSeams.size() + 1);     // Expand to include one more vector
    for (IndexType i = 0; i < IndexType(_terrainTypes.size()); ++i) {// Fill with TSTs
        TerrainSeamPtr ts(new TerrainSeam(self, _terrainTypes.size() - 1, i));
        _terrainSeams.back().push_back(ts);
    }

    // Finally, put NULL in for the diagonal (no seam between identical TTs)
    _terrainSeams.back().push_back(TerrainSeamPtr());

    // Finally, pass along the newly created TT
    return tt;
}

TerrainTypePtr TerrainLibrary::addTerrainType(TerrainTypePtr tt) {
    INCA_ERROR("addTerrainType(TTP) not implemented...not sure why...")
    return TerrainTypePtr();
    // FIXME: this is broken! -- copy c_tor for props, and appropriate add/name/index
//    add();                  // Create a new TerrainType and its TerrainTypeSeams
//    *_terrainTypes.back() = *tt;    // Copy the attributes from 'tt'
}
