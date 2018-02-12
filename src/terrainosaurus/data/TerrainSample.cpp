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

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "TerrainSample.hpp"

// Import application class definition
#include <terrainosaurus/TerrainosaurusApplication.hpp>

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

// Import Inca file-related exceptions
#include <inca/io/FileExceptions.hpp>

namespace terrainosaurus {
    // Forward declaration
    class FeatureTracker;
}

//using namespace inca;
using namespace inca;
using namespace inca::raster;
using namespace terrainosaurus;

typedef TerrainSample::LOD::FrequencySpectrum   FrequencySpectrum;
typedef TerrainSample::LOD::Stat                Stat;
typedef TerrainSample::LOD::StatList            StatList;
typedef TerrainSample::LOD::Feature             Feature;
typedef TerrainSample::LOD::FeatureList         FeatureList;


// Timer!
#include <inca/util/Timer>

// HACK
#define FIND_PEAKS  0
#define FIND_EDGES  1
#define FIND_RIDGES 0
#define FIND_FEATURES FIND_PEAKS || FIND_EDGES || FIND_RIDGES


// Edge-detection tracker class
class terrainosaurus::FeatureTracker {
public:
    typedef Feature::Point    Point;
    enum FeatureType { Edge, Ridge };


    // Constructors
    FeatureTracker(ColorImage * fm,
                   const std::vector<scalar_t> & sc,
                   Feature::Type t, Color c)
        : featureMap(fm), scales(sc), type(t), color(c) { }

    // Data
    ColorImage * featureMap;
    Feature::Type   type;
    Color           color;
    FeatureList features;
    Stat    lengthStats,
            strengthStats,
            scaleStats;
    std::vector<scalar_t> scales;

    template <class P>
    void operator()(const P & p, scalar_t s) {
        Point px(static_cast<scalar_t>(p[0]),
                 static_cast<scalar_t>(p[1]),
                 scales[p[2]],
                 s);
        add(px);
    }

    void begin();
    void add(const Point & p);
    void end();
    void finish();
//    void print(std::ostream & os);
};


void FeatureTracker::begin() {
    features.push_back(Feature(type));
}

void FeatureTracker::add(const Point & p) {
    // Increment per-curve quantities
    Feature & f = features.back();
    f.length += 1;
    f.scaleStats(p[2]);
    f.strengthStats(p[3]);
    f.points.push_back(p);
    
    // Draw it on the image
    if (featureMap)
        (*featureMap)(int(p[0]), int(p[1])) %= color;
}

void FeatureTracker::end() {
    // Calculate per-curve statistics
    Feature & f = features.back();
    f.scaleStats.finish();
    f.strengthStats.finish();

    Feature::PointList::const_iterator it;
    for (it = f.points.begin(); it != f.points.end(); ++it) {
        const Point & p = *it;
        f.scaleStats(p[2]);
        f.strengthStats(p[3]);
    }

    f.scaleStats.finish();
    f.strengthStats.finish();

    // Add this curve's quantities into the global stats
    lengthStats(f.length);
    scaleStats(f.scaleStats.mean());
    strengthStats(f.strengthStats.mean());
}

void FeatureTracker::finish() {
    // Calculate mean and variance
    lengthStats.finish();
    scaleStats.finish();
    strengthStats.finish();
    FeatureList::const_iterator it;
    for (it = features.begin(); it != features.end(); ++it) {
        const Feature & f = *it;
        lengthStats(f.length);
        scaleStats(f.scaleStats.mean());
        strengthStats(f.strengthStats.mean());
    }
    lengthStats.finish();
    scaleStats.finish();
    strengthStats.finish();

//    std::cerr << "Size of this thing is " << sizeof(FeatureTracker) << " and the vectors have " << scales.size() << ", " << atScale.size() << "and " << std::endl;
//    std::cerr << "There are " << curves.size() << " curves of size " << sizeof(Feature) << " and their vectors are : \n";
//    for (int i = 0; i < curves.size(); ++i)
//        std::cerr << i << ": " << curves[i].points.size() << " and " << curves[i].atScale.size() << endl;
}

