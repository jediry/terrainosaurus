/*
 * File: MapRendering.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "MapRendering.hpp"

namespace terrainosaurus {
    // Template specialization forward declarations
    template <> Color MapRendering::baseColor<Map::Face>(Map::Face const * f) const;
    template <> Color MapRendering::baseColor<Map::Edge>(Map::Edge const * e) const;
    template <> Color MapRendering::baseColor<Map::Vertex>(Map::Vertex const * v) const;
}


// Import OpenGL
#define GL_HPP_IMPORT_GLU 1
#include <inca/integration/opengl/GL.hpp>
using namespace GL;


// Features that we support
#define VERTICES                0
#define EDGES                   1
#define FACES                   2
#define REFINEMENTS             3
#define ENVELOPES               4
#define TANGENTS                5
#define SELECTIONS              6
#define CLICK_SELECTED_ELEMENTS 7
#define HOVER_SELECTED_ELEMENTS 8
#define LASSO_SELECTED_ELEMENTS 9
#define FEATURE_COUNT           10

// Scalar values that can be set
#define VERTEX_RADIUS       0
#define EDGE_WIDTH          1
#define REFINEMENT_WIDTH    2
#define ENVELOPE_WIDTH      3
#define TANGENT_WIDTH       4
#define VERTEX_SEL_RADIUS   5
#define EDGE_SEL_RADIUS     6
#define SCALAR_COUNT        7

// Colors that can be set
#define VERTEX_COLOR            0
#define EDGE_COLOR              1
#define REFINEMENT_COLOR        2
#define ENVELOPE_COLOR          3
#define TANGENT_COLOR           4
#define LASSO_COLOR             5
#define SELECT_HILIGHT_COLOR    6
#define HOVER_HILIGHT_COLOR     7
#define LASSO_HILIGHT_COLOR     8
#define COLOR_COUNT             9


using namespace terrainosaurus;
//using namespace inca::rendering;

#include <list>
#define CALLBACK

void CALLBACK printTessError(GLenum errorCode) {
    cerr << gluErrorString(errorCode) << endl;
}

void CALLBACK combineTessPoly(GLdouble coords[3], GLdouble *vertexData[4],
                              GLfloat weight[4], GLdouble **dataOut) {
    GLdouble *vtx = new GLdouble[3];
    vtx[0] = coords[0];
    vtx[1] = coords[1];
    vtx[2] = coords[2];
    *dataOut = vtx;
}

MapRendering::MapRendering() {
    setDefaults();

    GLUtesselator * tess = gluNewTess();
    gluTessCallback(tess, GLU_TESS_VERTEX,(GLvoid (CALLBACK*) ( )) &glVertex2dv);
    gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid (CALLBACK*) ()) &combineTessPoly);
    gluTessCallback(tess, GLU_TESS_BEGIN,(GLvoid (CALLBACK*) ( )) &glBegin);
    gluTessCallback(tess, GLU_TESS_END,(GLvoid (CALLBACK*) ( )) &glEnd);
    gluTessCallback(tess, GLU_TESS_ERROR, (GLvoid (CALLBACK*) ()) &printTessError);
    gluTessNormal(tess, 0.0, 0.0, 1.0);
    tesselator = tess;
}

MapRendering::MapRendering(MapConstPtr m,
                           MeshSelectionConstPtr ps,
                           MeshSelectionConstPtr ts) {
    setDefaults();
    setMap(m);
    setPersistentSelection(ps);
    setTransientSelection(ts);
}

MapRendering::~MapRendering() {
    gluDeleteTess(static_cast<GLUtesselator *>(tesselator));
}


void MapRendering::setDefaults() {
    _features.resize(FEATURE_COUNT);
    _features.at(VERTICES)      = true;
    _features.at(EDGES)         = true;
    _features.at(FACES)         = true;
    _features.at(REFINEMENTS)   = true;
    _features.at(ENVELOPES)     = true;
    _features.at(TANGENTS)      = true;
    _features.at(SELECTIONS)    = true;
    _features.at(CLICK_SELECTED_ELEMENTS)   = true;
    _features.at(HOVER_SELECTED_ELEMENTS)   = true;
    _features.at(LASSO_SELECTED_ELEMENTS)   = true;

    _scalars.resize(SCALAR_COUNT);
    _scalars.at(VERTEX_RADIUS)      = 4.0f;
    _scalars.at(EDGE_WIDTH)         = 4.0f;
    _scalars.at(REFINEMENT_WIDTH)   = 2.0f;
    _scalars.at(ENVELOPE_WIDTH)     = 1.0f;
    _scalars.at(TANGENT_WIDTH)      = 2.0f;
    _scalars.at(VERTEX_SEL_RADIUS)  = 5.0f;
    _scalars.at(EDGE_SEL_RADIUS)    = 4.0f;

    _colors.resize(COLOR_COUNT);
    _colors.at(VERTEX_COLOR)            = Color(0.7f, 0.7f, 1.0f, 1.0f);
    _colors.at(EDGE_COLOR)              = Color(0.0f, 0.5f, 0.5f, 1.0f);
    _colors.at(REFINEMENT_COLOR)        = Color(0.3f, 0.3f, 1.0f, 1.0f);
    _colors.at(ENVELOPE_COLOR)          = Color(0.8f, 0.8f, 0.4f, 0.7f);
    _colors.at(TANGENT_COLOR)           = Color(0.8f, 0.1f, 0.1f, 1.0f);
    _colors.at(LASSO_COLOR)             = Color(0.1f, 1.0f, 0.1f, 0.5f);
    _colors.at(SELECT_HILIGHT_COLOR)    = Color(1.0f, 1.0f, 0.5f, 0.7f);
    _colors.at(HOVER_HILIGHT_COLOR)     = Color(1.0f, 1.0f, 0.5f, 0.8f);
    _colors.at(LASSO_HILIGHT_COLOR)     = Color(1.0f, 1.0f, 0.5f, 0.5f);
}

bool MapRendering::toggle(const std::string & feature) {
    IndexType index = -1;
    if (feature == "Vertices")
        index = VERTICES;
    else if (feature == "Edges")
        index = EDGES;
    else if (feature == "Faces")
        index = FACES;
    else if (feature == "Refinements")
        index = REFINEMENTS;
    else if (feature == "Envelopes")
        index = ENVELOPES;
    else if (feature == "Tangents")
        index = TANGENTS;
    else if (feature == "Selections")
        index = SELECTIONS;
    else if (feature == "ClickSelectedElements")
        index = CLICK_SELECTED_ELEMENTS;
    else if (feature == "LassoSelectedElements")
        index = LASSO_SELECTED_ELEMENTS;
    else if (feature == "HoverSelectedElements")
        index = HOVER_SELECTED_ELEMENTS;

    if (index != -1) {
        _features.at(index) = ! _features.at(index);
        cerr << feature << ": " << (_features.at(index) ? "on" : "off") << endl;
    }
}

void MapRendering::setScalar(const std::string & name, scalar_arg_t s) {

}
void MapRendering::setColor(const std::string & name, const Color & c) {

}


MapConstPtr MapRendering::map() const {
    return _map;
}
MeshSelectionConstPtr MapRendering::persistentSelection() const {
    return _persistentSelection;
}
MeshSelectionConstPtr MapRendering::transientSelection() const {
    return _transientSelection;
}
void MapRendering::setMap(MapConstPtr m) {
    _map = m;
}
void MapRendering::setPersistentSelection(MeshSelectionConstPtr s) {
    _persistentSelection = s;
}
void MapRendering::setTransientSelection(MeshSelectionConstPtr s) {
    _transientSelection = s;
}


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/

#if 0
void MapRendering::renderLasso(Pixel p1, Pixel p2) {
    // Find the four corner points of the lasso FIXME
    Point2D ll(std::min(p1[0], p2[0]), std::min(p1[1], p2[1]));
    Point2D ur(std::max(p1[0], p2[0]), std::max(p1[1], p2[1]));
    Point2D ul(ll[0], ur[1]),
            lr(ur[0], ll[1]);

    Vector2D size(width(), height()),
             clip(-1.0, 1.0);
    Matrix screenspace = screenspaceULMatrix(size);
    projectionMatrix().push(screenspace);
    viewMatrix().push();
    viewMatrix().reset();

    rasterizer().setColor(lassoColor());
    beginRenderImmediate(LineLoop);
        vertexAt(ll);
        vertexAt(lr);
        vertexAt(ur);
        vertexAt(ul);
    endRenderImmediate();

    projectionMatrix.pop();
    viewMatrix.pop();
}
#endif

void MapRendering::operator()(Renderer & renderer) {
    Renderer::Rasterizer & rasterizer = renderer.rasterizer();
    rasterizer.setLineSmoothingEnabled(true);
    rasterizer.setPointSmoothingEnabled(true);
    rasterizer.setAlphaBlendingEnabled(true);
    GL::glEnable(GL_POINT_SMOOTH);
    GL::glEnable(GL_LINE_SMOOTH);
    GL::glEnable(GL_BLEND);
    GL::glDisable(GL_DEPTH_TEST);

    renderFaces<false>(renderer);
    renderEdges<false>(renderer);
    renderVertices<false>(renderer);
    renderRefinements<false>(renderer);
}

template <bool forSelection>
void MapRendering::renderVertices(Renderer & renderer) {
    if (! _features.at(VERTICES))
        return;                     // Not supposed to draw these...leave!

    Renderer::Rasterizer & rasterizer = renderer.rasterizer();

    // Set up the appropriate point size
    if (forSelection)   rasterizer.setPointDiameter(2 * _scalars.at(VERTEX_SEL_RADIUS));
    else                rasterizer.setPointDiameter(2 * _scalars.at(VERTEX_RADIUS));

    // Render each vertex
    const Map::VertexPtrList & vertices = map()->vertices();
    Map::VertexPtrList::const_iterator vs;
    for (vs = vertices.begin(); vs != vertices.end(); ++vs) {
        Map::VertexPtr v = *vs;

        // Set either selection ID, or color, depending on mode
        if (forSelection)   rasterizer.setSelectionID(v->id());
        else                rasterizer.setCurrentColor(pickColor(v));

        // Render the vertex
        rasterizer.beginPrimitive(inca::rendering::Points);
            rasterizer.vertexAt(v->position());
        rasterizer.endPrimitive();
    }
}

template <bool forSelection>
void MapRendering::renderFaces(Renderer & renderer) {
    if (! _features.at(FACES))
        return;                     // Not supposed to draw these...leave!

    Renderer::Rasterizer & rasterizer = renderer.rasterizer();

    const Map::FacePtrList & faces = map()->faces();
    Map::FacePtrList::const_iterator fs;
    for (fs = faces.begin(); fs != faces.end(); ++fs) {
        Map::FacePtr f = *fs;

        // Set either selection ID, or color, depending on mode
        if (forSelection)   rasterizer.setSelectionID(f->id());
        else                rasterizer.setCurrentColor(pickColor(f));

        // Draw the region
//         rasterizer.beginPrimitive(inca::rendering::Polygon);
//             Map::Face::ccw_vertex_iterator vs, done;
//             for (vs = f->verticesCCW(); vs != done; ++vs)
//                 // This could DEFINITELY be done faster...but this'll get ripped
//                 // out anyway once we support concave polys
//                 rasterizer.vertexAt(vs->position());
//         rasterizer.endPrimitive();
//         continue;

        GLUtesselator * tess = static_cast<GLUtesselator *>(tesselator);
        gluTessBeginPolygon(tess, NULL);
            gluTessBeginContour(tess);

            // Go through each edge of the face and draw each vertex
            std::list<GLdouble *> vtx;
            Map::Face::ccw_edge_iterator es, end_e;
            for (es = f->edgesCCW(); es != end_e; ++es) {
                Map::EdgePtr e = *es;
                // Depending on which way the edge is oriented,
                // we have handle it a little differently
                if (e->startVertex() == e->vertexCW(f)) {
                    if (e->isRefined()) {
                        const Map::PointList & points = e->refinement();
                        Map::PointList::const_iterator pt;
                        // Skip first (duplicated) point
                        for (pt = ++points.begin(); pt != points.end(); ++pt) {
                            GLdouble * vert = new GLdouble[2];
                            vtx.push_back(vert);
                            vert[0] = (*pt)[0];
                            vert[1] = (*pt)[1];
                            gluTessVertex(tess, vert, vert);
                        }
                    } else {
                        Map::Point p = e->endVertex()->position();
                        GLdouble * vert = new GLdouble[2];
                        vtx.push_back(vert);
                        vert[0] = p[0];
                        vert[1] = p[1];
                        gluTessVertex(tess, vert, vert);
                    }
                } else {
                    if (e->isRefined()) {
                        const Map::PointList & points = e->refinement();
                        Map::PointList::const_reverse_iterator pt;
                        // Skip first (duplicated) point
                        for (pt = ++points.rbegin(); pt != points.rend(); ++pt) {
                            GLdouble * vert = new GLdouble[2];
                            vtx.push_back(vert);
                            vert[0] = (*pt)[0];
                            vert[1] = (*pt)[1];
                            gluTessVertex(tess, vert, vert);
                        }
                    } else {
                        Map::Point p = e->startVertex()->position();
                        GLdouble * vert = new GLdouble[2];
                        vtx.push_back(vert);
                        vert[0] = p[0];
                        vert[1] = p[1];
                        gluTessVertex(tess, vert, vert);
                    }
                }
            }

            gluTessEndContour(tess);
        gluTessEndPolygon(tess);
        std::list<GLdouble *>::iterator pt;
//        cerr << "Deleting " << vtx.size() << " pointers:\n";
        for (pt = vtx.begin(); pt != vtx.end(); ++pt) {
//            cerr << "\t" << (*pt)[0] << ", " << (*pt)[1] << endl;
            delete [] *pt;
        }
    }
}

template <bool forSelection>
void MapRendering::renderEdges(Renderer & renderer) {
    if (! _features.at(EDGES))
        return;                     // Not supposed to draw these...leave!

    Renderer::Rasterizer & rasterizer = renderer.rasterizer();

    // Set up the appropriate line width
    if (forSelection)   rasterizer.setLineWidth(2 * _scalars.at(EDGE_SEL_RADIUS));
    else                rasterizer.setLineWidth(_scalars.at(EDGE_WIDTH));

    // Render each edge
    const Map::EdgePtrList & edges = map()->edges();
    Map::EdgePtrList::const_iterator es;
    for (es = edges.begin(); es != edges.end(); ++es) {
        Map::EdgePtr e = *es;

        // Set either selection ID, or color, depending on mode
        if (forSelection)   rasterizer.setSelectionID(e->id());
        else                rasterizer.setCurrentColor(pickColor(e));

        // Render the line
        rasterizer.beginPrimitive(inca::rendering::Lines);
            rasterizer.vertexAt(e->startVertex()->position());
            rasterizer.vertexAt(e->endVertex()->position());
        rasterizer.endPrimitive();
    }
}

// Render map refinements and decorations
template <bool forSelection>
void MapRendering::renderRefinements(Renderer & renderer) {
//    if (! _features.at(REFINEMENTS))
//        return;                     // Not supposed to draw these...leave!

    Renderer::Rasterizer & rasterizer = renderer.rasterizer();

    // Some tools for later
    const Map::VertexPtrList & vertices = map()->vertices();
    const Map::EdgePtrList & edges = map()->edges();
    Map::VertexPtrList::const_iterator vi;
    Map::EdgePtrList::const_iterator ei;

    // Render edges with refinements
    if (_features.at(REFINEMENTS)) {
        // Set up the appropriate line width & color
        rasterizer.setLineWidth(_scalars.at(REFINEMENT_WIDTH));
        rasterizer.setCurrentColor(_colors.at(REFINEMENT_COLOR));

        // Draw each existing Edge refinement
        for (ei = edges.begin(); ei != edges.end(); ++ei) {
            Map::EdgeConstPtr e = *ei;
            if (e->isRefined()) {
                rasterizer.beginPrimitive(inca::rendering::LineStrip);
                    const Map::PointList & refinement = e->refinement();
                    Map::PointList::const_iterator pt;
                    for (pt = refinement.begin(); pt != refinement.end(); ++pt)
                        rasterizer.vertexAt(*pt);
                rasterizer.endPrimitive();
            }
        }
    }

    // Render Edge envelopes
    if (_features.at(ENVELOPES)) {
        // Set up the appropriate line width & color
        rasterizer.setLineWidth(_scalars.at(ENVELOPE_WIDTH));
        rasterizer.setCurrentColor(_colors.at(ENVELOPE_COLOR));

        // Get a referenec to the view transformation matrix
        Renderer::MatrixStack & viewMatrix = renderer.viewMatrix();

        // Draw each Edge envelope
        Vector3D Z(0, 0, 1);    // Rotate around this
        for (ei = edges.begin(); ei != edges.end(); ++ei) {
            Map::EdgeConstPtr e = *ei;
            const Map::RangeList & envelope = e->envelope();
            if (envelope.size() > 1) {
                viewMatrix.push();
                    Point2D p2 = e->startPoint();
                    viewMatrix.translate(Point3D(p2[0], p2[1], 0.0));
                    viewMatrix.rotate(e->angle(), Z);
                    scalar_t dx = e->length() / (envelope.size() - 1);
                    Point3D p(scalar_t(0));
                    rasterizer.beginPrimitive(inca::rendering::LineLoop);
                        Map::RangeList::const_iterator ri;
                        for (ri = envelope.begin(); ri != envelope.end(); ++ri) {
                            p[1] = ri->first;
                            rasterizer.vertexAt(p);
                            p[0] += dx;
                        }

                        Map::RangeList::const_reverse_iterator rri;
                        for (rri = envelope.rbegin(); rri != envelope.rend(); ++rri) {
                            p[0] -= dx;
                            p[1] = rri->second;
                            rasterizer.vertexAt(p);
                        }
                    rasterizer.endPrimitive();
                viewMatrix.pop();
            }
        }
    }

    // Render Vertex tangent lines
    if (_features.at(TANGENTS)) {
        // Set up the appropriate line width & color
        rasterizer.setLineWidth(_scalars.at(TANGENT_WIDTH));
        rasterizer.setCurrentColor(_colors.at(TANGENT_COLOR));

        // Draw each valence-2 Vertex tangent line
        for (vi = vertices.begin(); vi != vertices.end(); ++vi) {
            Map::VertexConstPtr v = *vi;
            if (v->edgeCount() == 2) {
               rasterizer.beginPrimitive(inca::rendering::LineStrip);
//                const Map::RangeList & envelope = e->envelope();
//                Map::RangeList::const_iterator rg;
//                for (rg = refinement.begin(); rg != refinement.end(); ++rg)
//                    vertexAt(*pt);
                rasterizer.endPrimitive();
            }
        }
    }
}


template <class ElementType>
Color MapRendering::pickColor(ElementType const * e) const {
    // If we're not rendering selected things at all, we can quit now
    if (! _features.at(SELECTIONS))
        return baseColor(e);

    // If we are 'selected', we might want to draw that
    else if (persistentSelection()->isSelected(e) && _features.at(CLICK_SELECTED_ELEMENTS))
        return selectedColor(e);

    // Otherwise, this might be a "semi" selected element
    else if (transientSelection()->isSelected(e))
        if (_features.at(LASSO_SELECTED_ELEMENTS))      // ...then  within the lasso
            return lassoSelectedColor(e);
        else if (_features.at(HOVER_SELECTED_ELEMENTS)) // ...then under the mouse
            return hoverSelectedColor(e);

    // Failing all that, just return the base color
    return baseColor(e);
}

// Base color for a Face (map region)
template <>
Color MapRendering::baseColor<Map::Face>(Map::Face const * f) const {
    return f->terrainType()->color();
}

// Base color for an Edge (map edge)
template <>
Color MapRendering::baseColor<Map::Edge>(Map::Edge const * e) const {
    return _colors.at(EDGE_COLOR);
}

// Base color for a Vertex
template <>
Color MapRendering::baseColor<Map::Vertex>(Map::Vertex const * v) const {
    return _colors.at(VERTEX_COLOR);
}

// Selected color (calculated from base color and selection hilight)
template <class ElementType>
Color MapRendering::selectedColor(ElementType const * e) const {
    return baseColor(e) % _colors.at(SELECT_HILIGHT_COLOR);
}

// Lasso-selected color (calculated from base color and lasso hilight)
template <class ElementType>
Color MapRendering::lassoSelectedColor(ElementType const * e) const {
    return baseColor(e) % _colors.at(LASSO_HILIGHT_COLOR);
}

// Mouse-hover color (calculated from base color and hover hilight)
template <class ElementType>
Color MapRendering::hoverSelectedColor(ElementType const * e) const {
    return baseColor(e) % _colors.at(HOVER_HILIGHT_COLOR);
}

#if 0
/*---------------------------------------------------------------------------*
 | Selection/geometric functions
 *---------------------------------------------------------------------------*/
