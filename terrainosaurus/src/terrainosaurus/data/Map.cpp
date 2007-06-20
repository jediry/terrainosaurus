/*
 * File: Map.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the functions defined in Map.hpp.
 */

// Import class definition
#include "Map.hpp"
using namespace inca::math;
using namespace terrainosaurus;


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

// Find the direction this edge is going in 2D space
MapData::Vector MapData::EdgeData::direction() const {
    return endPoint() - startPoint();
}

// Find a (possibly) smoothed angle to try to match at each vertex
MapData::scalar_t MapData::EdgeData::startAngle() const {
    // Up-cast to the edge type, then get the endpoint
    Map::EdgeConstPtr edge = static_cast<Map::EdgeConstPtr>(this);
    Map::VertexConstPtr vertex = edge->startVertex();
    if (vertex->edgeCount() == 2) {     // Only smooth valence-2 vertices
        Map::EdgeConstPtr otherEdge = edge->edgeCCW(vertex);
        scalar_t myAngle  = angle(),
                 hisAngle = otherEdge->angle();
        if (vertex != otherEdge->startVertex()) {   // Flip direction
            if (hisAngle < 0)   hisAngle += PI<scalar_t>();
            else                hisAngle -= PI<scalar_t>();
        }
        scalar_t targetAngle = (hisAngle - myAngle - PI<scalar_t>()) / 2;
        if (targetAngle < -PI<scalar_t>() / 2)
            targetAngle += PI<scalar_t>();
        return targetAngle;
    } else {
        return scalar_t(0);
    }
}
MapData::scalar_t MapData::EdgeData::endAngle() const {
    // Up-cast to the edge type, then get the endpoint
    Map::EdgeConstPtr edge = static_cast<Map::EdgeConstPtr>(this);
    Map::VertexConstPtr vertex = edge->endVertex();
    if (vertex->edgeCount() == 2) {     // Only smooth valence-2 vertices
        Map::EdgeConstPtr otherEdge = edge->edgeCCW(vertex);
        scalar_t myAngle  = angle(),
                 hisAngle = otherEdge->angle();
        if (vertex != otherEdge->endVertex()) {   // Flip direction
            if (hisAngle < 0)   hisAngle += PI<scalar_t>();
            else                hisAngle -= PI<scalar_t>();
        }
        scalar_t targetAngle = (hisAngle - myAngle - PI<scalar_t>()) / 2;
        if (targetAngle < -PI<scalar_t>() / 2)
            targetAngle += PI<scalar_t>();
        return targetAngle;
    } else {
        return scalar_t(0);
    }
}

// Geometric, cartesian length of the edge
MapData::scalar_t MapData::EdgeData::length() const {
    return inca::math::distance(startPoint(), endPoint());
}

// Angle (between -pi and pi) with the X axis
MapData::scalar_t MapData::EdgeData::angle() const {
    return signedAngle(normalize(direction()), Vector(1, 0));
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

// Add a refinement for this Edge
void MapData::EdgeData::addRefinement(const PointList & ref) {
    // Make sure we don't have an old refinement lingering around
    deleteRefinement();

    // We have to transform these points so that the first and last points
    // match up with the endpoints of this Edge.

    // 1) Get the geometric properties of the original Edge
    Point start = startPoint();
    Point end   = endPoint();
    Vector edgePath = end - start;
    scalar_t edgeLength = magnitude(edgePath);
    scalar_t edgeAngle = signedAngle(edgePath, Vector(1.0, 0.0));

    // 2) Get the geometric properties of the generated Points
    Vector refPath = ref.back() - ref.front();
    scalar_t refLength = magnitude(refPath);
    scalar_t refAngle = signedAngle(refPath, Vector(1.0, 0.0));

    // 3) Find how much to transform to make them line up
    scalar_t scaleFactor = edgeLength / refLength;
    scalar_t rotationAngle = edgeAngle - refAngle;

    // 4) Build a to-world-space transformation matrix to "set things right"
    inca::math::Matrix<scalar_t, 3, 3, true> T, R, S, X;
    inca::math::loadTranslation<scalar_t,3>(T, start - Point(0.0, 0.0));
    inca::math::loadScaling<scalar_t, 3>(S, Vector(scaleFactor, scaleFactor));
    inca::math::loadRotation2D(R, rotationAngle);
    X = T % (R % S);
//    cerr << "Scale factor is " << scaleFactor << " (" << guideLength << " / " << pathLength << ")\n";
//    cerr << "S: -------------" << endl << S << endl;
//    cerr << "R: -------------" << endl << R << endl;
//    cerr << "T: -------------" << endl << T << endl;
//    cerr << "X: -------------" << endl << X << endl;
//    logger.info();

    // 5) Apply this transformation to each point and insert it into the refinement
    PointList::const_iterator pt;
    for (pt = ref.begin(); pt != ref.end(); pt++) {
//        cerr << "Old p " << p << endl;
//        p = operator%<scalar_t>(X, p);
        refinement().push_back(X % *pt); //FIXME
//        cerr << "New p " << p << endl << endl;
//        cerr << *pt << endl;
    }
}

void MapData::EdgeData::deleteRefinement() {
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

    SizeType levels = segments / 2 + segments % 2;    // Make sure we hit middle
    float y = 0.1f;
    for (IndexType i = 0; i < IndexType(levels); ++i) {
        envelope()[i].first  = -y;  // Just reflect about the X axis...
        envelope()[i].second = y;
        envelope()[segments - i - 1] = envelope()[i]; // ...and the Y axis
        y += dy;
    }

/*    cerr << "Generating a diamond of length " << length() << " and ratio "
         << aspectRatio << " and " << segments << " segments:\n";
    for (IndexType i = 0; i < IndexType(envelope().size()); ++i)
        cerr << '\t' << envelope()[i].second << endl;*/
}


/*---------------------------------------------------------------------------*
 | Map::Spike functions
 *---------------------------------------------------------------------------*/
// Equality comparison operator
bool Map::Spike::operator==(const Spike & s) const {
    return (type == s.type) && (elementID == s.elementID)
        && (snappedPosition == s.snappedPosition);
}


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
// Default constructor with optional map name
Map::Map(const std::string & nm) {
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
    INCA_ERROR(__FUNCTION__ << "(V-" << v->id() << ", " << p << ") failed!")
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
