/*
 * File: TerrainosaurusRenderer.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_RENDERER
#define TERRAINOSAURUS_RENDERER

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainosaurusRenderer;
    
    // Pointer typedefs
    typedef shared_ptr<TerrainosaurusRenderer>       TerrainosaurusRendererPtr;
    typedef shared_ptr<TerrainosaurusRenderer const> TerrainosaurusRendererConstPtr;
};


// Import superclass definitions
#include <inca/rendering.hpp>
#include "TerrainosaurusComponent.hpp"


class terrainosaurus::TerrainosaurusRenderer
          : public inca::rendering::OpenGLRenderer,
            public TerrainosaurusComponent {
private:
    // Set up this class to own properties
    PROPERTY_OWNING_OBJECT(TerrainosaurusRenderer);

public:
    // Import geometric typedefs from Map
    typedef Map::scalar_t           scalar_t;
    typedef Map::Point              Point;
    typedef Map::Vector             Vector;

    // XXX FIXME: these ought to come from the parent renderer
    typedef inca::math::Point<IndexType, 2> Pixel;
    typedef inca::math::Vector<SizeType, 2> Dimension;

    // Flags to control what is rendered
    enum RenderOptions {
        RenderFaces             = 0x0001,
        RenderEdges             = 0x0002,
        RenderVertices          = 0x0004,

        RenderRefinedEdges      = 0x0010,
        RenderTangentLines      = 0x0020,
        RenderEnvelopeBorders   = 0x0040,

        RenderSelected          = 0x0100,
        RenderHoverSelected     = 0x0200,
        RenderLassoSelected     = 0x0400,

        ElementMask             = 0x000F,   // All the map element options
        RefinementMask          = 0x00F0,   // All refinement-like options
        SelectionMask           = 0x0F00,   // All the selection options

        RenderAll               = 0xFFFF,   // Everything. 
    };


/*---------------------------------------------------------------------------*
 | Constructor and properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor
    TerrainosaurusRenderer();

    // Destructor
    ~TerrainosaurusRenderer();

    // Map element rendering/selection properties
    rw_property(Color, vertexColor,             Color(0.7f, 0.7f, 1.0f, 1.0f));
    rw_property(Color, edgeColor,               Color(0.0f, 0.5f, 0.5f, 1.0f));
    rw_property(float, vertexDiameter,          4.0f);
    rw_property(float, edgeWidth,               4.0f);
    rw_property(float, vertexSelectionRadius,   5.0f);
    rw_property(float, edgeSelectionRadius,     4.0f);

    // Refinement rendering properties
    rw_property(Color, refinedEdgeColor,        Color(0.3f, 0.3f, 1.0f, 1.0f));
    rw_property(Color, envelopeBorderColor,     Color(0.8f, 0.8f, 0.4f, 0.7f));
    rw_property(Color, tangentLineColor,        Color(0.8f, 0.1f, 0.1f, 1.0f));
    rw_property(float, refinedEdgeWidth,        2.0f);
    rw_property(float, envelopeBorderWidth,     1.0f);
    rw_property(float, tangentLineWidth,        2.0f);

    // Selected element hilight colors
    rw_property(Color, selectHilight,           Color(1.0f, 1.0f, 0.5f, 0.7f));
    rw_property(Color, hoverSelectHilight,      Color(1.0f, 1.0f, 0.5f, 0.8f));
    rw_property(Color, lassoSelectHilight,      Color(1.0f, 1.0f, 0.5f, 0.5f));

    // Other thingy properties
    rw_property(Color, lassoColor,              Color(0.1f, 1.0f, 0.1f, 0.5f));
    rw_property(Color, majorGridTickColor,      Color(0.8f, 0.8f, 0.8f, 0.5f));
    rw_property(Color, minorGridTickColor,      Color(0.8f, 0.8f, 0.8f, 0.3f));
    rw_property(float, majorGridTickWidth,      1.5f);
    rw_property(float, minorGridTickWidth,      1.0f);



    // Selection properties


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
public:
    void renderGrid();
    void renderLasso(Pixel p1, Pixel p2);
    void renderMap(unsigned int renderFlags = RenderAll);

private:
    // Rendering functions for the various map components
    // If 'forSelection' is true, then rather than drawing with texture,
    // selection-IDs will be generated
    template <bool forSelection>
    void renderVertices(unsigned int flags);
    template <bool forSelection>
    void renderFaces(unsigned int flags);
    template <bool forSelection>
    void renderEdges(unsigned int flags);

    // XXX Ugly hack to get around lack of function partial specialization
    template <class ElementType> void renderMapElements(bool forSelection,
                                                        unsigned int flags);

    // Rendering function for refinements and aids
    void renderRefinements(unsigned int flags);


    // Color-picking functions (based on selection state and RenderOptions)
    template <class Type> Color pickColor(Type const * e, unsigned int flags) const;
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
    template <class ElementType>
    void mapElementsAroundPoint(Point2D point, Vector2D size, MeshSelection &s);
    void mapElementsAroundPixel(Pixel pixel, MeshSelection &s);
    void mapElementsWithinLasso(Pixel start, Pixel end, MeshSelection &s);

    Map::Spike spikeAtPixel(Pixel p);

    Point pointOnGroundPlane(Pixel p);
    Vector screenspaceScaleFactors();

private:
    // Slip a matrix *under* the current one (i.e., reverse-order multiply)
    void slipPickMatrix(Point2D pickCenter, Vector2D pickSize);
};

#endif