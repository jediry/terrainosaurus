/*
 * File: Map.cpp
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

// Import our genetic algorithm code
#include "../genetics/GA.h"

// Import STL/Boost algorithms
using std::for_each;
using namespace boost::lambda;


/*---------------------------------------------------------------------------*
 | MapData functions
 *---------------------------------------------------------------------------*/
// Query the resolution (samples/world-space unit) of the elevation data
MapData::scalar_t MapData::resolution() const {
    return 20.0;    // FIXME: garbage!!!
}


/*---------------------------------------------------------------------------*
 | MapData::EdgeData functions
 *---------------------------------------------------------------------------*/
// Get the MapData for the owning Map
MapData & MapData::EdgeData::mapData() const {
    // Up-cast to the edge type, get the mesh object, down-cast to MapData
    return *static_cast<Map::EdgeConstPtr>(this)->mesh();
}

// Get the locations of the two endpoints of this edge
MapData::Point MapData::EdgeData::startPoint() const {
    // Up-cast to the edge type, then get the endpoint
    Map::EdgeConstPtr edge = static_cast<Map::EdgeConstPtr>(this);
    return edge->startVertex()->position();
}
MapData::Point MapData::EdgeData::endPoint() const {
    // Up-cast to the edge type, then get the endpoint
    Map::EdgeConstPtr edge = static_cast<Map::EdgeConstPtr>(this);
    return edge->endVertex()->position();
}

// Geometric, cartesian length of the edge
MapData::scalar_t MapData::EdgeData::length() const {
    return inca::math::distance(startPoint(), endPoint());
}

// Angle (between -pi and pi) with the X axis
MapData::scalar_t MapData::EdgeData::angle() const {
    Vector line = endPoint() - startPoint();
    return signedAngle(normalize(line), Vector(1, 0));
}

// The nearest point on the edge to a world-space point
MapData::Point MapData::EdgeData::nearestPointTo(Point p) const {
    Point p0 = startPoint();
    Point p1 = endPoint();
    Vector v = p1 - p0; // This runs the length of the edge

    // Find the distance along the line connecting these two points
    scalar_t t = dot(p - p0, v) / dot(v, v);
    if (t < 0)          t = 0;      // Clamp to stay in between the two points
    else if (t > 1)     t = 1;

    // Interpolate between the endpoints
    return (p0)*(1 - t) + (p1)*(t);
}

// Get the TerrainSeam according to the TerrainTypes of the adjacent faces
TerrainSeamConstPtr MapData::EdgeData::terrainSeam() const {
    // Up-cast to the edge type
    Map::EdgeConstPtr edge = static_cast<Map::EdgeConstPtr>(this);

    // Find the TT for each face
    Map::FaceConstPtr f1 = edge->positiveFace();
    Map::FaceConstPtr f2 = edge->negativeFace();
    TerrainTypeConstPtr tt1 = f1 != NULL ? f1->terrainType() : TerrainTypePtr();
    TerrainTypeConstPtr tt2 = f2 != NULL ? f2->terrainType() : TerrainTypePtr();

    // Retrieve the corresponding TS from the terrainLibrary
    return edge->mesh()->terrainLibrary()->terrainSeam(tt1, tt2);
}

