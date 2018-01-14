/*
 * File: TerrainLOD.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file declares the TerrainLOD enumeration, which provides symbolic
 *      names for the standard levels of detail. The levels of detail used in
 *      Terrainosaurus were chosen to align with those commonly available in
 *      USGS DEM format (10m, 30m, etc.). The LODs are ordered such that the
 *      highest numerical value corresponds to the finest amount of detail.
 *
 *      Also in this file is the definition of the 'LOD' template, which
 *      acts as a base for level-of-detail views onto other objects
 *      (TerrainSample and MapRasterization are a couple of examples). These
 *      LOD objects contain only the level of detail and a shared_ptr to the
 *      full multi-rez object, so they may be freely copied without a
 *      significant performance penalty. The first template parameter is the
 *      (possibly const-qualified) class that the LOD object references. If
 *      this parameter is declared 'const' then no modification of the full
 *      object will be possible through this LOD view. There are also several
 *      preprocessor macros for simplifying the creation of simple functions
 *      that just call similarly-named functions in the full multi-rez object.
 *
 *      This file also declares various free-functions for querying the
 *      properties of an LOD, such as the spatial resolution and the window
 *      size for computing local properties.
 *
 * Usage:
 *      Other code should use only these symbolic identifiers to reference
 *      levels of detail, enabling new LODs to be made available without
 *      breaking existing code.
 *
 * Implementation:
 *      This enumeration uses the augmented enumeration mechanism defined in
 *      inca/util/Enumeration.hpp. See that file for information on the extra
 *      capabilities this affords over the standard C 'enum'.
 */

#pragma once
#ifndef TERRAINOSAURUS_TERRAIN_LOD
#define TERRAINOSAURUS_TERRAIN_LOD

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// Import upgraded enumeration type
#include <inca/util/Enumeration.hpp>

