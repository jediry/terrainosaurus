/*
 * File: MapExplorerWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_MAP_EXPLORER_WIDGET
#define TERRAINOSAURUS_MAP_EXPLORER_WIDGET

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapExplorerWidget;

    // Pointer typedefs
    typedef shared_ptr<MapExplorerWidget>       MapExplorerWidgetPtr;
    typedef shared_ptr<MapExplorerWidget const> MapExplorerWidgetConstPtr;
};

// Import superclass definitions
#include <inca/ui/MultiplexorWidget.hpp>
#include "../TerrainosaurusComponent.hpp"

// Import terrain-specialized renderer
#include "../TerrainosaurusRenderer.hpp"

// Import map definition
#include "../data/Map.hpp"


class terrainosaurus::MapExplorerWidget : public inca::ui::MultiplexorWidget,
                                          protected TerrainosaurusComponent {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(MapExplorerWidget);


/*---------------------------------------------------------------------------*
 | Constructor & properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor with optional component name
    MapExplorerWidget(const std::string & nm = "");

    // The renderer we use for drawing
    rw_ptr_property(TerrainosaurusRenderer, renderer, NULL);

    // UI parameters
    rw_property_custom_set(Color, backgroundColor, Color(0.1f, 0.1f, 0.1f, 1.0f));
        void property_set(Color, backgroundColor);

    // XXX tools
    rw_ptr_property(MultiplexorWidget, toolSelect2D, NULL);

    // Widget event-handler functions
    void initializeView();
    void resizeView(Dimension d);
    void renderView();
    void keyPressed(inca::ui::KeyCode key, Pixel p);
};

#endif
