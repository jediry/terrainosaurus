/*
 * File: Map.h++
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The Map class is an abstraction of a 2D mesh of (possibly) connected
 *      polygons. A map is made up of regions (faces), divided by boundaries
 *      (edges), which meet at intersections (vertices).
 *
 *      While the actual data for the map is stored in a winged-edge mesh
 *      data structure, it is normally not necessary to access the mesh
 *      directly (or even to be aware of its existence), since the Map
 *      (and its constituent classes Intersection, Boundary, RefinedBoundary,
 *      and Region) provide a cleaner interface to the operations that would
 *      normally be needed. Read the comments before each class definition
 *      for clues on how to use that class.
 *
 *      A word of warning: since the Map objects are a very thin layer atop
 *      the winged-edge mesh structure, they are intended to be used
 *      'disposably' -- that is, you should usually instantiate them on
 *      the stack (rather than dynamically allocating them) and then let them
 *      die when they go out of scope. If you delete part of the mesh and then
 *      attempt to use a Map object that corresponds to the deleted object,
 *      you'll probably get a seg-fault. So the moral of the story is "map
 *      objects should have short lives".
 */

#ifndef TERRAINOSAURUS_MAP
#define TERRAINOSAURUS_MAP

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class Map;

    // Pointer typedefs
    typedef shared_ptr<Map>       MapPtr;
    typedef shared_ptr<Map const> MapConstPtr;
};

// Import the mesh that manages our topology
#include <inca/math/topology/WingedEdge-PolygonMesh>

// Import the TerrainDescriptor definition
#include "TerrainDescriptor.hpp"


class terrainosaurus::Map {
private:
    // Set up this class to own properties
    PROPERTY_OWNING_OBJECT(Map);
    

/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // Forward declarations for map parts
    class TerrainType;
    class Intersection;
    class Boundary;
    class RefinedBoundary;
    class Region;

    // Import geometric and topological types from PolygonMesh
    typedef inca::poly::WEPolygonMesh<double, 2>    PolygonMesh;
    typedef shared_ptr<PolygonMesh>             PolygonMeshPtr;
    typedef PolygonMesh::Vertex                 Vertex;
    typedef PolygonMesh::Face                   Face;
    typedef PolygonMesh::Edge                   Edge;
    typedef PolygonMesh::VertexPtr              VertexPtr;
    typedef PolygonMesh::FacePtr                FacePtr;
    typedef PolygonMesh::EdgePtr                EdgePtr;

    // Import scalar and vector type definitions
    INCA_MATH_SCALAR_TYPEDEFS(double, IS_NOT_WITHIN_TEMPLATE, /* */, /* */);
    INCA_MATH_VECTOR_TYPEDEFS(scalar_t, 2, /* */, /* */);

    // Import terrain-related types from TerrainDescriptor
    typedef TerrainDescriptor::Color            Color;

    // List of Points, representing a Boundary refinement
    typedef list<Point> PointList;
    typedef shared_ptr<PointList>       PointListPtr;
    typedef shared_ptr<PointList const> PointListConstPtr;


/*---------------------------------------------------------------------------*
 | TerrainType wrapper for a TerrainDescriptor
 |      Allows direct access to the TerrainDescriptor and its associated
 |      properties
 *---------------------------------------------------------------------------*/
    class TerrainType {
    public:
        // Constructor
        TerrainType(const Map &m, unsigned int i) : _map(m), _id(i) { }
        TerrainType & operator=(const TerrainType &t) {
            if (&_map == &(t._map))     // Make sure we belong to the same map!
                _id = t.id();
            return *this;
        }

        // Accessors
        const Map & map() const { return _map; }
        TerrainDescriptorPtr descriptor() const;
        unsigned int id() const { return _id; }
        const string & name() const { return descriptor()->name(); }
        const Color & color() const { return descriptor()->color(); }
    protected:
        const Map &_map;    // The Map object that we're associated with
        unsigned int _id;   // The ID of the TerrainDescriptor
    };


/*---------------------------------------------------------------------------*
 | Region wrapper for a Face
 |      Allows direct access to the terrain-type, and array-style and
 |      linked-list-style access to all adjacent Boundaries and Intersections
 *---------------------------------------------------------------------------*/
    class Region {
    public:
        // Constructor
        Region(const Map &m, FacePtr f) : _map(m), _face(f) { }
        Region & operator=(const Region &r) {
            if (&_map == &(r._map))     // Make sure we belong to the same map!
                _face = r.face();
            return *this;
        }

