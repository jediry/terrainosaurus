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

// HACK -- should be in math
namespace std {
    inline terrainosaurus::Vector2D pow(const terrainosaurus::Vector2D & v, float f) {
        return terrainosaurus::Vector2D(std::pow(v[0], f), std::pow(v[1], f));
    }
}

// Import application class
//#include <terrainosaurus/MapExplorer.hpp>

// Import raster operations
#include <inca/raster/algorithms/scale_space_project>
#include <inca/raster/algorithms/find_edges>
#include <inca/raster/algorithms/find_ridges>
#include <inca/raster/operators/select>
#include <inca/raster/operators/linear_map>
#include <inca/raster/generators/linear_fade>
#include <inca/raster/operators/magnitude>
#include <inca/raster/operators/gradient>
#include <inca/raster/operators/resample>
#include <inca/raster/operators/statistic>

// Import file I/O classes
#include <terrainosaurus/io/DEMInterpreter.hpp>

using namespace std;
using namespace inca::raster;
using namespace terrainosaurus;

typedef TerrainSample::LOD::SizeArray           SizeArray;
typedef TerrainSample::LOD::FrequencySpectrum   FrequencySpectrum;
typedef TerrainSample::LOD::ScalarStatistics    ScalarStatistics;
typedef TerrainSample::LOD::VectorStatistics    VectorStatistics;


#define TRIM 30     // How many pixels to trim from each side

// Timer!
#include <inca/util/Timer>

// HACK
#include <sys/stat.h>


CurveTracker::CurveTracker(GrayscaleImage * img, std::vector<scalar_t> sc)
    : image(img), scales(sc), atScale(sc.size()) { }


void CurveTracker::begin() {
    curves.push_back(Curve(scales.size()));
}

void CurveTracker::add(const Point & p) {
    // Increment per-curve quantities
    Curve & c = curves.back();
    c.lengthStats(1.0f);
    c.strengthStats(p[3]);
    c.scaleStats(scales[int(p[2])]);
    c.atScale[int(p[2])]++;
    c.points.push_back(p);

    // Draw it on the image
    (*image)(p) = 1.0f;
}

void CurveTracker::end() {
    // Calculate per-curve statistics
    Curve & c = curves.back();
    c.lengthStats.done();
    c.scaleStats.done();
    c.strengthStats.done();

    std::vector<Point>::const_iterator it;
    for (it = c.points.begin(); it != c.points.end(); ++it) {
        const Point & p = *it;
        c.scaleStats(scales[int(p[2])]);
        c.strengthStats(p[3]);
    }

    c.lengthStats.done();
    c.scaleStats.done();
    c.strengthStats.done();

    // Add this curve's quantities into the global stats
    lengthStats(c.lengthStats.sum());
    scaleStats(c.scaleStats.mean());
    strengthStats(c.strengthStats.mean());
    for (int i = 0; i < scales.size(); ++i)
        atScale[i] += c.atScale[i];
}

void CurveTracker::done() {
    // Calculate mean and variance
    lengthStats.done();
    scaleStats.done();
    strengthStats.done();
    for (int i = 0; i < curves.size(); ++i) {
        lengthStats(curves[i].lengthStats.sum());
        scaleStats(curves[i].scaleStats.mean());
        strengthStats(curves[i].strengthStats.mean());
    }
    lengthStats.done();
    scaleStats.done();
    strengthStats.done();

//    std::cerr << "Size of this thing is " << sizeof(CurveTracker) << " and the vectors have " << scales.size() << ", " << atScale.size() << "and " << std::endl;
//    std::cerr << "There are " << curves.size() << " curves of size " << sizeof(Curve) << " and their vectors are : \n";
//    for (int i = 0; i < curves.size(); ++i)
//        std::cerr << i << ": " << curves[i].points.size() << " and " << curves[i].atScale.size() << endl;
}

