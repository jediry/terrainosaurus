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
using namespace terrainosaurus;

using namespace inca::imaging;
using namespace inca::math;

// Import our genetic algorithm code
#include "genetics/GA.h"


/*---------------------------------------------------------------------------*
 | TerrainType functions
 *---------------------------------------------------------------------------*/
TerrainDescriptorPtr Map::TerrainType::descriptor() const {
    return map().library[_id];
}


/*---------------------------------------------------------------------------*
 | Region functions
 *---------------------------------------------------------------------------*/
Map::TerrainType Map::Region::terrainType() const {
    return TerrainType(map(), _face->material());
}

void Map::Region::setTerrainType(const TerrainType &tt) {
    _face->setMaterial(tt.id());    // Set the material ID for this face
}

// Boundary objects, looked up by index (CCW around the Region)
size_t Map::Region::boundaryCount() const {
    return _face->getEdgeCount();
}

Map::Boundary Map::Region::boundary(index_t index) const {
    index_t i = 0;
    EdgePtr e = _face->getLinkEdge();
    VertexPtr v = e->getCWVertex(_face);

    while (i < index) {
        v = e->getOtherVertex(v);
        e = e->getCCWEdge(_face);
        i++;
    }
    return Boundary(map(), e, v);
}

// Boundary objects, looked up from their neighbors
Map::Boundary Map::Region::boundaryCCW(const Boundary &from) const {
    EdgePtr fe = from.edge();
    VertexPtr v = from.startIntersection().vertex();
    EdgePtr e = fe->getCCWEdge(_face);
    if (e->touchesVertex(v))
        v = e->getOtherVertex(v);   // Intersection was start point
    else
        v = fe->getOtherVertex(v);  // Intersection was end point
    return Boundary(map(), e, v);
}

Map::Boundary Map::Region::boundaryCW(const Boundary &from) const {
    EdgePtr fe = from.edge();
    VertexPtr v = from.startIntersection().vertex();
    EdgePtr e = fe->getCWEdge(_face);
    if (e->touchesVertex(v))
        v = e->getOtherVertex(v);   // Intersection was start point
    else
        v = fe->getOtherVertex(v);  // Intersection was end point
    return Boundary(map(), e, v);
}

// Intersection objects, looked up by index (CCW around the Region)
size_t Map::Region::intersectionCount() const {
    return _face->getVertexCount();
}

Map::Intersection Map::Region::intersection(index_t index) const {
    index_t i = 0;
    EdgePtr e = _face->getLinkEdge();
    VertexPtr v = e->getCWVertex(_face);

    while (i < index) {
        v = e->getOtherVertex(v);
        e = e->getCCWEdge(_face);
        i++;
    }
    return Intersection(map(), v);
}

// Intersection objects, looked up from their neighbors
Map::Intersection Map::Region::intersectionCCW(const Intersection &from) const {
    EdgePtr e = from.vertex()->getEdgeBeforeCCW(_face);
    VertexPtr v = e->getOtherVertex(from.vertex());
    return Intersection(map(), v);
}

Map::Intersection Map::Region::intersectionCW(const Intersection &from) const {
    EdgePtr e = from.vertex()->getEdgeBeforeCW(_face);
    VertexPtr v = e->getOtherVertex(from.vertex());
    return Intersection(map(), v);
}


/*---------------------------------------------------------------------------*
 | Intersection functions
 *---------------------------------------------------------------------------*/
const Map::Point & Map::Intersection::location() const {
    return _vertex->location();
}

size_t Map::Intersection::boundaryCount() const {
    return _vertex->getAdjacentEdgeCount();
}

// Boundary objects, looked up by index (CCW around the Intersection)
// The resulting Boundary will have this intersection as its 'start'
Map::Boundary Map::Intersection::boundary(index_t index) const {
    EdgePtr e = _vertex->getLinkEdge();
    for (index_t i = 0; i < index; i++)
        e = e->getCCWEdge(_vertex);
    return Boundary(map(), e, _vertex);
}

// Boundary objects, looked up from their neighbors
// The resulting Boundary will have this intersection as its 'start'
Map::Boundary Map::Intersection::boundaryCCW(const Boundary &from) const {
    EdgePtr e = from.edge()->getCCWEdge(_vertex);
    return Boundary(map(), e, _vertex);
}

Map::Boundary Map::Intersection::boundaryCW(const Boundary &from) const {
    EdgePtr e = from.edge()->getCWEdge(_vertex);
    return Boundary(map(), e, _vertex);
}

size_t Map::Intersection::regionCount() const {
    return _vertex->getAdjacentEdgeCount();
}

Map::Region Map::Intersection::region(index_t index) const {
    EdgePtr e = _vertex->getLinkEdge();
    int i = -1;
    while (i < static_cast<int>(index)) {
        if (e->getCCWFace(_vertex) != NULL)
            i++;
    }
    return Region(map(), e->getCCWFace(_vertex));
}


