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
#include "TerrainLibraryParser.hpp"
using namespace antlr;

// Import GA global parameters
#include <terrainosaurus/genetics/HeightfieldGA.hpp>
using namespace terrainosaurus;

// Import file-related exception definitions
#include <inca/io/FileExceptions.hpp>
using namespace inca::io;


// Write a string representation of a property enumeration
inline std::ostream & operator<<(std::ostream & o,
                                 TerrainLibraryParser::PropertyType p) {
    switch (p) {
        case TerrainLibraryParser::TTColor:
            return o << "Color";
        case TerrainLibraryParser::TSSmoothness:
            return o << "Smoothness";
        case TerrainLibraryParser::TSAspectRatio:
            return o << "Aspect Ratio";
        case TerrainLibraryParser::GAPopulationSize:
            return o << "Population Size";
        case TerrainLibraryParser::GAEvolutionCycles:
            return o << "Evolution Cycles";
        case TerrainLibraryParser::GASelectionRatio:
            return o << "Selection Ratio";
        case TerrainLibraryParser::GAEliteRatio:
            return o << "Elite Ratio";
        case TerrainLibraryParser::GACrossoverProbability:
            return o << "Crossover Probability";
        case TerrainLibraryParser::GACrossoverRatio:
            return o << "Crossover Ratio";
        case TerrainLibraryParser::GAMutationProbability:
            return o << "Mutation Probability";
        case TerrainLibraryParser::GAMutationRatio:
            return o << "Mutation Ratio";
        case TerrainLibraryParser::HFMaxCrossoverWidth:
            return o << "Max Crossover Width";
        case TerrainLibraryParser::HFMaxJitterPixels:
            return o << "Max Jitter Pixels";
        case TerrainLibraryParser::HFMaxScaleFactor:
            return o << "Max Scale Factor";
        case TerrainLibraryParser::HFMaxOffsetAmount:
            return o << "Max Offset Amount";
        default:
            return o << "INVALID PropertyType";
    }
}


void TerrainLibraryParser::beginGlobalSection(RefToken tt) {
    // Close the previous record
    endRecord(tt);

    // Throw a fit if we've already read a global section
    static bool parsedGlobal = false;
    if (parsedGlobal) {
        FileFormatException e(getFilename(), tt->getLine(), tt->getColumn());
        e << "Duplicate [Global] section";
        throw e;
    }

    // Never do this again
    parsedGlobal = true;

    // Get ready for the properties
    setProperties = 0x0000;
    inGlobal = true;
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
    INCA_DEBUG("[TerrainType: " << tt->getText() << "]")
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
    INCA_DEBUG("[TerrainSeam: " << tt1->getText()
               << " & " << tt2->getText() << "]")
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
    } else if (inGlobal) {
        inGlobal = false;       // We're done with the global section
    }
}


// Adds a new terrain sample to the current terrain type. There can be any
// (non-zero) number of terrain samples for a single terrain type.
void TerrainLibraryParser::addTerrainSample(const std::string & path, int line) {
    currentTT->addTerrainSample(TerrainSamplePtr(new TerrainSample(path)));
    INCA_DEBUG("\tsample = "
               << currentTT->terrainSample(currentTT->size() - 1)->filename())
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
        INCA_ERROR("Internal parser error: unhandled color property " << p)
    }
    setProperties |= p;
    INCA_DEBUG("\t" << p << " = " << c)
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
    case TSAspectRatio:
        currentTS->aspectRatio = float(s);
        break;
    case GASelectionRatio:
        if (inGlobal)
            SELECTION_RATIO = float(s);
        else if (currentTS)
            currentTS->selectionRatio = float(s);
        break;
    case GAEliteRatio:
        if (inGlobal)
            ELITE_RATIO = float(s);
        break;
    case GACrossoverProbability:
        if (inGlobal)
            CROSSOVER_PROBABILITY = float(s);
        break;
    case GACrossoverRatio:
        if (inGlobal)
            CROSSOVER_RATIO = float(s);
        else if (currentTS)
            currentTS->crossoverRatio = float(s);
        break;
    case GAMutationProbability:
        if (inGlobal)
            MUTATION_PROBABILITY = float(s);
        break;
    case GAMutationRatio:
        if (inGlobal)
            MUTATION_RATIO = float(s);
        else if (currentTS)
            currentTS->mutationRatio = float(s);
        break;
    case HFMaxScaleFactor:
        if (inGlobal)
            MAX_SCALE_FACTOR = float(s);
        break;
    case HFMaxOffsetAmount:
        if (inGlobal)
            MAX_OFFSET_AMOUNT = float(s);
        break;
    default:
        INCA_ERROR("Internal parser error: unhandled scalar property " << p)
    }
    setProperties |= p;
    INCA_DEBUG("\t" << p << " = " << s)
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
    case GAPopulationSize:
        if (inGlobal)
            POPULATION_SIZE = i;
        else if (currentTS)
            currentTS->numberOfChromosomes = i;
        break;
    case GAEvolutionCycles:
        if (inGlobal)
            EVOLUTION_CYCLES = i;
        break;
    case HFMaxCrossoverWidth:
        if (inGlobal)
            MAX_CROSSOVER_WIDTH = i;
        break;
    case HFMaxJitterPixels:
        if (inGlobal)
            MAX_JITTER_PIXELS = i;
        break;
    default:
        INCA_ERROR("Internal parser error: unhandled integer property " << p)
    }
    setProperties |= p;
    INCA_DEBUG("\t" << p << " = " << i)
}
