/*
 * File: TerrainType.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "TerrainType.hpp"
using namespace terrainosaurus;

TerrainType & TerrainType::operator=(const TerrainType &tt) {
    name = tt.name();
    color = tt.color();
    samples = tt.samples();
    return *this;
}

// Force loading of a terrain type (meaning that we intend to use it)
void TerrainType::ensureLoaded() const {
    cerr << "Loading!!!!!" << endl;
}
