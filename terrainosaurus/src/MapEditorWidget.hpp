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

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapEditorWidget;
    
    // Pointer typedefs
    typedef shared_ptr<MapEditorWidget> MapEditorWidgetPtr;
};


// Import map definition
#include "Map.hpp"


class terrainosaurus::MapEditorWidget
         : public CameraControl,
           virtual public Widget {
private:
    typedef MapEditorWidget ThisType;

public:
    // Import types from Map
    typedef Map::Point  Point;
    typedef Map::Vector Vector;


/*---------------------------------------------------------------------------*
 | Constructor and properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor
    MapEditorWidget();

    typedef inca::imaging::Color<float, inca::imaging::sRGB, true> Color;

    // The map
    rw_ptr_property(Map, map, /* */);

    // Rendering properties
    rw_list_property(Color, terrainColors);
    rw_property(Color, backgroundColor,         Color(0.1f, 0.1f, 0.1f, 1.0f));
    rw_property(Color, intersectionColor,       Color(1.0f, 1.0f, 1.0f, 1.0f));
    rw_property(Color, boundaryColor,           Color(0.0f, 0.0f, 0.3f, 1.0f));
    rw_property(Color, refinedBoundaryColor,    Color(0.0f, 0.0f, 1.0f, 1.0f));
    rw_property(Color, gridColor,               Color(0.8f, 0.8f, 0.8f, 0.5f));
    rw_property(float, boundaryWidth,           5.0f);
    rw_property(float, refinedBoundaryWidth,    3.0f);
    rw_property(float, intersectionSize,        7.0f);

    // Rendering switches
    rw_property(bool, renderRegions,            true);
    rw_property(bool, renderIntersections,      true);
    rw_property(bool, renderBoundaries,         true);
    rw_property(bool, renderRefinedBoundaries,  true);
    rw_property(bool, renderGrid,               true);


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    void initializeView();
    void resizeView(size_t w, size_t h);
    void renderView();
    void resetCameraProjection();

protected:
    inca::rendering::OpenGLRenderer renderer;     // We use this for some OpenGL tasks


/*---------------------------------------------------------------------------*
 | Event-handlers
 *---------------------------------------------------------------------------*/
public:
    void mouseDragged(index_t x, index_t y);
    void mouseTracked(index_t x, index_t y);
    void buttonPressed(inca::ui::MouseButton button,
                       index_t x, index_t y);
    void keyPressed(inca::ui::KeyCode k,
                    index_t x, index_t y);

/*---------------------------------------------------------------------------*
 | Map I/O functions
 *---------------------------------------------------------------------------*/
public:
    void loadMap(const string &filename);
    void storeMap(const string &filename) const;
};

#endif
