/*
 * File: AddFaceWidget.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "AddFaceWidget.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::ui;



// Default constructor with optional component name
AddFaceWidget::AddFaceWidget(const string &nm)
    : MapEditWidget(nm, false),
      committedBoundaryColor(this), potentialBoundaryColor(this) { }

// Render the view from this Widget
void AddFaceWidget::renderView() {
    MapEditWidget::renderView();    // Render whatever's under me

    // If we're in progress, show the user what he's doing
    if (committedSpikes.size() > 0) {
        renderer->beginRenderImmediate(LineStrip);
            // Render the lines making up the already-laid-down part
            renderer->setColor(committedBoundaryColor());
            for (IndexType i = 0; i < IndexType(committedSpikes.size()); ++i) {
                renderer->renderVertex(committedSpikes[i].snappedPosition);
            }

            // Now render the trail out to the mouse pointer
            renderer->setColor(potentialBoundaryColor());
            renderer->renderVertex(committedSpikes.back().snappedPosition);
            renderer->renderVertex(trackedSpike.snappedPosition);
        renderer->endRenderImmediate();
    }
}

// If the user clicks the LMB, add a new Intersection at the place where he
// clicked. If he clicks the RMB, undo one Intersection
void AddFaceWidget::buttonReleased(MouseButton b, Pixel p) {

    // Left mouse button adds a new intersection
    if (b == LeftButton) {
        // Update the current spike
        updateTrackedSpike(p);

        // Actualize your potential!
        committedSpikes.push_back(trackedSpike);

        // If we've completed a loop, stick it into the map
        if (regionComplete) {
            committedSpikes.pop_back();         // Get rid of duplicate last pt
            map()->createFace(committedSpikes,   // Make the thingy
                             currentTerrainType());
            committedSpikes.clear();            // Start over
        }

    // Right mouse button deletes the previous intersection
    } else if (b == RightButton && committedSpikes.size() > 0) {
        committedSpikes.pop_back();
    }

    requestRedisplay();     // Update the display with our new point
}

// *WE* don't really care about the difference between a 'mouseDragged' and
// a 'mouseTracked', so we treat them pretty much identically. However, those
// who may be listening under us might care, so we have to pass the two events
// separately. As far as we're concerned, all we need to do is make 'potential'
// follow the mouse.
void AddFaceWidget::mouseDragged(Pixel p) {
    updateTrackedSpike(p);
    requestRedisplay();
    MapEditWidget::mouseDragged(p);
}

void AddFaceWidget::mouseTracked(Pixel p) {
    updateTrackedSpike(p);
    requestRedisplay();
    MapEditWidget::mouseTracked(p);
}

// This function brings the 'potential' spike up-to-date with the specified
// pixel. This handles the details of transforming 'p' back into world-space,
// finding any nearby objects/grid-points to snap to, and detecting completed
// topological loops (for deciding when to stop)
void AddFaceWidget::updateTrackedSpike(Pixel p) {
    // See what things are in the vicinity of our cursor
    trackedSpike = renderer->spikeAtPixel(p);

    // Snap to the grid, if we didn't hit a map part
    if (trackedSpike.type == Map::NewSpike) {
        trackedSpike.snappedPosition
            = grid()->snapToGrid(trackedSpike.worldPosition);
    }

    // See if we've (potentially) completed a loop
    if (committedSpikes.size() > 2 && trackedSpike == committedSpikes[0])
        regionComplete = true;
    else
        regionComplete = false;
}
