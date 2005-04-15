/*
 * File: TerrainSample.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      A TerrainSample represents a single sample of terrain data, either
 *      loaded from a data file or constructed in-memory. In addition to the
 *      elevation data, it also stores a number of global and local statistics
 *      derived from the elevation data (e.g., gradient, range, etc.).
 *
 *      A TerrainSample is a multi-resolution object, holding multiple levels
 *      of detail (LOD) of its data, in much the same way that mipmaps do for
 *      texture data. See TerrainLOD.hpp for more information.
 *
 * Usage:
 *      Because a TerrainSample has multiple levels of detail, to access a
 *      property of the object requires you to specify which LOD you are
 *      interested in (a zero-based, integer index, with zero being the finest
 *      LOD). For example, to access a pixel of elevation data in the second
 *      LOD:
 *
 *          scalar_t e = ts.elevation(LOD_30m)(10, 10);
 *
 *      However, since you will often be using only one LOD at a time, it
 *      would be much more convenient not to have to specify the LOD index with
 *      every function call. Therefore, TerrainSample provides an interface
 *      called "LOD" encapsulating all the attributes of a particular LOD (This
 *      is implemented as a lightweight proxy class, so all writes and reads
 *      operate on the actual data, not a copy). To access the same data as
 *      in the above example, any of the following forms may be used:
 *
 *          TerrainSample::LOD lod = ts[LOD_30m];
 *          scalar_t e0 = lod.elevation()(10, 10);      // Get whole raster
 *          scalar_t e1 = lod.elevation(10, 10);        // (i, j) coordinates
 *          scalar_t e2 = lod.elevation(Pixel(10, 10)); // Array coordinates
 */

#ifndef TERRAINOSAURUS_DATA_TERRAIN_SAMPLE
#define TERRAINOSAURUS_DATA_TERRAIN_SAMPLE

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// Import LOD template declaration
#include "TerrainLOD.hpp"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainSample;
    class CurveTracker;
    template <typename S> class Curve;
    template <> class LOD<TerrainSample>;   // Specialization of LOD template

    // Pointer typedefs
    typedef shared_ptr<TerrainSample>       TerrainSamplePtr;
    typedef shared_ptr<TerrainSample const> TerrainSampleConstPtr;
};

// Import related data class definitions
#include "TerrainType.hpp"

// Import statistics object definition
#include <inca/math/statistics/Statistics>


template <typename S>
class terrainosaurus::Curve {
public:
    typedef S Scalar;
    typedef inca::math::Point<Scalar, 4> Point;

    Curve(int nSc) : atScale(nSc) { }

    std::vector<Point> points;
    std::vector<int> atScale;
    inca::math::Statistics<Scalar> lengthStats, strengthStats, scaleStats;
};


// Edge-detection tracker class
class terrainosaurus::CurveTracker {
public:
    typedef Curve<scalar_t> Curve;
    typedef Curve::Point    Point;


    // Constructors
    CurveTracker();
    CurveTracker(const std::vector<scalar_t> & sc, GrayscaleImage * img = NULL);

    // Data
    std::vector<scalar_t> scales;
    std::vector<int> atScale;
    std::vector<Curve> curves;
    GrayscaleImage * image;
    inca::math::Statistics<scalar_t> lengthStats, strengthStats, scaleStats;

    template <class P>
    void operator()(const P & p, scalar_t s) {
        Point px(p[0], p[1], p[2], s);
        add(px);
    }

    void begin();
    void add(const Point & p);
    void end();
    void done();
    void print(std::ostream & os);
};


/*****************************************************************************
 * LOD specialization for TerrainSample
 *****************************************************************************/
