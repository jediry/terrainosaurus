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
 */

#ifndef TERRAINOSAURUS_MAP
#define TERRAINOSAURUS_MAP

// Import Inca::Geometry library
#include <geometry/Geometry.h++>

// This is part of the Terrainosaurus terrain generation engine
namespace Terrainosaurus {
    // Forward declarations
    class Map;
};


class Terrainosaurus::Map {
/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // Math typedefs
    typedef Inca::Math::ScalarDouble        Scalar;
    typedef Scalar::type                    scalar_t;
    typedef Scalar::arg_type                scalar_arg_t;
    typedef Inca::Math::Point<Scalar, 2>    Point2D;
    typedef Inca::Math::Vector<Scalar, 2>   Vector2D;

    // PolygonMesh object typedefs
    typedef Inca::Geometry::WEPolygonMesh<Scalar, 2>    PolygonMesh;
    typedef PolygonMesh::Vertex                         Vertex;
    typedef PolygonMesh::Face                           Face;
    typedef PolygonMesh::Edge                           Edge;
    typedef PolygonMesh::VertexPtr                      VertexPtr;
    typedef PolygonMesh::FacePtr                        FacePtr;
    typedef PolygonMesh::EdgePtr                        EdgePtr;

    // TerrainType wrapper for a Material
    typedef int TerrainType;


/*---------------------------------------------------------------------------*
 | Region wrapper for a Face
 *---------------------------------------------------------------------------*/
    class Region {
    public:
        // Constructor
        Region(FacePtr f) : _face(f) { }

        // Accessors
        FacePtr face() const { return _face; }

    protected:
        FacePtr _face;
    };

/*---------------------------------------------------------------------------*
 | Intersection wrapper for a Vertex
 *---------------------------------------------------------------------------*/
    class Intersection {
    public:
        // Constructor
        Intersection(VertexPtr v) : _vertex(v) { }

        // Accessors
        VertexPtr vertex() const { return _vertex; }

    protected:
        VertexPtr _vertex;
    };

/*---------------------------------------------------------------------------*
 | Boundary wrapper for an Edge
 *---------------------------------------------------------------------------*/
    class Boundary {
    public:
        // Constructor
        Boundary(EdgePtr e) : _edge(e) { }

        // Accessors
        EdgePtr edge() const { return _edge; }

    protected:
        EdgePtr _edge;
    };

/*---------------------------------------------------------------------------*
 | RefinedBoundary representing a GA-modified version of a map boundary
 *---------------------------------------------------------------------------*/
    class RefinedBoundary {
    public:
        // Constructor
        RefinedBoundary(EdgePtr e) : _edge(e) { }

        // Accessors
        EdgePtr edge() const { return _edge; }

    protected:
        EdgePtr _edge;
    };

    // Pointer typedefs
    typedef Region *                                RegionPtr;
    typedef Intersection *                          IntersectionPtr;
    typedef Boundary *                              BoundaryPtr;
    typedef RefinedBoundary *                       RefinedBoundaryPtr;

    // List typedefs
    typedef vector<Region>                          RegionList;
    typedef vector<Intersection>                    IntersectionList;
    typedef vector<Boundary>                        BoundaryList;
    typedef hash_map<EdgePtr, RefinedBoundaryPtr>   BoundaryRefinementMap;


/*---------------------------------------------------------------------------*
 | Constructors & Destructor
 *---------------------------------------------------------------------------*/
public:
    explicit Map() { }
    explicit Map(const PolygonMesh &mesh) : _mesh(mesh) { }

protected:
    // The data structure underlying the whole thing
    PolygonMesh _mesh;

    // How we keep track of the refinements we've made
    BoundaryRefinementMap refinedBoundaries;


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
public:
    // Access to the underlying PolygonMesh
//    void setMesh(const PolygonMesh &m) { _mesh = m; }
    const PolygonMesh & mesh() const { return _mesh; }

    // Access to mappy wrappers for mesh objects
    RegionList       regions() const;
    IntersectionList intersections() const;
    BoundaryList     boundaries() const;


/*---------------------------------------------------------------------------*
 | Map refinement courtesy of genetic algorithms
 *---------------------------------------------------------------------------*/
public:
    // Refine each boundary in the map, and then each intersection
    void refineMap();

    // Refine a single boundary in the map
    void refineBoundary(const Boundary &b);

protected:
    // Create a refinement of a Boundary
    void refineBoundary(const Boundary &b, RefinedBoundary &rb);

    // Smooth out the intersection of all the RefinedBoundaries that meet here
    void refineIntersection(Intersection &i);
};

#endif
