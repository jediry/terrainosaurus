/*
 * File: DEMM.g
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file specifies an ANTLR grammar for the Digital Elevation DEM
 *      (.dem) data file format. The DEM format
 *
 *      To start the parsing process, the "terrainDEM" rule in the generated
 *      parser should be called.
 */


// C++ Header section
header "pre_include_hpp" {
    // Import library configuration
    #include <terrainosaurus/terrainosaurus-common.h>

    // Forward declarations
    namespace terrainosaurus {
        class DEMLexer;
        class DEMParser;
    };

    // Import DEM and related object definitions 
    #include "../data/DEM.hpp"
}

// Global options section
options {
    language = "Cpp";               // Create C++ output
    namespace = "terrainosaurus";   // Put classes into terrainosaurus
    namespaceStd = "std";           // Cosmetic options to make ANTLR-generated
    namespaceAntlr = "antlr";       // code more readable
    genHashLines = true;            // Generate those #line thingies
}


/**
 * The DEMParser class processes tokens from a .dem file.
 */
class DEMParser extends Parser;
options {
    exportVocab = DEM;
    defaultErrorHandler=false;
    k = 2;
}


/* class terrainosaurus::DEMParser (internal functions) */ {
/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // Imported type definitions
    typedef DEM::scalar_t       scalar_t;
    typedef DEM::Point          Point;


/*---------------------------------------------------------------------------*
 | Helper function definitions (these help keep the parser grammar cleaner)
 | These are implemented in DEMParser-functions.cpp.
 *---------------------------------------------------------------------------*/
protected:
    void createVertex(scalar_t x, scalar_t y, antlr::RefToken pos);
    void setTerrainType(const string &tt, antlr::RefToken pos);
    void beginFace(antlr::RefToken pos);
    void addVertex(IDType id, antlr::RefToken pos);
    void endFace();

    // The map we're creating, plus the in-progress face (if any) and the
    // current TerrainType
    DEM * map;
    DEM::VertexPtrList vertices;
    TerrainTypePtr currentTT;
}


/*---------------------------------------------------------------------------*
 | Overall file format, record declarations/formats
 *---------------------------------------------------------------------------*/
// Entire file format (call this as the start rule)
terrainDEM [DEM * m]:
    { map = m; }            // Set this as the map we're gonna use
    { map != NULL }?        // Make sure it isn't NULL!
    { currentTT = map->terrainLibrary->terrainType(0); }    // Default TT
    ( vertexRecord
    | faceRecord
    | terrainTypeRecord
    | blankLine )* EOF ;

// A single-line vertex (vertex) declaration with x and y coordinates
vertexRecord:
    { scalar_t x, y; }
    t:VERTEX x=scalar y=scalar EOL
    { createVertex(x, y, t); } ;

// A single-line face (face) declaration with the list of vertices
faceRecord:
    { IDType id; }
    f:FACE { beginFace(f); } ( id=integer { addVertex(id, f); } )+ EOL
    { endFace(); } ;

// A single-line terrain-type declaration (applies hereafter, until changed)
terrainTypeRecord:
    TERRAIN_TYPE id:NAME EOL
    { setTerrainType(id->getText(), id); } ;

// A line with nothing important on it
blankLine: EOL ;


/*---------------------------------------------------------------------------*
 | Complex value types
 *---------------------------------------------------------------------------*/
// A real number, specified in standard notation (e.g. -3.42342)
scalar returns [DEMParser::scalar_t s]:
    (sg:SIGN)? s=fraction
    {
        // If we have a sign token and it is negative, then negate 's'
        if (sg != NULL && sg->getText() == "-")
            s *= -1;
    } ;


// A positive fractional value, >= 0.0
fraction returns [DEMParser::scalar_t s]:
    w:NUMBER ( DOT f:NUMBER )?
    {   string tmp(w->getText());           // Construct a composite string
        if (f != NULL) {                    // If we have a fractional part,
            tmp += '.';                     // then add it in
            tmp += f->getText();
        }
        s = atof(tmp.c_str());
    } ;


// A normalized fractional value, within [0.0, 1.0]
nFraction returns [DEMParser::scalar_t s]:
    s=fraction
    { s >= 0.0 && s <= 1.0 }? ; // Enforce s in [0.0, 1.0]


// An unsigned integer in decimal notation
integer returns [int value]: n:NUMBER { value = atoi(n->getText().c_str()); } ;


/**
 * The DEMLexer class creates tokens from the characters in
 * the .ttl file.
 */
class DEMLexer extends Lexer;
options {
    exportVocab = DEM;
    testLiterals = false;           // Don't usually look for literals
    caseSensitive = false;          // We're not picky...
    caseSensitiveLiterals = false;  // ...about identifiers either
    charVocabulary = '\3'..'\377';  // Stick to ASCII, please
    k = 2;
}

// Keywords
tokens {
    // Record type declarations
    VERTEX = "v" ;
    FACE   = "f" ;
    TERRAIN_TYPE = "tt" ;
}

// Primitive character classes
protected DIGIT  options { paraphrase = "digit"; }  : ( '0'..'9' ) ;
protected LETTER options { paraphrase = "letter"; } : ( 'a'..'z' ) ;

// Comments and whitespace
EOL options { paraphrase = "end of line"; } :
    ( "\r\n" | '\r' | '\n' )          { newline(); } ;
COMMENT options { paraphrase = "comment"; } :
    '#' (~('\r' | '\n'))* EOL         { $setType(EOL); } ;
WS : ( ' ' | '\t' )                   { $setType(antlr::Token::SKIP); } ;

// Delimiters
DOT options { paraphrase = "."; } : "." ;

// An unsigned integral number
NUMBER  options { paraphrase = "number"; } : (DIGIT)+ ;

// A positive or negative sign
SIGN  : ( '+' | '-' ) ;

// An identifier
NAME    options { paraphrase = "name"; testLiterals = true; } :
            ( LETTER | '_' ) ( LETTER | DIGIT | '_' )* ;