void CurveTracker::print(std::ostream & os) {
    os << curves.size() << " curves" << endl;
    for (int i = 0; i < atScale.size(); ++i)
        os << "\tScale " << i << ": " << atScale[i] << endl;

    os << "Length:" << endl
        << "\tSum     " << lengthStats.sum() << endl
        << "\tMinimum " << lengthStats.min() << endl
        << "\tMaximum " << lengthStats.max() << endl
        << "\tMean    " << lengthStats.mean() << endl
        << "\tStdDev  " << lengthStats.variance() << endl;

    os << "Scale:" << endl
        << "\tSum     " << scaleStats.sum() << endl
        << "\tMinimum " << scaleStats.min() << endl
        << "\tMaximum " << scaleStats.max() << endl
        << "\tMean    " << scaleStats.mean() << endl
        << "\tStdDev  " << scaleStats.variance() << endl;

    os << "Strength:" << endl
        << "\tSum     " << strengthStats.sum() << endl
        << "\tMinimum " << strengthStats.min() << endl
        << "\tMaximum " << strengthStats.max() << endl
        << "\tMean    " << strengthStats.mean() << endl
        << "\tStdDev  " << strengthStats.variance() << endl;
}

// FIXME: This should probably go elsewhere
void write(std::ostream &, const CurveTracker & c) {

}
void read(std::istream &, CurveTracker & c) {

}


/*****************************************************************************
 * LOD specialization for TerrainSample
 *****************************************************************************/
 // Default constructor
TerrainSample::LOD::LOD()
    : LODBase<TerrainSample>() { }

// Constructor linking back to TerrainSample
TerrainSample::LOD::LOD(TerrainSamplePtr ts, TerrainLOD lod)
    : LODBase<TerrainSample>(ts, lod) { }


// Access to related LOD objects
TerrainType::LOD & TerrainSample::LOD::terrainType() {
    return (*object().terrainType())[levelOfDetail()];
}
const TerrainType::LOD & TerrainSample::LOD::terrainType() const {
    return (*object().terrainType())[levelOfDetail()];
}


