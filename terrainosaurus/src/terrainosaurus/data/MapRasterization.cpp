/*
 * File: MapRasterization.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "MapRasterization.hpp"

using namespace terrainosaurus;
using namespace inca;

typedef MapRasterization::LOD::IDMap        IDMap;
typedef MapRasterization::LOD::DistanceMap  DistanceMap;

// Import raster operators
#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/clamp>
#include <inca/raster/operators/resample>
#include <inca/raster/algorithms/flood_fill>
using namespace inca::raster;


// Import Timer definition
#include <inca/util/Timer>

/*****************************************************************************
 * LOD specialization for MapRasterization
 *****************************************************************************/
 // Default constructor
MapRasterization::LOD::LOD() : LODBase<MapRasterization>() { }

// Constructor linking back to MapRasterization
MapRasterization::LOD::LOD(MapRasterizationPtr mr, TerrainLOD lod)
    : LODBase<MapRasterization>(mr, lod) { }


// Access to related LOD objects
TerrainLibrary::LOD & MapRasterization::LOD::terrainLibrary() {
    return (*object().terrainLibrary())[levelOfDetail()];
}
const TerrainLibrary::LOD & MapRasterization::LOD::terrainLibrary() const {
    return (*object().terrainLibrary())[levelOfDetail()];
}
TerrainType::LOD & MapRasterization::LOD::terrainType(IndexType i, IndexType j) {
    return terrainLibrary().terrainType(terrainTypeID(i, j));
}
const TerrainType::LOD & MapRasterization::LOD::terrainType(IndexType i, IndexType j) const {
    return terrainLibrary().terrainType(terrainTypeID(i, j));
}


/*---------------------------------------------------------------------------*
 | Loading & analysis
 *---------------------------------------------------------------------------*/
// Copy LOD data from an existing heightfield
void MapRasterization::LOD::createFromRaster(const IDMap & ids) {
    _terrainTypeIDs = ids;
    _loaded   = true;
    _analyzed = false;
}

// Load an LOD from a DEM file
void MapRasterization::LOD::loadFromFile(const std::string & filename) {
    INCA_ERROR("MR::loadFromFile not implemented")

    // Record what we did
    _loaded   = true;
    _analyzed = false;
}

// Generate elevation data by resampling from another LOD
void MapRasterization::LOD::resampleFromLOD(TerrainLOD lod) {
    _terrainTypeIDs = resample(object()[lod].terrainTypeIDs(),
                               scaleFactor(lod, levelOfDetail()));
    _loaded   = true;
    _analyzed = false;
}


// This is a rather dump, brute-force approach to finding the minimum distance to boundaries.
// It first sweeps across the image along 'dim' in both directions and tracks the distance
// from boundaries. Then it propagates these values in both directions along the other
// dimension. The actual minimum distance is found by running this algorithm along each
// dimension of the image and taking the minimum of all passes.
template <typename R0, typename R1>
void sweep(R0 & dist, const R1 & map, IndexType dim) {
    typedef typename R0::ElementType Distance;
    typedef typename R1::ElementType IDType;
    Pixel px;
    IndexType otherDim = (dim == 1) ? 0 : 1;

    // First, sweep both ways along 'dim', storing the distance
    for (px[otherDim] = map.base(otherDim); px[otherDim] <= map.extent(otherDim); ++px[otherDim]) {
        bool haveReferencePoint = false;
        IDType previousCellID = map(map.base(dim), px[otherDim]);
        Distance linearDistance = std::numeric_limits<Distance>::max();
        for (px[dim] = map.base(dim); px[dim] <= map.extent(dim); ++px[dim]) {
            IDType currentCellID = map(px);
            if (currentCellID != previousCellID) {
                previousCellID = currentCellID;
                haveReferencePoint = true;
                linearDistance = 0;
            } else if (haveReferencePoint) {
                ++linearDistance;
            }
            Distance & d = dist(px);
            d = linearDistance;
        }
    }
    for (px[otherDim] = map.base(otherDim); px[otherDim] <= map.extent(otherDim); ++px[otherDim]) {
        bool haveReferencePoint = false;
        IDType previousCellID = map(map.extent(dim), px[otherDim]);
        Distance linearDistance = std::numeric_limits<Distance>::max();
        for (px[dim] = map.extent(dim); px[dim] >= map.base(dim); --px[dim]) {
            IDType currentCellID = map(px);
            if (currentCellID != previousCellID) {
                previousCellID = currentCellID;
                haveReferencePoint = true;
                linearDistance = 0;
            } else if (haveReferencePoint) {
                ++linearDistance;
            }
            Distance & d = dist(px);
            if (linearDistance < d)
                d = linearDistance;
        }
    }

    // Now propagate values along the 'otherDim' for contiguous regions
    Pixel prev;
    for (px[otherDim] = map.base(otherDim) + 1; px[otherDim] <= map.extent(otherDim); ++px[otherDim]) {
        prev[otherDim] = px[otherDim] - 1;
        prev[dim] = map.base(dim);
        for (px[dim] = map.base(dim); px[dim] <= map.extent(dim); ++px[dim], ++prev[dim]) {
            IDType currentCellID = map(px);
            IDType previousCellID = map(prev);
            if (currentCellID == previousCellID) {
                Distance & d = dist(px);
                Distance p = dist(prev) + 1;
                if (p < d)
                    d = p;
            }
        }
    }
    for (px[otherDim] = map.extent(otherDim) - 1; px[otherDim] >= map.base(otherDim); --px[otherDim]) {
        prev[otherDim] = px[otherDim] + 1;
        prev[dim] = map.base(dim);
        for (px[dim] = map.base(dim); px[dim] <= map.extent(dim); ++px[dim], ++prev[dim]) {
            IDType currentCellID = map(px);
            IDType previousCellID = map(prev);
            if (currentCellID == previousCellID) {
                Distance & d = dist(px);
                Distance p = dist(prev) + 1;
                if (p < d)
                    d = p;
            }
        }
    }
}