#if 0
void FeatureTracker::print(std::ostream & os) {
    os << curves.size() << " curves" << endl;
    for (IndexType i = 0; i < IndexType(atScale.size()); ++i)
        os << "\tScale " << i << ": " << atScale[i] << endl;

    os << "Length:" << endl
        << "\tSum     " << lengthStats.sum() << endl
        << "\tMinimum " << lengthStats.min() << endl
        << "\tMaximum " << lengthStats.max() << endl
        << "\tMean    " << lengthStats.mean() << endl
        << "\tStdDev  " << lengthStats.stddev() << endl;

    os << "Scale:" << endl
        << "\tSum     " << scaleStats.sum() << endl
        << "\tMinimum " << scaleStats.min() << endl
        << "\tMaximum " << scaleStats.max() << endl
        << "\tMean    " << scaleStats.mean() << endl
        << "\tStdDev  " << scaleStats.stddev() << endl;

    os << "Strength:" << endl
        << "\tSum     " << strengthStats.sum() << endl
        << "\tMinimum " << strengthStats.min() << endl
        << "\tMaximum " << strengthStats.max() << endl
        << "\tMean    " << strengthStats.mean() << endl
        << "\tStdDev  " << strengthStats.stddev() << endl;
}
#endif


/*****************************************************************************
 * LOD specialization for TerrainSample
 *****************************************************************************/
// Default constructor
LOD<TerrainSample>::LOD()
    : LODBase<TerrainSample>() { }

// Constructor linking back to TerrainSample
LOD<TerrainSample>::LOD(TerrainSamplePtr ts, TerrainLOD lod)
    : LODBase<TerrainSample>(ts, lod) { }


// Access to related LOD objects
TerrainType::LOD & LOD<TerrainSample>::terrainType() {
    return (*object().terrainType())[levelOfDetail()];
}
const TerrainType::LOD & LOD<TerrainSample>::terrainType() const {
    return (*object().terrainType())[levelOfDetail()];
}

MapRasterization::LOD & LOD<TerrainSample>::mapRasterization() {
    return (*object().mapRasterization())[levelOfDetail()];
}
const MapRasterization::LOD & LOD<TerrainSample>::mapRasterization() const {
    return (*object().mapRasterization())[levelOfDetail()];
}

// Initialization & analysis of elevation data
void LOD<TerrainSample>::createFromRaster(const Heightfield & hf) {
    _elevations = hf;
    _loaded   = true;
    _analyzed = false;
    _studied  = false;
}
void LOD<TerrainSample>::resampleFromLOD(TerrainLOD lod) {
    // Resample the fundamental raster properties
    _elevations = resample(object()[lod].elevations(),
                           scaleFactor(lod, levelOfDetail()));
    _loaded = true;
    _analyzed = false;
}
void LOD<TerrainSample>::_calculateFrequencySpectrum() {
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
}

