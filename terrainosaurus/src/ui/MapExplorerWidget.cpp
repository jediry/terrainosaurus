/*
 * File: MapExplorerWidget.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "MapExplorerWidget.hpp"
using namespace terrainosaurus;

// Import Application class definition
#include "../MapExplorer.hpp"
using namespace inca::rendering;
using namespace inca::ui;


/*---------------------------------------------------------------------------*
 | Constructor & properties
 *---------------------------------------------------------------------------*/
// Default constructor with optional component name
MapExplorerWidget::MapExplorerWidget(const string &nm)
        : MultiplexorWidget(nm), renderer(this),
          backgroundColor(this), toolSelect2D(this) { }

// Custom setter for the "backgroundColor" property
void MapExplorerWidget::property_set(Color, backgroundColor) {
    _backgroundColor = value;
    renderer->setBackgroundColor(_backgroundColor);
}


/*---------------------------------------------------------------------------*
 | Widget event-handler functions
 *---------------------------------------------------------------------------*/
// Ensure the background color gets set the first time
void MapExplorerWidget::initializeView() {
    renderer->setBackgroundColor(backgroundColor());
}

// Handle all of the icky details of resizing
void MapExplorerWidget::resizeView(Dimension sz) {
    if (sz != size) {   // Don't do anything if we're already the right size
        renderer->resize(sz[0], sz[1]);     // Reshape the viewport
        MultiplexorWidget::resizeView(sz);  // Do superclass impl.
    }
}

// Set up for rendering and render child widgets
void MapExplorerWidget::renderView() {
    // First, clear the display and the renderer matrix state
    renderer->clear();
    renderer->resetTransformMatrix();

    // Last, render the sub-widgets
    MultiplexorWidget::renderView();
}

void MapExplorerWidget::keyPressed(KeyCode key, Pixel p) {
    switch (key) {
        // Alt-Q == quit
        case KeyQ:
            if (theseModifiersActive(AltModifier)) {
                MapExplorer::instance().exit(0, "Exiting normally...");
                return; // Of course, we never actually get here...
            }
            break;

        // F11 == toggle full-screen mode
        case KeyF11:
            if (theseModifiersActive(NoModifiers)) {
                WindowPtr w = getContainingWindow();
                if (w) w->setFullScreen(! w->isFullScreen());
                requestRedisplay();
                return;
            }
            break;

        // Alt-L == Load parameters file
        case KeyL:
            if (theseModifiersActive(AltModifier)) {
                MapExplorer::instance().loadTerrainLibrary("test.ttl");
                requestRedisplay();
                return;
            }
            break;

        // Alt-O == Open map file
        case KeyO:
            if (theseModifiersActive(AltModifier)) {
                MapExplorer::instance().loadMap("test.map");
                requestRedisplay();
                return;
            }
            break;

        // Alt-W == Dump contents to files
        case KeyW:
            if (theseModifiersActive(AltModifier)) {
                MapExplorer::instance().storeMap("dump.map");
                MapExplorer::instance().storeTerrainLibrary("dump.ttl");
                return;
            }
            break;

        // Tab/Shift-Tab switches between tools
        case KeyTab:
            if (theseModifiersActive(NoModifiers)) {
                toolSelect2D->selectNextWidget();
                return;
            } else if (theseModifiersActive(ShiftModifier)) {
                toolSelect2D->selectPreviousWidget();
                return;
            }
            break;
    }
    
    // If we got here, this message isn't for us. Move along. Move along.
    MultiplexorWidget::keyPressed(key, p);
}
