/*
 * File: RotateWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_ROTATE_WIDGET
#define TERRAINOSAURUS_ROTATE_WIDGET

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class RotateWidget;
    
    // Pointer typedefs
    typedef shared_ptr<RotateWidget>       RotateWidgetPtr;
    typedef shared_ptr<RotateWidget const> RotateWidgetConstPtr;
};

// Import superclass definition
#include "MapEditWidget.hpp"


class terrainosaurus::RotateWidget : public MapEditWidget {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(RotateWidget);

public:
    // Default constructor with optional component name
    RotateWidget(const string &nm = "");

    // Widget event-handler functions
    void renderView();

    inca::rendering::OpenGLRenderer renderer;
//    void keyPressed(KeyCode key, Point p);
//    void mouseDragged();
//    void mouseTracked();
};

#endif
