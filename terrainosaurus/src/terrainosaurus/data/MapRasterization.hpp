/*
 * File: MapRasterization.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      A MapRasterization is a rasterized representation of a 2D map, either
 *      loaded from a data file or (more likely) constructed by scan-converting
 *      a rectangular region of a vector map. In addition to recording the
 *      TerrainType at each pixel, it also stores a number of global and local
 *      measurements.
 *
 *      A MapRasterization is a multi-resolution object, holding multiple levels
 *      of detail (LOD) of its data, in much the same way that mipmaps do for
 *      texture data. See TerrainLOD.hpp for more information.
 */

#ifndef TERRAINOSAURUS_DATA_MAP_RASTERIZATION
#define TERRAINOSAURUS_DATA_MAP_RASTERIZATION

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// Import LOD template declaration
#include "TerrainLOD.hpp"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapRasterization;
    template <> class LOD<MapRasterization>;  // Specialization of LOD template

    // Pointer typedefs
    typedef shared_ptr<MapRasterization>       MapRasterizationPtr;
    typedef shared_ptr<MapRasterization const> MapRasterizationConstPtr;
};

// Import associated data classes
#include "Map.hpp"
#include "TerrainLibrary.hpp"
#include "TerrainType.hpp"

// Import container definitions
#include <vector>


/*****************************************************************************
 * LOD specialization for MapRasterization
 *****************************************************************************/
template <>
class terrainosaurus::LOD<terrainosaurus::MapRasterization>
    : public LODBase<MapRasterization> {
/*---------------------------------------------------------------------------*
 | Type & constant definitions
 *---------------------------------------------------------------------------*/
public:
    typedef inca::raster::MultiArrayRaster<IDType, 2>       IDMap;
    typedef inca::raster::MultiArrayRaster<scalar_t, 2>     DistanceMap;
    typedef IDMap::SizeArray                                SizeArray;
    typedef IDMap::IndexArray                               IndexArray;
    typedef IDMap::Region                                   Region;
    typedef std::vector<Region>                             RegionList;
    typedef std::vector<Pixel>                              PixelList;
    typedef std::vector<SizeType>                           SizeList;


/*---------------------------------------------------------------------------*
 | Constructors & fundmental properties
 *---------------------------------------------------------------------------*/
public:
    // Constructors
    explicit LOD();
    explicit LOD(MapRasterizationPtr mr, TerrainLOD lod);

    // Access to related LOD objects
          LOD<TerrainLibrary> & terrainLibrary();
    const LOD<TerrainLibrary> & terrainLibrary() const;
          LOD<TerrainType> & terrainType(IndexType i, IndexType j);
    const LOD<TerrainType> & terrainType(IndexType i, IndexType j) const;
    template <typename IndexList>
    LOD<TerrainType> & terrainType(const IndexList & indices) {
        return terrainType(indices[0], indices[1]);
    }
    template <typename IndexList>
    const LOD<TerrainType> & terrainType(const IndexList & indices) const {
        return terrainType(indices[0], indices[1]);
    }


/*---------------------------------------------------------------------------*
 | Loading & analysis
 *---------------------------------------------------------------------------*/
public:
    // Initialization & analysis of terrain type data
    template <typename R>
    void createFromRaster(const R & r) {
        createFromRaster(Heightfield(r));
    }
    void createFromRaster(const IDMap & ids);
    void loadFromFile(const std::string & filename);
    void resampleFromLOD(TerrainLOD lod);
    void analyze();

    // Lazy loading & analysis mechanism
    void ensureLoaded() const;
    void ensureAnalyzed() const;


/*---------------------------------------------------------------------------*
 | Loaded & analyzed data
 *---------------------------------------------------------------------------*/
public:
    // Per-cell data accessors
    RASTER_PROPERTY_ACCESSORS(IDMap,        terrainTypeID)
    RASTER_PROPERTY_ACCESSORS(IDMap,        regionID)
    RASTER_PROPERTY_ACCESSORS(DistanceMap,  boundaryDistance)

    // Raster geometry accessors
    SizeType size() const;
    SizeType size(IndexType d) const;
    IndexType base(IndexType d) const;
    IndexType extent(IndexType d) const;
    const SizeArray & sizes() const;
    const IndexArray & bases() const;
    const IndexArray & extents() const;
    const Region & bounds() const;

    // Region accessors
    SizeType regionCount() const;
    const TerrainType::LOD & regionTerrainType(IDType regionID) const;
    const Region           & regionBounds(IDType regionID) const;
    const Pixel            & regionSeed(IDType regionID) const;
    SizeType                 regionArea(IDType regionID) const;
    GrayscaleImage regionMask(IDType regionID, int border) const;

protected:
    IDMap               _terrainTypeIDs;
    IDMap               _regionIDs;
    DistanceMap         _boundaryDistances;
    RegionList          _regionBounds;
    PixelList           _regionSeeds;
    SizeList            _regionAreas;


/*---------------------------------------------------------------------------*
 | Access to parent MapRasterization properties
 *---------------------------------------------------------------------------*/
public:
    MapPtr map();
    MapConstPtr map() const;
};


/*****************************************************************************
 * MapRasterization class containing a raster version of a TerrainType map
 *****************************************************************************/
class terrainosaurus::MapRasterization
        : public MultiResolutionObject<MapRasterization> {
/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    // Create a MapRasterization
    explicit MapRasterization(TerrainLibraryPtr tl);

    // Create a MapRasterization from a Map
//    explicit MapRasterization(MapPtr m);

    // Create a MapRasterization from a raster of IDs representing a
    // particular LOD
    explicit MapRasterization(const LOD::IDMap & ids, TerrainLOD forLOD,
                              TerrainLibraryPtr tl);


/*---------------------------------------------------------------------------*
 | Property accessors
 *---------------------------------------------------------------------------*/
public:
    // Access to the associated TerrainLibrary
    TerrainLibraryPtr      terrainLibrary();
    TerrainLibraryConstPtr terrainLibrary() const;
    void setTerrainLibrary(TerrainLibraryPtr tl);

    // Access to the associated vector Map
    MapPtr      map();
    MapConstPtr map() const;
    void setMap(MapPtr m);

protected:
    TerrainLibraryPtr   _terrainLibrary;
    MapPtr              _map;
};

#endif