        // Accessors
        const Map & map() const { return _map; }
        FacePtr face() const { return _face; }
        unsigned int id() const { return face()->getID(); }

        // Properties of the Region
        TerrainType terrainType() const;
        void setTerrainType(const TerrainType &type);

        // Boundary objects, looked up by index (CCW around the Region)
        // The resulting Boundary will be oriented such that going from
        // 'start' to 'end' is a CCW rotation around the Region
        size_t boundaryCount() const;
        Boundary boundary(index_t index) const;

        // Boundary objects, looked up from their neighbors
        // The resulting Boundary will have the same orientation as the
        // 'from' Boundary (i.e., they will have a consistent direction
        // around this Region).
        Boundary boundaryCCW(const Boundary &from) const;
        Boundary boundaryCW(const Boundary &from) const;

        // Intersection objects, looked up by index (CCW around the Region)
        size_t intersectionCount() const;
        Intersection intersection(index_t index) const;

        // Intersection objects, looked up from their neighbors
        Intersection intersectionCCW(const Intersection &from) const;
        Intersection intersectionCW(const Intersection &from) const;

    protected:
        const Map &_map;
        FacePtr _face;
    };


/*---------------------------------------------------------------------------*
 | Intersection wrapper for a Vertex
 |      Allows array-style and linked-list-style access to all adjacent
 |      Boundaries and Regions
 *---------------------------------------------------------------------------*/
    class Intersection {
    public:
        // Constructor
        Intersection(const Map &m, VertexPtr v) : _map(m), _vertex(v) { }
        Intersection & operator=(const Intersection &i) {
            if (&_map == &(i._map))     // Make sure we belong to the same map!
                _vertex = i.vertex();
            return *this;
        }

        // Accessors
        const Map & map() const { return _map; }
        VertexPtr vertex() const { return _vertex; }
        unsigned int id() const { return vertex()->getID(); }

        // Properties of this intersection
        const Point & location() const;

        // Boundary objects, looked up by index (CCW around the Intersection)
        // The resulting Boundary will have this intersection as its 'start'
        size_t boundaryCount() const;
        Boundary boundary(index_t index) const;

        // Boundary objects, looked up from their neighbors
        // The resulting Boundary will have this intersection as its 'start'
        Boundary boundaryCCW(const Boundary & from) const;
        Boundary boundaryCW(const Boundary & from) const;

        // Region objects, looked up by index (CCW around the Intersection)
        size_t regionCount() const;
        Region region(index_t index) const;

        // Region objects, looked up from their neighbors
        Region regionCCW(const Region & from) const;
        Region regionCW(const Region & from) const;

    protected:
        const Map &_map;
        VertexPtr _vertex;
    };


/*---------------------------------------------------------------------------*
 | Boundary wrapper for an Edge
 |      Allows direct access to the start and end Intersections, the left
 |      and right Regions (i.e., what would be on your left/right if you
 |      walked down the Boundary from start to end), and the RefinedBoundary
 |      corresponding to it.
 |
 |      Note that there are two possible 'start' Intersections for any
 |      Boundary. Which one you choose will affect how the Boundary object
 |      behaves (see the comments on RefinedBoundary for more information).
 |      You can retrieve the Boundary with the opposite orientation by
 |      calling reverse().
 *---------------------------------------------------------------------------*/
    class Boundary {
    public:
        // Constructor
        Boundary(const Map &m, EdgePtr e, VertexPtr v)
            : _map(m), _edge(e), _vertex(v) { }
        Boundary & operator=(const Boundary &b) {
            if (&_map == &(b._map)) {   // Make sure we belong to the same map!
                _edge = b.edge();
                _vertex = b.vertex();
            }
            return *this;
        }

        // Accessors
        const Map & map() const { return _map; }
        EdgePtr edge() const { return _edge; }
        VertexPtr vertex() const { return _vertex; }
        unsigned int id() const { return edge()->getID(); }

        // Properties of the Boundary
        double length() const;

        // Get the same boundary, but oriented the other way (end -> start)
        Boundary reverse() const;