template <class ElementType>
void MapRendering::mapElementsAroundPoint(Point2D p, Vector2D size, MeshSelection &s) {
    // Sanity-check the picking criteria
    if (size[0] < 1)    size[0] = 1;    // If any dimension is zero, the pick
    if (size[1] < 1)    size[1] = 1;    // region is degenerate so fix it

    projectionMatrix().push();
    projectionMatrix().preMultiply(pickingMatrix(, ));

    beginRenderPass(Selection);
        renderMapElements<ElementType>(true, RenderAll);
    endRenderPass();

    projectionMatrix().pop();

    s.setElementType<ElementType>();    // Set the element type of the selection
    s.clear();                          // Stick the ids of the elements
    getSelectedIDs(s);                  // into the selection
    checkForError();
}

void MapRendering::mapElementsAroundPixel(Pixel p, MeshSelection &s) {
    scalar_t size;      // How wide a region around the pixel to look
    Point2D point(p[0], height() - p[1]);

    // Depending on the element type we're selecting, we have to do differently
    switch(s.elementType()) {
    case MeshSelection::Faces:
        // Just check to see if the pointer is directly over a Region
        size = 1.0;
        mapElementsAroundPoint<Map::Face>(point, Vector2D(size), s);
        break;
    case MeshSelection::Edges:
        // See if we're within 'edgeSelectionRadius' of an Edge
        size = edgeSelectionRadius() * 2.0;
        mapElementsAroundPoint<Map::Edge>(point, Vector2D(size), s);
        break;
    case MeshSelection::Vertices:
        // See if we're within 'vertexSelectionRadius' of a Vertex
        size = vertexSelectionRadius() * 2.0;
        mapElementsAroundPoint<Map::Vertex>(point, Vector2D(size), s);
        break;
    }
}

