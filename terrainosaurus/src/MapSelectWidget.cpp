#include "MapSelectWidget.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::ui;

// Import OpenGL
#if __MS_WINDOZE__
    // Windows OpenGL seems to need this
#   include <windows.h>

    // I'd also rather VS didn't complain about casting to boolean
#   pragma warning (disable : 4800)
#endif
#include <GL/gl.h>
#include <GL/glu.h>

MapSelectWidget::MapSelectWidget(const string &nm) : BasicWidget(nm),
    // Data fields
    map(this), selection(this), selectionMode(HoverSelect),

    // Rendering properties
    intersectionColor(this), boundaryColor(this), refinedBoundaryColor(this),
    gridColor(this), lassoColor(this), selectHilight(this), hoverSelectHilight(this),
    lassoSelectHilight(this),
    
    boundaryWidth(this), refinedBoundaryWidth(this), intersectionRadius(this),
    boundarySelectionRadius(this), intersectionSelectionRadius(this),

    // Rendering flags
    drawRegions(this), drawIntersections(this), drawBoundaries(this),
    drawRefinements(this), drawGrid(this), drawLasso(this),
    drawSelected(this), drawHoverSelected(this), drawLassoSelected(this) { }


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
void MapSelectWidget::renderView() {
    // Set up rendering properties
    renderer.enableLineSmoothing(true);
    renderer.enablePointSmoothing(true);

    // First, render the grid, if we're supposed to
    if (drawGrid) {
        renderer.beginRenderPass(Opacity);
            renderGrid();
        renderer.endRenderPass();
    }

    renderer.beginRenderPass(Transparency);
        if (map)        renderMap();    // Render the map on top
        if (drawLasso)  renderLasso();  // Draw the selection lasso 
    renderer.endRenderPass();
}

// Render a visible representation of the grid we're using
void MapSelectWidget::renderGrid() {
    // Render a regular grid over the window
    Point ps, pe;
    renderer.setColor(gridColor());
    renderer.beginRenderImmediate(Lines);

    // Draw horizontal grid lines
    
    // Draw vertical grid lines

    renderer.endRenderImmediate();
}


void MapSelectWidget::renderLasso() {
    if (selectionMode == LassoSelect) {
        // Find the four corner points of the lasso
        Point ll(std::min(lassoStart[0], lassoEnd[0]),
                    std::min(lassoStart[1], lassoEnd[1]));
        Point ur(std::max(lassoStart[0], lassoEnd[0]),
                    std::max(lassoStart[1], lassoEnd[1]));
        Point ul(ll[0], ur[1]), lr(ur[0], ll[1]);

        OpenGLRenderer::api::select_projection_matrix();
        OpenGLRenderer::api::push_matrix();
        OpenGLRenderer::api::reset_matrix();
        OpenGLRenderer::api::apply_orthographic_projection(size, Vector(-1.0, 1.0));
        OpenGLRenderer::api::apply_scaling(math::Vector<double, 3>(1.0, -1.0, 1.0));
        OpenGLRenderer::api::apply_translation(math::Vector<double, 3>(size[0] / -2.0, size[1] / -2.0, 0.0));
        OpenGLRenderer::api::select_transformation_matrix();
        OpenGLRenderer::api::push_matrix();
        OpenGLRenderer::api::reset_matrix();
        renderer.setColor(lassoColor());
        renderer.beginRenderImmediate(LineLoop);
            renderer.renderVertex(ll);
            renderer.renderVertex(lr);
            renderer.renderVertex(ur);
            renderer.renderVertex(ul);
        renderer.endRenderImmediate();
        OpenGLRenderer::api::select_projection_matrix();
        OpenGLRenderer::api::pop_matrix();
        OpenGLRenderer::api::select_transformation_matrix();
        OpenGLRenderer::api::pop_matrix();
    }
}


// Render the map (visually), not for selection
void MapSelectWidget::renderMap() {
    cerr << " before R "; renderer.checkForError();
    if (map) {
        cerr << " R "; renderer.checkForError();
        if (drawRegions)        renderRegions<false>();
        cerr << " B "; renderer.checkForError();
        if (drawBoundaries)     renderBoundaries<false>();
        cerr << " I "; renderer.checkForError();
        if (drawIntersections)  renderIntersections<false>();
        cerr << " RB "; renderer.checkForError();
        if (drawRefinements)    renderRefinements<false>();
    }
    cerr << " after R "; renderer.checkForError();
}


