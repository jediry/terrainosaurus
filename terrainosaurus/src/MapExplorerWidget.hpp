/*
 * File: MapExplorerWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_MAP_EXPLORER_WIDGET
#define TERRAINOSAURUS_MAP_EXPLORER_WIDGET

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapExplorerWidget;
    
    // Pointer typedefs
    typedef shared_ptr<MapExplorerWidget>       MapExplorerWidgetPtr;
    typedef shared_ptr<MapExplorerWidget const> MapExplorerWidgetConstPtr;
};

// Import superclass definition
#include <inca/ui/PassThruWidget.hpp>

// Import Inca rendering subsystem
#include <inca/rendering.hpp>

// Import map definition
#include "Map.hpp"

// Import sub-widget definitions
#include <inca/ui/MultiplexorWidget.hpp>
#include <inca/ui/CameraControl.hpp>
#include "MapEditWidget.hpp"
#include "TranslateWidget.hpp"
#include "RotateWidget.hpp"
#include "ScaleWidget.hpp"
#include "TopologyWidget.hpp"
#include "MapSelectWidget.hpp"


class terrainosaurus::MapExplorerWidget : public inca::ui::PassThruWidget {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(MapExplorerWidget);

public:
    // Container typedefs
    typedef vector<MapEditWidgetPtr> MapEditToolList;

    // Color typedef
    typedef Map::Color Color;


public:
    // Default constructor with optional component name
    MapExplorerWidget(const string &nm = "");

    // The map
    rw_ptr_property_custom_set(Map, map, NULL);
        void ptr_property_set(Map, map);

    // UI parameters
    rw_property_custom_set(Color, backgroundColor, Color(0.1f, 0.1f, 0.1f, 1.0f));
        void property_set(Color, backgroundColor);


    // Widget event-handler functions
    void initializeView();
    void resizeView(Dimension d);
    void renderView();
    void keyPressed(inca::ui::KeyCode key, Pixel p);

    // I/O functions
    void loadParams(const string &filename);
    void loadMap(const string &filename);
    void storeMap(const string &filename) const;

protected:
    // Rendering objects
    inca::world::OrthographicCameraPtr camera2D;
    inca::rendering::OpenGLRenderer renderer;

    // Subwidgets we have to hold onto
    inca::ui::MultiplexorWidgetPtr modeSelect, toolSelect2D;
    inca::ui::CameraControlPtr navigate2D;
    MapEditToolList tools2D;
    MapSelectWidgetPtr viewSelect2D;
};

#endif
