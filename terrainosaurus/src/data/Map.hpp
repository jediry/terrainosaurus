/*
 * File: Map.hpp
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
 *
 * FIXME: the above description is outdated!!! Now, Map is a subclass of the
 *      mesh and uses the data objct hooks to stick data in edges, vertices, etc.
 */

#ifndef TERRAINOSAURUS_MAP
#define TERRAINOSAURUS_MAP

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapData;
    class Map;

    // Pointer typedefs
    typedef shared_ptr<Map>       MapPtr;
    typedef shared_ptr<Map const> MapConstPtr;
};

// Import the mesh that manages our topology
#include <inca/math/topology/WingedEdgeMesh>

// Import the TerrainDescriptor definition
#include "TerrainLibrary.hpp"

// Import the STL function object classes
#include <functional>


// Extra, map-specific data that gets woven into the topological mesh
class terrainosaurus::MapData {
public:
    // Import geometric type definitions from the namespace
    typedef scalar_t        scalar_t;
    typedef scalar_arg_t    scalar_arg_t;
    typedef Point2D         Point;
    typedef Vector2D        Vector;

    // Range of scalar values
    typedef std::pair<float, float> Range;

    // Container typedefs
    typedef std::vector<Point>      PointList;
    typedef std::vector<float>      AngleList;
    typedef std::vector<Range>      RangeList;


/*---------------------------------------------------------------------------*
 | Constructors & properties
 *---------------------------------------------------------------------------*/
private:
    PROPERTY_OWNING_OBJECT(MapData);

public:
    MapData(const string &nm = "")
        : name(this, nm), terrainLibrary(this) { }

    // A string describing the map
    rw_property(string, name, "");

    // The library of available terrain types
    rw_ptr_property(TerrainLibrary, terrainLibrary, NULL);

    // Query the resolution (samples/world-space unit) of the elevation data
    scalar_t resolution() const;


    // The extra data that goes into a vertex:
    class VertexData {
    public:
        // Constructor
        VertexData(Point p = Point(0.0)) : _position(p) { }

        // 2D point giving map-space location of an intersection
        Point position() const { return _position; }
        void setPosition(Point &p) { _position = p; }

    protected:
        Point _position;
    };


    class FaceData {
    public:
        // Constructor
        FaceData(TerrainTypePtr tt = TerrainTypePtr()) : _terrainType(tt) { }

        // The TerrainType record describing the terrain characteristics for
        // the map region corresponding to this face
        TerrainTypePtr terrainType() const     { return _terrainType; }
        void setTerrainType(TerrainTypePtr tt) { _terrainType = tt; }

    protected:
        TerrainTypePtr _terrainType;
    };


    class EdgeData {
    public:
        // Constructor
        EdgeData() { /*unrefine();*/ }

        // Access to the MapData for the owning Map
        MapData & mapData() const;

        // Geometric properties of the Boundary
        Point startPoint() const;   // Utility functions for grabbing the
        Point endPoint() const;     // current geometric end-points of the edge
        scalar_t length() const;    // Length of the edge
        scalar_t angle() const;     // Polar angle with the X axis
        Point nearestPointTo(Point p) const;

        // The TerrainSeam record describing the type of seam that the
        // boundary represented by this edge has, including the parameters
        // for the GA refinement. This is determined dynamically from the
        // TerrainTypes of the faces on either side of the edge.
        TerrainSeamConstPtr terrainSeam() const;

        // The sequence of world-space points delineating the refined
        // boundary between the two Faces, and its containing envelope
        const PointList & refinement() const { return _refinement; }
              PointList & refinement()       { return _refinement; }
        const RangeList & envelope()   const { return _envelope; }
              RangeList & envelope()         { return _envelope; }

        // (Un)refine this Edge
        void refine();
        void unrefine();
        bool isRefined() const;

        // Construct the envelope for this Edge
        void buildDiamondEnvelope(int segments, scalar_arg_t aspectRatio);

