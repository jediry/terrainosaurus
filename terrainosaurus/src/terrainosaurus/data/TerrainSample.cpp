/*
 * File: TerrainSample.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */


// Import class definition
#include "TerrainSample.hpp"

// Import raster operations
#include <inca/raster/operators/selection>

// Import file I/O classes
#include <terrainosaurus/io/DEMInterpreter.hpp>

using namespace terrainosaurus;

//IndexType calculateLevelOfDetail(double res) {
#define TRIM 30     // How many pixels to trim from each side


// Constructor
TerrainSample::TerrainSample(const std::string & file)
    : filename(this, file), levelsOfDetail(this, 0), loaded(false),
      _heightfields(1) { }


// Overloaded assignment operator
TerrainSample & TerrainSample::operator=(const TerrainSample &ts) {
    _filename = ts.filename;
    loaded = ts.loaded;
    for (IndexType i = 0; i <= IndexType(_heightfields.size()); ++i) {
        _heightfields[i] = ts.heightfield(i);
    }

    return *this;
}


// Raster data accessors
const Heightfield::SizeArray & TerrainSample::size(IndexType lod) const {
    ensureLoaded();
    return _heightfields[lod].sizes();
}
const Heightfield & TerrainSample::heightfield(IndexType lod) const {
    ensureLoaded();
    return _heightfields[lod];
}
//const GradientMap & TerrainSample::gradient(IndexType lod) const;


// Perform on-demand loading and processing of the data
void TerrainSample::ensureLoaded() const {
    if (! loaded) {
        // Parse the file
        Heightfield temp;
        DEMInterpreter dem(temp);
        dem.filename = filename;
        dem.parse();

        // Figure out what LOD to store this as
//        IndexType lod = calculateLevelOfDetail(dem.resolution[0]);
        IndexType lod = 0; // XXX TODO!!

        // Trim it and store it to the right place
        Dimension size(temp.sizes());
        Pixel start(TRIM, TRIM),
              end(size - Dimension(TRIM));
        _heightfields[0] = select(temp, start, end);

        loaded = true;
    }
}
