/*
 * File: MeshSelection.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The MeshSelection class represents a selection of items in a map.
 *      It inherits all of the set operations of the Inca library's selection,
 *      and adds the characteristic that it can represent a selection of the
 *      different types of elements contained in a map: Faces, Vertices,
 *      and Edges (only one type of element may be selected at a time).
 *      Essentially, a MeshSelection is a sort of set that can switch between
 *      different element types.
 */

#ifndef TERRAINOSAURUS_MESH_SELECTION
#define TERRAINOSAURUS_MESH_SELECTION

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MeshSelection;

    // Pointer typedefs
    typedef shared_ptr<MeshSelection>       MeshSelectionPtr;
    typedef shared_ptr<MeshSelection const> MeshSelectionConstPtr;
};

// Import superclass definition
#include <inca/world/SelectionSet.hpp>

// Import Map definition
#include "Map.hpp"


class terrainosaurus::MeshSelection : public inca::world::SelectionSet {
private:
    // Set up this object to own properties
//    PROPERTY_OWNING_OBJECT(MeshSelection);


/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // Import topological types from the mesh
    typedef Map::VertexConstPtr     VertexConstPtr;
    typedef Map::FaceConstPtr       FaceConstPtr;
    typedef Map::EdgeConstPtr       EdgeConstPtr;

    // Enumeration of the different types of elements that may be selected
    enum ElementType {
        Vertices,       // We're selecting Vertices!
        Edges,          // No, wait! We're selecting Edges!
        Faces,          // No, you're both wrong. It's Faces!
    };


/*---------------------------------------------------------------------------*
 | Constructors & data
 *---------------------------------------------------------------------------*/
public:
    // Default constructor
    MeshSelection() : type(Faces) { }
//    MeshSelection() : map(this), type(Faces) { }

    // Constructor taking the Map whose parts are to be selected
    MeshSelection(MapConstPtr m) : _map(m), type(Faces) { }
//    MeshSelection(MapConstPtr m) : map(this, m), type(Faces) { }

    // The map we're selecting things from
//    rw_ptr_property(Map, map, NULL);
    MapConstPtr map() const { return _map; }
    void setMap(MapConstPtr m) { _map = m; }

    // The type of element we're currently selecting
    ElementType elementType() const { return type; }
    void setElementType(ElementType t); // Pick element type (clears selection)

    // Same as above, but based on template parameter
    template <class type> void setElementType();

protected:
    ElementType type;       // The type of elements that this we're selecting
    MapConstPtr _map;


/*---------------------------------------------------------------------------*
 | Selected element functions
 *---------------------------------------------------------------------------*/
public:
    // General set functions
    SizeType selectedCount() const;   // How many elements are selected?
    SizeType unselectedCount() const; // How many elements are not selected?

    // Vertex accessor functions
    SizeType selectedVertexCount() const;
    SizeType unselectedVertexCount() const;
    void select(VertexConstPtr i);
    void deselect(VertexConstPtr i);
    bool isSelected(VertexConstPtr i) const;

    // Face accessor functions
    SizeType selectedFaceCount() const;     // How many Faces are selected?
    SizeType unselectedFaceCount() const;   // How many not?
    void select(FaceConstPtr r);      // Make sure this is selected
    void deselect(FaceConstPtr r);    // Make sure this is not selected
    bool isSelected(FaceConstPtr r) const;    // Is this selected?

    // Edge accessor functions
    SizeType selectedEdgeCount() const;
    SizeType unselectedEdgeCount() const;
    void select(EdgeConstPtr b);
    void deselect(EdgeConstPtr b);
    bool isSelected(EdgeConstPtr b) const;

    // Effective selection calculation: convert selections between different
    // map element types. This is a bit tricky to explain, but the effective
    // selection allows us to select a bunch of elements of one type and then
    // find the topologically adjacent elements of a different type. So, for
    // example, selecting a single Edge and taking the effective selection
    // of Vertices would yield the two Vertices that terminate that
    // Edge.
    void getEffectiveSelection(const MeshSelection &ms, ElementType type);


/*---------------------------------------------------------------------------*
 | Set operations & tests
 *---------------------------------------------------------------------------*/
public:
    void complement();  // Invert the selection (not changing selection type)
    void selectAll();   // Select all of the current element type
};


// ostream writer for MeshSelection::ElementType
namespace terrainosaurus {
    inline std::ostream & operator<<(std::ostream & os,
                                     MeshSelection::ElementType t) {
        switch (t) {
            case MeshSelection::Vertices:
                return os << "Vert(ex/ices)";
            case MeshSelection::Faces:
                return os << "Face(s)";
            case MeshSelection::Edges:
                return os << "Edge(s)";
            default:
                return os << "INVALID MeshSelection::ElementType " << int(t);
        }
    }

    // This ought to be done inline in the class, but g++ doesn't accept it
    template <> inline void MeshSelection::setElementType<Map::Vertex>() { setElementType(Vertices); }
    template <> inline void MeshSelection::setElementType<Map::Face>() { setElementType(Faces); }
    template <> inline void MeshSelection::setElementType<Map::Edge>() { setElementType(Edges); }
};

#endif