// Initialization & analysis of elevation data
void TerrainSample::LOD::createFromRaster(const Heightfield & hf) {
    _elevations = hf;
    _loaded   = true;
    _analyzed = false;
}
void TerrainSample::LOD::loadFromFile(const std::string & filename) {
    std::cerr << "Loading TerrainSample<" << name() << "> from \""
              << filename << '"' << std::endl;

    // The file should scream bloody murder if something goes wrong
    std::ifstream file;
    file.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
    try {
        // Open the file
        file.open(filename.c_str(), std::ios::in | std::ios::binary);

        // Read the magic header, to make sure this file is what we think it is
        // If the header is not present, we print a warning and try anyway.
        char header[15];
        file.get(header, 15);
        if (std::string("Terrainosaurus") != header)
            std::cerr << filename << " does not have a correct magic header. "
                         "Are you sure this is a cache file?\n";

        // Read the LOD and dimensions
        TerrainLOD lod;
        SizeArray sz;
        file.read((char*)&lod, sizeof(TerrainLOD));
        file.read((char*)&sz, sizeof(SizeArray));

        // Warn if this doesn't seem like the right LOD
        if (lod != levelOfDetail())
            std::cerr << "Uh oh...\"" << filename << "\" contains " << lod
                      << ", not expected " << levelOfDetail() << std::endl;

        // Resize to be the right dimensions
        setSizes(sz);

        // Read in each of the rasters
        file.read((char*)_elevations.elements(),
                  size() * sizeof(Heightfield::ElementType));
        file.read((char*)_gradients.elements(),
                  size() * sizeof(VectorMap::ElementType));
        file.read((char*)_localElevationMeans.elements(),
                  size() * sizeof(Heightfield::ElementType));
        file.read((char*)_localGradientMeans.elements(),
                  size() * sizeof(VectorMap::ElementType));
        file.read((char*)_localElevationRanges.elements(),
                  size() * sizeof(VectorMap::ElementType));
        file.read((char*)_localSlopeRanges.elements(),
                  size() * sizeof(VectorMap::ElementType));

        // Write out each of the non-raster measurements
        file.read((char*)&_frequencySpectrum,          sizeof(FrequencySpectrum));
        file.read((char*)&_globalElevationStatistics,  sizeof(ScalarStatistics));
        file.read((char*)&_globalSlopeStatistics,      sizeof(ScalarStatistics));
        file.read((char*)&_globalGradientStatistics,   sizeof(VectorStatistics));

        // Write out each of the feature sets
        read(file, _edges);
        read(file, _ridges);

        // We should now be at the end of the file
        std::cerr << "Stream pointer is " << file.tellg() << std::endl;

        // Close the file
        file.close();

        // Nothing more to do here...
        _loaded = true;
        _analyzed = true;

    } catch (std::exception & e) {
        std::cerr << "Error reading \"" << filename << '"' << std::endl;
        if (file) file.close();
    }
}
void TerrainSample::LOD::storeToFile(const std::string & filename) const {
    // Make sure we have stuff to store first
    ensureAnalyzed();

    std::cerr << "Storing TerrainSample<" << name() << "> to \""
              << filename << '"' << std::endl;

    // The file should scream bloody murder if something goes wrong
    std::ofstream file;
    file.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
    try {
        // Open the file
        file.open(filename.c_str(), std::ios::out | std::ios::binary);

        // Write the magic header string to the file
        file << "Terrainosaurus";

        // Write the LOD and sizes of this TerrainSample
        TerrainLOD lod = levelOfDetail();
        SizeArray sz = sizes();
        file.write((char*)&lod, sizeof(TerrainLOD));
        file.write((char*)&sz, sizeof(SizeArray));

        // Write out each of the rasters
        file.write((char*)_elevations.elements(),
                   size() * sizeof(Heightfield::ElementType));
        file.write((char*)_gradients.elements(),
                   size() * sizeof(VectorMap::ElementType));
        file.write((char*)_localElevationMeans.elements(),
                   size() * sizeof(Heightfield::ElementType));
        file.write((char*)_localGradientMeans.elements(),
                   size() * sizeof(VectorMap::ElementType));
        file.write((char*)_localElevationRanges.elements(),
                   size() * sizeof(VectorMap::ElementType));
        file.write((char*)_localSlopeRanges.elements(),
                   size() * sizeof(VectorMap::ElementType));

        // Write out each of the non-raster measurements
        file.write((char*)&_frequencySpectrum,          sizeof(FrequencySpectrum));
        file.write((char*)&_globalElevationStatistics,  sizeof(ScalarStatistics));
        file.write((char*)&_globalSlopeStatistics,      sizeof(ScalarStatistics));
        file.write((char*)&_globalGradientStatistics,   sizeof(VectorStatistics));

        // Write out each of the feature sets
        write(file, _edges);
        write(file, _ridges);

        std::cerr << "Stream pointer is " << file.tellp() << std::endl;

        // Close the file, now that we're done with it
        file.close();

    } catch (std::exception & e) {
        std::cerr << "Error writing \"" << filename << '"' << std::endl;
        if (file) file.close();
    }
}
void TerrainSample::LOD::resampleFromLOD(TerrainLOD lod) {
    _elevations = resample(object()[lod].elevations(),
                           scaleFactor(lod, levelOfDetail()));
    _loaded = true;
    _analyzed = false;
}
void TerrainSample::LOD::analyze() {
    std::cerr << "Analyzing TerrainSample<" << name() << '>' << std::endl;
    Timer<float, false> total, phase;
    total.start();

    std::stringstream report;
    report << "Analasis of TerrainSample<" << name() << '>';
    if (object().filename() != "")
        report << " from " << object().filename();
    report << "...\n";

    // Calculate the per-cell gradient
    report << "\tCalculating gradient...";
    phase.start();
    _gradients = raster::gradient(_elevations, metersPerSampleForLOD(levelOfDetail()));
    phase.stop();
    report << phase() << " seconds\n";

    report << "\tCalculating gradient magnitude...";
    phase.reset(); phase.start();
    Heightfield gradientMag = raster::magnitude(_gradients);
    phase.stop();
    report << phase() << " seconds\n";

    // XXX HACK Calculate the local mean elevation, mean gradient and range
    SizeType halfWin = windowSize(levelOfDetail()) / 2;
    SizeArray windowSizes(windowSize(levelOfDetail()));
    Pixel px, start;

    report << "\tCalculating local elevation means...";
    phase.reset(); phase.start();
    _localElevationMeans.setSizes(sizes());
    for (px[0] = 0; px[0] < size(0); ++px[0])
        for (px[1] = 0; px[1] < size(1); ++px[1]) {
            start = Pixel(px[0] - halfWin, px[1] - halfWin);
            _localElevationMeans(px) = mean(selectBS(_elevations, start, windowSizes));
            //std::cerr << "meaning " << selectBS(_elevations, start, windowSizes).size() << " elements\n";
            //std::cerr << start << ", " << windowSizes << " -> " << selectBS(_elevations, start, windowSizes).bounds() << std::endl;
        }
    phase.stop();
    report << phase() << " seconds\n";

    report << "\tCalculating local gradient means...";
    phase.reset(); phase.start();
    _localGradientMeans.setSizes(sizes());
    for (px[0] = 0; px[0] < size(0); ++px[0])
        for (px[1] = 0; px[1] < size(1); ++px[1]) {
            start = Pixel(px[0] - halfWin, px[1] - halfWin);
            _localGradientMeans(px) = mean(selectBS(_gradients, start, windowSizes));
        }
    phase.stop();
    report << phase() << " seconds\n";

    report << "\tCalculating local elevation ranges...";
    phase.reset(); phase.start();
    _localElevationRanges.setSizes(sizes());
    for (px[0] = 0; px[0] < size(0); ++px[0])
        for (px[1] = 0; px[1] < size(1); ++px[1]) {
            start = Pixel(px[0] - halfWin, px[1] - halfWin);
            _localElevationRanges(px) = Vector2D(range(selectBS(_elevations, start, windowSizes)));
        }
    phase.stop();
    report << phase() << " seconds\n";

    report << "\tCalculating local slope ranges...";
    phase.reset(); phase.start();
    _localSlopeRanges.setSizes(sizes());
    for (px[0] = 0; px[0] < size(0); ++px[0])
        for (px[1] = 0; px[1] < size(1); ++px[1]) {
            start = Pixel(px[0] - halfWin, px[1] - halfWin);
            _localSlopeRanges(px)   = Vector2D(range(selectBS(gradientMag, start, windowSizes)));
        }
    phase.stop();
    report << phase() << " seconds\n";

    // Calculate the global mean elevation, mean gradient, and range
    report << "\tCalculating global elevation statistics...";
    phase.reset(); phase.start();
    _globalElevationStatistics = apply(ScalarStatistics(), _elevations);
    _globalElevationStatistics.done();
    _globalElevationStatistics = apply(_globalElevationStatistics, _elevations);
    _globalElevationStatistics.done();
    phase.stop();
    report << phase() << " seconds\n";
    report << "\tCalculating global slope statistics...";
    phase.reset(); phase.start();
    _globalSlopeStatistics = apply(ScalarStatistics(), gradientMag);
    _globalSlopeStatistics.done();
    _globalSlopeStatistics = apply(_globalSlopeStatistics, gradientMag);
    _globalSlopeStatistics.done();
    phase.stop();
    report << phase() << " seconds\n";
//    report << "\tCalculating global gradient statistics...";
//    phase.reset(); phase.start();
//    _globalGradientStatistics = apply(VectorStatistics(), _gradients);
//    _globalGradientStatistics.done();
//    _globalGradientStatistics = apply(_globalGradientStatistics[lod], _gradients);
//    _globalGradientStatistics.done();
//    phase.stop();
//    report << phase() << "seconds\n";

    // Determine the frequency content of the heightfield
    report << "\tCalculating frequency content...";
    phase.reset(); phase.start();
    scalar_t period = 30.0f;                    // Meters per sample
    scalar_t nyquist = 1.0f / period;
    scalar_t maxFreq = nyquist / 2.0f;    // Samples per meter
    Heightfield fftMag = dcToCenter(cmagnitude(dft(_elevations)))
                         / std::sqrt(scalar_t(size()));
//        cerr << "Period is " << period << endl;
//        cerr << "Nyquist is " << nyquist << endl;
//        cerr << "Max freq is " << maxFreq << endl;
    for (px[0] = fftMag.base(0); px[0] <= fftMag.extent(0); ++px[0])
        for (px[1] = fftMag.base(1); px[1] <= fftMag.extent(1); ++px[1]) {
            scalar_t freqX = scalar_t(px[0]) / fftMag.size(0) * nyquist;
            scalar_t freqY = scalar_t(px[1]) / fftMag.size(1) * nyquist;
            scalar_t freq = std::sqrt(freqX*freqX + freqY*freqY);
            int bucket = int(freq / maxFreq * frequencyBands);
            if (bucket < _frequencySpectrum.size())
                _frequencySpectrum[bucket] += fftMag(px);
        }
    phase.stop();
    report << phase() << " seconds\n";

    // Create the scale-space representation of the heightfield
    std::vector<scalar_t> scales;
    scales.push_back(0.0f);
    scales.push_back(1.0f);
//    scales.push_back(1.5f);
    scales.push_back(2.0f);
//    scales.push_back(3.0f);
    scales.push_back(4.0f);
//    scales.push_back(6.0f);
    scales.push_back(8.0f);
//    scales.push_back(12.0f);
    scales.push_back(16.0f);
//    scales.push_back(24.0f);
    scales.push_back(32.0f);
//    scales.push_back(48.0f);
    scales.push_back(64.0f);
//    scales.push_back(96.0f);
    scales.push_back(128.0f);
//    scales.push_back(192.0f);
    scales.push_back(256.0f);

    report << "\tCreating scale space projection (" << scales.size() << " scales)...";
    typedef MultiArrayRaster<float, 3> HyperImage;
    HyperImage hyper;
    phase.reset(); phase.start();
    scale_space_project(hyper, elevations(), scales);
    phase.stop();
    report << phase() << " seconds\n";

    // Analyze the edges in the heightfield
    report << "\tFinding edges...";
    phase.reset(); phase.start();
//    edgeImage.setSizes(sizes());
//    fill(edgeImage, 0.0f);
//    edges = inca::raster::find_edges(hyper, scales, CurveTracker(&edgeImage, scales));
    phase.stop();
    report << phase() << " seconds\n";

    // Analyze the ridges in the heightfield
    report << "\tFinding ridges...";
    phase.reset(); phase.start();
//    ridgeImage.setSizes(sizes());
//    fill(ridgeImage, 0.0f);
//    ridges = inca::raster::find_ridges(hyper, scales, CurveTracker(&ridgeImage, scales));
    phase.stop();
    report << phase() << " seconds\n";

    total.stop();
    report << "Analysis took " << total() << " seconds\n";

    // Record what we did
    std::cerr << report.str();
    _analyzed = true;
}


