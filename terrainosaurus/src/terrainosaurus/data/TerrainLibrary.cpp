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


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
// Default constructor with optional number of TerrainTypes
TerrainLibrary::TerrainLibrary(SizeType n) {
    // Initialize the static data structures
    addTerrainType("Void"); // This represents "empty space"

    // Create a bunch of TerrainTypes, held by shared_ptr
    for (IndexType i = 0; i < IndexType(n); ++i)
        addTerrainType();       // Create a TT with default attributes

    // Set up the LOD stuff
    _maximumLevelOfDetail = 0;      // Crippled for right now
    resolutions.push_back(scalar_t(1) / 30);
    windowSizes.push_back(16);
}


/*---------------------------------------------------------------------------*
 | LOD-dependent query functions
 *---------------------------------------------------------------------------*/
IndexType TerrainLibrary::maximumLevelOfDetail() {
    return _maximumLevelOfDetail;
}
scalar_t TerrainLibrary::resolution(IndexType lod) {
    return resolutions[lod];
}
int TerrainLibrary::windowSize(IndexType lod) {
    return windowSizes[lod];
}


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
// Number of TerrainTypes in the library
SizeType TerrainLibrary::size() const { return types.size(); }


// Look up a TerrainType by name
IndexType TerrainLibrary::indexOf(const string &name) const {
    TerrainTypeList::const_iterator i;
    for (i = types.begin(); i != types.end(); ++i)
        if ((*i)->name() == name)
            return i - types.begin();
    return -1;
}


// Access to the TerrainType objects
const TerrainLibrary::TerrainTypeList & TerrainLibrary::terrainTypes() const {
    return types;
}
TerrainTypeConstPtr TerrainLibrary::terrainType(IndexType i) const {
    if (i < 0 || i >= IndexType(types.size()))
        return TerrainTypeConstPtr();
    else
        return types[i];
}
TerrainTypePtr TerrainLibrary::terrainType(IndexType i) {
    if (i < 0 || i >= IndexType(types.size()))
        return TerrainTypePtr();
    else
        return types[i];
}
TerrainTypeConstPtr TerrainLibrary::terrainType(const string &tt) const {
    return terrainType(indexOf(tt));
}
TerrainTypePtr TerrainLibrary::terrainType(const string &tt) {
    return terrainType(indexOf(tt));
}


// Access to the TerrainSeam objects
const TerrainLibrary::TerrainSeamMatrix & TerrainLibrary::terrainSeams() const {
    return seams;
}
TerrainSeamConstPtr TerrainLibrary::terrainSeam(IndexType tt1,
                                                IndexType tt2) const {
    // We must make sure that the first index is the greater of the two,
    // since this is how the TerrainSeams are stored in the triangular
    // matrix described above.
    IndexType ttMajor = std::max(tt1, tt2),
            ttMinor = std::min(tt1, tt2);
    if (ttMinor < 0 || ttMajor >= IndexType(types.size()))
        return TerrainSeamConstPtr();
    else
        return seams[ttMajor][ttMinor];
}
TerrainSeamPtr TerrainLibrary::terrainSeam(IndexType tt1, IndexType tt2) {
    // See previous comment
    IndexType ttMajor = std::max(tt1, tt2),
            ttMinor = std::min(tt1, tt2);
    if (ttMinor < 0 || ttMajor >= IndexType(types.size()))
        return TerrainSeamPtr();
    else
        return seams[ttMajor][ttMinor];
}
TerrainSeamConstPtr TerrainLibrary::terrainSeam(const string &tt1,
                                                const string &tt2) const {
    return terrainSeam(indexOf(tt1), indexOf(tt2));
}
TerrainSeamPtr TerrainLibrary::terrainSeam(const string &tt1,
                                           const string &tt2) {
    return terrainSeam(indexOf(tt1), indexOf(tt2));
}
TerrainSeamConstPtr TerrainLibrary::terrainSeam(TerrainTypeConstPtr tt1,
                                                TerrainTypeConstPtr tt2) const {
    IDType id1 = (tt1 == NULL ? 0 : tt1->id());
    IDType id2 = (tt2 == NULL ? 0 : tt2->id());
    return terrainSeam(id1, id2);
}
TerrainSeamPtr TerrainLibrary::terrainSeam(TerrainTypeConstPtr tt1,
                                           TerrainTypeConstPtr tt2) {
    IDType id1 = (tt1 == NULL ? 0 : tt1->id());
    IDType id2 = (tt2 == NULL ? 0 : tt2->id());
    return terrainSeam(id1, id2);
}


/*---------------------------------------------------------------------------*
 | Library modification functions
 *---------------------------------------------------------------------------*/
TerrainTypePtr TerrainLibrary::addTerrainType(const string &name) {
    cerr << "Adding terrain type " << name << endl;

    // Create a new TerrainType object...
    TerrainTypePtr tt(new TerrainType(types.size(), name));
    types.push_back(tt);

    // ...then create a TerrainSeam for each new combination.
    seams.resize(seams.size() + 1);     // Expand to include one more vector
    for (IndexType i = 0; i < IndexType(types.size()); ++i) {// Fill with TSTs
        TerrainSeamPtr ts(new TerrainSeam(types.size() - 1, i));
        seams.back().push_back(ts);
    }

    // Finally, put NULL in for the diagonal (no seam between identical TTs)
    seams.back().push_back(TerrainSeamPtr());

    // Finally, pass along the newly created TT
    return tt;
}

TerrainTypePtr TerrainLibrary::addTerrainType(TerrainTypePtr tt) {
    return TerrainTypePtr();
    // FIXME: this is broken! -- copy c_tor for props, and appropriate add/name/index
//    add();                  // Create a new TerrainType and its TerrainTypeSeams
//    *types.back() = *tt;    // Copy the attributes from 'tt'
}

void TerrainLibrary::ensureLoaded(IndexType tt) const { types[tt]->ensureLoaded(); }
void TerrainLibrary::ensureLoaded(const string &tt) const { ensureLoaded(indexOf(tt)); }
