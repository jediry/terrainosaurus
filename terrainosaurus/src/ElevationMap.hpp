/*
 * File: ElevationMap.hpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      The ElevationMap class is an object wrapper around a
 *      "heightfield", a 3D object based on a 2D data set giving
 *      the height at regular intervals.
 */

#ifndef TERRAINOSAURUS_ELEVATION_MAP
#define TERRAINOSAURUS_ELEVATION_MAP

// Import system configuration and global includes
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain engine
namespace Terrainosaurus {
    // Forward declarations
    class ElevationMap;

    // Pointer typedefs
    typedef shared_ptr<ElevationMap> ElevationMapPtr;
};


class Terrainosaurus::ElevationMap : public Object {
public:
    // Type definitions
    typedef shared_array<double>        ElevationArray;
    typedef Polygon<ScalarDouble, 2>    Polygon;
    typedef Polygon::Point              Point2D;


    // Special sentinel values
    static const double OUT_OF_BOUNDS;
    static const double VOID;


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    ElevationMap();


/*---------------------------------------------------------------------------*
 | Grid cell accessors
 *---------------------------------------------------------------------------*/
public:
    // Grid cell allocation
    void setGridSize(size_t r, size_t c) {
        _size = r * c;
        _rows = r;
        _columns = c;
        elevationSamples = ElevationArray(new double[_size]);
    }

    // Height data accessors
    size_t size() const { return _size; }
    size_t rows() const { return _rows; }
    size_t columns() const { return _columns; }
    double operator[](index_t index) const { return elevationSamples[index]; }
    double & operator[](index_t index)     { return elevationSamples[index]; }
    double operator()(index_t r, index_t c) const {
        return elevationSamples[indexOf(r, c)];
    }
    double & operator()(index_t r, index_t c) {
        return elevationSamples[indexOf(r, c)];
    }

protected:
    // Translate a 2D access into a 1D access
    index_t indexOf(index_t r, index_t c) const {
        return r * _columns + c;
    }

    size_t _size, _rows, _columns;
    ElevationArray elevationSamples;  // The array of elevation points


/*---------------------------------------------------------------------------*
 | Trimming polygon
 *---------------------------------------------------------------------------*/
public:
    property_rw(Polygon, trimmingPolygon, Polygon());


/*---------------------------------------------------------------------------*
 | Relationship to the "real world"
 *---------------------------------------------------------------------------*/
public:
    property_rw(double, xResolution, 1.0);
    property_rw(double, yResolution, 1.0);
    property_rw(double, zResolution, 1.0);
    property_rw(double, deviationAngle, 0.0);


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    void updateTessellation(const Point &viewPoint, const Vector &look);
};


#if 0
    // Transformation operations
    void normalize(double min, double max);     // Scale to fit range
    void scaleHeight(double s);                 // Scale height by factor

    // Property accessors
    double minimum() const { return minH; }
    double maximum() const { return maxH; }
#endif


#endif
