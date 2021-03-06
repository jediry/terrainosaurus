/*
 * File: TerrainSeam.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// FIXME? Should this lik to the TerrainType objects?

#ifndef TERRAINOSAURUS_TERRAIN_SEAM
#define TERRAINOSAURUS_TERRAIN_SEAM

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainSeam;

    // Pointer typedefs
    typedef shared_ptr<TerrainSeam>       TerrainSeamPtr;
    typedef shared_ptr<TerrainSeam const> TerrainSeamConstPtr;
};

// Import associated data classes
#include "TerrainLibrary.hpp"
#include "TerrainType.hpp"


class terrainosaurus::TerrainSeam
    : public std::enable_shared_from_this<TerrainSeam> {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(TerrainSeam);


/*---------------------------------------------------------------------------*
 | Constructors & properties
 *---------------------------------------------------------------------------*/
public:
    // Constructor
    explicit TerrainSeam(TerrainLibraryPtr lib, IDType id1, IDType id2);

    // Associated data objects
    ro_ptr_property(TerrainLibrary, terrainLibrary, NULL);

    // IDs for constituent TerrainTypes
    IDType terrainType1() const { return tt1; }
    IDType terrainType2() const { return tt2; }

    // Genetic algorithm parameters
    rw_property(int,   numberOfCycles,      20);    // How many mutation cycles we do?
    rw_property(int,   numberOfChromosomes, 5);
    rw_property(float, smoothness,          0.2f);
    rw_property(float, mutationRatio,       0.05f);
    rw_property(float, crossoverRatio,      0.05f);
    rw_property(float, selectionRatio,      1.0f);
    rw_property(float, aspectRatio,         1.0f);

protected:
    IDType tt1, tt2;      // TerrainType IDs for the constituent TTs
};

#endif