// Lazy loading and analysis mechanism
void TerrainSample::LOD::ensureLoaded() const {
    if (! loaded()) {
        // First, see if there is a file we can load
        std::string demFile = object().filename();
        if (demFile != "") {
            // Look for a cache file corresponding to the DEM
            std::string cacheFile = demFile.substr(0, demFile.length() - 4) + ".cache";
            std::cerr << "DEM is " << demFile << " and cache would be " << cacheFile << std::endl;

            struct stat s;
            if (stat(cacheFile.c_str(), &s) == 0) {
//                loadFromFile(ca
            } else {
                // See if our parent TerrainSample has a file that it hasn't
                // loaded yet. We might be in that file.
                object().ensureFileLoaded();
            }
        }

        // If we're not loaded yet, let's look for another LOD that we
        // could resample from. We'd prefer to down-sample, but we'll up-sample
        // if we absolutely have to.
        if (! loaded()) {
            TerrainLOD ref = TerrainLOD_Underflow;;

            // First see if there is a higher-rez version we could down-sample from
            if (object().nearestLoadedLODAbove(levelOfDetail()) != TerrainLOD_Overflow)
                ref = levelOfDetail() + 1;

            // If that didn't work...look for a lower-rez version to up-sample
            if (object().nearestLoadedLODBelow(levelOfDetail()) != TerrainLOD_Underflow)
                ref = levelOfDetail() - 1;


            // If we got it, resample from our neighbor
            if (ref != TerrainLOD_Underflow) {
                std::cerr << "TS<" << name() << ":" << index() << ">"
                             "::ensureLoaded(" << levelOfDetail()
                          << "): Resampling from " << ref << std::endl;
                const_cast<TerrainSample::LOD *>(this)->resampleFromLOD(ref);

            // If that STILL didn't work...I don't know what to tell you
            } else {
                std::cerr << "TS<" << name() << ":" << index() << ">"
                             "::ensureLoaded(" << levelOfDetail()
                          << "): Unable to load LOD -- no other LODs exist and "
                             "no filename was specified...giving up" << std::endl;
            }
        }
    }
}
void TerrainSample::LOD::ensureAnalyzed() const {
    if (! analyzed()) {
        ensureLoaded();
        const_cast<TerrainSample::LOD *>(this)->analyze();
    }
}

