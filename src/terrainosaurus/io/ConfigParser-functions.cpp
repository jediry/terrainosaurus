/** -*- C++ -*-
 *
 * \file    ConfigParser-functions.cpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the helper functions that are part of the
 *      ANTLR-based ConfigParser class. These functions are
 *      implemented separately in order to keep the grammar definition
 *      (defined in TerrainLibrary.g) relatively free of behavior code.
 */

// Import class definition
#include "ConfigParser.hpp"
using namespace antlr;

// Import GA global parameters
#include <terrainosaurus/genetics/HeightfieldGA.hpp>
using namespace terrainosaurus;

// Import file-related exception definitions
#include <inca/io/FileExceptions.hpp>
using namespace inca::io;


// Begin the [Application] section
void ConfigParser::beginApplicationSection(RefToken t) {
    // Begin a new section
    IntegerMap req(0), lim(1); // No parameters required, all unique
    beginSection(t, "Application", req, lim);
}


// Begin the [Heightfield GA] section
void ConfigParser::beginHeightfieldGASection(RefToken t) {
    // Begin a new section
    IntegerMap req(0), lim(1); // No parameters required, all unique
    beginSection(t, "Heightfield GA", req, lim);
}


// Begin the [Boundary GA] section
void ConfigParser::beginBoundaryGASection(RefToken t) {
    // Begin a new section
    IntegerMap req(0), lim(1); // No parameters required, all unique
    beginSection(t, "Boundary GA", req, lim);
}


// Begin the [Erosion GA] section
void ConfigParser::beginErosionSection(RefToken t) {
    // Begin a new section
    IntegerMap req(0), lim(1); // No parameters required, all unique
    beginSection(t, "Erosion", req, lim);
}


// End the in-progress terrain type/seam
void ConfigParser::endSection() {
    INIParser::endSection();     // Check required properties
}


// Set the requested, color-valued property on the current TT or TS.
// If it has already been set, throw a SemanticException
void ConfigParser::assignColorProperty(RefToken t, PropertyType p,
                                       const Color & c) {
    // Make sure this is legal, record that we've seen it, then set it
    recordAssignment(t, p, c);
    switch (p) {
    case 0:
    default:
        INCA_ERROR("Internal parser error: unhandled color property "
                   << propertyName(p))
    }
}


// Set the requested, scalar-valued property on the current TT or TS.
// If it has already been set, throw a SemanticException
void ConfigParser::assignScalarProperty(RefToken t, PropertyType p,
                                        scalar_t s) {
    // Make sure this is legal, record that we've seen it, then set it
    recordAssignment(t, p, s);
    switch (p) {
    case SelectionRatio:
        if (currentSection() == "Boundary GA")  _application->setBoundaryGASelectionRatio(s);
        else                                    _application->setHeightfieldGASelectionRatio(s);
        break;
    case ElitismRatio:
        if (currentSection() == "Boundary GA")  _application->setBoundaryGAElitismRatio(s);
        else                                    _application->setHeightfieldGAElitismRatio(s);
        break;
    case MutationProbability:
        if (currentSection() == "Boundary GA")  _application->setBoundaryGAMutationProbability(s);
        else                                    _application->setHeightfieldGAMutationProbability(s);
        break;
    case MutationRatio:
        if (currentSection() == "Boundary GA")  _application->setBoundaryGAMutationRatio(s);
        else                                    _application->setHeightfieldGAMutationRatio(s);
        break;
    case CrossoverProbability:
        if (currentSection() == "Boundary GA")  _application->setBoundaryGACrossoverProbability(s);
        else                                    _application->setHeightfieldGACrossoverProbability(s);
        break;
    case CrossoverRatio:
        if (currentSection() == "Boundary GA")  _application->setBoundaryGACrossoverRatio(s);
        else                                    _application->setHeightfieldGACrossoverRatio(s);
        break;
    case OverlapFactor:
        _application->setHeightfieldGAOverlapFactor(s);
        break;
    case MaxVerticalScale:
        _application->setHeightfieldGAMaxVerticalScale(s);
        break;
    case MaxVerticalOffset:
        _application->setHeightfieldGAMaxVerticalOffset(s);
        break;
    default:
        INCA_ERROR("Internal parser error: unhandled scalar property "
                   << propertyName(p))
    }
}


// Set the requested, integer-valued property on the current TT or TS.
// If it has already been set, throw a SemanticException
void ConfigParser::assignIntegerProperty(RefToken t, PropertyType p, int i) {
    // Make sure this is legal, record that we've seen it, then set it
    recordAssignment(t, p, i);
    switch (p) {
    case PopulationSize:
        if (currentSection() == "Boundary GA")  _application->setBoundaryGAPopulationSize(i);
        else                                    _application->setHeightfieldGAPopulationSize(i);
        break;
    case EvolutionCycles:
        if (currentSection() == "Boundary GA")  _application->setBoundaryGAEvolutionCycles(i);
        else                                    _application->setHeightfieldGAEvolutionCycles(i);
        break;
    case GeneSize:
        _application->setHeightfieldGAGeneSize(i);
        break;
    case MaxCrossoverWidth:
        _application->setHeightfieldGAMaxCrossoverWidth(i);
        break;
    default:
        INCA_ERROR("Internal parser error: unhandled integer property "
                   << propertyName(p))
    }
}

// Set the requested, string-valued property.
// If it has already been set, throw a SemanticException
void ConfigParser::assignStringProperty(RefToken t, PropertyType p,
                                        const std::string & s) {
    // Make sure this is legal, record that we've seen it, then set it
    recordAssignment(t, p, s);
    switch (p) {
    case CacheDirectory:
        _application->setCacheDirectory(s);
        break;
    default:
        INCA_ERROR("Internal parser error: unhandled string property "
                   << propertyName(p))
    }
}

// This provides a symbolic interpretation of integer property IDs
const char * ConfigParser::propertyName(PropertyID pID) const {
    return static_cast<const char *>(PropertyType(pID));
}