/*---------------------------------------------------------------------------*
 | Boundary functions
 *---------------------------------------------------------------------------*/
// Properties of the Boundary
double Map::Boundary::length() const {
    return magnitude(_vertex->location() -
                     _edge->getOtherVertex(_vertex)->location());    
}

// Get the same boundary, but oriented the other way (end -> start)
Map::Boundary Map::Boundary::reverse() const {
    return Boundary(map(), _edge, _edge->getOtherVertex(_vertex));
}

// See if this corresponds to the orientation of the edge in the
// underlying mesh
bool Map::Boundary::isReversed() const {
    return _vertex == _edge->getStartVertex();
}

// Access to the corresponding refined boundary. The resulting
// RefinedBoundary will have the same orientation as this Boundary.
bool Map::Boundary::isRefined() const {
    return refinement().size() > 0;
}

Map::RefinedBoundary Map::Boundary::refinement() const {
    return map().refinementOf(*this);
}

// Intersection objects
Map::Intersection Map::Boundary::startIntersection() const {
    return Intersection(map(), _vertex);
}

Map::Intersection Map::Boundary::endIntersection() const {
    return Intersection(map(), _edge->getOtherVertex(_vertex));
}

// Region objects adjacent to this Boundary
Map::Region Map::Boundary::leftRegion() const {
    FacePtr f = _edge->getCCWFace(_vertex);
    return Region(map(), f);
}

Map::Region Map::Boundary::rightRegion() const {
    FacePtr f = _edge->getCWFace(_vertex);
    return Region(map(), f);
}


/*---------------------------------------------------------------------------*
 | RefinedBoundary functions
 *---------------------------------------------------------------------------*/
size_t Map::RefinedBoundary::size() const {
    return _refinement->size();
}

Map::PointList & Map::RefinedBoundary::points() {
    return *_refinement;
}
const Map::PointList & Map::RefinedBoundary::points() const {
    return *_refinement;
}

Map::Point & Map::RefinedBoundary::operator[](index_t index) {
    PointList::iterator i;
    if (_guide.isReversed())
        i = fromEnd(index);
    else
        i = fromStart(index);
    return *i;
}

const Map::Point & Map::RefinedBoundary::operator[](index_t index) const {
    PointList::const_iterator i;
    if (_guide.isReversed())
        i = fromEnd(index);
    else
        i = fromStart(index);
    return *i;
}

void Map::RefinedBoundary::push_front(Point p) {
    if (_guide.isReversed())    // This means the opposite if the Boundary is
        _refinement->push_back(p);   // reversed
    else
        _refinement->push_front(p);
}

void Map::RefinedBoundary::push_back(Point p) {
    if (_guide.isReversed())    // Ditto here.
        _refinement->push_front(p);
    else
        _refinement->push_back(p);
}

void Map::RefinedBoundary::clear() {
    _refinement->clear();
}

void Map::RefinedBoundary::erase(index_t index) {
    PointList::iterator i;
    if (_guide.isReversed())
        i = fromEnd(index);
    else
        i = fromStart(index);
    _refinement->erase(i); 
}

void Map::RefinedBoundary::insert(index_t beforeIndex, Point p) {
    PointList::iterator i;
    if (_guide.isReversed()) {
        i = fromEnd(beforeIndex);
        i++;
    } else
        i = fromStart(beforeIndex);
    _refinement->insert(i, p);
}

Map::PointList::iterator
Map::RefinedBoundary::fromStart(index_t index) {
    // Range check
    if (index >= index_t(_refinement->size())) {
        cerr << "Map::RefinedBoundary::fromStart(" << index
             << "): Index out of range [0, " << _refinement->size() << "]\n";
        return _refinement->end();
    }

    // Start from the first element
    PointList::iterator it = _refinement->begin();
    for (index_t i = 0; i < index; i++)
        it++;
    return it;
}

Map::PointList::const_iterator
Map::RefinedBoundary::fromStart(index_t index) const {
    // Range check
    if (index >= index_t(_refinement->size())) {
        cerr << "Map::RefinedBoundary::fromStart(" << index
             << "): Index out of range [0, " << _refinement->size() << "]\n";
        return _refinement->end();
    }

    // Start from the first element
    PointList::const_iterator it = _refinement->begin();
    for (index_t i = 0; i < index; i++)
        it++;
    return it;
}

Map::PointList::iterator
Map::RefinedBoundary::fromEnd(index_t index) {
    // Range check
    if (index >= index_t(_refinement->size())) {
        cerr << "Map::RefinedBoundary::fromEnd(" << index
             << "): Index out of range [0, " << _refinement->size() << "]\n";
        return _refinement->end();
    }

    PointList::iterator it = _refinement->end();
    it--;
    for (index_t i = 0; i < index; i++)
        it--;
    return it;
}