void LOD<TerrainSample>::_calculateStatistics() {
    // Determine whether we need to calculate per-region stats too
    bool hasRegions = (regionCount() > 1);

    // Calculate the gradient magnitude
    Heightfield gradientMag = raster::magnitude(_gradients);

    // Resize & reset the statistics objects
    _globalElevationStatistics.reset();
    _globalSlopeStatistics.reset();
    _globalEdgeStrengthStatistics.reset();
    _globalEdgeLengthStatistics.reset();
    _globalEdgeScaleStatistics.reset();
    if (hasRegions) {
        _regionElevationStatistics.resize(regionCount());
        _regionSlopeStatistics.resize(regionCount());
        _regionEdgeStrengthStatistics.resize(regionCount());
        _regionEdgeLengthStatistics.resize(regionCount());
        _regionEdgeScaleStatistics.resize(regionCount());
        for (IDType r = 0; r < IDType(regionCount()); ++r) {
            _regionElevationStatistics[r].reset();
            _regionSlopeStatistics[r].reset();
            _regionEdgeStrengthStatistics[r].reset();
            _regionEdgeLengthStatistics[r].reset();
            _regionEdgeScaleStatistics[r].reset();
        }            
    }
    
    // Get a pointer to the map, if necessary
    const MapRasterization::LOD * mr = NULL;
    if (hasRegions)
        mr = &(this->mapRasterization());
        
    // TODO: Implement unified iterator & replace here
    for (int pass = 1; pass <= 2; ++pass) {
        // Collect per-pixel quantities
        Pixel px;
        for (px[1] = base(1); px[1] <= extent(1); ++px[1])
            for (px[0] = base(0); px[0] <= extent(0); ++px[0]) {
                scalar_t h = elevation(px);
                scalar_t s = gradientMag(px);

                _globalElevationStatistics(h);
                _globalSlopeStatistics(s);

                if (hasRegions) {
                    IDType rID = mr->regionID(px);
                    _regionElevationStatistics[rID - 1](h);
                    _regionSlopeStatistics[rID - 1](s);
                }
            }
            
        // Collect per-feature quantities
        FeatureList::const_iterator fi;
        
#if FIND_EDGES
        for (fi = _edges.begin(); fi != _edges.end(); ++fi) {
            const Feature & f = *fi;
            _globalEdgeLengthStatistics(f.length);
            IDType lastRegion = 0;
            scalar_t regionLength;
            for (IndexType i = 0; i < IndexType(f.points.size()); ++i) {
                Feature::Point p = f.points[i];
                _globalEdgeScaleStatistics(p[2]);
                _globalEdgeStrengthStatistics(p[3]);

                if (hasRegions) {
                    IDType rID = mr->regionID(static_cast<int>(p[0]),
                                              static_cast<int>(p[1]));
                    if (rID != lastRegion) {
                        if (lastRegion != 0)
                            _regionEdgeLengthStatistics[lastRegion - 1](regionLength);
                        lastRegion = rID;
                        regionLength = 0.0f;
                    } else {
                        regionLength += 1.0f;
                    }
                    _regionEdgeScaleStatistics[rID - 1](p[2]);
                    _regionEdgeStrengthStatistics[rID - 1](p[3]);
                }
            }
        }
#endif

        // Finalize this pass and (possibly) prepare for the next        
        _globalElevationStatistics.finish();
        _globalSlopeStatistics.finish();
        _globalEdgeStrengthStatistics.finish();
        _globalEdgeLengthStatistics.finish();
        _globalEdgeScaleStatistics.finish();
        if (hasRegions) {
            for (IDType r = 0; r < IDType(regionCount()); ++r) {
                _regionElevationStatistics[r].finish();
                _regionSlopeStatistics[r].finish();
                _regionEdgeStrengthStatistics[r].finish();
                _regionEdgeLengthStatistics[r].finish();
                _regionEdgeScaleStatistics[r].finish();
            }
        }
    }
        
    Stat & s = _globalEdgeStrengthStatistics;
    INCA_DEBUG("Statistics: ")
    INCA_DEBUG("  Mean:     " << s.mean())
    INCA_DEBUG("  Variance: " << s.variance())
    INCA_DEBUG("  Skewness: " << s.skewness())
    INCA_DEBUG("  Kurtosis: " << s.kurtosis())
    
}
void LOD<TerrainSample>::_findFeatures() {
    inca::Timer<float, false> phase;
    
    // Create the scale-space representation of the heightfield
    std::vector<scalar_t> scales;
    scales.push_back(0.0f);
    scales.push_back(1.0f);
    scales.push_back(2.0f);
    scales.push_back(3.0f);

    ScaleSpaceImage scaleSpace;

    phase.start(true);
        scale_space_project(scaleSpace, elevations(), scales);
    phase.stop();
    INCA_DEBUG("Creating scale space projection (" << scales.size() << " scales)..."
               << phase() << " seconds")

    // Clear the feature map image
    _featureMap.setSizes(sizes());
    raster::fill(_featureMap, Color(0.0f, 0.0f, 0.0f, 0.0f));

#if FIND_PEAKS
    // Find the peaks in the heightfield
    phase.start(true);
        FeatureTracker pt(&_featureMap, scales, Feature::Peak,
                          Color(0.0f, 1.0f, 0.0f, 1.0f));
        pt = inca::raster::find_peaks(scaleSpace, scales, pt);
        _peaks.swap(pt.features);
    phase.stop();
    INCA_DEBUG("Finding peaks (" << _peaks.size() << " peaks) "
               << phase() << " seconds")
#endif

#if FIND_EDGES
    // Analyze the edges in the heightfield
    phase.start(true);
        FeatureTracker et(&_featureMap, scales, Feature::Edge,
                          Color(0.0f, 0.0f, 1.0f, 1.0f));
        et = inca::raster::find_edges(scaleSpace, scales, et);
        _edges.swap(et.features);
    phase.stop();
    INCA_DEBUG("Finding edges (" << _edges.size() << " edges) "
               << phase() << " seconds")
#endif

#if FIND_RIDGES
    // Analyze the ridges in the heightfield
    phase.start(true);
        FeatureTracker rt(&_featureMap, scales, Feature::Ridge,
                          Color(1.0f, 0.0f, 0.0f, 1.0f));
        rt = inca::raster::find_ridges(scaleSpace, scales, rt);
        _ridges.swap(rt.features);
    phase.stop();
    INCA_DEBUG("Finding ridges (" << _ridges.size() << " ridges) "
               << phase() << " seconds")
#endif
}
void LOD<TerrainSample>::analyze() {
    // Make sure we have a valid map to start with
    ensureLoaded();

    // Resize all rasters to the correct size
    setSizes(_elevations.sizes());

    INCA_INFO("Analyzing TerrainSample<" << name() << "> (" << sizes() << ')')
    inca::Timer<float, false> total, phase;
    total.start();

    std::stringstream report;
    report << "Analysis of TerrainSample<" << name() << "> (" << sizes() << ')';
    if (object().filename() != "")
        report << " from " << object().filename();
    report << "...\n";

    // Calculate the per-cell gradient
    phase.start(true);
        _gradients = raster::gradient(_elevations,
                                      metersPerSampleForLOD(levelOfDetail()));
    phase.stop();
    report << "\tCalculating gradient..." << phase() << " seconds\n";


    // Determine the frequency content of the heightfield
    phase.start(true);
        _calculateFrequencySpectrum();
    phase.stop();
    report << "\tCalculating frequency content..." << phase() << " seconds\n";

#if FIND_FEATURES
    // Find edges, ridges, etc. in the HF
    phase.start(true);
        _findFeatures();
    phase.stop();
    report << "\tFinding features..." << phase() << " seconds\n";
#endif

    // Calculate global and per-region statistics for elevation, slope, etc.
    phase.start(true);
        _calculateStatistics();
    phase.stop();
    report << "\tCalculating global & per-region statistics..."
           << phase() << " seconds\n";

    total.stop();
    report << "Analysis took " << total() << " seconds\n";

    // Record what we did
    INCA_INFO(report.str())
    _analyzed = true;
}

