#include "MeshSelection.hpp"
using namespace terrainosaurus;

void MeshSelection::setElementType(ElementType t) {
    if (t != type) {    // If we actually need to make a change
        type = t;       // Change the element type
        clear();        // And start with an empty selection
    }
}

SizeType MeshSelection::selectedCount() const { return size(); }
SizeType MeshSelection::unselectedCount() const {
    switch (elementType()) {
        case Vertices:  return map()->vertexCount() - size();
        case Faces:     return map()->faceCount() - size();
        case Edges:     return map()->edgeCount() - size();
        default:
            INCA_ERROR("Oops! MeshSelection::unselectedCount -- unrecognized: "
                       "ElementType " << int(elementType()))
            return 0;
    }
}


// Vertex accessor functions
SizeType MeshSelection::selectedVertexCount() const {
    if (elementType() == Vertices)  return size();
    else                            return 0;
}
SizeType MeshSelection::unselectedVertexCount() const {
    if (elementType() == Vertices)  return map()->vertexCount() - size();
    else                            return map()->vertexCount();
}
void MeshSelection::select(VertexConstPtr v) {
    // Change the element type if needed, then add this to the selection
    setElementType(Vertices);
    SelectionSet::select(v->id());
}
void MeshSelection::deselect(VertexConstPtr v) {
    // It can only be selected if it's of the current element type
    if (elementType() == Vertices)
        SelectionSet::deselect(v->id());
}
bool MeshSelection::isSelected(VertexConstPtr v) const {
    return (elementType() == Vertices && SelectionSet::isSelected(v->id()));
}


// Face accessor functions
SizeType MeshSelection::selectedFaceCount() const {
    if (elementType() == Faces)       return size();
    else                                return 0;
}
SizeType MeshSelection::unselectedFaceCount() const {
    if (elementType() == Faces)         return map()->faceCount() - size();
    else                                return map()->faceCount();
}
void MeshSelection::select(FaceConstPtr f) {
    setElementType(Faces);              // Change the element type if needed
    SelectionSet::select(f->id());      // Add this to the selection
}
void MeshSelection::deselect(FaceConstPtr f) {
    // It can only be selected if it's of the current element type
    if (elementType() == Faces)
        SelectionSet::deselect(f->id());
}
bool MeshSelection::isSelected(FaceConstPtr f) const {
    return (elementType() == Faces && SelectionSet::isSelected(f->id()));
}


// Edge accessor functions
SizeType MeshSelection::selectedEdgeCount() const {
    if (elementType() == Edges)     return size();
    else                            return 0;
}
SizeType MeshSelection::unselectedEdgeCount() const {
    if (elementType() == Edges)     return map()->edgeCount() - size();
    else                            return map()->edgeCount();
}
void MeshSelection::select(EdgeConstPtr e) {
    // Change the element type if needed, then add this to the selection
    setElementType(Edges);
    SelectionSet::select(e->id());
}
void MeshSelection::deselect(EdgeConstPtr e) {
    // It can only be selected if it's of the current element type
    if (elementType() == Edges)
        SelectionSet::deselect(e->id());
}
bool MeshSelection::isSelected(EdgeConstPtr e) const {
    return (elementType() == Edges && SelectionSet::isSelected(e->id()));
}


