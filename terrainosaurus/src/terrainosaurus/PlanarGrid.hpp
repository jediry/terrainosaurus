/*
 * File: PlanarGrid.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_PLANAR_GRID
#define TERRAINOSAURUS_PLANAR_GRID

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class PlanarGrid;

    // Pointer typedefs
    typedef shared_ptr<PlanarGrid>       PlanarGridPtr;
    typedef shared_ptr<PlanarGrid const> PlanarGridConstPtr;
};


class terrainosaurus::PlanarGrid {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(PlanarGrid);

/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // Import geometric type definitions from the namespace
    typedef ::terrainosaurus::scalar_t     scalar_t;
    typedef ::terrainosaurus::scalar_arg_t scalar_arg_t;
    typedef Point2D                        Point;
    typedef Vector2D                       Vector;


/*---------------------------------------------------------------------------*
 | Constructors & properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor
    explicit PlanarGrid()
        : extents(this), majorTickSpacing(this), minorTickSpacing(this) { }

    // Grid extents in each dimension
    rw_property(Vector, extents, Vector(20.0f, 20.0f));

    // How far apart the major and minor grid lines are
    rw_property(scalar_t, majorTickSpacing, 1.0f);
    rw_property(scalar_t, minorTickSpacing, 0.1f);

    // Snap a world-space, 2D point to the nearest grid intersection
    Point snapToGrid(Point p) const;
};

#endif