    protected:
        PointList _refinement;      // The points on our refined boundary
        RangeList _envelope;        // The bounds for the refinement
    };
};


class terrainosaurus::Map 
    : public inca::math::WingedEdgeMesh<terrainosaurus::MapData,
                                        terrainosaurus::MapData::VertexData,
                                        terrainosaurus::MapData::FaceData,
                                        terrainosaurus::MapData::EdgeData> {
/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // A surveying "spike", marking a world-space point on the map, including
    // whether that point is incident to a map boundary or intersection. Lists
    // of Spikes are used to create new regions in the map, which may be
    // topologically connected to existing regions (and may potentially split
    // existing regions).
    enum SpikeType {
        NewSpike,       // This spike is not connected to anything else
        SplitEdge,      // This spike splits an edge (add new vertex)
        LinkVertex,     // This spike reuses an existing vertex
    };

    struct Spike {
        // Constructor (defaults to a new, unattached spike)
        explicit Spike(Point wp = Point(0.0), Point sp = Point(0.0),
                       SpikeType t = NewSpike, id_t id = -1)
            : worldPosition(wp), snappedPosition(sp),
              type(t), elementID(id) { }

        // Equality comparison operator
        bool operator==(const Spike & s) const;

        // Data members
        Point worldPosition;    // World-space location of the spike
        Point snappedPosition;  // Position after snapping to other objects
        SpikeType type; // What relation does this have to existing topology?
        int elementID;  // What existing element are we touching (-1 == none)?
    };

    // Container typedefs
    typedef vector<Spike> SpikeList;

protected:
    // This function object takes two Vertices and searches around the first
    // for the FaceVertex that best "encloses" the other Vertex. In geometric
    // terms, this means the FaceVertex is considered to have an infinite
    // "wedge" envelope, delimited by the Edges on either side of it, and
    // a Vertex is considered "enclosed" if its location falls within that
    // wedge.
    class FindEnclosingFaceVertex
        : public std::binary_function<VertexConstPtr, Point, FaceVertexPtr> {
    public:
        // Constructor
        FindEnclosingFaceVertex(const Map &m) : map(m) { }

        // Function call operator
        FaceVertexPtr operator()(VertexConstPtr v, Point p) const {
            return map.enclosingFaceVertex(v, p);
        }

    protected:
        const Map &map;
    };


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    // Default constructor with optional map name
    explicit Map(const string &nm = "");


/*---------------------------------------------------------------------------*
 | Complex search functions
 *---------------------------------------------------------------------------*/
public:
    // Find the face-vertex that is "facing towards" this vertex
    FaceVertexPtr enclosingFaceVertex(VertexConstPtr v, Point p) const;


/*---------------------------------------------------------------------------*
 | Topology modification functions
 *---------------------------------------------------------------------------*/
public:
    // Create a new Vertex at the specified location
    VertexPtr createVertex(Point p);

    // (Possibly) create a new Vertex from a Spike
    VertexPtr createVertex(const Spike &spike);

    // Create a new face from a list of Spikes
    FacePtr createFace(const SpikeList & spikes, TerrainTypePtr tt);

    // Create a new face from an iterator range of VertexPtrs
    template <class iterator>
    FacePtr createFace(iterator begin, iterator end, TerrainTypePtr tt) {
        return Mesh::createFace(begin, end,
                                FindEnclosingFaceVertex(*this),
                                FaceData(tt));
    }

#if 0
/*---------------------------------------------------------------------------*
 | Map refinement courtesy of Mike's magical genetic algorithms
 *---------------------------------------------------------------------------*/
public:
    // Refine each boundary in the map, and then each intersection
    void refineMap();

    // Refine a single edge in the map
    void refineEdge(EdgePtr e);

    // Smooth out the intersection of all the edge refinements that meet here
    void refineVertex(VertexPtr v);
#endif
};
#endif