void MapRendering::mapElementsWithinLasso(Pixel start, Pixel end, MeshSelection &s) {
    // Translate the lasso bounds into a center/size combination
    Point2D pStart(start[0], height() - start[1]),
            pEnd(end[0], height() - end[1]);
    Vector2D pickSize(pEnd - pStart);     // Could be negative sizes right now
    Point2D pickCenter(pEnd - pickSize / 2.0);  // Half way in between

    // FIXME: this should be a single function call!
    pickSize[0] = abs(pickSize[0]);     // Now we're guaranteed that size
    pickSize[1] = abs(pickSize[1]);     // is positive

    // Depending on the element type we're selecting, we have to do differently
    switch(s.elementType()) {
    case MeshSelection::Faces:
        // Just check to see if the pointer is directly over a Region
        // FIXME: this should check all Vertices
        mapElementsAroundPoint<Map::Face>(pickCenter, pickSize, s);
        break;
    case MeshSelection::Edges:
        // See if we're within 'edgeSelectionRadius' of an Edge
        mapElementsAroundPoint<Map::Edge>(pickCenter, pickSize, s);
        break;
    case MeshSelection::Vertices:
        // See if we're within 'vertexSelectionRadius' of a Vertex
        mapElementsAroundPoint<Map::Vertex>(pickCenter, pickSize, s);
        break;
    }
}

