/*
 * File: MapEditorWidget.h++
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The MapEditorWidget class implements a toolkit-independent Widget for
 *      viewing and editing 2D maps made up of adjacent polygons (i.e., a 2D
 *      PolygonMesh). Polygons may be added and edited with the mouse, and
 *      each polygon may be dynamically assigned a terrain type.
 */

#ifndef TERRAINOSAURUS_MAP_EDITOR_WIDGET
#define TERRAINOSAURUS_MAP_EDITOR_WIDGET

// This is part of the Terrainosaurus terrain generation engine
namespace Terrainosaurus {
    // Forward declarations
    class MapEditorWidget;
};

// Import superclass definition
#include <interface/generic/Widget.h++>

// Import map definition
#include "Map.hpp"

// Import Inca::Geometry library
#include <geometry/Geometry.h++>


class Terrainosaurus::MapEditorWidget : public Inca::Interface::Widget {
/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    void initializeView();
    void resizeView(unsigned int w, unsigned int h);
    void renderView();


/*---------------------------------------------------------------------------*
 | Event-handlers
 *---------------------------------------------------------------------------*/
public:
    void mouseDragged(unsigned int x, unsigned int y);
    void mouseTracked(unsigned int x, unsigned int y);
    void buttonPressed(Inca::Interface::MouseButton button,
                       unsigned int x, unsigned int y);


/*---------------------------------------------------------------------------*
 | Map I/O functions
 *---------------------------------------------------------------------------*/
public:
    void loadMap(const string &filename);
    void storeMap(const string &filename) const;

protected:
    Map map;    // The rough map drawn by the user
};

#endif
