/*
 * File: AddVertexWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_ADD_VERTEX_WIDGET
#define TERRAINOSAURUS_ADD_VERTEX_WIDGET

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class AddVertexWidget;
    
    // Pointer typedefs
    typedef shared_ptr<AddVertexWidget>       AddVertexWidgetPtr;
    typedef shared_ptr<AddVertexWidget const> AddVertexWidgetConstPtr;
};

// Import superclass definition
#include "MapEditWidget.hpp"


class terrainosaurus::AddVertexWidget : public MapEditWidget {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(AddVertexWidget);

    typedef Map::Spike      Spike;

public:
    // Default constructor with optional component name
    explicit AddVertexWidget(const string & nm = std::string());

    // UI properties
    rw_property(Color, potentialVertexColor,    Color(1.0f, 0.2f, 0.2f, 0.4f));
    rw_property(float, potentialVertexDiameter, 6.0f);

    // Widget event-handler functions
    void renderView();
    void buttonPressed(MouseButton b, Pixel p) { }
    void buttonReleased(MouseButton b, Pixel p) { }
    void buttonClicked(MouseButton b, Pixel p);
    void mouseDragged(Pixel p);
    void mouseTracked(Pixel p);

protected:
    // This is called to bring the potential (next) intersection in sync with
    // the mouse pointer.
    void updateTrackedSpike(Pixel p);
    Spike trackedSpike;         // Where the user is pointing right now
};

#endif
