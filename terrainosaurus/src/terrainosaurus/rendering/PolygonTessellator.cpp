/*
 * File: PolygonTessellator.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "PolygonTessellator.hpp"
using namespace terrainosaurus;

#if 0
/*---------------------------------------------------------------------------*
 | Constructor and properties
 *---------------------------------------------------------------------------*/
// Default constructor
PolygonTessellator::PolygonTessellator() {
    tessellator = gluNewTess();
    gluTessCallback(tobj, GLU_TESS_VERTEX,(GLvoid (CALLBACK*) ( )) &glVertex3dv);
    gluTessCallback(tobj, GLU_TESS_BEGIN,(GLvoid (CALLBACK*) ( )) &glBegin);
    gluTessCallback(tobj, GLU_TESS_END,(GLvoid (CALLBACK*) ( )) &glEnd);

    static_cast<GLUtessellator *>(tessellator)
}

// Destructor
PolygonTessellator::~PolygonTessellator() {
    // Clean up the tessellator object
    glutDeleteTess(static_cast<GLUtessellator *>(tessellator));
}
#endif

/*---------------------------------------------------------------------------*
 | Tessellation functions
 *---------------------------------------------------------------------------*/
