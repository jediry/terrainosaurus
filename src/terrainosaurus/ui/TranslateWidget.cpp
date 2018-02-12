/*
 * File: TranslateWidget.cpp
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

// Import class definitions
#include "TranslateWidget.hpp"
#include "RotateWidget.hpp"
#include "ScaleWidget.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::ui;

// Default constructor with optional component name
TranslateWidget::TranslateWidget(const string &nm)
    : MapEditWidget(nm), activePart(NoPart), transforming(false),
      xPartColor(this), yPartColor(this), zPartColor(this),
      planePartColor(this), activePartColor(this), transformingPartColor(this),
      axisThickness(this), boxThickness(this), axisLength(this),
      boxRadius(this) { }


void TranslateWidget::renderView() {
    MapEditWidget::renderView();    // Render whatever's under me
//    renderer->pushTransform();
//        renderer.translate(
//        renderer.scale(getScreenspaceScaleFactor());
//        renderer.
//    renderGadget();                 // Draw my transformation gadget
}

template <bool forSelection>
void TranslateWidget::renderGadget() {
    // First, figure out the local coordinates to get the scale right
    Vector scale(1.0, 1.0, 1.0);

    Point origin(0.0, 0.0, 0.0);
    Point endX(scale[0] * axisLength(), 0.0, 0.0);
    Point endY(0.0, scale[1] * axisLength(), 0.0);
    Point endZ(0.0, 0.0, scale[2] * axisLength());

    Color c;    // The color object we'll use for drawing

    // Draw the gadget
    renderer.pushTransform();           // Save the old transformation
//    renderer.translate(gadgetOrigin);   // Move the the right spot
    renderer.beginRenderImmediate(Lines);

        // Draw the X axis part
        if (forSelection)
            renderer.setSelectionID(TranslateAlongXPart);
        else {
            if (activePart == TranslateAlongXPart) {
                if (transforming)   c = xPartColor() % transformingPartColor();
                else                c = xPartColor() % activePartColor();
            } else                  c = xPartColor();
            renderer.setColor(c);
        }
        renderer.renderVertex(origin);
        renderer.renderVertex(endX);

        // Draw the Y axis part
        if (forSelection)
            renderer.setSelectionID(TranslateAlongYPart);
        else {
            if (activePart == TranslateAlongYPart) {
                if (transforming)   c = yPartColor() % transformingPartColor();
                else                c = yPartColor() % activePartColor();
            } else                  c = yPartColor();
            renderer.setColor(c);
        }
        renderer.renderVertex(origin);
        renderer.renderVertex(endY);

        // Draw the Z axis part
        if (forSelection)
            renderer.setSelectionID(TranslateAlongZPart);
        else {
            if (activePart == TranslateAlongZPart) {
                if (transforming)   c = zPartColor() % transformingPartColor();
                else                c = zPartColor() % activePartColor();
            } else                  c = zPartColor();
            renderer.setColor(c);
        }
        renderer.renderVertex(origin);
        renderer.renderVertex(endZ);

    renderer.endRenderImmediate();
    renderer.popTransform();
}

void TranslateWidget::keyPressed(KeyCode key, Pixel p) {
    if (key == KeySpace) {
        activePart = GadgetPart((activePart + 1) % 4);
        requestRedisplay();
    } else
        MapEditWidget::keyPressed(key, p);
}

// Default constructor with optional component name
RotateWidget::RotateWidget(const string &nm)
    : MapEditWidget(nm) { }

// Default constructor with optional component name
ScaleWidget::ScaleWidget(const string &nm)
    : MapEditWidget(nm) { }

// Widget event-handler functions
void RotateWidget::renderView() {
#if 0
    cout << "Rendering" << endl;
    Color<float, sRGB, false> color(1.0f, 0.0f, 0.0f);
    renderer.setColor(color);
    renderer.beginRenderImmediate(Quads);
        renderer.renderVertex(inca::math::Point<double, 3>(-1.25, 1.25, 0.0));
        renderer.renderVertex(inca::math::Point<double, 3>(1.25, 1.25, 0.0));
        renderer.renderVertex(inca::math::Point<double, 3>(1.25, -1.25, 0.0));
        renderer.renderVertex(inca::math::Point<double, 3>(-1.25, -1.25, 0.0));

        renderer.renderVertex(inca::math::Point<double, 3>(-25, 25, 0.0));
        renderer.renderVertex(inca::math::Point<double, 3>(-25, -25, 0.0));
        renderer.renderVertex(inca::math::Point<double, 3>(25, -25, 0.0));
        renderer.renderVertex(inca::math::Point<double, 3>(25, 25, 0.0));
//        renderer.renderVertex(Point<double, 3>(-1.25, 1.25, 0.0));
//        renderer.renderVertex(Point<double, 3>(1.25, 1.25, 0.0));
//        renderer.renderVertex(Point<double, 3>(1.25, -1.25, 0.0));
//        renderer.renderVertex(Point<double, 3>(-1.25, -1.25, 0.0));
    renderer.endRenderImmediate();
#endif
}
