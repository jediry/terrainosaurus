/*
 * File: ElevationModelHandler.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_ELEVATION_MODEL_HANDLER
#define TERRAINOSAURUS_ELEVATION_MODEL_HANDLER

// Import system configuration and global includes
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain engine
namespace terrainosaurus {
    // Forward declarations
    class ElevationModelHandler;

    // Pointer typedefs
    typedef shared_ptr<ElevationModelHandler> ElevationModelHandlerPtr;
};


// Import ElevationMap class definition
#include "ElevationMap.hpp"
using namespace terrainosaurus;


class terrainosaurus::ElevationModelHandler {
public:
    ElevationModelHandler
};
