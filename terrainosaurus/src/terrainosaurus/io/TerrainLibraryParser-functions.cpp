/*
 * File: TerrainLibraryParser-functions.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the helper functions that are part of the
 *      ANTLR-based TerrainLibraryParser class. These functions are
 *      implemented separately in order to keep the grammar definition
 *      (defined in TerrainLibrary.g) relatively free of implementation code.
 */

// Import class definition
//#include "TerrainLibraryLexer.hpp"
#include "TerrainLibraryParser.hpp"
using namespace terrainosaurus;
using namespace antlr;

// Import file-related exception definitions
#include <inca/io/FileExceptions.hpp>
using namespace inca::io;


// Write a string representation of a property enumeration
inline ostream & operator<<(ostream &o, TerrainLibraryParser::PropertyType p) {
    switch (p) {
        case TerrainLibraryParser::TTColor:
            return o << "Color";
        case TerrainLibraryParser::TSNumChromosomes:
            return o << "Number of Chromosomes";
        case TerrainLibraryParser::TSSmoothness:
            return o << "Smoothness";
        case TerrainLibraryParser::TSMutationRatio:
            return o << "Mutation Ratio";
        case TerrainLibraryParser::TSCrossoverRatio:
            return o << "Crossover Ratio";
        case TerrainLibraryParser::TSSelectionRatio:
            return o << "Selection Ratio";
        case TerrainLibraryParser::TSAspectRatio:
            return o << "Aspect Ratio";
        default:
            return o << "INVALID PropertyType";
    }
}


// Creates a new terrain type and sets it as "current"
void TerrainLibraryParser::createTerrainType(RefToken tt) {
    // Close the previous record
    endRecord(tt);

    // Scream like hell if we've already got one
    TerrainTypePtr ttp = library->terrainType(tt->getText());
    if (ttp != NULL) {
        FileFormatException e(getFilename(), tt->getLine(), tt->getColumn());
        e << "TerrainType \"" << tt->getText()
          << "\" has already been created";
        throw e;
    }

    // Clear the collection of initialized properties
    setProperties = 0x0000;

    // Make a new one with this name
    currentTT = library->addTerrainType(tt->getText());
    cerr << "[TerrainType: " << tt->getText() << "]\n";
}


// Creates a new terrain type and sets it as "current"
void TerrainLibraryParser::createTerrainSeam(RefToken tt1, RefToken tt2) {
    // Close the previous record
    endRecord(tt1);

    // Make sure the associated TerrainType records exist
    TerrainTypePtr tt1p = library->terrainType(tt1->getText());
    TerrainTypePtr tt2p = library->terrainType(tt2->getText());
    if (tt1p == NULL || tt2p == NULL) {
        FileFormatException e(getFilename());
        e << "TerrainSeam between \"" << tt1->getText() << "\" and \""
          << tt2->getText() << "\" cannot be created: ";
        int col  = tt1->getColumn();
        int line = tt1->getLine();
        if (tt1p == NULL && tt2p == NULL) { // Both are missing
            e << "neither TerrainType record exists";
        } else if (tt1p == NULL) {          // The first is missing
            e << "a TerrainType record for \"" << tt1->getText()
              << "\" does not exist";
        } else {                            // The second is missing
            e << "a TerrainType record for \"" << tt2->getText()
              << "\" does not exist";
            col = tt2->getColumn();
        }
        e.setLine(line);
        e.setColumn(col);
        throw e;
    }

    // Look up the TS from its TTs
    TerrainSeamPtr ts = library->terrainSeam(tt1p->terrainTypeID(),
                                             tt2p->terrainTypeID());

    // Make sure we've not already initialized this TerrainSeam
    if (initializedTSs[ts]) {
        FileFormatException e(getFilename(), tt1->getLine());
        e << "TerrainSeam between \"" << tt1->getText() << "\" and \""
          << tt2->getText() << "\" has already been initialized";
        throw e;
    }

    // Clear the collection of initialized properties
    setProperties = 0x0000;

    // This is the new current TS; mark it as "initialized"
    currentTS = ts;
    initializedTSs[currentTS] = true;
    cerr << "[TerrainSeam: " << tt1->getText()
         << " & " << tt2->getText() << "]\n";
}


// End the in-progress terrain type/seam
void TerrainLibraryParser::endRecord(RefToken t) {
    if (currentTT) {
        // Make sure it had at least one TerrainSample
        if (currentTT->size() == 0) {
            FileFormatException e(getFilename(), t->getLine(), 0);
            e << "TerrainType \"" << currentTT->name()
              << "\" does not have any data samples";
            throw e;
        }
        currentTT.reset();      // We're no longer inside a TT
    } else if (currentTS) {
        currentTS.reset();      // We're no longer inside a TS
    }
}


// Adds a new terrain sample to the current terrain type. There can be any
// (non-zero) number of terrain samples for a single terrain type.
void TerrainLibraryParser::addTerrainSample(const string & path, int line) {
    currentTT->addTerrainSample(TerrainSamplePtr(new TerrainSample(path)));
    cerr << "\tsample = "
         << currentTT->terrainSample(currentTT->size() - 1)->filename()
         << endl;
}


// Set the requested, color-valued property on the current TT or TS.
// If it has already been set, throw a SemanticException
void TerrainLibraryParser::setColorProperty(PropertyType p, const Color &c, int line) {
    // Make sure we've not already set this
    if (p & setProperties) {
        FileFormatException e(getFilename(), line);
        e << "Color property \"" << p << "\" has already been set";
        throw e;
    }

    // Set it and record that we did so
    switch (p) {
    case TTColor:
        currentTT->setColor(c);
        break;
    default:
        cerr << "Internal parser error: unhandled color property "
             << p << endl;
    }
    setProperties |= p;
    cerr << "\t" << p << " = " << c << endl;
}


// Set the requested, scalar-valued property on the current TT or TS.
// If it has already been set, throw a SemanticException
void TerrainLibraryParser::setScalarProperty(PropertyType p, scalar_t s, int line) {
    // Make sure we've not already set this
    if (p & setProperties) {
        FileFormatException e(getFilename(), line);
        e << "Scalar property \"" << p << "\" has already been set\n";
        throw e;
    }

    // Set it and record that we did so
    switch (p) {
    case TSSmoothness:
        currentTS->smoothness = float(s);
        break;
    case TSMutationRatio:
        currentTS->mutationRatio = float(s);
        break;
    case TSCrossoverRatio:
        currentTS->crossoverRatio = float(s);
        break;
    case TSSelectionRatio:
        currentTS->selectionRatio = float(s);
        break;
    case TSAspectRatio:
        currentTS->aspectRatio = float(s);
        break;
    default:
        cerr << "Internal parser error: unhandled scalar property "
             << p << endl;
    }
    setProperties |= p;
    cerr << "\t" << p << " = " << s << endl;
}


// Set the requested, integer-valued property on the current TT or TS.
// If it has already been set, throw a SemanticException
void TerrainLibraryParser::setIntegerProperty(PropertyType p, int i, int line) {
    // Make sure we've not already set this
    if (p & setProperties) {
        FileFormatException e(getFilename(), line);
        e << "Integer property \"" << p << "\" has already been set";
        throw e;
    }

    // Set it and record that we did so
    switch (p) {
    case TSNumChromosomes:
        currentTS->numberOfChromosomes = i;
        break;
    default:
        cerr << "Internal parser error: unhandled integer property "
             << p << endl;
    }
    setProperties |= p;
    cerr << "\t" << p << " = " << i << endl;
}
