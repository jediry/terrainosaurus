/*
 * File: Map.c++
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the functions defined in Map.h++.
 */

// Import class definition
#include "Map.hpp"
using namespace Terrainosaurus;


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
// Get a list of all the regions in the map
Map::RegionList Map::regions() const {
    RegionList list;
    PolygonMesh::FacePtrList::const_iterator f;
    for (f = mesh().faces().begin(); f != mesh().faces().end(); f++)
        list.push_back(Region(*f));
    return list;
}

// Get a list of all the intersections in the map
Map::IntersectionList Map::intersections() const {
    IntersectionList list;
    PolygonMesh::VertexPtrList::const_iterator v;
    for (v = mesh().vertices().begin(); v != mesh().vertices().end(); v++)
        list.push_back(Intersection(*v));
    return list;
}

// Get a list of all the boundaries in the map
Map::BoundaryList Map::boundaries() const {
    BoundaryList list;
    PolygonMesh::EdgePtrList::const_iterator e;
    for (e = mesh().edges().begin(); e != mesh().edges().end(); e++)
        list.push_back(Boundary(*e));
    return list;
}


/*---------------------------------------------------------------------------*
 | GA refinement functions
 *---------------------------------------------------------------------------*/
// Refine every edge and intersection in the map
void Map::refineMap() {
    // Go do that thing you do on each boundary
    BoundaryList bs = boundaries();
    for (unsigned int i = 0; i < bs.size(); i++)
        refineBoundary(bs[i]);

    // Now that all the b's are done, do the i's
    IntersectionList is = intersections();
    for (unsigned int i = 0; i < is.size(); i++)
        refineIntersection(is[i]);
}

// Refine a single boundary in the map
void Map::refineBoundary(const Boundary &b) {
    EdgePtr edge = b.edge();

    // If we've already got one, we'll have to delete it
    BoundaryRefinementMap::iterator current;
    current = refinedBoundaries.find(edge);
    if (current != refinedBoundaries.end())
        delete current->first;

    // Make a new RefinedBoundary object, stick in hash_map
    RefinedBoundaryPtr rb = new RefinedBoundary(edge);
    refinedBoundaries[edge] = rb;

    // Now, do the refinement thingy
    refineBoundary(b, *rb);
}


//#############################################################################
//####               Now entering Mike's domain. Beware.                   ####
//#############################################################################

// Create a refinement of a Boundary.
// When this is called, 'rb' is guaranteed to be in its initialized state
void Map::refineBoundary(const Boundary &b, RefinedBoundary &rb) {
    // Your advertisement here!
}

// Smooth out the intersection of all the RefinedBoundaries that meet here
void Map::refineIntersection(Intersection &i) {
    // Blah, blah, blah. printf, i++, rm -rf /*
}