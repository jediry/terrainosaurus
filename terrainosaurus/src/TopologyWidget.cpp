/*
 * File: TopologyWidget.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "TopologyWidget.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::ui;


// Default constructor with optional component name
TopologyWidget::TopologyWidget(const string &nm)
    : MapEditWidget(nm),
      committedBoundaryColor(this), potentialBoundaryColor(this) { }

// Render the view from this Widget
void TopologyWidget::renderView() {
    MapEditWidget::renderView();    // Render whatever's under me

    // If we're in progress, show the user what he's doing
    if (committed.size() > 0) {
        renderer.beginRenderImmediate(LineStrip);
            // Render the lines making up the already-laid-down part
            renderer.setColor(committedBoundaryColor());
            for (index_t i = 0; i < index_t(committed.size()); i++) {
                renderer.renderVertex(committed[i].position);
                cerr << "Committed: " << committed[i].position << endl;
            }

            // Now render the trail out to the mouse pointer
            renderer.setColor(potentialBoundaryColor());
            renderer.renderVertex(committed.back().position);
            renderer.renderVertex(potential.position);
        renderer.endRenderImmediate();
    }
}

void TopologyWidget::keyPressed(KeyCode key, Pixel p) {
//    MapEditWidget::keyPressed(key, p);
}

// We simply pass thru 'buttonClicked' 'buttonPressed' events, unless we're
// currently busy making something (in which case we dump 'em in the trash).
void TopologyWidget::buttonClicked(MouseButton b, Pixel p) {
//    if (committed.size() == 0)  // Not doing anything right now
//        MapEditWidget::buttonClicked(b, p); // Pass on thru
}

void TopologyWidget::buttonPressed(MouseButton b, Pixel p) {
//    if (committed.size() == 0)  // Not doing anything right now
//        MapEditWidget::buttonPressed(b, p); // Pass on thru
}

// If the user clicks the LMB, add a new Intersection at the place where he
// clicked. If he clicks the RMB, undo one Intersection
void TopologyWidget::buttonReleased(MouseButton b, Pixel p) {

    // Left mouse button adds a new intersection
    if (b == LeftButton) {
        // Make 'potential' current
        updatePotentialIntersection(p);

        if (committed.size() > 0 && magnitude(potential.position - committed[0].position) < 5) {
            Map::PointList points;
            for (int i = 0; i < index_t(committed.size()); ++i)
                points.push_back(committed[i].position);
            map->addRegion(points);
            committed.clear();
        } else {
            // Actualize your potential!
            committed.push_back(potential);
        }

    // Right mouse button deletes the previous intersection
    } else if (b == RightButton) {
        committed.pop_back();
    }

    requestRedisplay();     // Update the display with our new point

//    MapEditWidget::buttonReleased(b, p);
}

// *WE* don't really care about the difference between a 'mouseDragged' and
// a 'mouseTracked', so we treat them pretty much identically. However, those
// who may be listening under us might care, so we have to pass the two events
// separately. As far as we're concerned, all we need to do is make 'potential'
// follow the mouse.
void TopologyWidget::mouseDragged(Pixel p) {
    updatePotentialIntersection(p);
    requestRedisplay();
    MapEditWidget::mouseDragged(p);
}

void TopologyWidget::mouseTracked(Pixel p) {
    updatePotentialIntersection(p);
    requestRedisplay();
    MapEditWidget::mouseTracked(p);
}

// This function brings the value of 'potential' up-to-date with the specified
// pixel. This handles the details of transforming 'p' back into world-space,
// and finding any nearby objects/grid-points to snap to.
void TopologyWidget::updatePotentialIntersection(Pixel p) {
    // Update the values in our potential intersection
    typedef inca::math::Point<double, 3> Point3D;

    Point3D world = renderer.unproject(Point3D(p[0], size[1] - p[1], 0.0));
    potential.position = Point(world[0], world[1]);
    potential.type = CreateNew;
    potential.id = 0;
}
