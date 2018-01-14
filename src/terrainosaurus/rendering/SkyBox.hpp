/*
 * File: SkyBox.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      FIXME: This class needs a different name. This is sorta just a hack until I figure out the "right" way to do this.
 */

#ifndef TERRAINOSAURUS_RENDERING_SKY_BOX
#define TERRAINOSAURUS_RENDERING_SKY_BOX

// Import Inca library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class SkyBox;
};

// Import rendering primitive definitions
#include <inca/rendering.hpp>
#include <inca/rendering/immediate-mode/PrimitiveArray>


class terrainosaurus::SkyBox {
public:
    typedef inca::rendering::OpenGLRenderer Renderer;
    typedef inca::rendering::PrimitiveArray<Point3D,
                                            Nothing,
                                            Point2D>    PrimitiveArray;


/*---------------------------------------------------------------------------*
 | Constructors & data accessors
 *---------------------------------------------------------------------------*/
public:
    // Default constructor
    SkyBox();


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    // Rendering feature toggles
    bool toggle(const std::string & feature);

    // Render function call operator
    void operator()(Renderer & renderer) const;

protected:
    mutable bool _displayListValid;
    mutable int _displayList;
};

#endif
