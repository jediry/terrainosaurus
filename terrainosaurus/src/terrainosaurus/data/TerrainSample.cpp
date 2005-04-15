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

// Import raster operations
#include <inca/raster/algorithms/scale_space_project>
#include <inca/raster/algorithms/find_edges>
#include <inca/raster/algorithms/find_ridges>
#include <inca/raster/operators/select>
#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/gradient>
#include <inca/raster/operators/resample>
#include <inca/raster/operators/fourier>
#include <inca/raster/operators/magnitude>
#include <inca/raster/operators/statistic>


// Import file I/O and exception classes
#include <terrainosaurus/io/DEMInterpreter.hpp>
#include <inca/util/StreamException.hpp>


using namespace inca;
using namespace inca::raster;
using namespace terrainosaurus;

typedef TerrainSample::LOD::FrequencySpectrum   FrequencySpectrum;
typedef TerrainSample::LOD::ScalarStatistics    ScalarStatistics;
typedef TerrainSample::LOD::VectorStatistics    VectorStatistics;


#define TRIM 30     // How many pixels to trim from each side
//#define CACHE_DIR   "C:/Documents and Settings/Dave/My Documents/Ry's Stuff/Media/cache"
#define CACHE_DIR   "/mnt/data/scratch/gis/cache"

// Timer!
#include <inca/util/Timer>

// HACK
#include <sys/stat.h>
#include <errno.h>


CurveTracker::CurveTracker() : image(NULL) { }

CurveTracker::CurveTracker(const std::vector<scalar_t> & sc, GrayscaleImage * img)
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
    if (image)
        (*image)(int(p[0]), int(p[1])) = 1.0f;
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
        c.lengthStats(1.0f);
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
    for (IndexType i = 0; i < IndexType(scales.size()); ++i)
        atScale[i] += c.atScale[i];
}

