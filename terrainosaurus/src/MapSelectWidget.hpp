/*
 * File: MapSelectWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_MAP_SELECT_WIDGET
#define TERRAINOSAURUS_MAP_SELECT_WIDGET

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapSelectWidget;
    
    // Pointer typedefs
    typedef shared_ptr<MapSelectWidget>       MapSelectWidgetPtr;
    typedef shared_ptr<MapSelectWidget const> MapSelectWidgetConstPtr;
};

// Import superclass definition
#include <inca/ui/CompositeWidget.hpp>

// Import Inca rendering subsystem
#include <inca/rendering.hpp>

// Import data object definitions
#include "MapSelection.hpp"


class terrainosaurus::MapSelectWidget : public inca::ui::CompositeWidget {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(MapSelectWidget);

public:
    // Default constructor with optional component name
    MapSelectWidget(const string &nm = "");

    // The selected things we should modify
    rw_ptr_property(MapSelection, selection, NULL);

    // Widget event-handler functions
//    void renderView() { }
//    void keyPressed(KeyCode key, Point p);
//    void mouseDragged();
//    void mouseTracked();
};

#endif
