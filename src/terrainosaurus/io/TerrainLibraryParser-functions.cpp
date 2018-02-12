/** -*- C++ -*-
 *
 * \file    TerrainLibraryParser-functions.cpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the helper functions that are part of the
 *      ANTLR-based TerrainLibraryParser class. These functions are
 *      implemented separately in order to keep the grammar definition
 *      (defined in TerrainLibrary.g) relatively free of behavior code.
 */

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "TerrainLibraryParser.hpp"
using namespace antlr;

// Import GA global parameters
#include <terrainosaurus/genetics/HeightfieldGA.hpp>
using namespace terrainosaurus;

// Import file-related exception definitions
#include <inca/io/FileExceptions.hpp>
using namespace inca::io;


// Begin a new [TerrainType] section
void TerrainLibraryParser::beginTerrainTypeSection(RefToken t,
                                                   const std::string & tt) {
    // Scream like hell if we've already got one
    TerrainTypePtr ttp = _terrainLibrary->terrainType(tt);
    if (ttp != NULL) {
        FileFormatException e(getFilename(), t->getLine(), 0);
        e << "TerrainType \"" << tt << "\" has already been created";
        throw e;
    }

    // Make a new one with this name
    _currentTT = _terrainLibrary->addTerrainType(tt);

    // Start a new section
    IntegerMap req(1), lim(1);
    lim[TTSample] = UNLIMITED;
    beginSection(t, std::string("TerrainType: ") + tt, req, lim);
}


// Creates a new terrain type and sets it as "current"
void TerrainLibraryParser::beginTerrainSeamSection(RefToken t,
                                                   const std::string & tt1,
                                                   const std::string & tt2) {
    // Make sure the associated TerrainType records exist
    TerrainTypePtr tt1p = _terrainLibrary->terrainType(tt1);
    TerrainTypePtr tt2p = _terrainLibrary->terrainType(tt2);
    if (tt1p == NULL || tt2p == NULL) {
        FileFormatException e(getFilename(), t->getLine(), 0);
        e << "TerrainSeam between \"" << tt1 << "\" and \"" << tt2
          << "\" cannot be created: ";
        if (tt1p == NULL && tt2p == NULL) { // Both are missing
            e << "neither TerrainType record exists";
        } else if (tt1p == NULL) {          // The first is missing
            e << "a TerrainType record for \"" << tt1 << "\" does not exist";
        } else {                            // The second is missing
            e << "a TerrainType record for \"" << tt2 << "\" does not exist";
        }
        throw e;
    }

    // Look up the TS from its TTs
    TerrainSeamPtr ts = _terrainLibrary->terrainSeam(tt1p->terrainTypeID(),
                                                     tt2p->terrainTypeID());

    // Make sure we've not already initialized this TerrainSeam
    if (_initializedTSs[ts]) {
        FileFormatException e(getFilename(), t->getLine(), 0);
        e << "TerrainSeam between \"" << tt1 << "\" and \""
          << tt2 << "\" has already been initialized";
        throw e;
    }


    // This is the new current TS; mark it as "initialized"
    _currentTS = ts;
    _initializedTSs[_currentTS] = true;

    // Begin a new section
    IntegerMap req(1), lim(1);
    beginSection(t, std::string("TerrainSeam: ") + tt1 + " & " + tt2, req, lim);
}


// End the in-progress terrain type/seam
void TerrainLibraryParser::endSection() {
    INIParser::endSection();    // Check required properties
    _currentTT.reset();         // Drop 'current' pointers
    _currentTS.reset();
}


// Set the requested, color-valued property on the current TT or TS.
// If it has already been set, throw a SemanticException
void TerrainLibraryParser::assignColorProperty(RefToken t, PropertyType p,
                                               const Color & c) {
    // Make sure this is legal, record that we've seen it, then set it
    recordAssignment(t, p, c);
    switch (p) {
    case TTColor:
        _currentTT->setColor(c);
        break;
    default:
        INCA_ERROR("Internal parser error: unhandled color property "
                   << propertyName(p))
    }
}


// Set the requested, scalar-valued property on the current TT or TS.
// If it has already been set, throw a SemanticException
void TerrainLibraryParser::assignScalarProperty(RefToken t, PropertyType p,
                                                scalar_t s) {
    // Make sure this is legal, record that we've seen it, then set it
    recordAssignment(t, p, s);
    switch (p) {
    case TSSmoothness:
        _currentTS->smoothness = s;
        break;
    case TSAspectRatio:
        _currentTS->aspectRatio = s;
        break;
    default:
        INCA_ERROR("Internal parser error: unhandled scalar property "
                   << propertyName(p))
    }
}


// Set the requested, integer-valued property on the current TT or TS.
// If it has already been set, throw a SemanticException
void TerrainLibraryParser::assignIntegerProperty(RefToken t, PropertyType p, int i) {
    // Make sure this is legal, record that we've seen it, then set it
    recordAssignment(t, p, i);
    switch (p) {
    case 0:
    default:
        INCA_ERROR("Internal parser error: unhandled integer property "
                   << propertyName(p))
    }
}

// Adds a new terrain sample to the current terrain type. There can be any
// (non-zero) number of terrain samples for a single terrain type.
void TerrainLibraryParser::assignStringProperty(RefToken t, PropertyType p,
                                                const std::string & s) {
    // Make sure this is legal, record that we've seen it, then set it
    recordAssignment(t, p, s);
    switch (p) {
    case TTSample:
        _currentTT->addTerrainSample(TerrainSamplePtr(new TerrainSample(s)));
        break;
    default:
        INCA_ERROR("Internal parser error: unhandled string property "
                   << propertyName(p))
    }
}

// This provides a symbolic interpretation of integer property IDs
const char * TerrainLibraryParser::propertyName(PropertyID pID) const {
    return static_cast<const char *>(PropertyType(pID));
}
