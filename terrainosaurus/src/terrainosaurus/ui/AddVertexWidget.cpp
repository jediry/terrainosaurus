/*
 * File: AddVertexWidget.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "AddVertexWidget.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::ui;



// Default constructor with optional component name
AddVertexWidget::AddVertexWidget(const string &nm)
    : MapEditWidget(nm, false),
      potentialVertexColor(this), potentialVertexDiameter(this) { }

// Render the view from this Widget
void AddVertexWidget::renderView() {
    MapEditWidget::renderView();    // Render whatever's under me

    // Render a point wherever we would be adding the vertex
    renderer->setColor(potentialVertexColor());
    renderer->setPointDiameter(potentialVertexDiameter());
    renderer->beginRenderImmediate(Points);
        renderer->renderVertex(trackedSpike.snappedPosition);
    renderer->endRenderImmediate();
}

// If the user clicks the LMB, add a new Intersection at the place where he
// clicked. If he clicks the RMB, undo one Intersection
void AddVertexWidget::buttonClicked(MouseButton b, Pixel p) {

    // Left mouse button adds a new intersection
    if (b == LeftButton) {
        // Update the current spike
        updateTrackedSpike(p);

        // Make a vertex at this location
        map()->createVertex(trackedSpike);

        requestRedisplay();     // Update the display with our new point
        return;
    }

    // Doesn't concern us, if we got this far
    MapEditWidget::buttonClicked(b, p);
}

// *WE* don't really care about the difference between a 'mouseDragged' and
// a 'mouseTracked', so we treat them pretty much identically. However, those
// who may be listening under us might care, so we have to pass the two events
// separately. As far as we're concerned, all we need to do is make 'potential'
// follow the mouse.
void AddVertexWidget::mouseDragged(Pixel p) {
    updateTrackedSpike(p);
    requestRedisplay();
    MapEditWidget::mouseDragged(p);
}

void AddVertexWidget::mouseTracked(Pixel p) {
    updateTrackedSpike(p);
    requestRedisplay();
    MapEditWidget::mouseTracked(p);
}

// This function brings the 'potential' spike up-to-date with the specified
// pixel. This handles the details of transforming 'p' back into world-space,
// finding any nearby objects/grid-points to snap to, and detecting completed
// topological loops (for deciding when to stop)
void AddVertexWidget::updateTrackedSpike(Pixel p) {
    // See what things are in the vicinity of our cursor
    trackedSpike = renderer->spikeAtPixel(p);

    // Snap to the grid, if we didn't hit a map part
    if (trackedSpike.type == Map::NewSpike) {
        trackedSpike.snappedPosition
            = grid()->snapToGrid(trackedSpike.worldPosition);
    }
}
