#include "MapEditWidget.hpp"
using namespace terrainosaurus;
using namespace inca::ui;

typedef TerrainosaurusRenderer TR;  // Shorthand for a long name


MapEditWidget::MapEditWidget(const string &nm, bool enableSelect)
        : BasicWidget(nm), renderer(this),

          // Rendering flags
          drawVertices(this), drawFaces(this), drawEdges(this),
          drawRefinedEdges(this), drawEnvelopeBorders(this),
          drawTangentLines(this), drawSelected(this),
          drawHoverSelected(this), drawLassoSelected(this),
          drawGrid(this), drawLasso(this) {

    // See if we allow selection at all
    selectionMode = enableSelect ? HoverSelect : Disabled;
}


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
void MapEditWidget::renderView() {
    if (renderer) {
        renderer->renderGrid();         // Draw the grid

        // Figure out what flags to pass to the renderer and render the map
        unsigned int flags = 0;
        if (drawVertices)       flags |= TR::RenderVertices;
        if (drawFaces)          flags |= TR::RenderFaces;
        if (drawEdges)          flags |= TR::RenderEdges;
        if (drawRefinedEdges)   flags |= TR::RenderRefinedEdges;
        if (drawEnvelopeBorders)flags |= TR::RenderEnvelopeBorders;
        if (drawTangentLines)   flags |= TR::RenderTangentLines;
        if (drawSelected)       flags |= TR::RenderSelected;
        if (drawHoverSelected && selectionMode == HoverSelect)
            flags |= TR::RenderHoverSelected;
        if (drawLassoSelected && selectionMode == LassoSelect)
            flags |= TR::RenderLassoSelected;
        renderer->renderMap(flags);         // Draw the map

        // Draw the lasso, if we're in lasso-select mode
        if (selectionMode == LassoSelect && drawLasso)
            renderer->renderLasso(lassoStart, lassoEnd);
    }
}


void MapEditWidget::buttonPressed(MouseButton b, Pixel p) {
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
            selectionOperation = Add;
        else if (theseModifiersActive(ControlModifier))
            selectionOperation = Subtract;
        else if (theseModifiersActive(ShiftModifier | ControlModifier))
            selectionOperation = Intersect;
        else
            selectionOperation = Replace;

        updateTransientSelection();     //...and find out what we clicked on
    }
    requestRedisplay();             // And draw us again
}

void MapEditWidget::buttonReleased(MouseButton b, Pixel p) {
    if (selectionMode == Disabled)  // No point in going on if we're not
        return;                     // permitted to make selections!

    if (b == LeftButton) {  // We're only interested in the LMB
        lassoEnd = p;                   // Update the end-point of our lasso
        if (lassoEnd != lassoStart)         // If this is too far away...
            selectionMode = LassoSelect;    // ...then ensure we in lasso mode
        updateTransientSelection();     // Bring selection up-to-date

        updatePersistentSelection();    // Combine with the persistent selection

        selectionMode = HoverSelect;    // Revert back to not-selecting

//    } else {
//        persistentSelection()->setElementType(MeshSelection::ElementType((persistentSelection()->elementType() + 1) % 3));
//        transientSelection()->setElementType(persistentSelection()->elementType());
//        cerr << "Selecting element type " << persistentSelection()->elementType() << endl;
    }
    requestRedisplay();             // And draw us again
}

void MapEditWidget::mouseDragged(Pixel p) {
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

void MapEditWidget::mouseTracked(Pixel p) {
    if (selectionMode == Disabled)  // No point in going on if we're not
        return;                     // permitted to make selections!

    if (! drawHoverSelected)        // No point in actually trying to select
        return;                     // anything if it won't be rendered

    lassoStart = p;                 // Update the mouse's position

    updateTransientSelection();     // Look for hover-selected items
    requestRedisplay();             // And draw us again
}

void MapEditWidget::updatePersistentSelection() {
    if (selectionMode == Disabled)  // No point in going on if we're not
        return;                     // permitted to make selections!

    // Figure out how to composite transient with persistent
    MeshSelection &persist = *persistentSelection(),
                  &trans   = *transientSelection();
    switch (selectionOperation) {
    case Replace:       // Replace the selection with a new one
        persist = trans;
        break;
    case Add:           // Add elements to the current selection
        persist += trans;
        break;
    case Subtract:      // Remove elements from the current selection
        persist -= trans;
        break;
    case Intersect:     // Only keep elements common to both
        persist ^= trans;
        break;
    }

    trans.clear();   // Clear the temporary selection

    // Print out what has happened
    cerr << persist.size() << persist.elementType() << " selected\n";
}

void MapEditWidget::updateTransientSelection() {
    if (selectionMode == Disabled)  // No point in going on if we're not
        return;                     // permitted to make selections!

    // Have the renderer figure out what's within the selection region
    if (renderer) {
        if (selectionMode == LassoSelect)
            renderer->mapElementsWithinLasso(lassoStart, lassoEnd,
                                             *transientSelection());
        else
            renderer->mapElementsAroundPixel(lassoStart, *transientSelection());
    }
}