void MeshSelection::getEffectiveSelection(const MeshSelection &ms,
                                          ElementType t) {
    IDSet effective;    // Do this in a temp set, just in case ms == *this
    setMap(ms.map());   // Make sure we point to the same Map object!

    // First, consider the possibility that the two selection types are the
    // same...in this case, just copy the selection
    if (t == ms.elementType()) {
        effective = ms.items;

    // Now, let's see if we're going from Vertices to Edges
    } else if (t == Vertices && ms.elementType() == Edges) {
        // Go through each selected item
        MeshSelection::const_iterator vi;
        for (vi = ms.begin(); vi != ms.end(); ++vi) {
            // Make sure each adjacent Edge is selected
            Map::Vertex::ccw_edge_iterator ei, done;
            for (ei = map()->vertex(*vi)->edgesCCW(); ei != done; ++ei)
                effective.insert((*ei)->id());
        }

    // Now, let's see if we're going from Vertices to Faces
    } else if (t == Vertices && ms.elementType() == Faces) {
        // Go through each selected item
        MeshSelection::const_iterator vi;
        for (vi = ms.begin(); vi != ms.end(); ++vi) {
            // Make sure each adjacent Face is selected
            Map::Vertex::ccw_face_iterator fi, done;
            for (fi = map()->vertex(*vi)->facesCCW(); fi != done; ++fi)
                effective.insert((*fi)->id());
        }
#if 0
    // Now, let's see if we're going from Edges to Vertices
    } else if (t == Edges && ms.elementType() == Vertices) {
        // Go through each selected item
        MeshSelection::const_iterator ei;
        for (ei = ms.begin(); ei != ms.end(); ++ei) {
            // Make sure each adjacent Vertex is selected
            Map::Edge::vertex_iterator vi, done;
            for (vi = map()->edge(*ei)->vertices(); vi != done; ++vi)
                effective.insert((*vi)->id());
        }

    // Now, let's see if we're going from Edges to Faces
    } else if (t == Edges && ms.elementType() == Faces) {
        // Go through each selected item
        MeshSelection::const_iterator ei;
        for (ei = ms.begin(); ei != ms.end(); ++ei) {
            // Make sure each adjacent Face is selected
            Map::Edge::face_iterator fi, done;
            for (fi = map()->edge(*ei)->faces(); fi != done; ++fi)
                effective.insert((*fi)->id());
        }
#endif
    // Now, let's see if we're going from Faces to Vertices
    } else if (t == Faces && ms.elementType() == Vertices) {
        // Go through each selected item
        MeshSelection::const_iterator fi;
        for (fi = ms.begin(); fi != ms.end(); ++fi) {
            // Make sure each adjacent Vertex is selected
            Map::Face::ccw_vertex_iterator vi, done;
            for (vi = map()->face(*fi)->verticesCCW(); vi != done; ++vi)
                effective.insert((*vi)->id());
        }

    // Now, let's see if we're going from Faces to Edges
    } else if (t == Faces && ms.elementType() == Edges) {
        // Go through each selected item
        MeshSelection::const_iterator fi;
        for (fi = ms.begin(); fi != ms.end(); ++fi) {
            // Make sure each adjacent Edge is selected
            Map::Face::ccw_edge_iterator ei, done;
            for (ei = map()->face(*fi)->edgesCCW(); ei != done; ++ei)
                effective.insert((*ei)->id());
        }
    }


    // Finally, commit our new selection and change to the appropriate type
    items.swap(effective);
    type = t;

    INCA_DEBUG("Effectively selected: " << items.size() << ' ' << t)
}


/*---------------------------------------------------------------------------*
 | Set operations & tests
 *---------------------------------------------------------------------------*/
void MeshSelection::complement() {
    MeshSelection all;                  // Find "all" of the possible selected
    all.setElementType(elementType());  // things, then take the difference
    all.selectAll();                    // with the current set
    *this = all - *this;
}

void MeshSelection::selectAll() {   // XXX The copying of lists from Map here
    switch (elementType()) {        // is not especially efficient
        case Vertices: {
            Map::VertexPtrList vertices = map()->vertices();
            Map::VertexPtrList::iterator i;
            for (i = vertices.begin(); i != vertices.end(); ++i)
                select(*i);
            break;
        } case Faces: {
            Map::FacePtrList faces = map()->faces();
            Map::FacePtrList::iterator i;
            for (i = faces.begin(); i != faces.end(); ++i)
                select(*i);
            break;
        } case Edges: {
            Map::EdgePtrList edges = map()->edges();
            Map::EdgePtrList::iterator i;
            for (i = edges.begin(); i != edges.end(); ++i)
                select(*i);
            break;
        }
    }
}