// Construct the list of points refining this Edge, using the current envelope
void MapData::EdgeData::refine() {
    // Get the endpoints 'n' stuff
    Point start = startPoint();
    Point end   = endPoint();
    Vector guide = end - start;
    scalar_t length = magnitude(guide);


    /////////////////////////////////////////////////////////////////////////
    // Refine this Edge via the genetic algorithm
    /////////////////////////////////////////////////////////////////////////

    // Set up our GA with the parameters from the TerrainSeam
    TerrainSeamConstPtr ts = terrainSeam();
    GA::Population population(ts->numberOfChromosomes(),
                              ts->smoothness(),
                              ts->mutationRatio(),
                              ts->crossoverRatio(),
                              ts->selectionRatio(), 25);

    // Construct the envelope (which also tells us the number of segments),
    // based on the resolution of the elevation data. We don't need any more
    // segments than half the number of elevation samples covering this length,
    // since we can view the elevation grid as sampling this edge, and it can't
    // resolve any boundary features with frequency higher than half its
    // resolution.
    int segments = static_cast<int>(mapData().resolution() * length / 2.0);
    buildDiamondEnvelope(segments, ts->aspectRatio());

return;

    // Report what we're about to do to our oh-so-intelligent user
    // (HAIL TO THEE, USER!)
    logger << "Refinement for Edge  " << static_cast<Map::EdgePtr>(this)->id() << endl
           << "   # of chromosomes: " << ts->numberOfChromosomes() << endl
           << "   Smoothness:       " << ts->smoothness() << endl
           << "   Mutation rate:    " << ts->mutationRatio() << endl
           << "   Crossover rate:   " << ts->crossoverRatio() << endl
           << "   Selection ratio:  " << ts->selectionRatio() << endl
           << "   Aspect ratio:     " << ts->aspectRatio() << endl
           << "   # of cycles:      " << ts->numberOfCycles() << endl
           << "   Segments:         " << envelope().size() << endl;
    logger.info();


    // Do it XXX ugly hacked thing
    GA::Point sPt, ePt;
    sPt.x = 0;
    sPt.y = 0;
    ePt.x = envelope().size();
    ePt.y = 0;
    
    float low[30], up[30];
    for (int i = 0; i < envelope().size(); ++i) {
        low[i] = envelope()[i].first;
        up[i] = envelope()[i].second;
    }

    float* angles_old = population.MakeLine(sPt, ePt, up, low, ts->numberOfCycles());
//    float* angles_old = population.MakeLine(sPt, ePt, &envelope(), ts->numberOfCycles());
//    float* angles_old = population.MakeLine(sPt, ePt, ts->numberOfCycles());
    AngleList angles;
    for (index_t i = 0; i < index_t(envelope().size()); ++i)
        angles.push_back(angles_old[i]);


    // Decode the angles into points (initially aligned semi-arbitrarily)
    Point p(0.0, 0.0);
    Vector path(0.0, 0.0);  // This keeps track of how far this moves us
    refinement().clear();
    refinement().push_back(p);
    for (index_t i = 0; i < index_t(angles.size()); ++i) {
        Vector dp(1.0, tan(angles[i]));   // Find motion in X / Y
//        Vector dp(cos(angles[i]), sin(angles[i]));   // Find motion in X / Y
        p += dp;            // Move us by this amound and place a point here
        refinement().push_back(p);  // Stick it in
        path += dp;                 // Update our path vector
//        logger << "Angle[" << i << "]: " << angles[i] << endl
//               << "  p is " << p;
//        logger.debug();
    }


    /////////////////////////////////////////////////////////////////////////
    // Transform these points to match up with this Edge's endpoints
    /////////////////////////////////////////////////////////////////////////

    // Geometric properties of the original Edge
    scalar_t guideLength = length;
    scalar_t guideAngle = signedAngle(guide, Vector(1.0, 0.0));

    // Geometric properties of the generated Points
    scalar_t pathLength = magnitude(path);
    scalar_t pathAngle = signedAngle(path, Vector(1.0, 0.0));
//    if (path[1] < 0.0)      // The angle might be negative
//        pathAngle = -pathAngle;

    // The differences between the two
    scalar_t scaleFactor = guideLength / pathLength;
    scalar_t rotationAngle = guideAngle - pathAngle;

    // Build a to-world-space transformation matrix to "set things right"
    inca::math::Matrix<scalar_t, 3, 3, true> T, R, S, X;
    loadTranslation<scalar_t,3>(T, start - Point(0.0, 0.0));
    loadScaling<scalar_t, 3>(S, Vector(scaleFactor, scaleFactor));
    loadRotation2D(R, rotationAngle);
    X = T % (R % S);
//    logger << "S: -------------" << endl << S << endl;
//    logger << "R: -------------" << endl << R << endl;
//    logger << "T: -------------" << endl << T << endl;
//    logger << "X: -------------" << endl << X << endl;
//    logger.info();

    // Now go back and apply the world-space transformation to each point
    PointList::iterator pt;
    for (pt = refinement().begin(); pt != refinement().end(); pt++) {
        Point & p = *pt;
        p = operator%<scalar_t, 3, 3, true>(X, p);
        //p = X % p; FIXME
//        cerr << *pt << endl;
    }
}

void MapData::EdgeData::unrefine() {
    refinement().clear();   // BOOM!!!...back to the stone age...
}

// Test whether or not this boundary has been refined
bool MapData::EdgeData::isRefined() const {
    // If there are more points than just the endpoints, it's refined
    return refinement().size() > 0;
}

