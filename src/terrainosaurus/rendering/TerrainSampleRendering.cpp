/*
 * File: TerrainSampleRendering.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// TODO: This could be greatly optimized
//      1) Make the geo rebuild only do Z-axis (leaving along the X,Y)
//      2) Make sure raster is doing ref-counting
//      3) Intelligently choose the rebuild window
//      4) Perhaps build (display-list-ify) in tiles?

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "TerrainSampleRendering.hpp"
#include <inca/raster/generators/constant>
#include <inca/raster/operators/statistic>
#include <inca/raster/operators/gradient>
#include <inca/raster/algorithms/fill>
using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::raster;

// HACK -- can we remove the direct dependence on OpenGL?
#include <inca/integration/opengl/GL.hpp>

// Features that we support
#define FEATURE_COUNT   5
#define AS_WIREFRAME    0
#define AS_POINTS       1
#define AS_POLYGONS     2
#define NORMALS         3
#define FOG             4

// HACK: things we can't change
#define FOG_MODEL       Linear
#define FOG_DENSITY     0.005f
#define FOG_START_DEPTH 5000.0f
#define FOG_END_DEPTH   25000.0f
#define FOG_COLOR       Color(0.0f, 0.0f, 0.0f, 1.0f)
#define NORMAL_COLOR    Color(1.0f, 1.0f, 0.0f, 1.0f)
#define FALLBACK_COLOR  Color(0.8f, 0.8f, 0.8f, 1.0f)


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
TerrainSampleRendering::TerrainSampleRendering()
        : _features(FEATURE_COUNT, false),
          _geometryDirty(false), _colorMapDirty(false), _setupComplete(false),
          _displayListBase(0), _displayListValid(FEATURE_COUNT, false) {
    _features[AS_POLYGONS] = true;
}
TerrainSampleRendering::TerrainSampleRendering(const TerrainSample::LOD & ts)
        : _features(FEATURE_COUNT, false),
          _geometryDirty(false), _colorMapDirty(false), _setupComplete(false),
          _displayListBase(0), _displayListValid(FEATURE_COUNT, false) {
    _features[AS_POLYGONS] = true;
    load(ts);
}


/*---------------------------------------------------------------------------*
 | Grid (re)construction functions
 *---------------------------------------------------------------------------*/
// Populate the primitive grid with values from a heightfield
void TerrainSampleRendering::load(const TerrainSample::LOD & tsl) {
    // Make sure this is the right size
    this->resize(tsl.sizes());

    // Calculate how much to translate & scale by in each direction
    _calculateScaleAndOffset(tsl);

    // Copy the elevations
    _elevations = tsl.elevations();
    
    // Copy the gradients, if we've already computed 'em, else find 'em
    _gradients = tsl.analyzed()
        ? tsl.gradients()
        : gradient(_elevations, Vector2D(_xAxisScale, _yAxisScale));

	// Get/make the map of colors
	if (tsl.object().mapRasterization()) {
		_colors = tsl.mapRasterization().colors();
	} else if (tsl.object().terrainType()) {
		_colors.setSizes(tsl.sizes());
		inca::raster::fill(_colors, tsl.terrainType().color());
	} else {
		_colors.setSizes(tsl.sizes());
		inca::raster::fill(_colors, FALLBACK_COLOR);
	}

	// Mark everything "dirty", then rebuild
	_geometryDirty = true;
	_colorMapDirty = true;
	_rebuildGeometry();
	_rebuildColorMap();
}


/*---------------------------------------------------------------------------*
 | (Re)construction functions
 *---------------------------------------------------------------------------*/
// Calculate basic scale & offset properties of the TerrainSample
void TerrainSampleRendering::_calculateScaleAndOffset(const TerrainSample::LOD & tsl) {
    // Determine the scale in each of the axial directions
    _xAxisScale = metersPerSampleForLOD(tsl.levelOfDetail());
    _yAxisScale = _xAxisScale;
    _zAxisScale = 1.0f;

    // Figure out the offsets along each axis to center the mesh at the origin.
    // In the case of the Z-axis, we use the pre-calculated mean if it has
    // already been calculated, and if not, we do it ourselves (so as not to
    // trigger the rather heavy analysis process).
    _xAxisOffset = tsl.size(0) / -2.0f;
    _yAxisOffset = tsl.size(1) / -2.0f;
    
    if (! _setupComplete) {
        _zAxisOffset = tsl.analyzed()
            ? -tsl.globalElevationStatistics().mean()
            : -inca::raster::mean(tsl.elevations());
        _setupComplete = true;
    }
}

// Recalculate the geometry of the heightfield (elevation & gradient)
void TerrainSampleRendering::_rebuildGeometry(const Region & r) {
    INCA_DEBUG("Rebuilding geometry in the region " << r)

    Pixel px;
    int nanCount = 0, goodCount = 0;
    for (px[1] = r.base(1); px[1] <= r.extent(1); ++px[1])
        for (px[0] = r.base(0); px[0] <= r.extent(0); ++px[0]) {
            scalar_t h = _elevations(px);
            Vector2D n = _gradients(px);
#if DEBUG
            if (std::isnan(h))      nanCount++;
            else                    goodCount++;
#endif
            Primitive p = this->primitive(px);
            p.vertex() = Point3D((px[0] + _xAxisOffset) * _xAxisScale,
                                 (px[1] + _yAxisOffset) * _yAxisScale,
                                 (h     + _zAxisOffset) * _zAxisScale);
            p.normal() = normalize(Vector3D(n[0], n[1], scalar_t(1)));
        }

#if DEBUG
    INCA_DEBUG(nanCount << " NaNs and " << goodCount << " good values")
    INCA_DEBUG("Mean height is " << meanHeight)
#endif

    // All is well
    _geometryDirty = false;

    // But now the display lists are stale
	for (IndexType f = 0; f < FEATURE_COUNT; ++f)
	    _displayListValid[f] = false;
}