void CurveTracker::done() {
    // Calculate mean and variance
    lengthStats.done();
    scaleStats.done();
    strengthStats.done();
    for (IndexType i = 0; i < IndexType(curves.size()); ++i) {
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
    for (IndexType i = 0; i < IndexType(atScale.size()); ++i)
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
template <typename T, inca::SizeType dim>
void write(std::ostream & os, const inca::raster::MultiArrayRaster<T, dim> & r) {
    typedef inca::raster::MultiArrayRaster<T, dim> Raster;

    // Write the dimensions of the raster
    typename Raster::SizeType sz = r.size();
    typename Raster::IndexArray bs = r.bases();
    typename Raster::IndexArray ex = r.extents();
    os.write((char const *)&bs, sizeof(typename Raster::IndexArray));
    os.write((char const *)&ex, sizeof(typename Raster::IndexArray));

    // Write the raster contents
    os.write((char const*)r.elements(), sz * sizeof(T));
}
template <typename T, inca::SizeType dim>
void read(std::istream & is, inca::raster::MultiArrayRaster<T, dim> & r) {
    typedef inca::raster::MultiArrayRaster<T, dim> Raster;

    // Set the dimensions of the raster
    typename Raster::IndexArray bs, ex;
    is.read((char*)&bs, sizeof(typename Raster::IndexArray));
    is.read((char*)&ex, sizeof(typename Raster::IndexArray));
    r.setBounds(bs, ex);
    typename Raster::SizeType sz = r.size();

    // Read the raster contents
    is.read((char*)r.elements(), sz * sizeof(T));
}

template <typename T, inca::SizeType dim>
void write(std::ostream & os, const inca::Array<T, dim> & a) {
    os.write((char const *)a.elements(), dim * sizeof(T));
}
template <typename T, inca::SizeType dim>
void read(std::istream & is, inca::Array<T, dim> & a) {
    is.read((char *)a.elements(), dim * sizeof(T));
}

template <typename T, bool cache>
void write(std::ostream & os, const inca::math::Statistics<T, cache> & s) {
    int count = s.count();
    os.write((char const *)s.elements(), s.size() * sizeof(T));
    write(os, s.histogram());
    os.write((char const *)&count,                  sizeof(int));
}
template <typename T, bool cache>
void read(std::istream & is, inca::math::Statistics<T, cache> & s) {
    int count;
    is.read((char*)s.elements(), s.size() * sizeof(T));
    is.read((char*)&count,                  sizeof(int));
    read(is, s.histogram());
    s.count() = count;
    s.setValid(true);
}

void write(std::ostream & os, const CurveTracker::Curve & c) {
    int np = c.points.size(),
        ns = c.atScale.size();
    os.write((char const *)&np, sizeof(int));
    os.write((char const *)&c.points[0], np * sizeof(CurveTracker::Curve::Point));
    os.write((char const *)&ns, sizeof(int));
    os.write((char const *)&c.atScale[0], ns * sizeof(int));
    write(os, c.lengthStats);
    write(os, c.strengthStats);
    write(os, c.scaleStats);
}
void read(std::istream & is, CurveTracker::Curve & c) {
    int np, ns;
    is.read((char *)&np, sizeof(int));
    c.points.resize(np);
    is.read((char *)&c.points[0], np * sizeof(CurveTracker::Curve::Point));
    is.read((char *)&ns, sizeof(int));
    c.atScale.resize(ns);
    is.read((char *)&c.atScale[0], ns * sizeof(int));
    read(is, c.lengthStats);
    read(is, c.strengthStats);
    read(is, c.scaleStats);
}

void write(std::ostream & os, const CurveTracker & c) {
    int sc = c.scales.size();
    int nc = c.curves.size();
    os.write((char const *)&sc,                sizeof(int));
    os.write((char const *)&c.scales[0],  sc * sizeof(scalar_t));
    os.write((char const *)&c.atScale[0], sc * sizeof(int));
    write(os, c.lengthStats);
    write(os, c.strengthStats);
    write(os, c.scaleStats);
    os.write((char const *)&nc, sizeof(int));
    for (int i = 0; i < nc; ++i)
        write(os, c.curves[i]);
}
void read(std::istream & is, CurveTracker & c) {
    int sc, nc;
    is.read((char *)&sc, sizeof(int));
    c.scales.resize(sc);
    c.atScale.resize(sc);
    is.read((char *)&c.scales[0],  sc * sizeof(scalar_t));
    is.read((char *)&c.atScale[0], sc * sizeof(int));
    read(is, c.lengthStats);
    read(is, c.strengthStats);
    read(is, c.scaleStats);
    is.read((char *)&nc, sizeof(int));
    c.curves.resize(nc, CurveTracker::Curve(sc));
    for (int i = 0; i < nc; ++i)
        read(is, c.curves[i]);
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
    _studied  = false;
}
void TerrainSample::LOD::loadFromFile(const std::string & filename) {
    INCA_INFO("Loading TerrainSample<" << name() << "> from \""
              << filename << '"')

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
            INCA_WARNING(filename << " does not have a correct magic header. "
                         "Are you sure this is a cache file?")

        // Read the LOD and dimensions
        TerrainLOD lod;
        file.read((char*)&lod, sizeof(TerrainLOD));

        // Warn if this doesn't seem like the right LOD
        if (lod != levelOfDetail())
            INCA_WARNING("Uh oh...\"" << filename << "\" contains " << lod
                         << ", not expected " << levelOfDetail())

        // Read in each of the rasters
        read(file, _elevations);
        read(file, _gradients);
        read(file, _localElevationMeans);
        read(file, _localGradientMeans);
        read(file, _localElevationLimits);
        read(file, _localSlopeLimits);

        // Write out each of the non-raster measurements
        read(file, _frequencySpectrum);
        read(file, _globalElevationStatistics);
        read(file, _globalSlopeStatistics);
        read(file, _globalGradientStatistics);

        // Write out each of the feature sets
        read(file, _edges);
        read(file, _ridges);

        // We should now be at the end of the file
        INCA_DEBUG("Stream pointer is " << file.tellg())

        // Close the file
        file.close();

        // Nothing more to do here...
        _loaded = true;
        _analyzed = true;
        _studied = true;

    } catch (std::exception & e) {
        INCA_ERROR("Error reading \"" << filename << "\": " << e.what())
        if (file) file.close();
    }
}
void TerrainSample::LOD::storeToFile(const std::string & filename) const {
    // Make sure we have stuff to store first
    ensureStudied();

    INCA_INFO("Storing TerrainSample<" << name() << "> to \""
              << filename << '"')

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
        file.write((char*)&lod, sizeof(TerrainLOD));

        // Write out each of the rasters
        write(file, _elevations);
        write(file, _gradients);
        write(file, _localElevationMeans);
        write(file, _localGradientMeans);
        write(file, _localElevationLimits);
        write(file, _localSlopeLimits);

        // Write out each of the non-raster measurements
        write(file, _frequencySpectrum);
        write(file, _globalElevationStatistics);
        write(file, _globalSlopeStatistics);
        write(file, _globalGradientStatistics);

        // Write out each of the feature sets
        write(file, _edges);
        write(file, _ridges);

        INCA_DEBUG("Stream pointer is " << file.tellp())

        // Close the file, now that we're done with it
        file.close();

    } catch (std::exception & e) {
        INCA_ERROR("Error writing \"" << filename << "\": " << e.what())
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
    INCA_INFO("Analyzing TerrainSample<" << name() << "> (" << sizes() << ')')
    Timer<float, false> total, phase;
    total.start();

    std::stringstream report;
    report << "Analysis of TerrainSample<" << name() << "> (" << sizes() << ')';
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
//    _globalGradientStatistics = apply(_globalGradientStatistics, _gradients);
//    _globalGradientStatistics.done();
//    phase.stop();
//    report << phase() << "seconds\n";

    // Determine the frequency content of the heightfield
    report << "\tCalculating frequency content...";
    phase.reset(); phase.start();
    scalar_t period = metersPerSampleForLOD(levelOfDetail());
    scalar_t nyquist = 1.0f / period;
    scalar_t maxFreq = nyquist / 2.0f;    // Samples per meter

    Heightfield fftMag = dcToCenter(cmagnitude(dft(_elevations)));
//                         / std::sqrt(scalar_t(size()));
//        cerr << "Period is " << period << endl;
//        cerr << "Nyquist is " << nyquist << endl;
//        cerr << "Max freq is " << maxFreq << endl;
    FrequencySpectrum counts(0.0f);
    Pixel px;
    for (px[0] = fftMag.base(0); px[0] <= fftMag.extent(0); ++px[0])
        for (px[1] = fftMag.base(1); px[1] <= fftMag.extent(1); ++px[1]) {
            scalar_t freqX = scalar_t(px[0]) / fftMag.size(0) * nyquist;
            scalar_t freqY = scalar_t(px[1]) / fftMag.size(1) * nyquist;
            scalar_t freq = std::sqrt(freqX*freqX + freqY*freqY);
            int bucket = int(freq / maxFreq * frequencyBands);
            if (bucket < _frequencySpectrum.size()) {
                _frequencySpectrum[bucket] += fftMag(px);
                counts[bucket] += 1.0f;
            }
        }
    for (int i = 0; i < _frequencySpectrum.size(); ++i)
        _frequencySpectrum[i] /= counts[i];
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
    phase.reset(); phase.start();
    scale_space_project(scaleSpace, elevations(), scales);
    phase.stop();
    report << phase() << " seconds\n";

    // Analyze the edges in the heightfield
    report << "\tFinding edges...";
    phase.reset(); phase.start();
    _edgeImage.setSizes(sizes());
    _edges = inca::raster::find_edges(scaleSpace, scales, CurveTracker(scales, &_edgeImage));
    phase.stop();
    report << '(' << _edges.curves.size() << " edges) " << phase() << " seconds\n";

    // Analyze the ridges in the heightfield
    report << "\tFinding ridges...";
    phase.reset(); phase.start();
    _ridges = inca::raster::find_ridges(scaleSpace, scales, CurveTracker(scales, NULL));
    phase.stop();
    report << '(' << _ridges.curves.size() << " ridges) " << phase() << " seconds\n";

    total.stop();
    report << "Analysis took " << total() << " seconds\n";

    // Record what we did
    INCA_INFO(report.str())
    _analyzed = true;
}


// Lazy loading and analysis mechanism
void TerrainSample::LOD::ensureLoaded() const {
    if (! loaded()) {
        // First, see if we have a current cache file that we could load
        struct stat cacheStat, demStat;
        int cs = stat(cacheFilename().c_str(), &cacheStat);
        int ds = stat(object().filename().c_str(), &demStat);

        // If we have only the cache and no DEM (!!), or if the cache is no
        // older than the DEM, we just load the cache
        if (cs == 0 && (ds != 0 || cacheStat.st_mtime >= demStat.st_mtime)) {
            // Yay! Let's be lazy and just load the cache
            const_cast<TerrainSample::LOD *>(this)->loadFromFile(_cacheFilename);

        // Otherwise, if we have a valid DEM, ask the object to load it. Note
        // that we don't load it ourselves, because it might contain a different
        // LOD than us (and THEN what would we do??).
        } else if (ds == 0) {
            // See if our parent TerrainSample has a file that it hasn't
            // loaded yet. We might be in that file.
            object().ensureFileLoaded();

        // If we got here...then I sure hope we have data somewhere we can resample
        } else {
            INCA_INFO(name() << ": No data files available")
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
                INCA_INFO("TS<" << name() << ":" << index() << ">"
                          "::ensureLoaded(" << levelOfDetail() << "): "
                          "Resampling from " << ref)
                const_cast<TerrainSample::LOD *>(this)->resampleFromLOD(ref);

            // If that STILL didn't work...I don't know what to tell you
            } else {
                INCA_INFO("TS<" << name() << ":" << index() << ">"
                          "::ensureLoaded(" << levelOfDetail() << "): "
                          "Unable to load LOD -- no other LODs exist and "
                          "no filename was specified...giving up")
            }
        }
    }
}
void TerrainSample::LOD::ensureAnalyzed() const {
    ensureLoaded();
    if (! analyzed())
        const_cast<TerrainSample::LOD *>(this)->analyze();
}
void TerrainSample::LOD::ensureStudied() const {
    ensureAnalyzed();
    if (! studied())
        const_cast<TerrainSample::LOD *>(this)->study();
}

