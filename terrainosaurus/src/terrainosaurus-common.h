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

// Import Boost libraries XXX should these go into inca-common?
#include <boost/lambda/lambda.hpp>

namespace terrainosaurus {
    using namespace boost;

    // Import Inca library namespaces
    using namespace inca;
    using namespace inca::math;

    // Define scalar and vector types used for geometry
    INCA_MATH_SCALAR_TYPEDEFS(double, IS_NOT_WITHIN_TEMPLATE, /* */, /* */);
    INCA_MATH_VECTOR_TYPEDEFS(scalar_t, 2, /* */, 2D);
    INCA_MATH_VECTOR_TYPEDEFS(scalar_t, 3, /* */, 3D);

    // Define the Color type that will be used throughout
    typedef inca::imaging::Color<float, inca::imaging::sRGB, true> Color;
};

// Choose which GUI toolkit we will be using (for windows, etc.)
#define GUI_TOOLKIT GLUT


#endif