// Raster geometry accessors
SizeType TerrainSample::LOD::size() const {
    ensureLoaded();
    return _elevations.size();
}
SizeType TerrainSample::LOD::size(IndexType d) const {
    ensureLoaded();
    return _elevations.size(d);
}
IndexType TerrainSample::LOD::base(IndexType d) const {
    ensureLoaded();
    return _elevations.base(d);
}
IndexType TerrainSample::LOD::extent(IndexType d) const {
    ensureLoaded();
    return _elevations.extent(d);
}
const TerrainSample::LOD::SizeArray & TerrainSample::LOD::sizes() const {
    ensureLoaded();
    return _elevations.sizes();
}
const TerrainSample::LOD::IndexArray & TerrainSample::LOD::bases() const {
    ensureLoaded();
    return _elevations.bases();
}
const TerrainSample::LOD::IndexArray & TerrainSample::LOD::extents() const {
    ensureLoaded();
    return _elevations.extents();
}

void TerrainSample::LOD::setSizes(const SizeArray & sz) {
    _elevations.setSizes(sz);
    _gradients.setSizes(sz);
    _localElevationMeans.setSizes(sz);
    _localGradientMeans.setSizes(sz);
    _localElevationRanges.setSizes(sz);
    _localSlopeRanges.setSizes(sz);
}

