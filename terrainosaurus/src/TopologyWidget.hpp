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

    // Import types from Map
    typedef Map::scalar_t           scalar_t;
    typedef Map::Point              Point;
    typedef Map::Vector             Vector;
    typedef Map::Intersection       Intersection;
    typedef Map::Region             Region;
    typedef Map::Boundary           Boundary;
    typedef Map::RefinedBoundary    RefinedBoundary;

public:
    // Default constructor with optional component name
    TopologyWidget(const string &nm = "");

    // UI properties
    rw_property(Color, committedBoundaryColor, Color(0.8f, 0.2f, 0.2f, 0.4f));
    rw_property(Color, potentialBoundaryColor, Color(0.2f, 0.8f, 0.2f, 0.4f));

    // Widget event-handler functions
    void renderView();
    void keyPressed(inca::ui::KeyCode key, Pixel p);
    void buttonPressed(inca::ui::MouseButton b, Pixel p);
    void buttonReleased(inca::ui::MouseButton b, Pixel p);
    void buttonClicked(inca::ui::MouseButton b, Pixel p);
    void mouseDragged(Pixel p);
    void mouseTracked(Pixel p);

protected:
    // This is called to bring the potential (next) intersection in sync with
    // the mouse pointer.
    void updatePotentialIntersection(Pixel p);

    // This enumerates the possible cases when adding an Intersection to the
    // in-progress region.
    enum IntersectionType {
        CreateNew,          // The click didn't hit an existing element
        ReuseExisting,      // The user clicked an existing Intersection
        SplitBoundary,      // The user clicked an existing Boundary
    };

    // This struct holds the information for a single vertex in the new region.
    // This data does not go into the Map until the region is complete.
    struct HitRecord {
        Point position;         // Where did the user click (world space)
        IntersectionType type;  // What sort of new intersection is this?
        unsigned int id;        // The ID of the thing we hit
    };

    vector<HitRecord> committed;    // List of points in the in-progress region
    HitRecord potential;            // Where the user is pointing right now
};

#endif