template <bool forSelection>
void MapSelectWidget::renderRegions() {
    // XXX This could probably be done more efficiently by exposing a Region
    // iterator that patched thru to the iterator in the mesh...
    const Map::RegionList &rs = map()->regions();   // Get a list of all regions
    Map::RegionList::const_iterator it;
    for (it = rs.begin(); it != rs.end(); it++) {
        Map::Region r = *it;

        cerr << " q "; renderer.checkForError();

        // Set either selection ID, or color, depending on mode
        if (forSelection)       renderer.setSelectionID(r.id());
        else                    renderer.setColor(pickColor(r));

        // Draw the region
        renderer.beginRenderImmediate(inca::rendering::Polygon);
            size_t count = r.intersectionCount();
            for (index_t i = 0; i < index_t(count); ++i)
                // This could DEFINITELY be done faster...but this'll get ripped
                // out anyway once we support concave polys
                renderer.renderVertex(r.intersection(i).location());
        renderer.endRenderImmediate();
        cerr << " t2 "; renderer.checkForError();
    }
}

template <bool forSelection>
void MapSelectWidget::renderBoundaries() {
    const Map::BoundaryList &bs = map()->boundaries();
    Map::BoundaryList::const_iterator it;

    // Set up the appropriate line width
    if (forSelection)   renderer.setLineWidth(boundarySelectionRadius());
    else                renderer.setLineWidth(boundaryWidth());

    // Render each boundary
    for (it = bs.begin(); it != bs.end(); it++) {
        Map::Boundary b = *it;

        // Set either selection ID, or color, depending on mode
        if (forSelection)       renderer.setSelectionID(b.id());
        else                    renderer.setColor(pickColor(b));

        // Render the line
        renderer.beginRenderImmediate(Lines);
            renderer.renderVertex(b.startIntersection().location());
            renderer.renderVertex(b.endIntersection().location());
        renderer.endRenderImmediate();
    }
}

template <bool forSelection>
void MapSelectWidget::renderRefinements() {
    const Map::BoundaryList &bs = map()->boundaries();
    Map::BoundaryList::const_iterator it;

    // Set up the appropriate line width
    renderer.setLineWidth(refinedBoundaryWidth());

    renderer.setColor(refinedBoundaryColor());

    // Render each boundary that actually has a refinement
    for (it = bs.begin(); it != bs.end(); it++) {
        if (it->isRefined()) {
            Map::RefinedBoundary rb = it->refinement();
            const Map::PointList & points = rb.points();
            renderer.beginRenderImmediate(LineStrip);
                Map::PointList::const_iterator pt;
                for (pt = points.begin(); pt != points.end(); ++pt)
                    renderer.renderVertex(*pt);
            renderer.endRenderImmediate();
        }
    }
}

template <bool forSelection>
void MapSelectWidget::renderIntersections() {
    const Map::IntersectionList &is = map()->intersections();
    Map::IntersectionList::const_iterator it;

    // Set up the appropriate point size
    if (forSelection)   renderer.setPointDiameter(intersectionSelectionRadius() * 2);
    else                renderer.setPointDiameter(intersectionRadius() * 2);

    // Render each intersection
    for (it = is.begin(); it != is.end(); it++) {
        Map::Intersection i = *it;
        
        // Set either selection ID, or color, depending on mode
        if (forSelection)       renderer.setSelectionID(i.id());
        else                    renderer.setColor(pickColor(i));

        // Render the intersection
        renderer.beginRenderImmediate(Points);
            renderer.renderVertex(i.location());
        renderer.endRenderImmediate();
    }
}

MapSelectWidget::Color MapSelectWidget::pickColor(const Region &r) const {
    Color baseColor = r.terrainType().color();

    if (!drawSelected)      // If we're not rendering selections, we already
        return baseColor;   // know the answer, so quit right now

    // Is it part of the persistent selection?
    if (selection->isSelected(r))
        return baseColor % selectHilight();

    // Is it transiently selected?
    else if (transientSelection.isSelected(r)) {
        if (drawHoverSelected && selectionMode == HoverSelect)
            return baseColor % hoverSelectHilight();
        else if (drawLassoSelected && selectionMode == LassoSelect)
            return baseColor % lassoSelectHilight();
    }

    // ...then it must just be "normal"
    return baseColor;
}