// Import shared_ptr utilities
#include <boost/enable_shared_from_this.hpp>


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Enumeration of the available LODs
    ENUM( TerrainLOD,
        ( LOD_810m,
        ( LOD_270m,
        ( LOD_90m,
        ( LOD_30m,
        ( LOD_10m,
          BOOST_PP_NIL ))))));


    // Correspondence between horizontal (X,Y) heightfield resolution
    // in pixels / meter and an LOD
    scalar_t metersPerSampleForLOD(TerrainLOD lod);
    scalar_t samplesPerMeterForLOD(TerrainLOD lod);
    TerrainLOD LODForMetersPerSample(scalar_t rez);
    TerrainLOD LODForSamplesPerMeter(scalar_t rez);

    // Scale factor for converting sizes at one LOD into another LOD
    scalar_t scaleFactor(TerrainLOD from, TerrainLOD to);

    // Window size in pixels at a specific LOD
    SizeType windowSize(TerrainLOD lod);

    // At what percentage from the center of the patch to the perimeter the
    // blend function falls off. This is used to compute the blend falloff
    // radius.
    scalar_t blendFalloffRatio(TerrainLOD lod);

    // How "spread out" the blending function is at a specific LOD.
    // This is the standard deviation of the gaussian function.
    scalar_t blendFalloffRadius(TerrainLOD lod);

    // What percentage of a terrain patch should overlap with the
    // neighboring patch.
    scalar_t blendOverlapRatio(TerrainLOD lod);

    // How far apart should patches be placed, such that they overlap correctly
    DifferenceType blendPatchSpacing(TerrainLOD lod);

    // Pixel blending masks of different sizes and shapes
    const GrayscaleImage & gaussianMask(TerrainLOD lod);
    const GrayscaleImage & sphericalMask(TerrainLOD lod);
    const GrayscaleImage & coneMask(TerrainLOD lod);
    const GrayscaleImage & exponentialMask(TerrainLOD lod);


    // The LOD view base class template
    template <typename T>
    class LODBase {
    public:
        // Type declarations
        typedef T                               ObjectType;
        typedef shared_ptr<ObjectType const>    ObjectConstPtr;
        typedef shared_ptr<ObjectType>          ObjectPtr;

        // Default (no-init) constructor
        explicit LODBase()
            : _object(), _levelOfDetail(),
              _loaded(false), _analyzed(false), _studied(false) { }

        // Normal constructor
        explicit LODBase(ObjectPtr obj, TerrainLOD lod)
            : _object(obj), _levelOfDetail(lod),
              _loaded(false), _analyzed(false), _studied(false) { }

        // Parent object accessors
              ObjectType & object()       { return *_object; }
        const ObjectType & object() const { return *_object; }
        ObjectPtr      getObject()       { return _object; }
        ObjectConstPtr getObject() const { return _object; }
        void setObject(ObjectPtr obj)         { _object = obj; }

        // Level of detail enumerant accessor
        TerrainLOD levelOfDetail() const { return _levelOfDetail; }
        void setLevelOfDetail(TerrainLOD lod) { _levelOfDetail = lod; }

        // Lazy loading/analysis flags
        bool loaded()   const { return _loaded; }
        bool analyzed() const { return _analyzed; }
        bool studied()  const { return _studied; }
        virtual void ensureLoaded()   const = 0;
        virtual void ensureAnalyzed() const = 0;
        virtual void ensureStudied()  const = 0;

    protected:
        TerrainLOD  _levelOfDetail;
        ObjectPtr   _object;
        mutable bool _loaded, _analyzed, _studied;
    };

    // The actual LOD template, which must be specialized for different object
    // types.
    template <typename TP> class LOD;


    // Base class for objects that have LODs
    template <typename TP>
    class MultiResolutionObject : public boost::enable_shared_from_this<TP> {
    public:
        // LOD type
        typedef LOD<TP> LOD;
        static const int lodCount = TerrainLOD::count;

        // Constructor
        explicit MultiResolutionObject() : initialized(false) { }

        // LOD accessors
        LOD & operator[](TerrainLOD lod) {
            if (! initialized)
                initialize();
            return _lods[lod];
        }
        const LOD & operator[](TerrainLOD lod) const {
            if (! initialized)
                const_cast<MultiResolutionObject<TP>*>(this)->initialize();
            return _lods[lod];
        }

        // LOD search functions
        TerrainLOD nearestLoadedLODAbove(TerrainLOD lod) const {
            do {
                ++lod;
            } while (lod != TerrainLOD_Overflow && ! _lods[lod].loaded());
            return lod;
        }
        TerrainLOD nearestLoadedLODBelow(TerrainLOD lod) const {
            do {
                --lod;
            } while (lod != TerrainLOD_Underflow && ! _lods[lod].loaded());
            return lod;
        }
        TerrainLOD nearestAnalyzedLODAbove(TerrainLOD lod) const {
            do {
                ++lod;
            } while (lod != TerrainLOD_Overflow && ! _lods[lod].analyzed());
            return lod;
        }
        TerrainLOD nearestAnalyzedLODBelow(TerrainLOD lod) const {
            do {
                --lod;
            } while (lod != TerrainLOD_Underflow && ! _lods[lod].analyzed());
            return lod;
        }

        // Bulk lazy loading & analysis of a range of consituent LODs
        void ensureLoaded() const {
            ensureLoaded(TerrainLOD::minimum(), TerrainLOD::maximum());
        }
        void ensureAnalyzed() const {
            ensureAnalyzed(TerrainLOD::minimum(), TerrainLOD::maximum());
        }
        void ensureStudied() const {
            ensureStudied(TerrainLOD::minimum(), TerrainLOD::maximum());
        }
        void ensureLoaded(TerrainLOD min, TerrainLOD max) const {
            for (TerrainLOD lod = min; lod <= max; ++lod)
                ensureLoaded(lod);
        }
        void ensureAnalyzed(TerrainLOD min, TerrainLOD max) const {
            for (TerrainLOD lod = min; lod <= max; ++lod)
                ensureAnalyzed(lod);
        }
        void ensureStudied(TerrainLOD min, TerrainLOD max) const {
            for (TerrainLOD lod = min; lod <= max; ++lod)
                ensureStudied(lod);
        }
        void ensureLoaded(TerrainLOD lod)   const { (*this)[lod].ensureLoaded(); }
        void ensureAnalyzed(TerrainLOD lod) const { (*this)[lod].ensureAnalyzed(); }
        void ensureStudied(TerrainLOD lod)  const { (*this)[lod].ensureStudied(); }

    protected:
        void initialize() {
            for (TerrainLOD lod = TerrainLOD::minimum(); lod <= TerrainLOD::maximum(); ++lod) {
                _lods[lod].setLevelOfDetail(lod);
                _lods[lod].setObject(this->shared_from_this());
            }
            initialized = true;
        }

        bool initialized;
        inca::Array<LOD, lodCount> _lods;
    };


    #define RASTER_PROPERTY_ACCESSORS(TYPE, NAME)                           \
        const TYPE & NAME ## s() const;                                     \
        TYPE::ConstReference NAME(IndexType i, IndexType j) const {         \
            return NAME ## s()(i, j);                                       \
        }                                                                   \
        template <typename IndexList>                                       \
        typename TYPE::ConstReference NAME(const IndexList & indices) const {\
            return NAME ## s()(indices);                                    \
        }
}

#endif