Map::PointList::const_iterator
Map::RefinedBoundary::fromEnd(index_t index) const {
    // Range check
    if (index >= index_t(_refinement->size())) {
        cerr << "Map::RefinedBoundary::fromEnd(" << index
             << "): Index out of range [0, " << _refinement->size() << "]\n";
        return _refinement->end();
    }

    // Start from the last real element
    PointList::const_iterator it = _refinement->end();
    it--;
    for (index_t i = 0; i < index; i++)
        it--;
    return it;
}


/*---------------------------------------------------------------------------*
 | Map constructors
 *---------------------------------------------------------------------------*/
// Default constructor with optional map name
Map::Map(const string &nm)
      : name(this, nm), mesh(this), library(this),
        numberOfCycles(this), numberOfChromosomes(this), smoothness(this),
        mutationRate(this), crossoverRate(this), selectionRatio(this),
        resolution(this) { }

Map::Map(PolygonMeshPtr m, const string &nm)
      : name(this, nm), mesh(this, m), library(this),
        numberOfCycles(this), numberOfChromosomes(this), smoothness(this),
        mutationRate(this), crossoverRate(this), selectionRatio(this),
        resolution(this) { }


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
// Get a list of all the terrain types in the library
Map::TerrainTypeList Map::terrainTypes() const {
    TerrainTypeList list;
    for (index_t t = 0; t < index_t(library.size()); ++t)
        list.push_back(TerrainType(*this, t));
    return list;
}

// Get a list of all the regions in the map
Map::RegionList Map::regions() const {
    RegionList list;
    PolygonMesh::FacePtrList::const_iterator f;
    for (f = mesh()->faces().begin(); f != mesh()->faces().end(); ++f)
        list.push_back(Region(*this, *f));
    return list;
}

// Get a list of all the intersections in the map
Map::IntersectionList Map::intersections() const {
    IntersectionList list;
    PolygonMesh::VertexPtrList::const_iterator v;
    for (v = mesh()->vertices().begin(); v != mesh()->vertices().end(); ++v)
        list.push_back(Intersection(*this, *v));
    return list;
}

// Get a list of all the boundaries in the map
Map::BoundaryList Map::boundaries() const {
    BoundaryList list;
    PolygonMesh::EdgePtrList::const_iterator e;
    for (e = mesh()->edges().begin(); e != mesh()->edges().end(); ++e)
        list.push_back(Boundary(*this, *e, (*e)->getStartVertex()));
    return list;
}

// Look up how many of each type of thing there is in the map
size_t Map::terrainTypeCount()  const { return library.size(); }
size_t Map::regionCount()       const { return mesh()->faceCount(); }
size_t Map::intersectionCount() const { return mesh()->vertexCount(); }
size_t Map::boundaryCount()     const { return mesh()->edgeCount(); }

// Look up a TerrainType by index
Map::TerrainType Map::terrainType(index_t index) const {
    return TerrainType(*this, index);
}

// Look up a Region by index
Map::Region Map::region(index_t index) const {
    return Region(*this, mesh()->face(index));
}

// Look up an Intersection by index
Map::Intersection Map::intersection(index_t index) const {
    return Intersection(*this, mesh()->vertex(index));
}

// Look up a Boundary by index
Map::Boundary Map::boundary(index_t index) const {
    EdgePtr e = mesh()->edge(index);
    return Boundary(*this, e, e->getStartVertex());
}

// Get a RefinedBoundary for a Boundary
Map::RefinedBoundary Map::refinementOf(const Boundary &b) const {
    PointListPtr points;
    BoundaryRefinementMap::const_iterator i = refinedBoundaries.find(b.edge());
    if (i == refinedBoundaries.end()) { // Gotta create one
        points = PointListPtr(new PointList());
        refinedBoundaries[b.edge()] = points;
    } else
        points = i->second;

    return RefinedBoundary(b, points);
}

void Map::addRegion(const PointList &points) {
    PolygonMesh::FaceVertexPtrList fvs;
    for (PointList::const_iterator i = points.begin(); i != points.end(); ++i)
        fvs.push_back(mesh->createFaceVertex(mesh->createVertex(*i)));
    mesh->createFace(fvs);
}


/*---------------------------------------------------------------------------*
 | GA refinement functions
 *---------------------------------------------------------------------------*/
// Refine every edge and intersection in the map
void Map::refineMap() {
    // Go do that thing you do on each boundary
    BoundaryList bs = boundaries();
    for (index_t i = 0; i < index_t(bs.size()); i++)
        refineBoundary(bs[i]);

    // Now that all the b's are done, do the i's
    IntersectionList is = intersections();
    for (index_t i = 0; i < index_t(is.size()); i++)
        refineIntersection(is[i]);
}