// Per-cell sample data accessors
const Heightfield & TerrainSample::LOD::elevations() const {
    ensureLoaded();
    return _elevations;
}
const VectorMap & TerrainSample::LOD::gradients() const {
    ensureAnalyzed();
    return _gradients;
}
const Heightfield & TerrainSample::LOD::localElevationMeans() const {
    ensureAnalyzed();
    return _localElevationMeans;
}
const VectorMap & TerrainSample::LOD::localGradientMeans() const {
    ensureAnalyzed();
    return _localGradientMeans;
}
const VectorMap & TerrainSample::LOD::localElevationRanges() const {
    ensureAnalyzed();
    return _localElevationRanges;
}
const VectorMap & TerrainSample::LOD::localSlopeRanges() const {
    ensureAnalyzed();
    return _localSlopeRanges;
}


// Global sample data accessors
const FrequencySpectrum & TerrainSample::LOD::frequencyContent() const {
    ensureAnalyzed();
    return _frequencySpectrum;
}
const ScalarStatistics & TerrainSample::LOD::globalElevationStatistics() const {
    ensureAnalyzed();
    return _globalElevationStatistics;
}
const ScalarStatistics & TerrainSample::LOD::globalSlopeStatistics() const {
    ensureAnalyzed();
    return _globalSlopeStatistics;
}
const VectorStatistics & TerrainSample::LOD::globalGradientStatistics() const {
    ensureAnalyzed();
    return _globalGradientStatistics;
}
const CurveTracker & TerrainSample::LOD::edges() const {
    ensureAnalyzed();
    return _edges;
}
const CurveTracker & TerrainSample::LOD::ridges() const {
    ensureAnalyzed();
    return _ridges;
}


