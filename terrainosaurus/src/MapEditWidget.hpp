/*
 * File: MapEditWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_MAP_EDIT_WIDGET
#define TERRAINOSAURUS_MAP_EDIT_WIDGET

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapEditWidget;
    
    // Pointer typedefs
    typedef shared_ptr<MapEditWidget>       MapEditWidgetPtr;
    typedef shared_ptr<MapEditWidget const> MapEditWidgetConstPtr;
};

// Import superclass definition
#include <inca/ui/PassThruWidget.hpp>

// Import Inca rendering subsystem
#include <inca/rendering.hpp>

// Import data object definitions
#include "Map.hpp"
#include "MapSelection.hpp"


class terrainosaurus::MapEditWidget : public inca::ui::PassThruWidget {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(MapEditWidget);

public:
    // Default constructor with optional component name
    MapEditWidget(const string &nm = "")
        : PassThruWidget(nm), map(this), selection(this) { }

    // The map we're editing
    rw_ptr_property(Map, map, NULL);

    // The selected things we should modify
    rw_ptr_property(MapSelection, selection, NULL);

    // Widget event-handler functions
//    void renderView() { }
//    void keyPressed(KeyCode key, Point p);
//    void mouseDragged();
//    void mouseTracked();
};

#endif
