/*
 * File: ModifyPropertyWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_MODIFY_PROPERTY_WIDGET
#define TERRAINOSAURUS_MODIFY_PROPERTY_WIDGET

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class ModifyPropertyWidget;
    
    // Pointer typedefs
    typedef shared_ptr<ModifyPropertyWidget>       ModifyPropertyWidgetPtr;
    typedef shared_ptr<ModifyPropertyWidget const> ModifyPropertyWidgetConstPtr;
};

// Import superclass definition
#include "MapEditWidget.hpp"


class terrainosaurus::ModifyPropertyWidget : public MapEditWidget {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(ModifyPropertyWidget);

public:
    // Default constructor with optional component name
    ModifyPropertyWidget(const string &nm = "");

    // Widget event-handler functions
    void keyPressed(KeyCode key, Pixel p);
    
protected:
    // High-level functions (called from event-handlers)
    void refineSelectedEdges(bool refined);
    void adjustSelectedEdgesGAIterations(int deltaCycles);
    void setSelectedFacesTerrainType(unsigned int tt);
    void setSelectedFacesTerrainType(TerrainTypePtr tt);
};

#endif