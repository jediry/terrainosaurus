/*
 * File: TranslateWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_TRANSLATE_WIDGET
#define TERRAINOSAURUS_TRANSLATE_WIDGET

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TranslateWidget;
    
    // Pointer typedefs
    typedef shared_ptr<TranslateWidget>       TranslateWidgetPtr;
    typedef shared_ptr<TranslateWidget const> TranslateWidgetConstPtr;
};

// Import superclass definition
#include "MapEditWidget.hpp"


class terrainosaurus::TranslateWidget : public MapEditWidget {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(TranslateWidget);


/*---------------------------------------------------------------------------*
 | Gadget part definitions
 *---------------------------------------------------------------------------*/
protected:
    // The parts of our graphical gadget the user is allowed to manipulate
    enum GadgetPart {
        NoPart,                     // No part of the gadget
        TranslateAlongXPart,        // Translation in only X
        TranslateAlongYPart,        // Translation in only Y
        TranslateAlongZPart,        // Translation in only Z
        TranslateInViewPlanePart,   // Translation in the plane orthogonal to
    };                              // the view direction


/*---------------------------------------------------------------------------*
 | Constructor & properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor with optional component name
    TranslateWidget(const string &nm = "");

    rw_property(Color,    xPartColor,            Color(1.0f, 0.0f, 0.0f, 0.5f));
    rw_property(Color,    yPartColor,            Color(0.0f, 1.0f, 0.0f, 0.5f));
    rw_property(Color,    zPartColor,            Color(0.0f, 0.0f, 1.0f, 0.5f));
    rw_property(Color,    planePartColor,        Color(0.8f, 0.8f, 0.0f, 0.5f));
    rw_property(Color,    activePartColor,       Color(1.0f, 1.0f, 1.0f, 0.5f));
    rw_property(Color,    transformingPartColor, Color(1.0f, 1.0f, 1.0f, 0.9f));
    rw_property(scalar_t, axisThickness,  1.0);   // Line width (pixels)
    rw_property(scalar_t, boxThickness,   1.0);   // Box line width (pixels)
    rw_property(scalar_t, axisLength,     10.0);  // Axis length (pixels)
    rw_property(scalar_t, boxRadius,      5.0);   // 1/2 box side length (pixels)

    // Widget event-handler functions
    void renderView();
    void keyPressed(KeyCode key, Pixel p);
//    void mouseDragged(Pixel p);

protected:
    template <bool forSelection> void renderGadget();

    bool transforming;      // Are we in the middle of doing a transformation?
    GadgetPart activePart;  // Which part is under the mouse and/or being used
};

#endif
