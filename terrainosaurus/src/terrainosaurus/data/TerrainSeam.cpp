/*
 * File: TerrainSeam.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "TerrainSeam.hpp"
using namespace terrainosaurus;

// Constructor
TerrainSeam::TerrainSeam(TerrainLibraryPtr lib, IDType id1, IDType id2)
    : terrainLibrary(this, lib), tt1(id1), tt2(id2),
      numberOfCycles(this), numberOfChromosomes(this), smoothness(this),
      mutationRatio(this), crossoverRatio(this), selectionRatio(this),
      aspectRatio(this) { };