void TerrainSample::LOD::study() {
    INCA_INFO("Studying TerrainSample<" << name() << "> (" << sizes() << ')')
    Timer<float, false> total, phase;
    total.start();

    std::stringstream report;
    report << "Study of TerrainSample<" << name() << "> (" << sizes() << ')';
    if (object().filename() != "")
        report << " from " << object().filename();
    report << "...\n";

    // Calculate the local mean elevation, mean gradient and range
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
    _localElevationLimits.setSizes(sizes());
    for (px[0] = 0; px[0] < size(0); ++px[0])
        for (px[1] = 0; px[1] < size(1); ++px[1]) {
            start = Pixel(px[0] - halfWin, px[1] - halfWin);
            _localElevationLimits(px) = Vector2D(range(selectBS(_elevations, start, windowSizes)));
        }
    phase.stop();
    report << phase() << " seconds\n";

    report << "\tCalculating gradient magnitude...";
    phase.reset(); phase.start();
    Heightfield gradientMag = raster::magnitude(_gradients);
    phase.stop();
    report << phase() << " seconds\n";

    report << "\tCalculating local slope ranges...";
    phase.reset(); phase.start();
    _localSlopeLimits.setSizes(sizes());
    for (px[0] = 0; px[0] < size(0); ++px[0])
        for (px[1] = 0; px[1] < size(1); ++px[1]) {
            start = Pixel(px[0] - halfWin, px[1] - halfWin);
            _localSlopeLimits(px)   = Vector2D(range(selectBS(gradientMag, start, windowSizes)));
        }
    phase.stop();
    report << phase() << " seconds\n";

    total.stop();
    report << "Study took " << total() << " seconds\n";

    // Record what we did
    INCA_DEBUG(report.str())
    _studied = true;

    // Write to the cache file
    if (cacheFilename() != "")
        storeToFile(cacheFilename());
}


