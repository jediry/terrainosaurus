/** -*- C++ -*-
 *
 * \file    INIParser.cpp
 * 
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the section/property tracking functions for
 *      the INIParser class, defined in INIParser.hpp.
 */

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "INIParser.hpp"
using namespace terrainosaurus;

// Import exception definition
#include <inca/io/FileExceptions.hpp>
using namespace inca::io;

// Import string processing tools
#include <sstream>


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
INIParser::INIParser(antlr::TokenBuffer & tokenBuf, int k)
    : antlr::LLkParser(tokenBuf, k), _lastAssignments(antlr::RefToken()),
      _counts(0),_limits(UNLIMITED), _requirements(0) { }

INIParser::INIParser(antlr::TokenStream & lexer, int k)
    : antlr::LLkParser(lexer, k), _lastAssignments(antlr::RefToken()),
      _counts(0),_limits(UNLIMITED), _requirements(0) { }

INIParser::INIParser(const antlr::ParserSharedInputState & state, int k)
    : antlr::LLkParser(state, k), _lastAssignments(antlr::RefToken()),
      _counts(0),_limits(UNLIMITED), _requirements(0) { }


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
// Parsing state accessors
const std::string & INIParser::currentSection() const {
    return _currentSection;
}
antlr::RefToken INIParser::currentSectionStart() const {
    return _currentSectionStart;
}
antlr::RefToken INIParser::lastValidLine() const {
    return _lastValidLine;
}
antlr::RefToken INIParser::lastAssignment(PropertyID pID) const {
    return _lastAssignments[pID];
}


// Property counts, limits & requirements
int INIParser::propertyCount() const { 
    return _propertyCount;
}
int INIParser::count(PropertyID pID) const {
    return _counts[pID];
}
int INIParser::requirement(PropertyID pID) const {
    return _requirements[pID];
}
int INIParser::limit(PropertyID pID) const {
    return _limits[pID];
}
bool INIParser::requirementReached(PropertyID pID) const {
    return count(pID) >= requirement(pID);
}
bool INIParser::limitReached(PropertyID pID) const {
    int lim = limit(pID);
    return (lim != UNLIMITED) && (count(pID) >= lim);
}


/*---------------------------------------------------------------------------*
 | Parsing event functions
 *---------------------------------------------------------------------------*/
// Begin a section, specifying "limited" and "required" properties
void INIParser::beginSection(antlr::RefToken t, const std::string & name,
                             IntegerMap req, IntegerMap lim) {
    INCA_DEBUG("Beginning section [" << name << ']') 

    // Record the section we're starting
    _currentSection = name;
    _currentSectionStart = t;
    _lastValidLine = t;

    // Get ready to count properties
    _counts.clear();            // Visit counts zero'd
    _propertyCount = 0;
    _requirements = req;        // Record requirements
    _limits       = lim;        // And limits
}

// Increment the property assignment count, or blow up if we've already seen
// our limit of this property.
void INIParser::recordAssignment(antlr::RefToken t, PropertyID pID) {
//    INCA_DEBUG("Assigning property " << propertyName(pID))

    if (limitReached(pID)) {
        FileFormatException e(getFilename(), t->getLine(), t->getColumn());
        e << "Property " << propertyName(pID) << " cannot occur more than ";
        if (limit(pID) == 1)    e << "once";
        else                    e << limit(pID) << " times";
        e << " in section [" << currentSection() << "] at " << fileLine(t)
          << "; previously defined at " << fileLine(lastAssignment(pID));
        throw e;
    }
    
    // Record that this happend
    _lastAssignments[pID] = t;
    _lastValidLine = t;
    ++_counts[pID];
    ++_propertyCount;
}


// End a section, blowing up if any "required" properties are unvisited
void INIParser::endSection() {
    INCA_DEBUG("Ending section [" << currentSection() << ']')

    // Collect a list of required properties that didn't appear enough times
    std::vector<PropertyID> missing;
    IntegerMap::const_iterator rit;
    for (rit = _requirements.begin(); rit != _requirements.end(); ++rit) {
        PropertyID pID = rit->first;
        if (! requirementReached(pID))
            missing.push_back(pID);
    }
    
    // If some required properties were not found, we have to explode now
    if (! missing.empty()) { 
        FileFormatException e(getFilename(), lastValidLine()->getLine() + 1, 0);
        e << "Required properties not specified in section ["
          << currentSection() << "] beginning at "
          << fileLine(currentSectionStart()) << ':';
        for (IndexType i = 0; i < IndexType(missing.size()); ++i) {
            PropertyID pID = missing[i];
            e << ' ' << propertyName(pID)
              << '(' << count(pID) << '/' << requirement(pID) << ") ";
        }
        throw e;
    }
    
    // We're no longer in a section
    _currentSection.clear();
    _lastAssignments.clear();
    _currentSectionStart = antlr::RefToken();
}


/*---------------------------------------------------------------------------*
 | Formatting functions
 *---------------------------------------------------------------------------*/
std::string INIParser::fileLocation(antlr::RefToken t) const {
    std::ostringstream ss;
    ss << "line " << t->getLine() << ", column " << t->getColumn();
    return ss.str();
}
std::string INIParser::fileLine(antlr::RefToken t) const {
    std::ostringstream ss;
    ss << "line " << t->getLine();
    return ss.str();
}
std::string INIParser::fileColumn(antlr::RefToken t) const {
    std::ostringstream ss;
    ss << "column " << t->getColumn();
    return ss.str();
}