// Lazy loading and analysis mechanism
void LOD<TerrainSample>::ensureLoaded() const {
    if (! loaded()) {
        TerrainosaurusApplication & app = TerrainosaurusApplication::instance();

        try {
            // First, try to load the cache file, if it exists and is current
            app.loadAnalysisCache(const_cast<TerrainSample::LOD &>(*this));
            
        } catch (inca::io::FileException & e) {
            INCA_DEBUG("Cache load failed: " << e)
            
            // Crud. Either the cache doesn't exist, or else it's out-of-date
            // Let's see if we have another LOD that we could resample from.
            // We'd prefer to down-sample, but we'll up-sample if we must
            TerrainLOD ref = TerrainLOD_Underflow;;

            // First see if there is a higher-rez version we could down-sample from
            if (object().nearestLoadedLODAbove(levelOfDetail()) != TerrainLOD_Overflow)
                ref = levelOfDetail() + 1;

            // If that didn't work...look for a lower-rez version to up-sample
            if (object().nearestLoadedLODBelow(levelOfDetail()) != TerrainLOD_Underflow)
                ref = levelOfDetail() - 1;


            // If we don't have a neighbor from whom we could resample, we
            // need to try to reload the original source files
            if (ref == TerrainLOD_Underflow) {
                try {
                    app.loadSourceFiles(const_cast<TerrainSample &>(object()));

                    // First see if there is a higher-rez version we could down-sample from
                    if (object().nearestLoadedLODAbove(levelOfDetail()) != TerrainLOD_Overflow)
                        ref = levelOfDetail() + 1;

                    // If that didn't work...look for a lower-rez version to up-sample
                    if (object().nearestLoadedLODBelow(levelOfDetail()) != TerrainLOD_Underflow)
                        ref = levelOfDetail() - 1;

                } catch (inca::io::FileException & e) {
                    INCA_WARNING("Source file load failed: " << e)
                }
            }

            // If we found an LOD to resample from, we're all good
            if (ref != TerrainLOD_Underflow) {
                INCA_INFO("TS<" << name() << ":" << index() << ">"
                          "::ensureLoaded(" << levelOfDetail() << "): "
                          "Resampling from " << ref)
                const_cast<TerrainSample::LOD *>(this)->resampleFromLOD(ref);
            } else {
                // If THIS blew up, then we're hosed.
                INCA_ERROR("TS<" << name() << ":" << index() << ">"
                        "::ensureLoaded(" << levelOfDetail() << "): "
                        "Unable to load LOD -- no other LODs exist and "
                        "no filename was specified...giving up")
            }
        }
    }
}
void LOD<TerrainSample>::ensureAnalyzed() const {
    ensureLoaded();
    if (! analyzed())
        const_cast<TerrainSample::LOD *>(this)->analyze();
}
void LOD<TerrainSample>::ensureStudied() const {
    ensureAnalyzed();
    if (! studied())
        const_cast<TerrainSample::LOD *>(this)->study();
}