template <>
class terrainosaurus::LOD<terrainosaurus::TerrainSample>
    : public LODBase<TerrainSample> {
/*---------------------------------------------------------------------------*
 | Type & constant definitions
 *---------------------------------------------------------------------------*/
public:
    // How many "buckets" do we break up the frequency spectrum into?
    static const SizeType frequencyBands = 10;

    typedef Heightfield::SizeArray              SizeArray;
    typedef Heightfield::IndexArray             IndexArray;
    typedef Heightfield::Region                 Region;
    typedef Heightfield::ElementType            Scalar;
    typedef VectorMap::ElementType              Vector;
    typedef inca::math::Statistics<Scalar>      ScalarStatistics;
    typedef inca::math::Statistics<Vector>      VectorStatistics;
    typedef inca::Array<Scalar, frequencyBands> FrequencySpectrum;


/*---------------------------------------------------------------------------*
 | Constructors & fundmental properties
 *---------------------------------------------------------------------------*/
public:
    // Constructors
    explicit LOD();
    explicit LOD(TerrainSamplePtr ts, TerrainLOD lod);

    // Access to related LOD objects
          LOD<TerrainType> & terrainType();
    const LOD<TerrainType> & terrainType() const;


/*---------------------------------------------------------------------------*
 | Loading & analysis
 *---------------------------------------------------------------------------*/
public:
    // Initialization & analysis of elevation data
    template <typename R>
    void createFromRaster(const R & r) { createFromRaster(Heightfield(r)); }
    void createFromRaster(const Heightfield & hf);
    void loadFromFile(const std::string & filename);
    void storeToFile(const std::string & filename) const;
    void resampleFromLOD(TerrainLOD lod);
    void analyze();
    void study();

    // Lazy loading & analysis mechanism
    void ensureLoaded() const;
    void ensureAnalyzed() const;
    void ensureStudied() const;

    // Cache filename
    const std::string & cacheFilename() const;
    void setCacheFilename(const std::string & f);

protected:
    // Filename for caching analysis results
    std::string _cacheFilename;


/*---------------------------------------------------------------------------*
 | Loaded & analyzed data
 *---------------------------------------------------------------------------*/
public:
    // Raster geometry accessors
    SizeType size() const;
    SizeType size(IndexType d) const;
    IndexType base(IndexType d) const;
    IndexType extent(IndexType d) const;
    const SizeArray & sizes() const;
    const IndexArray & bases() const;
    const IndexArray & extents() const;
    const Region & bounds() const;

    // FIXME Orphaned function (needs other versions)
    void setSizes(const SizeArray & sz);

    // Per-cell sample data accessors
    RASTER_PROPERTY_ACCESSORS(Heightfield, elevation)
    RASTER_PROPERTY_ACCESSORS(VectorMap,   gradient)
    RASTER_PROPERTY_ACCESSORS(Heightfield, localElevationMean)
    RASTER_PROPERTY_ACCESSORS(VectorMap,   localGradientMean)
    RASTER_PROPERTY_ACCESSORS(VectorMap,   localElevationLimits)
    RASTER_PROPERTY_ACCESSORS(VectorMap,   localSlopeLimits)

    // Global sample data accessors
    const FrequencySpectrum & frequencyContent() const;
    const ScalarStatistics & globalElevationStatistics() const;
    const ScalarStatistics & globalSlopeStatistics() const;
    const VectorStatistics & globalGradientStatistics() const;
    const CurveTracker & edges() const;
    const CurveTracker & ridges() const;

    Heightfield _edgeImage;
    ScaleSpaceImage scaleSpace;
protected:
    // Per-cell sample data
    Heightfield _elevations, _localElevationMeans;
    VectorMap   _gradients, _localGradientMeans,
                _localElevationLimits, _localSlopeLimits;

    // Global sample data
    FrequencySpectrum   _frequencySpectrum;
    ScalarStatistics    _globalElevationStatistics, _globalSlopeStatistics;
    VectorStatistics    _globalGradientStatistics;
    CurveTracker        _edges, _ridges;


/*---------------------------------------------------------------------------*
 | Access to parent TerrainSample properties
 *---------------------------------------------------------------------------*/
public:
    std::string name() const;
    IndexType index() const;
};


class terrainosaurus::TerrainSample
        : public MultiResolutionObject<TerrainSample> {
/*---------------------------------------------------------------------------*
 | Type & constant definitions
 *---------------------------------------------------------------------------*/
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(TerrainSample);


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    // Create an empty TerrainSample
    explicit TerrainSample();

    // Create a TerrainSample that will be loaded from a file, on demand
    explicit TerrainSample(const std::string & file);

    // Create a TerrainSample from a heightfield representing a particular LOD
    explicit TerrainSample(const Heightfield & hf, TerrainLOD lod);


/*---------------------------------------------------------------------------*
 | Lazy loading and analysis
 *---------------------------------------------------------------------------*/
public:
    // Loading from DEM files
    const std::string & filename() const;
    void loadFromFile(const std::string & filename);

    // Lazy loading & analysis mechanism
    bool fileLoaded() const;
    void ensureFileLoaded() const;

protected:
    // File loading
    std::string _filename;
    bool _fileLoaded;


/*---------------------------------------------------------------------------*
 | Property accessor functions
 *---------------------------------------------------------------------------*/
public:
    // Associated terrain type
    TerrainTypePtr terrainType();
    TerrainTypeConstPtr terrainType() const;
    void setTerrainType(TerrainTypePtr tt);

    std::string name() const;

    IndexType index() const;
    void setIndex(IndexType idx);

protected:
    // Associated terrain type
    TerrainTypePtr _terrainType;

    // Index within TerrainType
    IndexType _index;
};

#endif