// Create the envelope for this edge in a diamond pattern with the specified
// number of segments across the main axis and the given aspect ratio of
// perpendicular axis length to main axis length
void MapData::EdgeData::buildDiamondEnvelope(int segments,
                                             scalar_arg_t aspectRatio) {
    // Figure out how much to increment each segment's range by
    float dy = float(length() / 2.0 * aspectRatio / (segments / 2.0));
    envelope().resize(segments);    // Make sure we've got the right # of slots

    size_t levels = segments / 2 + segments % 2;    // Make sure we hit middle
    float y = 0;
    for (index_t i = 0; i < index_t(levels); ++i) {
        envelope()[i].first  = -y;  // Just reflect about the X axis...
        envelope()[i].second = y;
        envelope()[segments - i - 1] = envelope()[i]; // ...and the Y axis
        y += dy;
    }

    cerr << "Generating a diamond of length " << length() << " and ratio "
         << aspectRatio << " and " << segments << " segments:\n";
    for (index_t i = 0; i < index_t(envelope().size()); ++i)
        cerr << '\t' << envelope()[i].second << endl;
}


/*---------------------------------------------------------------------------*
 | Map::Spike functions
 *---------------------------------------------------------------------------*/
// Equality comparison operator
bool Map::Spike::operator==(const Spike &s) const {
    return (type == s.type) && (elementID == s.elementID)
        && (snappedPosition == s.snappedPosition);
}


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
// Default constructor with optional map name
Map::Map(const string &nm) {
    name = nm;
}


/*---------------------------------------------------------------------------*
 | Complex search functions
 *---------------------------------------------------------------------------*/
// Find the face-vertex that is "facing towards" this vertex
Map::FaceVertexPtr
Map::enclosingFaceVertex(VertexConstPtr v, Point p) const {
    Vertex::ccw_face_vertex_iterator fvi, done;
    for (fvi = v->faceVerticesCCW(); fvi != done; ++fvi) {
        FaceVertexPtr fv = *fvi;
        EdgePtr eCCW = fv->edgeCCW();
        EdgePtr eCW  = fv->edgeCW();

        // See if we should trivially accept this FV
        if (eCCW == eCW)    // Then this is the only FV there is!
            return fv;      // This catches one-edge and no-edge cases

        VertexConstPtr vCCW = eCCW->otherVertex(v);
        VertexConstPtr vCW  = eCW->otherVertex(v);
        Point vp = v->position();
        Vector toCCW = vCCW->position() - vp;
        Vector toCW  = vCW->position() - vp;
        Vector toP   = p - vp;

        scalar_t alpha = canonicalAngle(toCW, toP),
                 beta  = canonicalAngle(toCW, toCCW);

        // See if this FV's "wedge" contains v2
        if (alpha < beta)
            return fv;
    }

    // This shouldn't happen...
    cerr << "enclosingFaceVertex(V-" << v->id() << ", " << p
         << ") failed!" << endl;
    return NULL;
}


/*---------------------------------------------------------------------------*
 | Topology modification functions
 *---------------------------------------------------------------------------*/
// Create a new Vertex at the specified location
Map::VertexPtr Map::createVertex(Point p) {
    return Mesh::createVertex(p);
}

// (Possibly) create a new Vertex from a Spike
Map::VertexPtr Map::createVertex(const Spike &spike) {
    switch (spike.type) {
        case NewSpike:          // OK, we need to make a new one
            return createVertex(spike.snappedPosition);
        case SplitEdge:         // Cut an edge in half
            return edge(spike.elementID)->split(spike.snappedPosition);
        case LinkVertex:        // Just take an existing one
            return vertex(spike.elementID);
        default:
            return NULL;
    }
}

// Create a face out of a sequence of spikes
Map::FacePtr Map::createFace(const SpikeList &spikes, TerrainTypePtr tt) {
    // First, create any necessary Vertices
    VertexPtrList vtx;  // The sequence of vertices
    for (SpikeList::const_iterator s = spikes.begin(); s != spikes.end(); ++s)
        vtx.push_back(createVertex(*s));

    // Now that the vertices all exist, create the face from them
    return createFace(vtx.begin(), vtx.end(), tt);
}


/*---------------------------------------------------------------------------*
 | GA refinement functions
 *---------------------------------------------------------------------------*/