MapSelectWidget::Color MapSelectWidget::pickColor(const Boundary &b) const {
    Color baseColor = boundaryColor();

    if (!drawSelected)      // If we're not rendering selections, we already
        return baseColor;   // know the answer, so quit right now

    // Is it part of the persistent selection?
    if (selection->isSelected(b))
        return baseColor % selectHilight();

    // Is it transiently selected?
    else if (transientSelection.isSelected(b)) {
        if (drawHoverSelected && selectionMode == HoverSelect)
            return baseColor % hoverSelectHilight();
        else if (drawLassoSelected && selectionMode == LassoSelect)
            return baseColor % lassoSelectHilight();
    }

    // ...then it must just be "normal"
    return baseColor;
}

MapSelectWidget::Color MapSelectWidget::pickColor(const Intersection &i) const {
    Color baseColor = intersectionColor();

    if (!drawSelected)      // If we're not rendering selections, we already
        return baseColor;   // know the answer, so quit right now

    // Is it part of the persistent selection?
    if (selection->isSelected(i))
        return baseColor % selectHilight();

    // Is it transiently selected?
    else if (transientSelection.isSelected(i)) {
        if (drawHoverSelected && selectionMode == HoverSelect)
            return baseColor % hoverSelectHilight();
        else if (drawLassoSelected && selectionMode == LassoSelect)
            return baseColor % lassoSelectHilight();
    }

    // ...then it must just be "normal"
    return baseColor;
}

void MapSelectWidget::buttonPressed(MouseButton b, Pixel p) {
    if (selectionMode == Disabled)  // No point in going on if we're not
        return;                     // permitted to make selections!

    if (b == LeftButton) {  // We're only interested in the LMB
        // To start with, we'll assume we're single-clicking...
        selectionMode = ClickSelect;

        // ...but prepare for lasso select, just in case
        lassoStart = p;

        // Figure out how this next selection will be combined
        // with the persistent selection
        if (theseModifiersActive(ShiftModifier))
            setOperation = Add;
        else if (theseModifiersActive(ControlModifier))
            setOperation = Subtract;
        else if (theseModifiersActive(ShiftModifier | ControlModifier))
            setOperation = Intersect;
        else
            setOperation = Replace;

        updateTransientSelection();     //...and find out what we clicked on
    }
    requestRedisplay();             // And draw us again
}

void MapSelectWidget::buttonReleased(MouseButton b, Pixel p) {
    if (selectionMode == Disabled)  // No point in going on if we're not
        return;                     // permitted to make selections!

    if (b == LeftButton) {  // We're only interested in the LMB
        lassoEnd = p;                   // Update the end-point of our lasso
        if (lassoEnd != lassoStart)         // If this is too far away...
            selectionMode = LassoSelect;    // ...then ensure we in lasso mode
        updateTransientSelection();     // Bring selection up-to-date

        // Now, figure out how to composite it with the persistent selection
        MapSelection & persistentSelection = *selection;
        switch (setOperation) {
        case Replace:       // Replace the selection with a new one
            persistentSelection = transientSelection;
            break;
        case Add:           // Add elements to the current selection
            persistentSelection += transientSelection;
            break;
        case Subtract:      // Remove elements from the current selection
            persistentSelection -= transientSelection;
            break;
        case Intersect:     // Only keep elements common to both
            persistentSelection ^= transientSelection;
            break;
        }

        selectionMode = HoverSelect;    // Revert back to not-selecting
    } else {
        selection->setElementType(MapSelection::ElementType((selection->elementType() + 1) % 3));
        transientSelection.setElementType(selection->elementType());
        cerr << "Selecting element type " << selection->elementType() << endl;
    }
    requestRedisplay();             // And draw us again
}

void MapSelectWidget::mouseDragged(Pixel p) {
    if (selectionMode == Disabled)  // No point in going on if we're not
        return;                     // permitted to make selections!

    if (! drawLassoSelected)        // No point in actually trying to select
        return;                     // anything if it won't be rendered

    lassoEnd = p;                   // Update the end-point of our lasso
    if (selectionMode == ClickSelect)
        selectionMode = LassoSelect;    // Alright, switch to lasso-mode

    updateTransientSelection();     // Look for lasso-selected items
    requestRedisplay();             // And draw us again
}