// Create a spike at the specifed pixel, handling snapping to any nearby
// Map::Vertex or Map::Edge
Map::Spike MapRendering::spikeAtPixel(Pixel p) {
    Map::Spike spike;   // The result we're looking for
    MeshSelection temp;  // A temporary selection to hold the result
    Vector2D size;      // How far around the pixel center to look
    Point2D point(p[0], height() - p[1]);   // The pixel center

    // Calculate the world-space position of this pixel
    spike.worldPosition = pointOnGroundPlane(p);
    spike.snappedPosition = spike.worldPosition;

    // Look for an vertex we can snap to
    size = Vector2D(vertexSelectionRadius() * 2); // This is our 'snap'
    mapElementsAroundPoint<Map::Vertex>(point, size, temp);

    // If that failed, look for a edge we can snap to
    if (temp.size() == 0) {
        size = Vector2D(edgeSelectionRadius() * 2);
        mapElementsAroundPoint<Map::Edge>(point, size, temp);
    }

    // Now, if we actually hit anything, snap to it
    if (temp.size() > 0) {
        switch (temp.elementType()) {
        case MeshSelection::Vertices:
            {   // We're attaching to an existing Vertex
                spike.type = Map::LinkVertex;

                // Find the closest Vertex to our world position
                MeshSelection::iterator it;
                scalar_t distance = 1e20;   // SURELY this is enough...
                for (it = temp.begin(); it != temp.end(); ++it) {
                    Map::VertexPtr v = map()->vertex(*it);
                    scalar_t d = magnitude(v->position() - spike.worldPosition);
                    if (d < distance) { // This one is the new "closest"
                        distance = d;
                        spike.elementID = v->id();
                        spike.snappedPosition = v->position();
                    }
                }
            }
            break;

        case MeshSelection::Edges:
            {   // We're attaching to an existing Vertex
                spike.type = Map::SplitEdge;

                // Find the closest Edge to our world position
                MeshSelection::iterator it;
                scalar_t distance = 1e20;   // SURELY this is enough...
                for (it = temp.begin(); it != temp.end(); ++it) {
                    Map::EdgePtr e = map()->edge(*it);
                    Point p = e->nearestPointTo(spike.worldPosition);
                    scalar_t d = magnitude(p - spike.worldPosition);
                    if (d < distance) { // This one is the new "closest"
                        distance = d;
                        spike.elementID = e->id();
                        spike.snappedPosition = p;
                    }
                }
            }
            break;
        }
    }

    return spike;
}

// Find the 2D point on the ground plane
MapRendering::Point MapRendering::pointOnGroundPlane(Pixel p) {
    Point3D screen(p[0], height() - p[1], 1.0);
    Point3D p0 = screenToWorld(screen); // Unproject at two different Z values
    screen[2] = 2.0;
    Point3D p1 = screenToWorld(screen); // so we can get the vector connecting

    scalar_t t = p0[2] / (p0[2] - p1[2]);   // t value to intersect with plane

    return Point2D(p0[0] + (p1[0] - p0[0]) * t,     // The point at that T value
                   p0[1] + (p1[1] - p0[1]) * t);
}

#endif