/*---------------------------------------------------------------------------*
 | Access to parent TerrainSample properties
 *---------------------------------------------------------------------------*/
std::string TerrainSample::LOD::name() const {
    std::stringstream ss;
    ss << object().name() << '[' << levelOfDetail() << ']';
    return ss.str();
}
IndexType TerrainSample::LOD::index() const {
    return object().index();
}


/*****************************************************************************
 * TerrainSample class for holding a single sample of terrain
 *****************************************************************************/

/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
// Minimal constructor specifying TerrainType
TerrainSample::TerrainSample() : _index(0) { }

// Constructor taking a filename
TerrainSample::TerrainSample(const std::string & file)
        : _filename(file), _fileLoaded(false), _index(0) { }

// Constructor taking a heightfield
TerrainSample::TerrainSample(const Heightfield & hf, TerrainLOD forLOD)
        : _filename(), _fileLoaded(false), _index(0) {

    // Stick this in as our first LOD heightfield
    (*this)[forLOD].createFromRaster(hf);
}


/*---------------------------------------------------------------------------*
 | Lazy DEM file loading
 *---------------------------------------------------------------------------*/

// File loading
const std::string & TerrainSample::filename() const { return _filename; }
void TerrainSample::loadFromFile(const std::string & filename) {
    // Parse the file
    Heightfield temp;
    DEMInterpreter dem(temp);
    dem.filename = filename;
    dem.parse();

    // Trim it and store it to the right place
    Pixel       start(TRIM);
    Dimension   size(temp.sizes());
    size -= Dimension(2 * TRIM);

    // FIXME: This is just here until DFT works with all shapes
    size[0] = size[1] = std::min((size[0] / 2) *2- 1, (size[1] / 2) *2- 1);

    // Put the loaded data into the correct LOD
    scalar_t resX(dem.resolution[0]);
    scalar_t resY(dem.resolution[1]);
    if (resX != resY) {
        std::cerr << "loadFromFile(): DEM has different X and Y "
                        "resolutions -- using X resolution" << std::endl;
    }

    TerrainLOD fileLOD = LODForMetersPerSample(resX);
    std::cerr << "File contains " << fileLOD << endl;
    if (false && (dem.verticalUnits == 1 || dem.resolution[2] != 1.0f)) {
        scalar_t scaleFactor = dem.resolution[2];
        if (dem.verticalUnits == 1) scaleFactor /= 3.25f;
        std::cerr << "Rescaling elevations by factor of " << scaleFactor << std::endl;
        (*this)[fileLOD].createFromRaster(selectBS(temp, start, size) * scaleFactor);
    } else {
        (*this)[fileLOD].createFromRaster(selectBS(temp, start, size));
    }
}

// Lazy loading mechanism
bool TerrainSample::fileLoaded() const { return _fileLoaded; }
void TerrainSample::ensureFileLoaded() const {
    if (! fileLoaded() && filename() != "") {
        cerr << "ensureFileLoaded(): Loading DEM file " << filename() << endl;
        const_cast<TerrainSample*>(this)->loadFromFile(filename());
        const_cast<TerrainSample*>(this)->_fileLoaded = true;
    }
}


/*---------------------------------------------------------------------------*
 | Properties
 *---------------------------------------------------------------------------*/
// Associated terrain type
TerrainTypePtr TerrainSample::terrainType() {
    return _terrainType;
}
TerrainTypeConstPtr TerrainSample::terrainType() const {
    return _terrainType;
}
void TerrainSample::setTerrainType(TerrainTypePtr tt) {
    _terrainType = tt;
}
std::string TerrainSample::name() const {
    std::stringstream ss;
    if (_terrainType != NULL)
        ss << terrainType()->name();
    else
        ss << "unattached";
    ss << ':' << index();
    return ss.str();
}
IndexType TerrainSample::index() const { return _index; }
void TerrainSample::setIndex(IndexType idx) { _index = idx; }