#if 0
// Refine every edge and vertex in the map
void Map::refineMap() {
    // Go do that thing you do on each Edge
    EdgePtrList::iterator es;
    for (es = edges().begin(); es != edges().end(); ++es)
        refineEdge(*es);

    // Now that all the b's are done, do the i's
    VertexPtrList::iterator vs;
    for (vs = vertices().begin(); vs != vertices().end(); ++vs)
        refineVertex(*vs);
}

// Create a refinement of a Boundary.
void Map::refineEdge(EdgePtr e) {
    // Get the endpoints 'n' stuff
    Point start = e->startVertex()->position();
    Point end   = e->endVertex()->position();
    Vector guide = end - start;

    // Get the terrain properties for this seam
    TerrainSeamConstPtr ts = e->terrainSeam();

    // Figure what this looks like
    scalar_t guideLength = magnitude(guide);          // Length of the edge
    scalar_t guideAngle = angle(guide, Vector(1.0, 0.0));   // Angle with the X axis
    if (guide[1] < 0.0)      // The angle might be negative
        guideAngle = -guideAngle;

    // Choose the number of segments 
    int segments = 20;

    // Report it to our oh-so-intelligent user (HAIL TO THEE, USER!)
    logger << "Refinement for Boundary " << e->id() << endl
           << "   # of chromosomes: " << ts->numberOfChromosomes() << endl
           << "   Smoothness: " << ts->smoothness() << endl
           << "   Mutation rate: " << ts->mutationRatio() << endl
           << "   Crossover rate: " << ts->crossoverRatio() << endl
           << "   Selection ratio: " << ts->selectionRatio() << endl
           << "   # of cycles: " << ts->numberOfCycles() << endl
           << "   Segments: " << segments << endl;
    logger.info();

    // Set up our genetic algorithm and generate a line
    GA::Population population(ts->numberOfChromosomes(),
                              ts->smoothness(),
                              ts->mutationRatio(),
                              ts->crossoverRatio(),
                              ts->selectionRatio());

    
    GA::Point sPt, ePt;
    sPt.x = 0;
    sPt.y = 0;
    ePt.x = 5;
    ePt.y = 0;
    //ePt.x = int(guideLength * resolution());
    float* angles = population.MakeLine(sPt, ePt, ts->numberOfCycles());

    // Decode the angles into points (initially aligned semi-arbitrarily)
    Point p(0.0, 0.0);
    Vector path(0.0, 0.0);  // This keeps track of how far this moves us
    scalar_t accumulatedAngle = 0.0;
    PointList & refinement = e->refinement();
    refinement.clear();
    refinement.push_back(p);
    for (int i = 0; i < ePt.x; i++) {
        accumulatedAngle = angles[i];
        Vector dp(cos(accumulatedAngle), sin(accumulatedAngle));   // Find motion in X / Y
        p += dp;            // Move us by this amound and place a point here
        refinement.push_back(p);    // Stick it in
        path += dp;                 // Update our path vector
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

    inca::math::Matrix<scalar_t, 3, 3, true> T, R, S, X;
    loadTranslation<double,3>(T, start - Point(0.0, 0.0));
    loadScaling<double, 3>(S, Vector(scaleFactor, scaleFactor));
    loadRotation2D(R, rotationAngle);
    X = T % (R % S);
//    logger << "S: -------------" << endl << S << endl;
//    logger << "R: -------------" << endl << R << endl;
//    logger << "T: -------------" << endl << T << endl;
//    logger << "X: -------------" << endl << X << endl;
//    logger.info();

    // Now go back and finesse the points into place
    PointList::iterator pt;
    for (pt = refinement.begin(); pt != refinement.end(); pt++) {
        Point & p = *pt;
        p = operator%<scalar_t, 3, 3, true>(X, p);
        //p = X % p;
//        cerr << *pt << endl;
    }
}

// Smooth out the intersection of all the edge refinements that meet here
void Map::refineVertex(VertexPtr v) {
//    size_t eCount = v->edgeCount();

    // Calculate an unweighted average of the nearby points
//    Point p(0.0, 0.0);
//    for (index_t i = 0; i < index_t(eCount); ++i)
//        p += .boundary(i).refinement()[1];
//    p /= bCount;

    // Modify the start point of each boundary refinement
//    for (index_t i = 0; i < index_t(bCount); i++)
//        in.boundary(i).refinement()[0] = p;
}
#endif