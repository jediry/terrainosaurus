/*
 * File: MapView.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The MapView class implements a toolkit-independent Widget for
 *      viewing and editing 2D maps made up of adjacent polygons (i.e., a 2D
 *      PolygonMesh). Polygons may be added and edited with the mouse, and
 *      each polygon may be dynamically assigned a terrain type.
 */

#ifndef TERRAINOSAURUS_MAP_EDITOR_WIDGET
#define TERRAINOSAURUS_MAP_EDITOR_WIDGET

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapView;
    
    // Pointer typedefs
    typedef shared_ptr<MapView>       MapViewPtr;
    typedef shared_ptr<MapView const> MapViewConstPtr;
};

// Import superclass definition
#include <inca/ui/View.hpp>

// Import Inca rendering subsystem
#include <inca/rendering.hpp>

// Import map definition
#include "Map.hpp"


class terrainosaurus::MapView : public inca::ui::View {
private:
    // Set up this class to own properties
    PROPERTY_OWNING_OBJECT(MapView);

public:
    // Import types from Map
    typedef Map::Point              Point;
    typedef Map::Vector             Vector;
    typedef Map::Intersection       Intersection;
    typedef Map::Region             Region;
    typedef Map::Boundary           Boundary;
    typedef Map::RefinedBoundary    RefinedBoundary;

    typedef inca::imaging::Color<float, inca::imaging::sRGB, true> Color;


/*---------------------------------------------------------------------------*
 | Constructor and properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor with optional component name
    MapView(const string &nm = "");

    // The map
    rw_ptr_property(Map, map, NULL);

    // The camera we're looking through
    rw_ptr_property(Camera, camera, NULL);

    // Rendering properties
    rw_list_property(Color, terrainColor);
    rw_property(Color, backgroundColor,         Color(0.1f, 0.1f, 0.1f, 1.0f));
    rw_property(Color, intersectionColor,       Color(1.0f, 1.0f, 1.0f, 1.0f));
    rw_property(Color, boundaryColor,           Color(0.0f, 0.5f, 0.5f, 1.0f));
    rw_property(Color, refinedBoundaryColor,    Color(0.3f, 0.3f, 1.0f, 1.0f));
    rw_property(Color, gridColor,               Color(0.8f, 0.8f, 0.8f, 0.5f));
    rw_property(Color, selectionColor,          Color(1.0f, 1.0f, 1.0f, 0.2f));

    rw_property(float, boundaryWidth,           4.0f);
    rw_property(float, refinedBoundaryWidth,    2.0f);
    rw_property(float, intersectionSize,        5.0f);

    // Rendering switches
    rw_property(bool, renderRegions,            true);
    rw_property(bool, renderIntersections,      true);
    rw_property(bool, renderBoundaries,         true);
    rw_property(bool, renderRefinedBoundaries,  true);
    rw_property(bool, renderGrid,               true);


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    void initializeView();
    void resizeView(Dimension d);
    void renderView();

protected:
    void renderMap(inca::rendering::RenderPass pass);
    void resetCameraProjection();

    bool isSelected(const Intersection &i) const;
    bool isSelected(const Region &r) const;
    bool isSelected(const Boundary &b) const;

    inca::rendering::OpenGLRenderer renderer; // Draw, my pretty!
};

#endif
