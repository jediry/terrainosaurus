/*
 * File: DEMInterpreter.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The DEM (Digital Elevation Model) format is a USGS (U.S. Geological
 *      Survey) standardized file format for storing gridded terrain elevation
 *      data. DEMs come in several flavors, with the most significant
 *      parameters being:
 *          horizontal resolution (****)
 *          vertical resolution & units (can be in meters or feet)
 *          map-projection type (UTM and Geodetic are the most common)
 *          zero-elevation reference datum
 *          horizontal reference datum
 *      The map-projection type is of particular importance to the resulting
 *      shape of the elevation-mapped area. Because of the annoyingly
 *      non-planar shape of the Earth, it is difficult (or perhaps
 *      impossible) to construct a surface -> grid mapping that works
 *      adequately for all parts of the Earth and at all scales.
 *
 *      Geodetic projection (latitude/longitude) handles this by sacrificing
 *      equdistance between points in order to cover the entire planet in
 *      a systematic way (thus inheriting all of the problems of a polar
 *      spherical coordinate system).
 *
 *      UTM (Universal Transverse Mercator) projection keeps grid points
 *      equidistant by dividing the Earth into zones (polar regions are not
 *      included) and establishing coordinates relative to a reference point
 *      for that zone. The resulting quadrilateral sections are not, in
 *      general, rectangular when chosen according to geodetic coordinates,
 *      and become less and less so as one moves away from the reference point.
 *      For this reason, UTM is only appropriate for large-scale (i.e.,
 *      closer to 1:1) maps.
 *
 *      WARNING: A significant consequence of UTM projection is that columns of
 *      elevation points ("profiles" in DEM parlance) may be of differing
 *      lengths. This is due to the grid axes not aligning with the
 *      quadrilateral boundaries -- think of a rectangular polygon being
 *      rotated 10 degrees with respect to the screen axes and then rasterized.
 *      Obviously, the scan lines for that polygon will be of different
 *      lengths. Currently, this is handled by simply using a regular,
 *      rectilinear grid and storing an "out-of-bounds" value in the regions
 *      outside of the boundary quadrilateral. This approach has implications
 *      for ROAM (and other terrain-visualization algorithms which expect a
 *      rectangular grid of points).
 */

#ifndef TERRAINOSAURUS_DEM_INTERPRETER
#define TERRAINOSAURUS_DEM_INTERPRETER

// Import system configuration and global includes
#include "terrainosaurus-common.h"


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class DEMInterpreter;
};

#include "ElevationMap.hpp"

class terrainosaurus::DEMInterpreter {
private:
    typedef DEMInterpreter ThisType;
public:
    // Template typedefs
    typedef ElevationMap        Handler;
    typedef shared_ptr<Handler> HandlerPtr;

    static const int VOID = -32767;

/*---------------------------------------------------------------------------*
 | Constructors & properties
 *---------------------------------------------------------------------------*/
public:
    // Constructor
    DEMInterpreter(HandlerPtr h);

    // Destructor
    ~DEMInterpreter();

    rw_ptr_property(Handler, handler, /* */);
    rw_property(string, filename, "");

    // Parse the file
    void parse();

protected:
    // These return the # of CHUNK_SIZE blocks consumed by the parsed record
    void parseRecordTypeA();
    void parseRecordTypeB(index_t r, index_t c);
    void parseRecordTypeC();

    void warn(const string &msg) { cerr << msg << '\n'; }


/*---------------------------------------------------------------------------*
 | Retained fields from the DEM file
 *---------------------------------------------------------------------------*/
protected:
    double deviationAngle;  // How far this grid deviates from horizontal
    double resolution[3];   // The X, Y, Z resolutions (world units per cell)
    double extents[2][2];   // The lower left and upper right corners of the
                            // full grid, in planimetric coordinates


/*---------------------------------------------------------------------------*
 | Low-level file I/O
 *---------------------------------------------------------------------------*/
protected:
    // Low-level functions to read data from the file
    size_t readBytes(size_t numBytes);
    bool readBoolean(size_t numBytes);
    int readInteger(size_t numBytes);
    double readDouble(size_t numBytes);
    char readChar();
    const char * readChars(size_t numBytes);

    static const size_t BUFFER_SIZE;    // Size of the char buffer
    static const size_t CHUNK_SIZE;
    scoped_array<char> buffer;          // ...the buffer.
    ifstream file;                      // The file we're reading
};

#endif