void MapSelectWidget::mouseTracked(Pixel p) {
    cerr << endl << endl;
    cerr << " m "; renderer.checkForError();
    if (selectionMode == Disabled)  // No point in going on if we're not
        return;                     // permitted to make selections!

    if (! drawHoverSelected)        // No point in actually trying to select
        return;                     // anything if it won't be rendered

    lassoStart = p;                 // Update the mouse's position

    updateTransientSelection();     // Look for hover-selected items
    requestRedisplay();             // And draw us again
}

void MapSelectWidget::updateTransientSelection() {
    cerr << endl << endl;
    if (selectionMode == Disabled)  // No point in going on if we're not
        return;                     // permitted to make selections!

    ///////////////////////////////////////////////////////////////////
    // Set up the picking parameters, based on the selection mode
    ///////////////////////////////////////////////////////////////////
    OpenGLRenderer::Point2D  pickCenter;
    OpenGLRenderer::Vector2D pickSize;
    if (selectionMode == LassoSelect) {  // Picking region is the lasso
        // FIXME: this needs good vector ops
        pickSize[0] = lassoEnd[0] - lassoStart[0];       // Could be negative!!
        pickSize[1] = lassoEnd[1] - lassoStart[1];       // Could be negative!!
        pickCenter[0] = lassoEnd[0] - pickSize[0] / 2.0; // Half way between
        pickCenter[1] = lassoEnd[1] - pickSize[1] / 2.0; // Half way between
        pickSize[0] = abs(pickSize[0]);               // Now it's guaranteed positive
        pickSize[1] = abs(pickSize[1]);               // Now it's guaranteed positive

    } else {            // Picking region is only under the mouse pointer
        pickSize = Vector(1.0, 1.0);            // Select in only one pixel.
        pickCenter = lassoStart;                // THIS pixel.
    }

    // Flip y coordinate
    pickCenter[1] = size[1] - pickCenter[1];

//    cerr << "Pick center is " << pickCenter << " and size " << pickSize << endl;

    ///////////////////////////////////////////////////////////////////
    // Have the renderer figure out what's within the selection region
    ///////////////////////////////////////////////////////////////////

    // Slip a picking projection underneath the current projection matrix
    OpenGLRenderer::Matrix projection;
    renderer.saveProjectionMatrix(projection);
    renderer.resetProjectionMatrix();
    renderer.applyPickingProjection(pickCenter, pickSize);
    renderer.applyMatrixProjection(projection);

    // Let the renderer see what can be seen
    renderer.beginRenderPass(Selection);
        switch (transientSelection.elementType()) {
            case MapSelection::Regions:
                cerr << " c1"; renderer.checkForError();
                renderRegions<true>();
                break;
            case MapSelection::Boundaries:
                cerr << " c2"; renderer.checkForError();
                renderBoundaries<true>();
                break;
            case MapSelection::Intersections:
                cerr << " c3"; renderer.checkForError();
                renderIntersections<true>();
                break;
        }
    renderer.endRenderPass();
    transientSelection.clear();                 // Make the selection empty
    renderer.getSelectedIDs(transientSelection);// Go get 'em

    // Restore the original projection matrix
    renderer.resetProjectionMatrix();
    renderer.applyMatrixProjection(projection);

    if (transientSelection.size() > 0) {
        switch (transientSelection.elementType()) {
            case MapSelection::Regions:
                cerr << "Selected Regions: ";
                for (int i = 0; i < index_t(map->regionCount()); ++i)
                    if (transientSelection.isSelected(map->region(i)))
                        cerr << i << ' ';
                break;
            case MapSelection::Boundaries:
                cerr << "Selected Boundaries: ";
                for (int i = 0; i < index_t(map->boundaryCount()); ++i)
                    if (transientSelection.isSelected(map->boundary(i)))
                        cerr << i << ' ';
                break;
            case MapSelection::Intersections:
                cerr << "Selected Intersections: ";
                for (int i = 0; i < index_t(map->intersectionCount()); ++i)
                    if (transientSelection.isSelected(map->intersection(i)))
                        cerr << i << ' ';
                break;
        }
        cerr << endl;
    }
    renderer.checkForError();
}
