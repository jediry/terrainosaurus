/* -*- antlr -*-
 *
 * File: Map.g
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file specifies an ANTLR grammar for the Terrainosaurus
 *      map (.map) data file format. The map format is essentially a 2D,
 *      stripped-down version of the Alias|Wavefront OBJ model format, with
 *      a few keywords changed in order to be more consistent with
 *      Terrainosaurus terminology.
 *
 *      To start the parsing process, the "terrainMap" rule in the generated
 *      parser should be called.
 */


// C++ Header section
header "pre_include_hpp" {
    // Import library configuration
    #include <terrainosaurus/terrainosaurus-common.h>

    // Forward declarations
    namespace terrainosaurus {
        class MapLexer;
        class MapParser;
    };

    // Import Map and related object definitions 
    #include <terrainosaurus/data/Map.hpp>

    // Import parser superclass and supergrammer definitions
    #include "CommonParser.hpp"
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
 * The MapParser class processes tokens from a .map file.
 */
class MapParser extends Parser;
options {
    exportVocab = Map;
    defaultErrorHandler=false;
    k = 2;
}


/* class terrainosaurus::MapParser (internal functions) */ {
/*---------------------------------------------------------------------------*
 | Helper function definitions (these help keep the parser grammar cleaner)
 | These are implemented in MapParser-functions.cpp.
 *---------------------------------------------------------------------------*/
protected:
    void createVertex(scalar_t x, scalar_t y, antlr::RefToken pos);
    void setTerrainType(const std::string & tt, antlr::RefToken pos);
    void beginFace(antlr::RefToken pos);
    void addVertex(IDType id, antlr::RefToken pos);
    void endFace();

    // The map we're creating, plus the in-progress face (if any) and the
    // current TerrainType
    Map * map;
    Map::VertexPtrList vertices;
    TerrainTypePtr currentTT;
}


/*---------------------------------------------------------------------------*
 | Overall file format, record declarations/formats
 *---------------------------------------------------------------------------*/
// Entire file format (call this as the start rule)
terrainMap [Map * m]:
    { map = m; }            // Set this as the map we're gonna use
    { map != NULL }?        // Make sure it isn't NULL!
    { currentTT = map->terrainLibrary->terrainType(0); }    // Default TT
    ( vertexRecord
    | faceRecord
    | terrainTypeRecord
    | blankLine )* EOF ;

// A single-line vertex declaration with x and y coordinates
vertexRecord:
    { scalar_t x, y; }
    t:VERTEX x=scalar y=scalar EOL
    { createVertex(x, y, t); } ;

// A single-line face declaration with the list of vertices
faceRecord:
    { IDType id; }
    f:FACE { beginFace(f); } ( id=integer { addVertex(id, f); } )+ EOL
    { endFace(); } ;

// A single-line terrain-type declaration (applies hereafter, until changed)
terrainTypeRecord:
    TERRAIN_TYPE id:NAME EOL
    { setTerrainType(id->getText(), id); } ;


/*---------------------------------------------------------------------------*
 | Dummy rule
 *---------------------------------------------------------------------------*/


// A line with nothing important on it
blankLine: EOL ;


// A positive real number, specified in any of the following formats:
//      standard decimal notation (1.234)
//      percent notation (123.4%)
//      scientific notation (0.1234e1)
// These may be combined (e.g., 1234.0e-1%)
fraction returns [scalar_t s]:
    w:NUMBER ( DOT f:NUMBER )? ( "e" (sg:SIGN)? e:NUMBER )? (p:PERCENT)?
    {
        // First, convert the floating point number
        std::string tmp(w->getText());          // Whole # part
        tmp += ".";                             // Decimal point
        if (f != NULL)  tmp += f->getText();    // Fractional part
        else            tmp += "0";             // Implicit zero
        if (e != NULL) {                        // Exponent
            tmp += "e";
            if (sg != NULL) tmp += sg->getText();
            tmp += e->getText();
        }
        s = scalar_t(atof(tmp.c_str()));
        
        // If this is a percent, convert to normal decimal by dividing by 100
        if (p != NULL)  s /= 100;
    } ;


// A positive or negative real number, with the same format as the 'fraction'
// type (see preceding 'fraction' rule).
scalar returns [scalar_t s]:
    (sg:SIGN)? s=fraction
    {
        // If we have a sign token and it is negative, then negate 's'
        if (sg != NULL && sg->getText() == "-")
            s *= -1;
    } ;


// A positive real number, clamped to the range [0.0, 1.0], with the same
// format as the 'fraction' type (see preceding 'fraction' rule).
nFraction returns [scalar_t s]:
    s=fraction
    { s >= scalar_t(0) && s <= scalar_t(1) }? ; // Enforce s in [0.0, 1.0]


// An unsigned integer in decimal notation
integer returns [int value]: n:NUMBER { value = atoi(n->getText().c_str()); } ;


/**
 * The MapLexer class creates tokens from the characters in
 * the .map file.
 */
class MapLexer extends Lexer;
options {
    exportVocab = Map;
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
protected DIGIT     : ( '0'..'9' ) ;
protected LETTER    : ( 'a'..'z' ) ;
protected WS_CHAR   : ( ' ' | '\t' ) ;
protected EOL_CHAR  : ( "\r\n" | '\r' | '\n' ) ;
protected DQUOTE    : '"'   { $setText(""); } ;
protected SQUOTE    : '\''  { $setText(""); } ;
protected SPACE     : ' ' ;

// Comments and whitespace
EOL options { paraphrase = "end of line"; } : EOL_CHAR  { newline(); } ;
COMMENT options { paraphrase = "comment"; } :
    '#' ( ~('\r' | '\n') )* EOL_CHAR  { newline(); $setType(EOL); } ;
WS : ( WS_CHAR )+                     { $setType(antlr::Token::SKIP); } ;

// Delimiters
LEFT_ABRACKET   : '<' ;
RIGHT_ABRACKET  : '>' ;
LEFT_SBRACKET   : '[' ;
RIGHT_SBRACKET  : ']' ;
COMMA           : ',' ;
ASSIGN          : '=' ;
PERCENT         : '%' ;
COLON           : ':' ;
AMPERSAND       : '&' ;
DOT             : '.' ;
BACKSLASH       : '\\' ;
FORESLASH       : '/' ;
NBSP            : "\\ " ;

// A positive or negative sign
SIGN  : ( '+' | '-' ) ;

// An unsigned integral number
NUMBER  options { paraphrase = "number"; } : (DIGIT)+ ;

// An identifier
NAME options { paraphrase = "name"; testLiterals = true; } :
            ( LETTER | '_' ) ( LETTER | DIGIT | '_' )* ;

// A string in quotes
QUOTED_STRING options { paraphrase = "quoted string"; } :
          ( DQUOTE (~'"')* DQUOTE )
        | ( SQUOTE (~'\'')* SQUOTE ) ;
//          ( DQUOTE s:(~S)* DQUOTE ) { $setText(s->getText()); }
//        | ( SQUOTE t:(.)* SQUOTE ) { $setText(t->getText()); } ;

// Dummy lexical token (antlr 2.7.5 won't compile this w/o at least one rule)
ANTLR_DUMMY_TOKEN : "%#%dummy%#%" ;
