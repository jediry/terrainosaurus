/*
 * File: MapSelection.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_MAP_SELECTION
#define TERRAINOSAURUS_MAP_SELECTION

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapSelection;
    
    // Pointer typedefs
    typedef shared_ptr<MapSelection>       MapSelectionPtr;
    typedef shared_ptr<MapSelection const> MapSelectionConstPtr;
};


class terrainosaurus::MapSelection {
};

#endif
