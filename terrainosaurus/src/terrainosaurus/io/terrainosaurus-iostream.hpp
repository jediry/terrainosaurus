/*
 * File: terrainosaurus-iostream.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_IOSTREAM
#define TERRAINOSAURUS_IOSTREAM

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// Import data object definitions
#include "../data/Map.hpp"
#include "../data/TerrainLibrary.hpp"

namespace terrainosaurus {
    // IOstream operators for (de)serializing instances of TerrainLibrary
    std::istream & operator>>(std::istream &is, TerrainLibrary &tl);
    std::ostream & operator<<(std::ostream &os, const TerrainLibrary &tl);

    // IOstream operators for (de)serializing instances of Map
    std::istream & operator>>(std::istream &is, Map &m);
    std::ostream & operator<<(std::ostream &os, const Map &m);
};

#endif