// Analyze the contents of an LOD and store the results
void MapRasterization::LOD::analyze() {
    // Make sure we have a valid map to start with
    ensureLoaded();

    Timer<float, false> total, phase;
    total.start();

    std::stringstream report;
    report << "Analyzing MapRasterization(" << levelOfDetail() << ")...\n";

    // First, we need to make sure that this LOD has data
    const IDMap::Region & bounds = _terrainTypeIDs.bounds();
    if (! (bounds.size() > 0)) {
        std::cerr << "No data in MR::LOD " << levelOfDetail() << std::endl;
        ::exit(1);      // FIXME: This should throw a meaningful exception
    }

    // Before we do anything else, we need to make sure the IDs in our map
    // correspond to actual TerrainTypes in the TerrainLibrary. At the same
    // time, we can also check to make sure that there are actually multiple
    // TerrainTypes present (if not, we can skip most of the rest of this).
    report << "\tChecking TerrainType ID validity...";
    phase.start();
    Pixel px;
    SizeType librarySize = terrainLibrary().size();
    IDType refCellID = *_terrainTypeIDs.begin();
    bool multipleTypes = false;
    for (px[1] = bounds.base(1); px[1] <= bounds.extent(1); ++px[1]) {
        for (px[0] = bounds.base(0); px[0] <= bounds.extent(0); ++px[0]) {
            IDType cellID = _terrainTypeIDs(px);
            if (SizeType(cellID) < 0 || SizeType(cellID) >= librarySize) {
                INCA_WARNING("Out of bounds TT ID " << cellID << " at " << px)
                ::exit(1);      // FIXME: This should throw a meaningful exception
            } else if (refCellID != cellID) {
                multipleTypes = true;
            }
        }
    }

    phase.stop();
    report << phase() << " seconds\n";


    // Now, we're going to go back and find the regions in the map. This may or
    // may not correspond exactly to the regions in the vector map used to
    // generate this rasterization, because regions may be too small to appear,
    // or may be outside the rasterized region.
    report << "\tFinding regions...";
    phase.start();

    _regionIDs.setSizes(sizes());
    IDType nextRegionID = 1;

    if (! multipleTypes) {
        fill(_regionIDs, 1);            // Only one region fills the whole thing
        _regionBounds.push_back(bounds);
        _regionSeeds.push_back(Pixel(bounds.bases()));
        _regionAreas.push_back(bounds.size());
        std::cerr << "Found only one solid, rectangular region\n";

    } else {
        fill(_regionIDs, 0);            // Start with no regions marked

        for (px[1] = bounds.base(1); px[1] <= bounds.extent(1); ++px[1]) {
            for (px[0] = bounds.base(0); px[0] <= bounds.extent(0); ++px[0]) {
                IDType currentTTID = _terrainTypeIDs(px);
                if (currentTTID != 0) {
                    IDType currentRegionID = _regionIDs(px);
                    if (currentRegionID == 0) { // Hey! We found a new region!
                        std::pair<Region, SizeType> result =
                            flood_fill(_regionIDs, px,
                                       EqualsValue<IDMap>(_terrainTypeIDs, currentTTID),
                                       constant<IDType, 2>(nextRegionID++));
                        _regionBounds.push_back(result.first);
                        _regionSeeds.push_back(px);
                        _regionAreas.push_back(result.second);
                        std::cerr << "Found " << _regionAreas.back()
                                  << " pixel region spanning "
                                  << _regionBounds.back() << std::endl;
                    }
                }
            }
        }

        std::cerr << "Found " << (nextRegionID - 1) << " regions\n";
    }

    phase.stop();
    report << phase() << " seconds\n";


    // Now, we create the boundary distance image.
    report << "\tCalculating boundary distances...";
    phase.start();

    _boundaryDistances.setBounds(bounds);

    if (! multipleTypes) {
        fill(_boundaryDistances,
             std::numeric_limits<DistanceMap::ElementType>::max());
    } else {
        DistanceMap tmp(bounds);
        sweep(tmp, _terrainTypeIDs, 0);
        sweep(_boundaryDistances, _terrainTypeIDs, 1);
        _boundaryDistances = min(_boundaryDistances, tmp);
    }

    phase.stop();
    report << phase() << " seconds\n";


    total.stop();
    report << "Analysis took " << total() << " seconds\n";

    // Record what we did
    std::cerr << report.str();
    _analyzed = true;
}