void LOD<TerrainSample>::study() {
    INCA_INFO("Studying TerrainSample<" << name() << "> (" << sizes() << ')')
    inca::Timer<float, false> total, phase;
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
    phase.start(true);
    _localElevationMeans.setSizes(sizes());
    for (px[0] = 0; px[0] < size(0); ++px[0])
        for (px[1] = 0; px[1] < size(1); ++px[1]) {
            start = Pixel(px[0] - halfWin, px[1] - halfWin);
            _localElevationMeans(px) = mean(selectBS(_elevations, start, windowSizes));
        }
    phase.stop();
    report << phase() << " seconds\n";

    report << "\tCalculating local gradient means...";
    phase.start(true);
    _localGradientMeans.setSizes(sizes());
    for (px[0] = 0; px[0] < size(0); ++px[0])
        for (px[1] = 0; px[1] < size(1); ++px[1]) {
            start = Pixel(px[0] - halfWin, px[1] - halfWin);
            _localGradientMeans(px) = mean(selectBS(_gradients, start, windowSizes));
        }
    phase.stop();
    report << phase() << " seconds\n";

    report << "\tCalculating local elevation ranges...";
    phase.start(true);
    _localElevationLimits.setSizes(sizes());
    for (px[0] = 0; px[0] < size(0); ++px[0])
        for (px[1] = 0; px[1] < size(1); ++px[1]) {
            start = Pixel(px[0] - halfWin, px[1] - halfWin);
            _localElevationLimits(px) = Vector2D(range(selectBS(_elevations, start, windowSizes)));
        }
    phase.stop();
    report << phase() << " seconds\n";

    report << "\tCalculating gradient magnitude...";
    phase.start(true);
    Heightfield gradientMag = raster::magnitude(_gradients);
    phase.stop();
    report << phase() << " seconds\n";

    report << "\tCalculating local slope ranges...";
    phase.start(true);
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

    // Write to the cache file, if we have an associated filename...
    if (object().filename() != "") {
        TerrainosaurusApplication & app = TerrainosaurusApplication::instance();
        app.storeAnalysisCache(*this);
    }
}


/*---------------------------------------------------------------------------*
 | Raster geometry accessors
 *---------------------------------------------------------------------------*/
SizeType LOD<TerrainSample>::size() const {
    ensureLoaded();
    return _elevations.size();
}
SizeType LOD<TerrainSample>::size(IndexType d) const {
    ensureLoaded();
    return _elevations.size(d);
}
IndexType LOD<TerrainSample>::base(IndexType d) const {
    ensureLoaded();
    return _elevations.base(d);
}
IndexType LOD<TerrainSample>::extent(IndexType d) const {
    ensureLoaded();
    return _elevations.extent(d);
}
const LOD<TerrainSample>::SizeArray & LOD<TerrainSample>::sizes() const {
    ensureLoaded();
    return _elevations.sizes();
}
const LOD<TerrainSample>::IndexArray & LOD<TerrainSample>::bases() const {
    ensureLoaded();
    return _elevations.bases();
}
const LOD<TerrainSample>::IndexArray & LOD<TerrainSample>::extents() const {
    ensureLoaded();
    return _elevations.extents();
}
const LOD<TerrainSample>::Region & LOD<TerrainSample>::bounds() const {
    ensureLoaded();
    return _elevations.bounds();
}

