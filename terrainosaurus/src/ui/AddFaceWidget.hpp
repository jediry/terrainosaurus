/*
 * File: AddFaceWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_ADD_FACE_WIDGET
#define TERRAINOSAURUS_ADD_FACE_WIDGET

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class AddFaceWidget;
    
    // Pointer typedefs
    typedef shared_ptr<AddFaceWidget>       AddFaceWidgetPtr;
    typedef shared_ptr<AddFaceWidget const> AddFaceWidgetConstPtr;
};

// Import superclass definition
#include "MapEditWidget.hpp"


class terrainosaurus::AddFaceWidget : public MapEditWidget {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(AddFaceWidget);

    typedef Map::SpikeList  SpikeList;
    typedef Map::Spike      Spike;
    typedef Map::SpikeType  SpikeType;

public:
    // Default constructor with optional component name
    AddFaceWidget(const string &nm = "");

    // UI properties
    rw_property(Color, committedBoundaryColor, Color(0.8f, 0.2f, 0.2f, 0.4f));
    rw_property(Color, potentialBoundaryColor, Color(0.2f, 0.8f, 0.2f, 0.4f));

    // Widget event-handler functions
    void renderView();
    void buttonPressed(MouseButton b, Pixel p) { }
    void buttonClicked(MouseButton b, Pixel p) { }
    void buttonReleased(MouseButton b, Pixel p);
    void mouseDragged(Pixel p);
    void mouseTracked(Pixel p);

protected:
    // This is called to bring the potential (next) intersection in sync with
    // the mouse pointer.
    void updateTrackedSpike(Pixel p);

    SpikeList committedSpikes;  // List of points in the in-progress region
    Spike trackedSpike;         // Where the user is pointing right now
    bool regionComplete;        // Have we made a valid region?
};

#endif
