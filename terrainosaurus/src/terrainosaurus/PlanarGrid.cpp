/*
 * File: PlanarGrid.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "PlanarGrid.hpp"
using namespace terrainosaurus;


// Snap a world-space, 2D point to the nearest grid intersection
PlanarGrid::Point PlanarGrid::snapToGrid(Point p) const {
    // Find the lower-bounding ticks in X and Y
    int minorTicksX = int(p[0] / minorTickSpacing()),
        minorTicksY = int(p[1] / minorTickSpacing());

    // Find the opposite (min and max) corners of the cell
    Point minBound((minorTicksX) * _minorTickSpacing,
                   (minorTicksY) * _minorTickSpacing);
    Point maxBound(minBound[0] + _minorTickSpacing,
                   minBound[1] + _minorTickSpacing);

    // Figure out which point 'p' is closer to along each axis
    Point snapped;
    for (IndexType i = 0; i < 2; ++i)
        snapped[i] = std::abs(minBound[i] - p[i]) < std::abs(maxBound[i] - p[i])
                        ? minBound[i]
                        : maxBound[i];
    return snapped;
}