// Access to cache filename
const std::string & TerrainSample::LOD::cacheFilename() const {
    return _cacheFilename;
}
void TerrainSample::LOD::setCacheFilename(const std::string & f) {
    _cacheFilename = f;
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
const TerrainSample::LOD::Region & TerrainSample::LOD::bounds() const {
    ensureLoaded();
    return _elevations.bounds();
}

void TerrainSample::LOD::setSizes(const SizeArray & sz) {
    _elevations.setSizes(sz);
    _gradients.setSizes(sz);
    _localElevationMeans.setSizes(sz);
    _localGradientMeans.setSizes(sz);
    _localElevationLimits.setSizes(sz);
    _localSlopeLimits.setSizes(sz);
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
    ensureStudied();
    return _localElevationMeans;
}
const VectorMap & TerrainSample::LOD::localGradientMeans() const {
    ensureStudied();
    return _localGradientMeans;
}
const VectorMap & TerrainSample::LOD::localElevationLimitss() const {
    ensureStudied();
    return _localElevationLimits;
}
const VectorMap & TerrainSample::LOD::localSlopeLimitss() const {
    ensureStudied();
    return _localSlopeLimits;
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
        : _filename(file), _fileLoaded(false), _index(0) {

    // Make sure the file exists
    struct stat s;
    if (stat(file.c_str(), &s) != 0) {
        inca::StreamException e;
        e << "TerrainSample(" << file << "): ";
#if __GNUC__
        char message[50];
        e << strerror_r(errno, message, 50);
#elif _MS_WINDOZE_
        e << strerror(errno);
#endif
        throw e;
    }

    // Determine the cache filename for each LOD
    std::stringstream ss;
    std::string::size_type slash = file.rfind('/');
    std::string::size_type paren = file.rfind('(');
    ss << CACHE_DIR << '/' << file.substr(slash + 1, paren - slash - 1) << '(';
    std::string prefix = ss.str(),
                suffix = "m).cache";
    for (TerrainLOD lod = TerrainLOD::minimum(); lod <= TerrainLOD::maximum(); ++lod) {
        std::stringstream ss;
        ss << prefix << metersPerSampleForLOD(lod) << suffix;
        this->_lods[lod].setCacheFilename(ss.str());
//        std::cerr << "Cache file is '" << ss.str() << '\'' << std::endl;
    }
}

// Constructor taking a heightfield
TerrainSample::TerrainSample(const Heightfield & hf, TerrainLOD forLOD)
        : _filename(), _fileLoaded(false), _index(0) {

    // Stick this in as our first LOD heightfield
    this->_lods[forLOD].createFromRaster(hf);
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
    INCA_DEBUG("Extrema of DEM are " << min(temp) << " => " << max(temp))

    // Trim it and store it to the right place
    Pixel       start(TRIM);
    Dimension   size(temp.sizes());
    size -= Dimension(2 * TRIM);

    // FIXME: This is just here until DFT works with all shapes
    size[0] = size[1] = std::min((size[0] / 2) * 2, (size[1] / 2) * 2);

    // Put the loaded data into the correct LOD
    scalar_t resX(dem.resolution[0]);
    scalar_t resY(dem.resolution[1]);
    if (resX != resY) {
        INCA_WARNING("loadFromFile(): DEM has different X and Y "
                     "resolutions -- using X resolution")
    }

    TerrainLOD fileLOD = LODForMetersPerSample(resX);
    INCA_DEBUG("File contains " << fileLOD)

    // Take out min elevation
    scalar_t mini = min(selectBS(temp, start, size));
    temp -= mini;
    if (dem.verticalUnits == 1 || dem.resolution[2] != 1.0f) {
        scalar_t scaleFactor = dem.resolution[2];
        if (dem.verticalUnits == 1) scaleFactor *= 3.25f;
        INCA_DEBUG("Rescaling elevations by factor of " << scaleFactor)
        (*this)[fileLOD].createFromRaster(selectBS(temp, start, size) * scaleFactor);
        //(*this)[fileLOD].createFromRaster(selectBS(resample(temp, 2) - dem.elevationExtrema[0], start, size) * scaleFactor);
    } else {
        (*this)[fileLOD].createFromRaster(selectBS(temp, start, size));
//        (*this)[fileLOD].createFromRaster(selectBS(resample(temp, 2)//        ridges = (*sample)[lod]._edgeImage;
//, start, size));
    }

    INCA_DEBUG("Resulting min is " << min((*this)[fileLOD].elevations()))
}

// Lazy loading mechanism
bool TerrainSample::fileLoaded() const { return _fileLoaded; }
void TerrainSample::ensureFileLoaded() const {
    if (! fileLoaded() && filename() != "") {
        INCA_INFO("ensureFileLoaded(): Loading DEM file " << filename())
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
