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

#ifndef TERRAINOSAURUS_MAP_VIEW
#define TERRAINOSAURUS_MAP_VIEW

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
#include <inca/ui/BasicWidget.hpp>

// Import Inca rendering subsystem
#include <inca/rendering.hpp>

// Import map and selection definitions
#include "Map.hpp"
#include "MapSelection.hpp"


class terrainosaurus::MapSelectWidget : public inca::ui::BasicWidget {
private:
    // Set up this class to own properties
    PROPERTY_OWNING_OBJECT(MapSelectWidget);

public:
    // Import topological types from Map
    typedef Map::Intersection       Intersection;
    typedef Map::Region             Region;
    typedef Map::Boundary           Boundary;
    typedef Map::RefinedBoundary    RefinedBoundary;

    // Import geometric and imaging typedefs from Map
    typedef Map::scalar_t           scalar_t;
    typedef Map::Point              Point;
    typedef Map::Vector             Vector;
    typedef Map::Color              Color;

    // Possible selection modes we can be in
    enum SelectionMode {
        Disabled,           // We're not allowing selection at all
        HoverSelect,        // We're not selecting right now, but could be soon
        ClickSelect,        // We're selecting by clicking a single point
        LassoSelect,        // We're selecting using a rectangular lasso
    };
    
    // Possible ways that additional selections can combine with the old one
    enum SetOperation {
        Replace,            // Replace the old selection with the new
        Add,                // Combine the two
        Intersect,          // Only keep elements common to both
        Subtract,           // Take elements away from the old one
    };


/*---------------------------------------------------------------------------*
 | Constructor and properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor with optional component name
    MapSelectWidget(const string &nm = "");

    // Constructor initializing the Map we're viewing
    MapSelectWidget(MapPtr m, const string &nm = "");

    // The map and its selection
    rw_ptr_property(Map, map, NULL);
    rw_ptr_property(MapSelection, selection, new MapSelection());

    // Map rendering properties
    rw_property(Color, intersectionColor,       Color(1.0f, 1.0f, 1.0f, 1.0f));
    rw_property(Color, boundaryColor,           Color(0.0f, 0.5f, 0.5f, 1.0f));
    rw_property(Color, refinedBoundaryColor,    Color(0.3f, 0.3f, 1.0f, 1.0f));
    rw_property(Color, gridColor,               Color(0.8f, 0.8f, 0.8f, 0.5f));
    rw_property(Color, lassoColor,              Color(0.1f, 1.0f, 0.1f, 0.5f));
    rw_property(Color, selectHilight,           Color(1.0f, 1.0f, 1.0f, 0.2f));
    rw_property(Color, hoverSelectHilight,      Color(1.0f, 1.0f, 1.0f, 0.2f));
    rw_property(Color, lassoSelectHilight,      Color(1.0f, 1.0f, 1.0f, 0.2f));

    rw_property(float, boundaryWidth,           4.0f);
    rw_property(float, refinedBoundaryWidth,    2.0f);
    rw_property(float, intersectionRadius,      5.0f);

    // Map selection properties
    rw_property(float, boundarySelectionRadius,     4.0f);
    rw_property(float, intersectionSelectionRadius, 5.0f);

    // Rendering switches
    rw_property(bool, drawRegions,          true);
    rw_property(bool, drawIntersections,    true);
    rw_property(bool, drawBoundaries,       true);
    rw_property(bool, drawRefinements,      true);
    rw_property(bool, drawGrid,             true);
    rw_property(bool, drawLasso,            true);
    rw_property(bool, drawSelected,         true);
    rw_property(bool, drawHoverSelected,    true);
    rw_property(bool, drawLassoSelected,    true);


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    void renderView();

protected:
    // Functions for rendering sub-components of the view
    void renderGrid();
    void renderLasso();
    void renderMap();

    // Rendering functions for the various map components
    // If 'forSelection' is true, then rather than drawing with texture,
    // selection-IDs will be generated
    template <bool forSelection> void renderRegions();
    template <bool forSelection> void renderBoundaries();
    template <bool forSelection> void renderRefinements();
    template <bool forSelection> void renderIntersections();

    Color pickColor(const Region &r) const;
    Color pickColor(const Boundary &b) const;
    Color pickColor(const Intersection &i) const;

    inca::rendering::OpenGLRenderer renderer; // Draw, my pretty!


/*---------------------------------------------------------------------------*
 | Event handlers
 *---------------------------------------------------------------------------*/
public:
    void buttonPressed(inca::ui::MouseButton b, Pixel p);
    void buttonReleased(inca::ui::MouseButton b, Pixel p);
    void mouseDragged(Pixel p);
    void mouseTracked(Pixel p);


/*---------------------------------------------------------------------------*
 | Selection functions
 *---------------------------------------------------------------------------*/
protected:
    // Determine what things are within range of the mouse pointer
    void updateTransientSelection();

    SelectionMode selectionMode;        // Click select? Lasso select?
    SetOperation setOperation;          // How does this selection combine?
    MapSelection  transientSelection;   // Intermediate selection object
    Pixel lassoStart, lassoEnd;         // The bounds of our selection lasso
};

#endif