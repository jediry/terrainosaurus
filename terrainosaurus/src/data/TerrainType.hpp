/*
 * File: TerrainType.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_TERRAIN_TYPE
#define TERRAINOSAURUS_TERRAIN_TYPE

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainType;

    // Pointer typedefs
    typedef shared_ptr<TerrainType>       TerrainTypePtr;
    typedef shared_ptr<TerrainType const> TerrainTypeConstPtr;
};


// Import associated data classes
#include "TerrainSample.hpp"


class terrainosaurus::TerrainType {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(TerrainType);


/*---------------------------------------------------------------------------*
 | Constructors & properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor, optionally initializing name
    explicit TerrainType(IDType eyeDee, const string &nm = "")
        : id(this, eyeDee), name(this, nm), color(this), samples(this) { }

    // Assignment operator overload copy all but ID field
    TerrainType & operator=(const TerrainType &tt);

    // The name of this type of terrain, and a color with which to render it
    ro_property(IDType, id, 0);
    rw_property(std::string, name, "");
    rw_property(Color, color, Color(1.0f, 0.0f, 1.0f, 0.2f));
    rw_list_property(TerrainSamplePtr, samples);

    // Force loading of a terrain type (meaning that we intend to use it)
    void ensureLoaded() const;
};

#endif
