/*
 * File: ScaleWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_SCALE_WIDGET
#define TERRAINOSAURUS_SCALE_WIDGET

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class ScaleWidget;
    
    // Pointer typedefs
    typedef shared_ptr<ScaleWidget>       ScaleWidgetPtr;
    typedef shared_ptr<ScaleWidget const> ScaleWidgetConstPtr;
};

// Import superclass definition
#include "MapEditWidget.hpp"


class terrainosaurus::ScaleWidget : public MapEditWidget {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(ScaleWidget);

public:
    // Default constructor with optional component name
    ScaleWidget(const string &nm = "");

    // Widget event-handler functions
//    void renderView() { }
//    void keyPressed(KeyCode key, Point p);
//    void mouseDragged();
//    void mouseTracked();
};

#endif
