/*
 * File: TerrainPatch.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The TerrainPatch class represents a rectangular piece of terrain that
 *      can be viewed at multiple levels of detail (LODs). The various LODs
 *      are stored using a quadtree of TerrainChunks, most of which will not
 *      be in memory at the same time.
 */

#ifndef TERRAINOSAURUS_TERRAIN_PATCH
#define TERRAINOSAURUS_TERRAIN_PATCH

// Import Inca library configuration
#include <inca/inca-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainPatch;
    
    // Pointer typedefs
    typedef shared_ptr<TerrainPatch>       TerrainPatchPtr;
    typedef shared_ptr<TerrainPatch const> TerrainPatchConstPtr;
};

// Import superclass and sub-object definitions
#include <inca/world/Object.hpp>
#include "TerrainChunk.hpp"


class terrainosaurus::TerrainPatch : public inca::world::Object {
/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // Type definitions
    typedef inca::math::QuadPrism<double, 3>    BoundingBox;
    typedef boost::multi_array<float, 2>        HeightField;
    typedef shared_ptr<Heightfield>             HeightFieldPtr;


/*---------------------------------------------------------------------------*
 | Constructors & properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor
    TerrainPatch() {
        _heightField.reset(new HeightField(boost::extents[20][20]));
        for (IndexType i = 0; i < 20; ++i)
            for (IndexType j = 0; j < 20; ++j) {
                float h = 0;
                if (i < 10)     h += i;
                else            h += 20 - i;
                if (j < 10)     h += j;
                else            h += 20 - j;
                (*_heightField)[i][j] = h / 20;
            }
        regenerateBoundingBox();
    }


    //FIXME: replace with "good" properties
    const BoundingBox & boundingBox() const { return _boundingBox; }
    HeightFieldPtr heightField() { return _heightField; }

protected:
    BoundingBox    _boundingBox;    // Axially-aligned bounding box
    HeightFieldPtr _heightField;    // The actual grid of elevation data


/*---------------------------------------------------------------------------*
 | Update/modify operations
 *---------------------------------------------------------------------------*/
protected:
    void regenerateBoundingBox() {
        HeightField::iterator i;
        BoundingBox::Point min(0.0, 0.0, POS_INF<double>()),
                           max(1.0, 1.0, NEG_INF<double>());
        for (i = heightField()->begin(); i != heightField.end(); ++i) {
            BoundingBox::scalar_t value = *i;
            if (value < min[2])    min[2] = value;
            if (value < max[2])    max[2] = value;
        }
        _boundingBox.setExtrema(min, max);//FIXME!! H/V bounds!
    }
};

#endif
