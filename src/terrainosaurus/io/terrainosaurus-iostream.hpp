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
#include "../data/TerrainSample.hpp"

namespace terrainosaurus {
    std::string chomp(const std::string& s);

    // IOstream operators for (de)serializing instances of TerrainLibrary
    std::istream & operator>>(std::istream & is, TerrainLibrary & tl);
    std::ostream & operator<<(std::ostream & os, const TerrainLibrary & tl);

    // IOstream operators for (de)serializing instances of Map
    std::istream & operator>>(std::istream & is, Map &m);
    std::ostream & operator<<(std::ostream & os, const Map &m);
    
    // IOstream operators for (de)serializing Heightfields
    std::istream & operator>>(std::istream & is, Heightfield & hf);
    std::ostream & operator<<(std::ostream & os, const Heightfield & hf);
    
    // IOstream operators for (de)serializing TerrainSample::LOD cache files
    std::istream & operator>>(std::istream & is, TerrainSample::LOD & ts);
    std::ostream & operator<<(std::ostream & os, const TerrainSample::LOD & ts);
};

#endif
