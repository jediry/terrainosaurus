/*
 * File: MapEditorWidget.c++
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The MapEditor class implements a toolkit-independent GUI widget for
 *      viewing and editing 2D maps made up of adjacent polygons. Each polygon
 *      may be dynamically assigned a terrain type. Any number of polygons may
 *      be drawn -- overlapping polygons of the same type will be merged, while
 *      overlapping polygons of different types will
 */

// Import class definition
#include "MapEditorWidget.hpp"
using Terrainosaurus::MapEditorWidget;

// Import Application definition
#include <interface/generic/Application.h++>
using namespace Inca::Interface;

// Import PolygonMesh I/O
#include <io/IOUtilities.h++>
using namespace Inca::IO;


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
void MapEditorWidget::initializeView() { }

void MapEditorWidget::resizeView(unsigned int w, unsigned int h) {
    cameraProxy.setupProjection();
    glViewport(0, 0, w, h);
}

void MapEditorWidget::renderView() {
    glClear(GL_COLOR_BUFFER_BIT);
    cameraProxy.setupView();
    mapMeshProxy.render(Opacity);
}


/*---------------------------------------------------------------------------*
 | Event-handlers
 *---------------------------------------------------------------------------*/
void MapEditorWidget::mouseDragged(unsigned int x, unsigned int y) {

}

void MapEditorWidget::mouseTracked(unsigned int x, unsigned int y) {

}

void MapEditorWidget::buttonPressed(MouseButton button,
                                    unsigned int x, unsigned int y) {

}

void MapEditorWidget::keyPressed(KeyCode k, unsigned int x, unsigned int y) {
    switch (k) {
    case KEY_Q:
        application->exit(0, "Exited normally");
        break;
    }
}


/*---------------------------------------------------------------------------*
 | Map I/O functions
 *---------------------------------------------------------------------------*/
void MapEditorWidget::loadMap(const string &filename) {
    ObjectPtr obj = loadModel(filename);

}

void MapEditorWidget::storeMap(const string &filename) const {

}

