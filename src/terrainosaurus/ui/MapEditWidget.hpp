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
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapEditWidget;

    // Pointer typedefs
    typedef shared_ptr<MapEditWidget>       MapEditWidgetPtr;
    typedef shared_ptr<MapEditWidget const> MapEditWidgetConstPtr;
};

// Import superclass definitions
#include <inca/ui/BasicWidget.hpp>


class terrainosaurus::MapEditWidget : public inca::ui::BasicWidget {
private:
    // Set up this class to own properties
    PROPERTY_OWNING_OBJECT(MapEditWidget);

public:
    // Import topological types from Map
    typedef Map::VertexPtr          VertexPtr;
    typedef Map::FacePtr            FacePtr;
    typedef Map::EdgePtr            EdgePtr;

    // Import geometric and imaging typedefs from Map
    typedef Map::scalar_t           scalar_t;
    typedef Map::Point              Point;
    typedef Map::Vector             Vector;

    // Import Inca UI event types
    typedef inca::ui::MouseButton   MouseButton;
    typedef inca::ui::KeyCode       KeyCode;

    // Possible selection modes we can be in
    enum SelectionMode {
        Disabled,           // We're not allowing selection at all
        HoverSelect,        // We're not selecting right now, but could be soon
        ClickSelect,        // We're selecting by clicking a single point
        LassoSelect,        // We're selecting using a rectangular lasso
    };

    // Possible ways that additional selections can combine with the old one
    enum SelectionOperation {
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
    MapEditWidget(const string &nm = std::string(), bool enableSelect = true);

    // Rendering switches
    rw_property(bool, drawVertices,         true);
    rw_property(bool, drawFaces,            true);
    rw_property(bool, drawEdges,            true);
    rw_property(bool, drawRefinedEdges,     true);
    rw_property(bool, drawEnvelopeBorders,  true);
    rw_property(bool, drawTangentLines,     true);
    rw_property(bool, drawSelected,         true);
    rw_property(bool, drawHoverSelected,    true);
    rw_property(bool, drawLassoSelected,    true);
    rw_property(bool, drawGrid,             true);
    rw_property(bool, drawLasso,            true);


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    void renderView();


/*---------------------------------------------------------------------------*
 | Event handlers
 *---------------------------------------------------------------------------*/
public:
    void buttonPressed(MouseButton b, Pixel p);
    void buttonReleased(MouseButton b, Pixel p);
    void mouseDragged(Pixel p);
    void mouseTracked(Pixel p);


/*---------------------------------------------------------------------------*
 | Selection functions
 *---------------------------------------------------------------------------*/
protected:
    // Combine the transient selection with the persistent one appropriately
    void updatePersistentSelection();

    // Determine what things are within range of the mouse pointer
    void updateTransientSelection();

    SelectionMode selectionMode;            // Click select? Lasso select?
    SelectionOperation selectionOperation;  // How does this selection combine?
    Pixel lassoStart, lassoEnd;             // The bounds of our selection lasso
};

#endif
