/*
 * File: terrainosaurus-common.h
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file acts as the central configuration point for the
 *      Terrainosaurus terrain generation/visualization engine. Its job is to
 *      include external libraries (e.g., C++ STL, Inca) that should be
 *      considered "global" to the Terrainosaurus source. It also may include
 *      Terrainosaurus-global definitions and constants.
 */
 
#ifndef TERRAINOSAURUS_COMMON
#define TERRAINOSAURUS_COMMON

// Import Inca library configuration
#include <inca/inca-common.h>

// Import Inca sub-libraries
#include <inca/math.hpp>
#include <inca/imaging.hpp>
#include <inca/world.hpp>
#include <inca/rendering.hpp>

#include <inca/ui/Framework.hpp>
#include <inca/ui/CameraControl.hpp>
#include <inca/integration/glut/GLUTFramework.hpp>

// Import namespaces
using namespace inca;
using namespace inca::math;
using namespace inca::imaging;
using namespace inca::world;
using namespace inca::rendering;
using namespace inca::ui;

#endif
