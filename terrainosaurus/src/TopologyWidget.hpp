/*
 * File: TopologyWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_TOPOLOGY_WIDGET
#define TERRAINOSAURUS_TOPOLOGY_WIDGET

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TopologyWidget;
    
    // Pointer typedefs
    typedef shared_ptr<TopologyWidget>       TopologyWidgetPtr;
    typedef shared_ptr<TopologyWidget const> TopologyWidgetConstPtr;
};

// Import superclass definition
#include "MapEditWidget.hpp"


class terrainosaurus::TopologyWidget : public MapEditWidget {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(TopologyWidget);

public:
    // Default constructor with optional component name
    TopologyWidget(const string &nm = "");

    // Widget event-handler functions
//    void renderView() { }
//    void keyPressed(KeyCode key, Point p);
//    void mouseDragged();
//    void mouseTracked();
};

#endif
