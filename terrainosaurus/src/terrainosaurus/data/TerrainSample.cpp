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


// Constructor taking a filename
TerrainSample::TerrainSample(const std::string & file)
    : filename(this, file), levelsOfDetail(this),
      _loaded(false), _analyzed(false) { }

// Constructor taking a heightfield
TerrainSample::TerrainSample(const Heightfield & hf)
    : filename(this), levelsOfDetail(this),
      _loaded(true), _analyzed(false) {

    // Stick this in as our first LOD heightfield
    _elevations.push_back(hf);
}


// Overloaded assignment operator
TerrainSample & TerrainSample::operator=(const TerrainSample &ts) {
    // Copy source/status properties
    _filename = ts._filename;
    _loaded   = ts._loaded;
    _analyzed = ts._analyzed;

    // We only have to copy these other things if they're valid
    if (loaded()) {
        // Copy the original heightfield(s)
        _elevations = ts._elevations;
    }
    if (analyzed()) {
        // Copy the per-cell gradients
        _gradients  = ts._gradients;

        // Copy local heightfield properties
        _localElevationMeans = ts._localElevationMeans;
        _localGradientMeans  = ts._localGradientMeans;
        _localElevationRanges         = ts._localElevationRanges;

        // Copy global heightfield properties
        _globalElevationMeans = ts._globalElevationMeans;
        _globalGradientMeans  = ts._globalGradientMeans;
        _globalElevationRanges         = ts._globalElevationRanges;
    }

    return *this;
}


// Lazy evaluation status
bool TerrainSample::loaded() const { return _loaded; }
bool TerrainSample::analyzed() const { return _analyzed; }


// LOD property accessors
const Heightfield::SizeArray & TerrainSample::sizes(IndexType lod) const {
    ensureLoaded();
    return _elevations[lod].sizes();
}


// Per-cell heightfield property accessors
const Heightfield & TerrainSample::elevation(IndexType lod) const {
    ensureLoaded();
    return _elevations[lod];
}
const VectorMap & TerrainSample::gradient(IndexType lod) const {
    ensureAnalyzed();
    return _gradients[lod];
}

// Local (windowed) heightfield property accessors
const Heightfield & TerrainSample::localElevationMean(IndexType lod) const {
    ensureAnalyzed();
    return _localElevationMeans[lod];
};
const VectorMap & TerrainSample::localGradientMean(IndexType lod) const {
    ensureAnalyzed();
    return _localGradientMeans[lod];
};
const VectorMap & TerrainSample::localElevationRange(IndexType lod) const {
    ensureAnalyzed();
    return _localElevationRanges[lod];
};


// Global heightfield property accessors
const Heightfield::ElementType & TerrainSample::globalElevationMean(IndexType lod) const {
    ensureAnalyzed();
    return _globalElevationMeans[lod];
}
const VectorMap::ElementType & TerrainSample::globalGradientMean(IndexType lod) const {
    ensureAnalyzed();
    return _globalGradientMeans[lod];
}
const VectorMap::ElementType & TerrainSample::globalElevationRange(IndexType lod) const {
    ensureAnalyzed();
    return _globalElevationRanges[lod];
}


// Perform on-demand loading of the elevation data
void TerrainSample::ensureLoaded() const {
    if (! loaded()) {
        std::cerr << "Loading terrain sample from " << filename() << "...";

        // Parse the file
        Heightfield temp;
        DEMInterpreter dem(temp);
        dem.filename = filename;
        dem.parse();

        // Create the appropriate LODs
        _elevations.resize(levelsOfDetail);
        _gradients.resize(levelsOfDetail);
        _localElevationMeans.resize(levelsOfDetail);
        _localGradientMeans.resize(levelsOfDetail);
        _localElevationRanges.resize(levelsOfDetail);
        _globalElevationMeans.resize(levelsOfDetail);
        _globalGradientMeans.resize(levelsOfDetail);
        _globalElevationRanges.resize(levelsOfDetail);

        // Figure out what LOD to store this as
//        IndexType lod = calculateLevelOfDetail(dem.resolution[0]);
        IndexType lod = 0; // XXX TODO!!

        // Trim it and store it to the right place
        Dimension size(temp.sizes());
        Pixel start(TRIM, TRIM),
              end(size - Dimension(TRIM));
        _elevations[lod] = select(temp, start, end);

        _loaded = true;
        std::cerr << "done\n";
    }
}


// Perform on-demand analysis of the elevation data
void TerrainSample::ensureAnalyzed() const {
    if (! analyzed()) {
        IndexType lod = 0;
        std::cerr << "Analyzing LOD " << lod << " from " << filename() << "...";

        // Calculate the per-cell gradient
        _gradients[lod] = raster::gradient(_elevations[lod]);

        // XXX HACK Calculate the local mean elevation, mean gradient and range
        SizeType windowSize = MapExplorer::instance().terrainLibrary()->windowSize(lod);
        Heightfield::SizeArray lodSize = this->sizes(lod);

        _localElevationMeans[lod].resize(lodSize);
        _localGradientMeans[lod].resize(lodSize);
        _localElevationRanges[lod].resize(lodSize);
        Pixel px, start, end;
        for (px[0] = 0; px[0] < lodSize[0]; ++px[0])
            for (px[1] = 0; px[1] < lodSize[1]; ++px[1]) {
                start = Pixel(px[0] - windowSize / 2, px[1] - windowSize / 2);
                end = Pixel(start[0] + windowSize, start[1] + windowSize);
                _localElevationMeans[lod](px) = mean(select(_elevations[lod], start, end));
                _localGradientMeans[lod](px) = mean(select(_gradients[lod], start, end));
                _localElevationRanges[lod](px) = Vector2D(range(select(_elevations[lod], start, end)));
            }

        // Calculate the global mean elevation, mean gradient, and range
        _globalElevationMeans[lod]  = mean(_elevations[lod]);
        _globalGradientMeans[lod]   = mean(_gradients[lod]);
        _globalElevationRanges[lod] = VectorMap::ElementType(range(_elevations[lod]));

        _analyzed = true;
        std::cerr << "done\n";
    }
}
