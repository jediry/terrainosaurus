/*
 * File: PolygonTessellator.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_POLYGON_TESSELLATOR
#define TERRAINOSAURUS_POLYGON_TESSELLATOR

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class PolygonTessellator;
    
    // Pointer typedefs
    typedef shared_ptr<PolygonTessellator>       PolygonTessellatorPtr;
    typedef shared_ptr<PolygonTessellator const> PolygonTessellatorConstPtr;
};

class terrainosaurus::PolygonTessellator {
/*---------------------------------------------------------------------------*
 | Constructor and properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor
    PolygonTessellator();

    // Destructor
    ~PolygonTessellator();

protected:
    void * tessellator;


/*---------------------------------------------------------------------------*
 | Tessellation functions
 *---------------------------------------------------------------------------*/
public:
    template <class Iterator>
    void tessellate(Iterator start, Iterator end);
};

#endif