void LOD<TerrainSample>::setSizes(const SizeArray & sz) {
    _elevations.setSizes(sz);
    _gradients.setSizes(sz);
    _featureMap.setSizes(sz);
    _localElevationMeans.setSizes(sz);
    _localGradientMeans.setSizes(sz);
    _localElevationLimits.setSizes(sz);
    _localSlopeLimits.setSizes(sz);
}


/*---------------------------------------------------------------------------*
 | Per-cell properties
 *---------------------------------------------------------------------------*/
// Fundamental properties (initialized at load-time)
const Heightfield & LOD<TerrainSample>::elevations() const {
    ensureLoaded();
    return _elevations;
}

// Derived properties (initialized at analysis-time)
const VectorMap & LOD<TerrainSample>::gradients() const {
    ensureAnalyzed();
    return _gradients;
}
const ColorImage & LOD<TerrainSample>::featureMaps() const {
    ensureAnalyzed();
    return _featureMap;
}

// Windowed properties (initialized at study-time)
const Heightfield & LOD<TerrainSample>::localElevationMeans() const {
    ensureStudied();
    return _localElevationMeans;
}
const VectorMap & LOD<TerrainSample>::localGradientMeans() const {
    ensureStudied();
    return _localGradientMeans;
}
const VectorMap & LOD<TerrainSample>::localElevationLimitss() const {
    ensureStudied();
    return _localElevationLimits;
}
const VectorMap & LOD<TerrainSample>::localSlopeLimitss() const {
    ensureStudied();
    return _localSlopeLimits;
}


/*---------------------------------------------------------------------------*
 | Global and per-region properties
 *---------------------------------------------------------------------------*/
// Detected features
const FeatureList & LOD<TerrainSample>::peaks() const {
    ensureAnalyzed();
    return _peaks;
}
const FeatureList & LOD<TerrainSample>::edges() const {
    ensureAnalyzed();
    return _edges;
}
const FeatureList & LOD<TerrainSample>::ridges() const {
    ensureAnalyzed();
    return _ridges;
}

// Region accessors
SizeType LOD<TerrainSample>::regionCount() const {
    if (! object().mapRasterization())  return 1;
    else                                return mapRasterization().regionCount();
}
SizeType LOD<TerrainSample>::regionArea(IDType regionID) const {
    if (! object().mapRasterization())  return size();
    else                                return mapRasterization().regionArea(regionID);
}
const TerrainType::LOD &
LOD<TerrainSample>::regionTerrainType(IDType regionID) const {
    if (! object().mapRasterization())
        return terrainType();
    else
        return mapRasterization().regionTerrainType(regionID);
}

// Global statistics
const Stat & LOD<TerrainSample>::globalElevationStatistics() const {
    ensureAnalyzed();
    return _globalElevationStatistics;
}
const Stat & LOD<TerrainSample>::globalSlopeStatistics() const {
    ensureAnalyzed();
    return _globalSlopeStatistics;
}
const Stat & LOD<TerrainSample>::globalEdgeStrengthStatistics() const {
    ensureAnalyzed();
    return _globalEdgeStrengthStatistics;
}
const Stat & LOD<TerrainSample>::globalEdgeLengthStatistics() const {
    ensureAnalyzed();
    return _globalEdgeLengthStatistics;
}
const Stat & LOD<TerrainSample>::globalEdgeScaleStatistics() const {
    ensureAnalyzed();
    return _globalEdgeScaleStatistics;
}
const Stat & LOD<TerrainSample>::regionElevationStatistics(IDType rID) const {
    ensureAnalyzed();
    INCA_BOUNDS_CHECK(0, _regionCount.size() - 1, regionID, -1,
                      __FUNCTION__ << "(" << regionID << ")")
    if (regionCount() == 1)     return _globalElevationStatistics;
    else                        return _regionElevationStatistics[rID];
}
const Stat & LOD<TerrainSample>::regionSlopeStatistics(IDType rID) const {
    ensureAnalyzed();
    INCA_BOUNDS_CHECK(0, _regionCount.size() - 1, regionID, -1,
                      __FUNCTION__ << "(" << regionID << ")")
    if (regionCount() == 1)     return _globalSlopeStatistics;
    else                        return _regionSlopeStatistics[rID];
}
const Stat & LOD<TerrainSample>::regionEdgeStrengthStatistics(IDType rID) const {
    ensureAnalyzed();
    INCA_BOUNDS_CHECK(0, _regionCount.size() - 1, regionID, -1,
                      __FUNCTION__ << "(" << regionID << ")")
    if (regionCount() == 1)     return _globalEdgeStrengthStatistics;
    else                        return _regionEdgeStrengthStatistics[rID];
}
const Stat & LOD<TerrainSample>::regionEdgeLengthStatistics(IDType rID) const {
    ensureAnalyzed();
    INCA_BOUNDS_CHECK(0, _regionCount.size() - 1, regionID, -1,
                      __FUNCTION__ << "(" << regionID << ")")
    if (regionCount() == 1)     return _globalEdgeLengthStatistics;
    else                        return _regionEdgeLengthStatistics[rID];
}
const Stat & LOD<TerrainSample>::regionEdgeScaleStatistics(IDType rID) const {
    ensureAnalyzed();
    INCA_BOUNDS_CHECK(0, _regionCount.size() - 1, regionID, -1,
                      __FUNCTION__ << "(" << regionID << ")")
    if (regionCount() == 1)     return _globalEdgeScaleStatistics;
    else                        return _regionEdgeScaleStatistics[rID];
}
const FrequencySpectrum & LOD<TerrainSample>::frequencyContent() const {
    ensureAnalyzed();
    return _frequencySpectrum;
}