// Recalculate the color map
void TerrainSampleRendering::_rebuildColorMap(const Region & r) {
    INCA_DEBUG("Rebuilding color map")

    // Copy the colors, but blur in the neighbors a bit
    Pixel px;
    for (px[1] = r.base(1); px[1] <= r.extent(1); ++px[1])
        for (px[0] = r.base(0); px[0] <= r.extent(0); ++px[0]) {
            Color c = _colors(px) * 2;
            if (px[0] > r.base(0))      c += _colors(px[0] - 1, px[1]);
            if (px[0] < r.extent(0))    c += _colors(px[0] + 1, px[1]);
            if (px[1] > r.base(1))      c += _colors(px[0], px[1] - 1);
            if (px[1] < r.extent(1))    c += _colors(px[0], px[1] + 1);
            color(px) = c / 6;
        }

    // Done with this
    _colorMapDirty = false;

    // But now the display lists are stale
	for (IndexType f = 0; f < FEATURE_COUNT; ++f)
	    _displayListValid[f] = false;
}


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
// Feature toggle function
bool TerrainSampleRendering::toggle(const std::string & feature) {
    IndexType index;
    if (feature == "Wireframe")
        index = AS_WIREFRAME;
    else if (feature == "Points")
        index = AS_POINTS;
    else if (feature == "Polygons")
        index = AS_POLYGONS;
    else if (feature == "Normals")
        index = NORMALS;
    else if (feature == "Fog")
        index = FOG;
    else
        index = -1;
    _features.at(index) = ! _features.at(index);
    INCA_INFO(feature << (_features.at(index) ? " on" : " off"))
    return _features.at(index);
}


// Rendering functor
void TerrainSampleRendering::operator()(TerrainSampleRendering::Renderer & renderer) const {
    if (this->size() == 0)      // Nothing to do...
        return;

    Renderer::Rasterizer & rasterizer = renderer.rasterizer();
    bool lightingEnabled = rasterizer.isLightingEnabled();
    bool fogEnabled      = rasterizer.isFogEnabled();

    // If we've not yet allocated display lists, do that now
    if (_displayListBase == 0)
        _displayListBase = GL::glGenLists(FEATURE_COUNT);

    // Set up fog, if enabled
    if (_features[FOG]) {
        rasterizer.setFogEnabled(true);
        rasterizer.setFogModel(FOG_MODEL);
        rasterizer.setFogColor(FOG_COLOR);
        rasterizer.setFogStartDepth(FOG_START_DEPTH);
        rasterizer.setFogEndDepth(FOG_END_DEPTH);
        rasterizer.setFogDensity(FOG_DENSITY);
    }

    if (_features[AS_POLYGONS]) {
        if (! _displayListValid[AS_POLYGONS]) {
            INCA_DEBUG("Regenerating Polygon display list")
            GL::glNewList(_displayListBase + AS_POLYGONS, GL_COMPILE);
                GL::glEnable(GL_COLOR_MATERIAL);
                GL::glEnable(GL_POLYGON_OFFSET_FILL);
                rasterizer.setPolygonOffset(1.5f);
            
                static_cast<Superclass const &>(*this)(renderer, inca::rendering::Solid());
            GL::glEndList();
            _displayListValid[AS_POLYGONS] = true;
        }

        GL::glCallList(_displayListBase + AS_POLYGONS);
    }

    if (_features[AS_WIREFRAME]) {
        if (! _displayListValid[AS_WIREFRAME]) {
            INCA_DEBUG("Regenerating Wireframe display list")
            GL::glNewList(_displayListBase + AS_WIREFRAME, GL_COMPILE);
                rasterizer.setLightingEnabled(false);
                rasterizer.setLineSmoothingEnabled(true);
                rasterizer.setAlphaBlendingEnabled(true);

                static_cast<Superclass const &>(*this)(renderer, inca::rendering::Wireframe());
            GL::glEndList();
            _displayListValid[AS_WIREFRAME] = true;
        }

        GL::glCallList(_displayListBase + AS_WIREFRAME);
    }

    if (_features[AS_POINTS]) {
        if (! _displayListValid[AS_POINTS]) {
            INCA_DEBUG("Regenerating Points display list")
            GL::glNewList(_displayListBase + AS_POINTS, GL_COMPILE);
                rasterizer.setLightingEnabled(false);

                static_cast<Superclass const &>(*this)(renderer, inca::rendering::Vertices());
            GL::glEndList();
            _displayListValid[AS_POINTS] = true;
        }

        GL::glCallList(_displayListBase + AS_POINTS);
    }
    
    if (_features[NORMALS]) {
        if (! _displayListValid[NORMALS]) {
            INCA_DEBUG("Regenerating Normals display list")
            GL::glNewList(_displayListBase + NORMALS, GL_COMPILE);
                rasterizer.setLightingEnabled(false);
                rasterizer.setCurrentColor(NORMAL_COLOR);    
                static_cast<Superclass const &>(*this)(renderer, inca::rendering::Normals());
            GL::glEndList();
            _displayListValid[NORMALS] = true;
        }

        GL::glCallList(_displayListBase + NORMALS);
    }

    // TODO: stack-based cleanup of temp render state
    // Re-enable prior state, if it was set previously
    rasterizer.setLightingEnabled(lightingEnabled);
    rasterizer.setFogEnabled(fogEnabled);
}