void MapRasterization::LOD::ensureLoaded() const {
    if (! loaded()) {
        // First, see if our parent object has a file that it hasn't
        // loaded yet. We might be in that file.
//        object().ensureFileLoaded();

        // If we're still not loaded, let's look for another LOD that we
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
                std::cerr << "MR::ensureLoaded(" << levelOfDetail() << "): "
                             "Resampling from " << ref << std::endl;
                const_cast<MapRasterization::LOD *>(this)->resampleFromLOD(ref);

            // If that STILL didn't work...I don't know what to tell you
            } else {
                std::cerr << "MR::ensureLoaded(" << levelOfDetail() << "): "
                             "Unable to load LOD -- no other LODs exist and "
                             "no filename was specified...giving up" << std::endl;
            }
        }
    }
}
void MapRasterization::LOD::ensureAnalyzed() const {
    if (! analyzed())
        const_cast<MapRasterization::LOD *>(this)->analyze();
}


/*---------------------------------------------------------------------------*
 | Loaded & analyzed data
 *---------------------------------------------------------------------------*/
// Raster geometry accessors
SizeType MapRasterization::LOD::size() const {
    ensureLoaded();
    return _terrainTypeIDs.size();
}
SizeType MapRasterization::LOD::size(IndexType d) const {
    ensureLoaded();
    return _terrainTypeIDs.size(d);
}
IndexType MapRasterization::LOD::base(IndexType d) const {
    ensureLoaded();
    return _terrainTypeIDs.base(d);
}
IndexType MapRasterization::LOD::extent(IndexType d) const {
    ensureLoaded();
    return _terrainTypeIDs.extent(d);
}
const MapRasterization::LOD::SizeArray & MapRasterization::LOD::sizes() const {
    ensureLoaded();
    return _terrainTypeIDs.sizes();
}
const MapRasterization::LOD::IndexArray & MapRasterization::LOD::bases() const {
    ensureLoaded();
    return _terrainTypeIDs.bases();
}
const MapRasterization::LOD::IndexArray & MapRasterization::LOD::extents() const {
    ensureLoaded();
    return _terrainTypeIDs.extents();
}
const MapRasterization::LOD::Region & MapRasterization::LOD::bounds() const {
    ensureLoaded();
    return _terrainTypeIDs.bounds();
}

