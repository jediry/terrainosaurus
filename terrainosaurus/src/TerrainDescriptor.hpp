/*
 * File: TerrainDescriptor.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_TERRAIN_DESCRIPTOR
#define TERRAINOSAURUS_TERRAIN_DESCRIPTOR

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainDescriptor;

    // Pointer typedefs
    typedef shared_ptr<TerrainDescriptor>       TerrainDescriptorPtr;
    typedef shared_ptr<TerrainDescriptor const> TerrainDescriptorConstPtr;
};


class terrainosaurus::TerrainDescriptor {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(TerrainDescriptor);

public:
    // Color typedef
    typedef inca::imaging::Color<float, inca::imaging::sRGB, true> Color;


/*---------------------------------------------------------------------------*
 | Constructors & properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor
    explicit TerrainDescriptor() : name(this), color(this) { }

    // Copy constructor
    TerrainDescriptor(const TerrainDescriptor &td)
        : name(this, td.name()), color(this, td.color()) { }
    
    // Constructor intializing name and color
    explicit TerrainDescriptor(const string &nm, const Color &c)
        : name(this, nm), color(this, c) { }

    // The name of this type of terrain, and a color with which to render it
    rw_property(string, name, "");
    rw_property(Color, color, Color(1.0f, 0.0f, 1.0f, 0.2f));
};

#endif