/*---------------------------------------------------------------------------*
 | Access to parent TerrainSample properties
 *---------------------------------------------------------------------------*/
std::string LOD<TerrainSample>::name() const {
    std::stringstream ss;
    ss << object().name() << '[' << levelOfDetail() << ']';
    return ss.str();
}
IndexType LOD<TerrainSample>::index() const {
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

// Constructor taking a heightfield and a map of integer TerrainType IDs
TerrainSample::TerrainSample(const Heightfield & hf, const IDMap & map,
                             TerrainLOD forLOD)
        : _filename(), _fileLoaded(false), _index(0) {

    // Stick this in as our first LOD heightfield
    this->_lods[forLOD].createFromRaster(hf);
    
    // Create an associated MapRasterization
    TerrainosaurusApplication & app = TerrainosaurusApplication::instance();
    MapRasterizationPtr mr(new MapRasterization(map, forLOD,
                                                app.lastTerrainLibrary()));
    setMapRasterization(mr);
}

// Constructor taking a heightfield
TerrainSample::TerrainSample(const Heightfield & hf,
                             TerrainLOD forLOD)
        : _filename(), _fileLoaded(false), _index(0) {

    // Stick this in as our first LOD heightfield
    this->_lods[forLOD].createFromRaster(hf);
}


/*---------------------------------------------------------------------------*
 | Lazy DEM file loading
 *---------------------------------------------------------------------------*/

// File loading
const std::string & TerrainSample::filename() const { return _filename; }
void TerrainSample::setFilename(const std::string & f) { _filename = f; }

// Lazy loading mechanism
bool TerrainSample::fileLoaded() const { return _fileLoaded; }
void TerrainSample::ensureFileLoaded() const {
    if (! fileLoaded() && filename() != "") {
        try {
            INCA_INFO(__FUNCTION__ << "(): Loading DEM file " << filename())
            TerrainosaurusApplication & app = TerrainosaurusApplication::instance();
            app.loadSourceFiles(const_cast<TerrainSample &>(*this));
            const_cast<TerrainSample*>(this)->_fileLoaded = true;
        } catch (inca::StreamException & e) {
            INCA_DEBUG("Loading file failed: " << e)
        }
    }
}


/*---------------------------------------------------------------------------*
 | Properties
 *---------------------------------------------------------------------------*/
// Associated terrain type (if any)
TerrainTypePtr TerrainSample::terrainType() {
    return _terrainType;
}
TerrainTypeConstPtr TerrainSample::terrainType() const {
    return _terrainType;
}
void TerrainSample::setTerrainType(TerrainTypePtr tt) {
    _terrainType = tt;
}


// Associated MapRasterization (if any)
MapRasterizationPtr TerrainSample::mapRasterization() {
    return _mapRasterization;
}
MapRasterizationConstPtr TerrainSample::mapRasterization() const {
    return _mapRasterization;
}
void TerrainSample::setMapRasterization(MapRasterizationPtr mr) {
    _mapRasterization = mr;
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
