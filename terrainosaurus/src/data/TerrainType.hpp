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


class terrainosaurus::TerrainType {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(TerrainType);


/*---------------------------------------------------------------------------*
 | Constructors & properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor, optionally initializing name
    explicit TerrainType(unsigned int eyeDee, const string &nm = "")
        : name(this, nm), color(this), _id(eyeDee) { }

    // Assignment operator overload (don't copy id field)
    TerrainType & operator=(const TerrainType &tt) {
        name = tt.name();
        color = tt.color();
        return *this;
    }

    // The name of this type of terrain, and a color with which to render it
    rw_property(string, name, "");
    rw_property(Color, color, Color(1.0f, 0.0f, 1.0f, 0.2f));

    unsigned int id() const { return _id; }

protected:
    unsigned int _id;
};

#endif