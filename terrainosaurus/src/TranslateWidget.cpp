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

// Import class definitions
#include "TranslateWidget.hpp"
#include "RotateWidget.hpp"
#include "ScaleWidget.hpp"
#include "TopologyWidget.hpp"
#include "MapSelectWidget.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::ui;

// Default constructor with optional component name
TranslateWidget::TranslateWidget(const string &nm)
    : MapEditWidget(nm) { }

// Default constructor with optional component name
RotateWidget::RotateWidget(const string &nm)
    : MapEditWidget(nm) { }

// Default constructor with optional component name
ScaleWidget::ScaleWidget(const string &nm)
    : MapEditWidget(nm) { }

// Default constructor with optional component name
TopologyWidget::TopologyWidget(const string &nm)
    : MapEditWidget(nm) { }

// Default constructor with optional component name
MapSelectWidget::MapSelectWidget(const string &nm)
    : CompositeWidget(nm), selection(this) { }

// Widget event-handler functions
void RotateWidget::renderView() {
    cout << "Rendering" << endl;
    Color<float, sRGB, false> color(1.0f, 0.0f, 0.0f);
    renderer.setColor(color);
    renderer.beginRenderImmediate(Quads);
        renderer.renderVertex(Point<double, 3>(-1.25, 1.25, 0.0));
        renderer.renderVertex(Point<double, 3>(1.25, 1.25, 0.0));
        renderer.renderVertex(Point<double, 3>(1.25, -1.25, 0.0));
        renderer.renderVertex(Point<double, 3>(-1.25, -1.25, 0.0));

        renderer.renderVertex(Point<double, 3>(-25, 25, 0.0));
        renderer.renderVertex(Point<double, 3>(-25, -25, 0.0));
        renderer.renderVertex(Point<double, 3>(25, -25, 0.0));
        renderer.renderVertex(Point<double, 3>(25, 25, 0.0));
//        renderer.renderVertex(Point<double, 3>(-1.25, 1.25, 0.0));
//        renderer.renderVertex(Point<double, 3>(1.25, 1.25, 0.0));
//        renderer.renderVertex(Point<double, 3>(1.25, -1.25, 0.0));
//        renderer.renderVertex(Point<double, 3>(-1.25, -1.25, 0.0));
    renderer.endRenderImmediate();
}
