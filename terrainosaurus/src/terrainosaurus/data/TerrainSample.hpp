/** -*- C++ -*-
 *
 * \file    TerrainSample.hpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes only.
 *
 * Description:
 *      A TerrainSample represents a single sample of terrain data (i.e., a
 *      rectangular height field), either loaded from a data file or constructed
 *      in-memory by some other means (such as the GA). A terrain sample is
 *      logically composed of a set of (generally non-rectangular) adjacent
 *      "regions" of different, semantic terrain types (e.g., "mountains",
 *      "plains", "desert"), though it is quite common for a terrain to be
 *      entirely covered by a rectangular region of a single terrain type. 
 *
 *      A TerrainSample is a multi-resolution object, holding multiple levels
 *      of detail (LOD) of its data, in much the same way that mipmaps do for
 *      texture data. See TerrainLOD.hpp for more information.
 *
 *      A TerrainSample LOD is an information-rich (and therefore rather large)
 *      representation of a terrain, containing numerous raster fields, as well
 *      as scalar and vector measurements of terrain characteristics. Some
 *      important raster (i.e., per cell) quantities stored include:
 *          * the elevation
 *          * the semantic terrain type
 *          * the distance to the nearest region boundary
 *          * the gradient
 *          * the (zero-based) ID of the enclosing region
 *      Some important non-raster quantities stored include elevation and slope
 *      statistics, ridges (and other features) present in the LOD, and the
 *      area covered by each region.
 *
 *      Furthermore, an LOD can be studied in more depth (a rather
 *      computationally intensive process) to calculate aggregate, windowed
 *      quantities (e.g., the mean elevation of the neighborhood around a cell).
 *
 *      Each LOD of the TerrainSample is logically a single raster with a
 *      rich set of measurements available for each point in the raster. In
 *      other words, even though the LOD is implemented as a number of
 *      separate raster objects, these are all guaranteed to have the same
 *      size and bounds, and the LOD exposes an iterator interface allowing
 *      the grid to be traversed and accessed as though it were a single raster
 *      of structs.
 *      TODO: LOD iterator interface has not been implemented
 *
 * Implementation notes:
 *      Using the iterator interface described above, which treats the entire
 *      LOD as a unified raster, is likely to be more efficient than accessing
 *      the individual rasters separately, since the 2D -> 1D address
 *      calculation can be cached.
 *
 *      Since loading, analyzing and studying terrains are rather expensive
 *      processes, these are done on a lazy basis, allowing the TerrainSample
 *      object to be created inexpensively, and further constructed as
 *      necessary. Furthermore, since these values are often highly static (as
 *      in the case of terrain samples acquired from a GIS data provider), they
 *      can be computed only once and stored in a cache file on disk, enabling
 *      future loading and analysis to be done much more quickly.
 */

// FIXME: the meaning of "Region" is overloaded in this context:
//          1) a non-rectangular part of the TS
//          2) the rectangular bounds covered by a raster
// TODO: Perhaps the whole LOD could be re-cast as a raster?

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
    template <> class LOD<TerrainSample>;   // Specialization of LOD template

    std::istream & operator>>(std::istream &, LOD<TerrainSample> &);
    std::ostream & operator<<(std::ostream &, const LOD<TerrainSample> &);

    // Pointer typedefs
    typedef shared_ptr<TerrainSample>       TerrainSamplePtr;
    typedef shared_ptr<TerrainSample const> TerrainSampleConstPtr;
};

// Import related data class definitions
#include "TerrainType.hpp"
#include "MapRasterization.hpp"

// Import statistics object definition
#include <inca/math/statistics/Statistics>


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
    // Grant friend access to IOstream operators
    friend std::istream & ::terrainosaurus::operator>>(std::istream &,
                                                       LOD<TerrainSample> &);
    friend std::ostream & ::terrainosaurus::operator<<(std::ostream &,
                                                       const LOD<TerrainSample> &);

    // How many "buckets" do we break up the frequency spectrum into?
    static const SizeType frequencyBands = 10;

    // Raster types
    typedef terrainosaurus::Heightfield                     Heightfield;
    typedef terrainosaurus::VectorMap                       VectorMap;

    typedef Heightfield::SizeArray              SizeArray;
    typedef Heightfield::IndexArray             IndexArray;
    typedef Heightfield::Region                 Region;
    typedef Heightfield::ElementType            Scalar;
    typedef VectorMap::ElementType              Vector;
    typedef inca::math::Statistics<Scalar>      Stat;
    typedef std::vector<Stat>                   StatList;
    typedef inca::Array<Scalar, frequencyBands> FrequencySpectrum;

    // Feature types
    class Feature {
    public:
        // Possible features types
        enum Type {
            Peak,
            Edge,
            Ridge,
        };
        typedef inca::math::Point<Scalar, 4>    Point;
        typedef std::vector<Point>              PointList;
        
        // Constructor
        explicit Feature() { }
        explicit Feature(Type t) : type(t), length(0) { }

        // Data
        Type        type;
        Scalar      length;
        PointList   points;
        Stat        strengthStats,
                    scaleStats;
    };

    typedef std::vector<Feature> FeatureList;


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

          LOD<MapRasterization> & mapRasterization();
    const LOD<MapRasterization> & mapRasterization() const;


