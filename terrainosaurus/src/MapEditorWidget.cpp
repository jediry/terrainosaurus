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

using namespace Inca::Interface;

/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
void MapEditorWidget::initializeView() { }
void MapEditorWidget::resizeView(unsigned int w, unsigned int h) { }
void MapEditorWidget::renderView() { }


/*---------------------------------------------------------------------------*
 | Event-handlers
 *---------------------------------------------------------------------------*/
void MapEditorWidget::mouseDragged(unsigned int x, unsigned int y) { }
void MapEditorWidget::mouseTracked(unsigned int x, unsigned int y) { }
void MapEditorWidget::buttonPressed(MouseButton button, unsigned int x, unsigned int y) { }


/*---------------------------------------------------------------------------*
 | Map I/O functions
 *---------------------------------------------------------------------------*/
void MapEditorWidget::loadMap(const string &filename) { }
void MapEditorWidget::storeMap(const string &filename) const { }
