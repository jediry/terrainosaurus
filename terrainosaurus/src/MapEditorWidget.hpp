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
namespace Terrainosaurus {
    // Forward declarations
    class MapEditorWidget;
    
    // Pointer typedefs
    typedef shared_ptr<MapEditorWidget> MapEditorWidgetPtr;
};


// Import map definition
#include "Map.hpp"


class Terrainosaurus::MapEditorWidget
         : public Inca::Interface::CameraControl,
           virtual public Inca::Interface::Widget {
/*---------------------------------------------------------------------------*
 | Constructor
 *---------------------------------------------------------------------------*/
public:
    MapEditorWidget();


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
public:
    MapPtr map() const { return _map; }


protected:
    MapPtr _map;                              // The terrain map drawn by the user
    Inca::World::SolidObject2DPtr mapObject;  // Inca's full-blown object for it


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    void initializeView();
    void resizeView(unsigned int w, unsigned int h);
    void renderView();

protected:
    Inca::Rendering::GLRenderer renderer;     // We use this for some OpenGL tasks
    Inca::Rendering::GLRenderer::ObjectProxyPtr mapProxy;
    Inca::Rendering::GLRenderer::CameraProxyPtr cameraProxy;


/*---------------------------------------------------------------------------*
 | Event-handlers
 *---------------------------------------------------------------------------*/
public:
    void mouseDragged(unsigned int x, unsigned int y);
    void mouseTracked(unsigned int x, unsigned int y);
    void buttonPressed(Inca::Interface::MouseButton button,
                       unsigned int x, unsigned int y);
    void keyPressed(Inca::Interface::KeyCode k,
                    unsigned int x, unsigned int y);

protected:
    // Input modes
//    enum EditMode {
        

/*---------------------------------------------------------------------------*
 | Map I/O functions
 *---------------------------------------------------------------------------*/
public:
    void loadMap(const string &filename);
    void storeMap(const string &filename) const;
};

#endif
