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

// Import application class
#include <terrainosaurus/MapExplorer.hpp>

// Import raster operations
#include <inca/raster/operators/selection>
#include <inca/raster/operators/gradient>
#include <inca/raster/operators/statistic>

// Import file I/O classes
#include <terrainosaurus/io/DEMInterpreter.hpp>

using namespace inca::raster;
using namespace terrainosaurus;

//IndexType calculateLevelOfDetail(double res) {
#define TRIM 30     // How many pixels to trim from each side


// Constructor
TerrainSample::TerrainSample(const std::string & file)
    : filename(this, file), levelsOfDetail(this, 0), loaded(false),
      _heightfields(1), _gradients(1), _averageGradients(1), _localRanges(1) { }


// Overloaded assignment operator
TerrainSample & TerrainSample::operator=(const TerrainSample &ts) {
    _filename = ts.filename;
    loaded = ts.loaded;
    for (IndexType i = 0; i <= IndexType(_heightfields.size()); ++i) {
        _heightfields[i]        = ts.heightfield(i);
        _gradients[i]           = ts.gradient(i);
        _averageGradients[i]    = ts.averageGradient(i);
        _localRanges[i]         = ts.localRange(i);
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
const VectorMap & TerrainSample::gradient(IndexType lod) const {
    ensureLoaded();
    return _gradients[lod];
}
const VectorMap & TerrainSample::averageGradient(IndexType lod) const {
    ensureLoaded();
    return _averageGradients[lod];
};
const VectorMap & TerrainSample::localRange(IndexType lod) const {
    ensureLoaded();
    return _localRanges[lod];
};


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

        // Calculate the gradient
        _gradients[0] = raster::gradient(_heightfields[0]);

        // XXX HACK Calculate the average gradient and local range
        int winSize = MapExplorer::instance().terrainLibrary()->windowSize(0);
        _averageGradients[0].resize(_heightfields[0].sizes());
        _localRanges[0].resize(_heightfields[0].sizes());
        Pixel px;
        for (px[0] = 0; px[0] < _gradients[0].size(0); ++px[0])
            for (px[1] = 0; px[1] < _gradients[0].size(1); ++px[1]) {
                start = Pixel(px[0] - winSize / 2, px[1] - winSize / 2);
                end = Pixel(start[0] + winSize, start[1] + winSize);
                _averageGradients[0](px) = mean(select(_gradients[0], start, end));
                _localRanges[0](px) = Vector2D(range(select(_heightfields[0], start, end)));
            }
        loaded = true;
    }
}
