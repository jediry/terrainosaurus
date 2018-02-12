/** -*- C++ -*-
 *
 * \file    INIParser.hpp
 * 
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The INIParser class is an ANTLR LLk parser subclass providing extra
 *      functionality for parsing text file formats similar to the MS Windoze
 *      .ini format. It is not intended to be used directly, but may be
 *      specified as the Parser superclass in a grammar.
 *
 *      The basic characteristics of an INI-style format are:
 *          1) comments are introduced by a semicolon (';'), as in Windows, or
 *             a hash symbol ('#'), my extension, and they continue to the
 *             end of the current line
 *          2) the file text is case-insensitive ASCII (except for file paths,
 *             if the underlying filesystem is case-sensitive)
 *          3) Blank lines are allowed anywhere in the file, and whitespace is
 *             not significant, except within quoted strings, which may be
 *             quoted with either single- or double-quotes
 *          4) the file is broken up into "sections", which are introduced by
 *             a section header, which is the section name in square brackets,
 *             like "[<section-name>]", and continue until the next section
 *             header.
 *          5) Within each section, there are zero or more property
 *             assignments, which look like "<property-name> = <value>".
 *             Property names may be multi-word, but the amound of whitespace
 *             between words is not significant. No properties may occur before
 *             the first section header.
 *          6) There may be additional semantic restrictions peculiar to the
 *             specific file format, such as "exactly one of these three
 *             properties may occur in a given section", or "the [Files]
 *             section must contain only properties with the same names as
 *             previously declared sections.
 *
 *      The INIParser class does not attempt to impose any semantic
 *      restrictions, but instead provides simple mechanisms that can be used
 *      easily to create these restrictions. This class is agnostic w/r to the
 *      vocabulary of the file being parsed, and instead keeps track of
 *      properties by numeric identifiers. The subclass is responsible for
 *      supplying the symbolic meaning (i.e., a string) of these identifiers.
 *      In particular, the INIParser keeps track of the name of the current
 *      section, and of the properties seen so far in the current section. A
 *      property may be "required", meaning that it must appear *at least*
 *      once in a particular section, and "limited", meaning that it can appear
 *      *at most* once in that section, or it could be neither (the default).
 *
 *      Note that it is not necessary for the meanings of the numeric
 *      identifiers for properties to be consistent between sections.
 */

#ifndef TERRAINOSAURUS_IO_INI_PARSER
#define TERRAINOSAURUS_IO_INI_PARSER

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class INIParser;
};

// Import superclass definition
#include <antlr/LLkParser.hpp>

#include <unordered_map>


class terrainosaurus::INIParser : public antlr::LLkParser {
/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // The integral numeric ID type
    typedef unsigned long PropertyID;
    
    // A mapping of PropertyID => ValueType, returning a configurable default
    // value if the key is not found in the map
    template <typename ValueType>
    class PropertyMap : public std::unordered_map<PropertyID, ValueType> {
    public:
        typedef std::unordered_map<PropertyID, ValueType> MapType;

        // Constructor
        explicit PropertyMap(ValueType def) : _defaultValue(def) { }
        
        // Const and non-const accessors returning default if not found
        ValueType operator[](PropertyID pID) const {
            typename MapType::const_iterator it = this->find(pID);
            return (it != this->end() ? it->second : defaultValue());
        }
        ValueType & operator[](PropertyID pID) {
            typename MapType::iterator it = this->find(pID);
            if (it == this->end())        // Stick in the default
                return insert(typename MapType::value_type(pID, defaultValue())).first->second;
            else
                return it->second;
        }
        
        // Accessors for 
        ValueType defaultValue() const { return _defaultValue; }
        void setDefaultValue(const ValueType & dv) { _defaultValue = dv; }
    
    protected:
        ValueType _defaultValue;
    };

    // Mappings of PropertyID => int or => RefToken
    typedef PropertyMap<int>                IntegerMap;
    typedef PropertyMap<antlr::RefToken>    TokenMap;


/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
public:
    explicit INIParser(antlr::TokenBuffer & tokenBuf, int k);
    explicit INIParser(antlr::TokenStream & lexer, int k);
    explicit INIParser(const antlr::ParserSharedInputState & state, int k);


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
public:
    // The sentinel value indicating "no limit"
    static const int UNLIMITED = -1;

    // Access the current state of the parsing process
    const std::string & currentSection() const;
    antlr::RefToken currentSectionStart() const;
    antlr::RefToken lastValidLine() const;
    antlr::RefToken lastAssignment(PropertyID) const;

    // Property counts, limits & requirements
    int propertyCount() const;
    int count(PropertyID pID) const;
    int requirement(PropertyID pID) const;
    int limit(PropertyID pID) const;
    bool limitReached(PropertyID pID) const;
    bool requirementReached(PropertyID pID) const;
    
protected:
    antlr::RefToken _currentSectionStart, _lastValidLine;
    TokenMap    _lastAssignments;
    std::string _currentSection;
    int         _propertyCount;
    IntegerMap _counts, _requirements, _limits;


/*---------------------------------------------------------------------------*
 | Parsing event functions
 *---------------------------------------------------------------------------*/
public:
    // Begin a section, specifying which properties are limited and/or required
    void beginSection(antlr::RefToken t, const std::string & name,
                      IntegerMap req = IntegerMap(0),
                      IntegerMap lim = IntegerMap(UNLIMITED));

    // Record that we saw a property assignment. If this exceeds the limit (if
    // any) specified in the preceding call to 'beginSection', this will raise
    // an exception.
    void recordAssignment(antlr::RefToken t, PropertyID pID);
    
    template <typename T>
    void recordAssignment(antlr::RefToken t, PropertyID pID, const T & val) {
        recordAssignment(t, pID);   // Validate the assignment
        INCA_DEBUG("Assigning " << propertyName(pID) << " = " << val)
    }
        
    
    // End a section. If any "required" properties have not been seen in this
    // section, this will raise an exception.
    void endSection();


/*---------------------------------------------------------------------------*
 | Formatting functions
 *---------------------------------------------------------------------------*/
public:
    // Retrieve the symbolic name for a property, given its ID. This
    // should be implemented by the subclass in order to give nice, friendly
    // error messages.
    virtual const char * propertyName(PropertyID pID) const = 0;

    // Format a line/column address for a token. This is used to report where
    // format errors occured.
    std::string fileLocation(antlr::RefToken t) const;
    std::string fileLine(antlr::RefToken t) const;
    std::string fileColumn(antlr::RefToken t) const;
};

#endif
