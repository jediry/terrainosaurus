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
    // Import scalar model used for geometry
    INCA_MATH_SCALAR_TYPEDEFS(double, IS_NOT_WITHIN_TEMPLATE, /* */, /* */);
    INCA_MATH_VECTOR_TYPEDEFS(scalar_t, 3, /* */, /* */);

    typedef inca::imaging::Color<float, inca::imaging::sRGB, true> Color;


public:
    // Default constructor with optional component name
    MapEditWidget(const string &nm = "")
        : PassThruWidget(nm), map(this), selection(this) { }

    // The map we're editing
    rw_ptr_property(Map, map, NULL);

    // The selected things we should modify
    rw_ptr_property(MapSelection, selection, NULL);

protected:
    inca::rendering::OpenGLRenderer renderer;   // For drawing (duh!)
};

#endif