/*---------------------------------------------------------------------------*
 | Loading & analysis
 *---------------------------------------------------------------------------*/
public:
    // Initialization & analysis of elevation data
    template <typename R0>
    void createFromRaster(const R0 & hf) {
        createFromRaster(Heightfield(hf));
    }
    void createFromRaster(const Heightfield & hf);
    void resampleFromLOD(TerrainLOD lod);
    void analyze();
    void study();

    // Lazy loading & analysis mechanism
    void ensureLoaded() const;
    void ensureAnalyzed() const;
    void ensureStudied() const;

protected:
    // Analysis steps
    void _calculateFrequencySpectrum();
    void _calculateStatistics();
    void _findFeatures();


/*---------------------------------------------------------------------------*
 | Raster geometry accessors
 *---------------------------------------------------------------------------*/
public:
    // Raster geometry getters
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


/*---------------------------------------------------------------------------*
 | Per-cell properties
 *---------------------------------------------------------------------------*/
public:
    // Fundamental properties (initialized at load-time)
    RASTER_PROPERTY_ACCESSORS(Heightfield,  elevation)

    // Derived properties (initialized at analysis-time)
    RASTER_PROPERTY_ACCESSORS(VectorMap,    gradient)
    RASTER_PROPERTY_ACCESSORS(ColorImage,   featureMap)

    // Windowed properties (initialized at study-time)
    RASTER_PROPERTY_ACCESSORS(Heightfield,  localElevationMean)
    RASTER_PROPERTY_ACCESSORS(VectorMap,    localGradientMean)
    RASTER_PROPERTY_ACCESSORS(VectorMap,    localElevationLimits)
    RASTER_PROPERTY_ACCESSORS(VectorMap,    localSlopeLimits)

protected:
    Heightfield _elevations;

    VectorMap   _gradients;
    ColorImage  _featureMap;
 
    Heightfield _localElevationMeans;
    VectorMap   _localGradientMeans,
                _localElevationLimits,
                _localSlopeLimits;


/*---------------------------------------------------------------------------*
 | Global and per-region properties
 *---------------------------------------------------------------------------*/
public:
    // Features
    const FeatureList & peaks() const;
    const FeatureList & edges() const;
    const FeatureList & ridges() const;

    // Regions
    SizeType regionCount() const;
    SizeType regionArea(IDType regionID) const;
    const LOD<TerrainType> & regionTerrainType(IDType regionID) const;

    // Per-region, derived properties (initialized at analysis-time)
    const Stat & regionElevationStatistics(IDType regionID) const;
    const Stat & regionSlopeStatistics(IDType regionID) const;
    const Stat & regionEdgeStrengthStatistics(IDType regionID) const;
    const Stat & regionEdgeLengthStatistics(IDType regionID) const;
    const Stat & regionEdgeScaleStatistics(IDType regionID) const;

    // Global, derived properties (initialized at analysis-time)
    const Stat & globalElevationStatistics() const;
    const Stat & globalSlopeStatistics() const;
    const Stat & globalEdgeStrengthStatistics() const;
    const Stat & globalEdgeLengthStatistics() const;
    const Stat & globalEdgeScaleStatistics() const;
    const FrequencySpectrum & frequencyContent() const;

protected:
    FeatureList _peaks,
                _edges,
                _ridges;

    StatList    _regionElevationStatistics,
                _regionSlopeStatistics,
                _regionEdgeLengthStatistics,
                _regionEdgeScaleStatistics,
                _regionEdgeStrengthStatistics;

    Stat        _globalElevationStatistics,
                _globalSlopeStatistics,
                _globalEdgeLengthStatistics,
                _globalEdgeScaleStatistics,
                _globalEdgeStrengthStatistics;

    FrequencySpectrum   _frequencySpectrum;


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

    // Create a TerrainSample from a heightfield and a map, representing a
    // particular LOD
    explicit TerrainSample(const Heightfield & hf,
                           const IDMap & map,
                           TerrainLOD lod);

    // Create a TerrainSample from a heightfield and a single TerrainType,
    // representing a particular LOD
    explicit TerrainSample(const Heightfield & hf,
                           TerrainLOD lod);


/*---------------------------------------------------------------------------*
 | Lazy loading and analysis
 *---------------------------------------------------------------------------*/
public:
    // Loading from DEM files
    const std::string & filename() const;
    void setFilename(const std::string & f);

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
    // Associated TerrainType (if any)
    TerrainTypePtr      terrainType();
    TerrainTypeConstPtr terrainType() const;
    void setTerrainType(TerrainTypePtr tt);
    
    // Associated MapRasterization (if any)
    MapRasterizationPtr      mapRasterization();
    MapRasterizationConstPtr mapRasterization() const;
    void setMapRasterization(MapRasterizationPtr mr);

    std::string name() const;

    IndexType index() const;
    void setIndex(IndexType idx);

protected:
    // Associated terrain type (if any)
    TerrainTypePtr _terrainType;
    
    // Associated map rasterization (if any)
    MapRasterizationPtr _mapRasterization;

    // Index within TerrainType
    IndexType _index;
};

#endif
