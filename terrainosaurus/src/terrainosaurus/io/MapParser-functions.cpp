/*
 * File: MapParser-functions.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the helper functions that are part of the
 *      ANTLR-based MapParser class. These functions are implemented
 *      separately in order to keep the grammar definition (defined in Map.g)
 *      relatively free of implementation code.
 */

// Import class definition
#include "MapLexer.hpp"
#include "MapParser.hpp"
using namespace terrainosaurus;
using namespace antlr;

// Import file-related exception definitions
#include <inca/io/FileExceptions.hpp>
using namespace inca::io;


void MapParser::createVertex(scalar_t x, scalar_t y, RefToken pos) {
    cerr << "Vertex: " << x << ' ' << y << ' ' << pos->getLine() << endl;
    map->createVertex(Point2D(x, y));
}

void MapParser::setTerrainType(const string &tt, RefToken pos) {
    cerr << "Terrain: " << tt << endl;
    TerrainTypePtr ttp = map->terrainLibrary->terrainType(tt);
    if (ttp == NULL) {
        FileFormatException e(getFilename(), pos->getLine(), pos->getColumn());
        e << "TerrainType \"" << tt << "\" does not exist";
        throw e;
    }

    // If we made it here, then the TT we want exists. Yay!
    currentTT = ttp;
}

void MapParser::beginFace(RefToken pos) {
    cerr << "Face: ";
    if (currentTT == NULL) {    // Ensure that a TT has been set
        FileFormatException e(getFilename(), pos->getLine(), pos->getColumn());
        e << "A TerrainType must be selected before faces may be created";
        throw e;
    }
    vertices.clear();           // Start fresh
}

void MapParser::addVertex(IDType id, RefToken pos) {
    cerr << id << ' ';
    // Look up the Vertex with that ID (remember file starts at 1, vector at 0)
    Map::VertexPtr v = map->vertex(id - 1);
    if (v == NULL) {                    // Ensure that the Vertex does exist
        FileFormatException e(getFilename(), pos->getLine(), pos->getColumn());
        e << "Vertex \"" << id << "\" does not exist";
        throw e;
    }
    vertices.push_back(v);      // Stick it in the list
}

void MapParser::endFace() {
    // Make a face using the current TT
    cerr << endl;
    map->createFace(vertices.begin(), vertices.end(), currentTT);
}