// Per-cell data accessors
const IDMap & MapRasterization::LOD::terrainTypeIDs() const {
    ensureLoaded();
    return _terrainTypeIDs;
}
const IDMap & MapRasterization::LOD::regionIDs() const {
    ensureAnalyzed();
    return _regionIDs;
}
const DistanceMap & MapRasterization::LOD::boundaryDistances() const {
    ensureAnalyzed();
    return _boundaryDistances;
}

// Region accessors
SizeType MapRasterization::LOD::regionCount() const {
    ensureAnalyzed();
    return _regionBounds.size();
}
const TerrainType::LOD &
MapRasterization::LOD::regionTerrainType(IDType regionID) const {
    ensureAnalyzed();
    INCA_BOUNDS_CHECK(0, _regionSeeds.size() - 1, regionID, -1,
                      "regionTerrainType(" << regionID << ")")
    return terrainType(_regionSeeds[regionID]);
}
const MapRasterization::LOD::Region &
MapRasterization::LOD::regionBounds(IDType regionID) const {
    ensureAnalyzed();
    INCA_BOUNDS_CHECK(0, _regionBounds.size() - 1, regionID, -1,
                      "regionBounds(" << regionID << ")")
    return _regionBounds[regionID];
}
const Pixel & MapRasterization::LOD::regionSeed(IDType regionID) const {
    ensureAnalyzed();
    INCA_BOUNDS_CHECK(0, _regionSeeds.size() - 1, regionID, -1,
                      "regionSeed(" << regionID << ")")
    return _regionSeeds[regionID];
}
SizeType MapRasterization::LOD::regionArea(IDType regionID) const {
    ensureAnalyzed();
    INCA_BOUNDS_CHECK(0, _regionAreas.size() - 1, regionID, -1,
                      "regionArea(" << regionID << ")")
    return _regionAreas[regionID];
}
GrayscaleImage MapRasterization::LOD::regionMask(IDType regionID, int borderWidth) const {
    ensureAnalyzed();
    INCA_BOUNDS_CHECK(0, _regionBounds.size() - 1, regionID, -1,
                      "regionMask(" << regionID << ")")
    Region bounds = _regionBounds[regionID];        // Get the region bounds
    bounds.expand(borderWidth / 2);                 // Expand to hold fuzzy border
    bounds.clipAgainst(_regionIDs.bounds());        // Keepin' it legal
    GrayscaleImage mask(bounds);
    Pixel px;
    float scale = 2.0f / borderWidth;
    for (px[0] = mask.base(0); px[0] <= mask.extent(0); ++px[0])
        for (px[1] = mask.base(1); px[1] <= mask.extent(1); ++px[1])
            if (_regionIDs(px) == regionID + 1) // Inside region
                mask(px) = 0.5f * std::min(2.0f, 1.0f + _boundaryDistances(px) * scale);
            else                                // Outside region
                mask(px) = 0.5f * std::max(0.0f, 1.0f - _boundaryDistances(px) * scale);
    return mask;
}


/*---------------------------------------------------------------------------*
 | Access to parent MapRasterization properties
 *---------------------------------------------------------------------------*/
MapPtr MapRasterization::LOD::map() {
    return object().map();
}
MapConstPtr MapRasterization::LOD::map() const {
    return object().map();
}


/*****************************************************************************
 * MapRasterization class containing a raster version of a TerrainType map
 *****************************************************************************/
/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
MapRasterization::MapRasterization(const LOD::IDMap & ids, TerrainLOD forLOD,
                                   TerrainLibraryPtr tl) {
    setTerrainLibrary(tl);

    // Create our first LOD
    this->_lods[forLOD].createFromRaster(ids);
}

MapRasterization::MapRasterization(TerrainLibraryPtr tl) {
    setTerrainLibrary(tl);
}


/*---------------------------------------------------------------------------*
 | Properties
 *---------------------------------------------------------------------------*/
// Access to the associated TerrainLibrary
TerrainLibraryPtr MapRasterization::terrainLibrary() {
    return _terrainLibrary;
}
TerrainLibraryConstPtr MapRasterization::terrainLibrary() const {
    return _terrainLibrary;
}
void MapRasterization::setTerrainLibrary(TerrainLibraryPtr tl) {
    _terrainLibrary = tl;
}


// Access to the associated vector Map
MapPtr MapRasterization::map() {
    return _map;
}
MapConstPtr MapRasterization::map() const {
    return _map;
}
void MapRasterization::setMap(MapPtr m) {
    _map = m;
}
