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
using namespace inca;

// Import Inca sub-libraries
#include <inca/math.hpp>
using namespace inca::math;

#include <inca/imaging.hpp>
using namespace inca::imaging;

#include <inca/world.hpp>
using namespace inca::world;

#endif
