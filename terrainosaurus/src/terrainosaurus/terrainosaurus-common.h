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

// We want to use Inca preprocessor macros w/o the annoying INCA_ prefix
#define INCA_DROP_MACRO_PREFIX 1

// Import Inca sub-libraries
#include <inca/math.hpp>
#include <inca/raster.hpp>

namespace terrainosaurus {
    using namespace boost;
    using namespace inca;

    // Define scalar and vector types used for geometry
    INCA_MATH_SCALAR_TYPES(float, IS_NOT_WITHIN_TEMPLATE);
    INCA_MATH_LINALG_TYPES_CUSTOM(scalar_t, 2, /* */, 2D);
    INCA_MATH_LINALG_TYPES_CUSTOM(scalar_t, 3, /* */, 3D);
    typedef inca::math::Block<scalar_t, 2>  Block;

    // Define screenspace types
    typedef inca::math::Point<IndexType, 2>     Pixel;
    typedef inca::math::Vector<IndexType, 2>    Dimension;

    // Define array types
    typedef inca::Array<inca::SizeType, 2>      SizeArray;
    typedef inca::Array<inca::IndexType, 2>     IndexArray;

    // Define the Color type that will be used throughout
    INCA_MATH_COLOR_TYPES(float, sRGB, true);

    // Raster types
    typedef inca::raster::MultiArrayRaster<float, 2>    GrayscaleImage;
    typedef inca::raster::MultiArrayRaster<float, 3>    ScaleSpaceImage;
    typedef inca::raster::MultiArrayRaster<Vector2D, 2> VectorMap;
    typedef GrayscaleImage                              Heightfield;

};

// Import Boost smart-pointer utilities
#include <boost/enable_shared_from_this.hpp>

// Choose which GUI toolkit we will be using (for windows, etc.)
#define GUI_TOOLKIT GLUT


#endif