        // See if this corresponds to the orientation of the edge in the
        // underlying mesh
        bool isReversed() const;

        // Access to the corresponding refined boundary. The resulting
        // RefinedBoundary will have the same orientation as this Boundary.
        bool isRefined() const;
        RefinedBoundary refinement() const;

        // Intersection objects
        Intersection startIntersection() const;
        Intersection endIntersection() const;

        // Region objects adjacent to this Boundary
        Region leftRegion() const;
        Region rightRegion() const;

    protected:
        const Map &_map;
        EdgePtr _edge;
        VertexPtr _vertex;
    };


/*---------------------------------------------------------------------------*
 | RefinedBoundary representing a GA-modified version of a map boundary
 *---------------------------------------------------------------------------*/
    class RefinedBoundary {
    public:
        // Constructors
        RefinedBoundary(const Boundary &b, PointListPtr p)
            : _guide(b), _refinement(p) { }

        // The simple 'guide' edge that this refines
        Boundary guide() const { return _guide; }

        // Points comprising the refinement
        const PointList & points() const;
              PointList & points();
        size_t size() const;                   // Number of points
        const Point & operator[](index_t index) const;
              Point & operator[](index_t index);        // Array syntax accessors
        void push_front(Point p);
        void push_back(Point p);
        void clear();
        void erase(index_t index);
        void insert(index_t beforeIndex, Point p);

    protected:
        // Low-level functions for dealing with points bidirectionally
        PointList::iterator fromStart(index_t index);
        PointList::const_iterator fromStart(index_t index) const;
        PointList::iterator fromEnd(index_t index);
        PointList::const_iterator fromEnd(index_t index) const;

        PointListPtr _refinement;
        Boundary _guide;
    };


/*---------------------------------------------------------------------------*
 | Container typedefs
 *---------------------------------------------------------------------------*/
    typedef vector<TerrainType>             TerrainTypeList;
    typedef vector<Region>                  RegionList;
    typedef vector<Intersection>            IntersectionList;
    typedef vector<Boundary>                BoundaryList;
    typedef hash_map<EdgePtr, PointListPtr> BoundaryRefinementMap;


/*---------------------------------------------------------------------------*
 | Constructors & properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor with optional map name
    explicit Map(const string &nm = "");
    
    // Constructor taking an already-constructed mesh for the topology
    explicit Map(PolygonMeshPtr m, const string &nm = "");

    // A string describing the map
    rw_property(string, name, "");

    // The mesh that maintains the map's connectivity
    rw_ptr_property(PolygonMesh, mesh, new PolygonMesh());

    // The library of available terrain types
    rw_list_property(TerrainDescriptorPtr, library);

    // Genetic algorithm parameters
    rw_property(int,   numberOfCycles,      5);     // How many mutation cycles we do?
    rw_property(int,   numberOfChromosomes, 5);
    rw_property(float, smoothness,          0.2f);
    rw_property(float, mutationRate,        0.05f);
    rw_property(float, crossoverRate,       0.05f);
    rw_property(float, selectionRatio,      1.0f);
    rw_property(float, resolution,          20.0f); // Segments per world-unit

protected:
    // How we keep track of the refinements we've made
    mutable BoundaryRefinementMap refinedBoundaries;


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
public:
    // Access to mappy wrappers for mesh objects
    TerrainTypeList  terrainTypes()  const;
    RegionList       regions()       const;
    IntersectionList intersections() const;
    BoundaryList     boundaries()    const;

    size_t terrainTypeCount()  const;
    size_t regionCount()       const;
    size_t intersectionCount() const;
    size_t boundaryCount()     const;

    TerrainType terrainType(index_t index)   const;
    Region region(index_t index)             const;
    Intersection intersection(index_t index) const;
    Boundary boundary(index_t index)         const;

    RefinedBoundary refinementOf(const Boundary &b) const;


    void addRegion(const PointList & points);


/*---------------------------------------------------------------------------*
 | Map refinement courtesy of Mike's magical genetic algorithms
 *---------------------------------------------------------------------------*/
public:
    // Refine each boundary in the map, and then each intersection
    void refineMap();

    // Refine a single boundary in the map
    void refineBoundary(const Boundary &b);

    // Smooth out the intersection of all the RefinedBoundaries that meet here
    void refineIntersection(Intersection &i);
};

#endif
