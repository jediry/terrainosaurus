/*
 * File: MapRendering.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_RENDERING_MAP_RENDERING
#define TERRAINOSAURUS_RENDERING_MAP_RENDERING

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapRendering;
};


// Import data object definitions
#include <terrainosaurus/data/Map.hpp>
#include <terrainosaurus/data/MeshSelection.hpp>


// FIXME!!!
#include <inca/rendering.hpp>
// Renderer type
typedef inca::rendering::OpenGLRenderer Renderer;


class terrainosaurus::MapRendering {
/*---------------------------------------------------------------------------*
 | Constructor and properties
 *---------------------------------------------------------------------------*/
public:
    // Constructor
    MapRendering();
    MapRendering(MapConstPtr m,
                 MeshSelectionConstPtr ps,
                 MeshSelectionConstPtr ts);

    // Destructor
    ~MapRendering();

    // Set/toggle features and properties
    void setDefaults();
    bool toggle(const std::string & feature);
    void setScalar(const std::string & name, scalar_arg_t s);
    void setColor(const std::string & name, const Color & c);

    // Data object accessors
    MapConstPtr map() const;
    MeshSelectionConstPtr persistentSelection() const;
    MeshSelectionConstPtr transientSelection() const;
    void setMap(MapConstPtr m);
    void setPersistentSelection(MeshSelectionConstPtr s);
    void setTransientSelection(MeshSelectionConstPtr s);

protected:
    std::vector<bool>       _features;
    std::vector<Color>      _colors;
    std::vector<scalar_t>   _scalars;

    MapConstPtr _map;                           // The Map that we're rendering
    MeshSelectionConstPtr _persistentSelection, // "selected" and "potentially
                          _transientSelection;  // selected" objects


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
//    void renderLasso(Pixel p1, Pixel p2);

    // Rendering function call operator
    void operator()(Renderer & renderer);

protected:
    // Rendering functions for the various map components
    // If 'forSelection' is true, then rather than drawing with texture,
    // selection-IDs will be generated
    template <bool forSelection>    void renderVertices(Renderer & renderer);
    template <bool forSelection>    void renderFaces(Renderer & renderer);
    template <bool forSelection>    void renderEdges(Renderer & renderer);
    template <bool forSelection>    void renderRefinements(Renderer & renderer);


    // Color-picking functions (based on selection state and RenderOptions)
    template <class Type> Color pickColor(Type const * e) const;
    template <class Type> Color baseColor(Type const * e) const;
    template <class Type> Color selectedColor(Type const * e) const;
    template <class Type> Color lassoSelectedColor(Type const * e) const;
    template <class Type> Color hoverSelectedColor(Type const * e) const;

    // GLUtessellator object for rendering concave polygons 'n' stuff XXX
    void * tesselator;


/*---------------------------------------------------------------------------*
 | Selection/geometric functions
 *---------------------------------------------------------------------------*/
public:
#if 0
    template <class ElementType>
    void mapElementsAroundPoint(Point2D point, Vector2D size, MeshSelection &s);
    void mapElementsAroundPixel(Pixel pixel, MeshSelection &s);
    void mapElementsWithinLasso(Pixel start, Pixel end, MeshSelection &s);

    Map::Spike spikeAtPixel(Pixel p);

    Point3D pointOnGroundPlane(Pixel p);
    Vector2D screenspaceScaleFactors();
#endif
};

#endif
