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
#include "terrainosaurus-common.h"

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

public:
    // Default constructor with optional component name
    TranslateWidget(const string &nm = "");

    // Widget event-handler functions
//    void renderView() { }
//    void keyPressed(KeyCode key, Point p);
//    void mouseDragged();
//    void mouseTracked();
};

#endif