// Create a refinement of a Boundary.
void Map::refineBoundary(const Boundary &b) {
    RefinedBoundary rb = b.refinement();

    // Make sure it's empty
    rb.clear();

    // Get the endpoints 'n' stuff
    Point start = b.startIntersection().vertex()->location();
    Point end = b.endIntersection().vertex()->location();
    Vector guide = end - start;

    // Report it to our oh-so-intelligent user (HAIL TO THEE, USER!)
    logger << "Refinement for Boundary " << b.id() << endl
           << "   # of chromosomes: " << numberOfChromosomes() << endl
           << "   # of cylcles: " << numberOfCycles() << endl
           << "   Smoothness: " << smoothness() << endl
           << "   Mutation rate: " << mutationRate() << endl
           << "   Crossover rate: " << crossoverRate() << endl
           << "   Selection ratio: " << selectionRatio() << endl
           << "   Resolution: " << resolution();
    logger.info();

    // Figure what this looks like
    double guideLength = magnitude(guide);          // Length of the edge
    double guideAngle = angle(guide, Vector(1.0, 0.0));   // Angle with the X axis
    if (guide[1] < 0.0)      // The angle might be negative
        guideAngle = -guideAngle;

    // Set up our genetic algorithm and generate a line
    GA::Population population(numberOfChromosomes(), smoothness(),
                              mutationRate(), crossoverRate(),
                              selectionRatio());
    GA::Point sPt, ePt;
    sPt.x = -2;
    sPt.y = 1;
    ePt.x = 20;
    ePt.y = 3;
    //ePt.x = int(guideLength * resolution());
    float* angles = population.MakeLine(sPt, ePt, numberOfCycles());
    float line1[22] = {
        3.0, 3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,
3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,
3.0,3.0,3.0};

    // Decode the angles into points (initially aligned semi-arbitrarily)
    Point p(0.0, 0.0);
    Vector path(0.0, 0.0);  // This keeps track of how far this moves us
    scalar_t accumulatedAngle = 0.0;
    rb.push_back(p);
    for(int i = 0; i < ePt.x; i++) {
        accumulatedAngle = angles[i];
        Vector dp(cos(accumulatedAngle), sin(accumulatedAngle));   // Find motion in X / Y
        p += dp;            // Move us by this amound and place a point here
        rb.push_back(p);    // Stick it in
        path += dp;         // Update our path vector
        logger << "Angle[" << i << "]: " << angles[i] << " -> " << accumulatedAngle << endl
               << "  p is " << p;
        logger.debug();
    }

    double pathLength = magnitude(path);          // Length of the edge
    double pathAngle = angle(path, Vector(1.0, 0.0));
    if (path[1] < 0.0)      // The angle might be negative
        pathAngle = -pathAngle;

    scalar_t scaleFactor = guideLength / pathLength;
    scalar_t rotationAngle = guideAngle - pathAngle;
    logger << "Result:" << endl
           << "   Start: " << start << ", end: " << end << endl
           << "   Guide length: " << guideLength << endl
           << "   Guide angle: " << guideAngle << endl
           << "   Path length: " << pathLength << endl
           << "   Path angle: " << pathAngle << endl
           << "   Scale factor: " << scaleFactor << endl
           << "   Rotation angle: " << rotationAngle << endl;
    logger.info();

    inca::math::Matrix<scalar_t, 3, 3, true> T, R, S, X;
    loadTranslation<double,3>(T, start - Point(0.0, 0.0));
    loadScaling<double, 3>(S, Vector(scaleFactor, scaleFactor));
    loadRotation2D(R, rotationAngle);
    X = T % (R % S);
    logger << "S: -------------" << endl << S << endl;
    logger << "R: -------------" << endl << R << endl;
    logger << "T: -------------" << endl << T << endl;
    logger << "X: -------------" << endl << X << endl;
    logger.info();

    // Now go back and finesse the points into place
    Map::PointList::iterator pt;
    Map::PointList & points = rb.points();
    for (pt = points.begin(); pt != points.end(); pt++) {
        Point & p = *pt;
        p = operator%<scalar_t, 3, 3, true>(X, p);
        //p = X % p;
        cerr << *pt << endl;
    }
}

// Smooth out the intersection of all the RefinedBoundaries that meet here
void Map::refineIntersection(Intersection &in) {
    size_t bCount = in.boundaryCount();

    // Calculate an unweighted average of the nearby points
    Point p(0.0, 0.0);
    for (index_t i = 0; i < index_t(bCount); i++)
        p += in.boundary(i).refinement()[1];
//    p /= bCount;

    // Modify the start point of each boundary refinement
//    for (index_t i = 0; i < index_t(bCount); i++)
//        in.boundary(i).refinement()[0] = p;
}
