/* -*- antlr -*-
 *
 * File: TerrainLibrary.g
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file specifies an ANTLR grammar for the Terrainosaurus
 *      terrain library (.ttl) data file format, which is based on the Windows
 *      .ini format.
 *
 *      To start the parsing process, the "sectionList" rule in the generated
 *      parser should be called.
 */


// C++ Header section
header "pre_include_hpp" {
    // Import library configuration
    #include <terrainosaurus/terrainosaurus-common.h>

    // Forward declarations
    namespace terrainosaurus {
        class TerrainLibraryParser;
        class TerrainLibraryLexer;
    };

    // Import parser superclass and supergrammer definitions
    #include <terrainosaurus/io/INIParser.hpp>
    #include "CommonParser.hpp"


    // Include STL string algorithms
    #include <algorithm>

    #include <unordered_map>


    // Import TerrainLibrary and related object definitions
    #include <terrainosaurus/data/TerrainLibrary.hpp>

    // Import augmented enumeration mechanism
    #include <inca/util/Enumeration.hpp>
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
 * The TerrainLibraryParser class processes tokens from a .ttl file.
 */
class TerrainLibraryParser extends Parser("::terrainosaurus::INIParser");
options {
    exportVocab = TerrainLibrary;
    defaultErrorHandler=false;
    k = 2;
}


/* class terrainosaurus::TerrainLibraryParser (internal functions) */ {
/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // This enumerates all the properties that we can check/set. The
    // functions that check/set them implicitly work on the "current"
    // TerrainType or TerrainSeam.
    ENUM ( PropertyType,
        /* Properties for a TerrainType */
        ( TTColor,
        ( TTSample,

        /* Properties for the TerrainSeam GA */
        ( TSSmoothness, 
        ( TSAspectRatio,
          EOL )))));


/*---------------------------------------------------------------------------*
 | Helper function definitions (these help keep the parser grammar cleaner)
 | These are implemented in TerrainLibraryParser-functions.cpp.
 *---------------------------------------------------------------------------*/
protected:
    // These create new TTs and TSs and assign properties on them (implicity
    // modifying the most recent TT or TS). They will throw exceptions if
    // an attempt is made to assign/create something that has already been
    // assign/created, thus effectively preventing duplicate entries
    void beginTerrainTypeSection(antlr::RefToken t, const std::string & tt);
    void beginTerrainSeamSection(antlr::RefToken t, const std::string & tt1,
                                                    const std::string & tt2);
    void endSection();
    void assignColorProperty(antlr::RefToken t, PropertyType p, const Color & c);
    void assignScalarProperty(antlr::RefToken t, PropertyType p, scalar_t s);
    void assignIntegerProperty(antlr::RefToken t, PropertyType p, int i);
    void assignStringProperty(antlr::RefToken t, PropertyType p, const std::string & s);
    
    // This provides a symbolic interpretation of integer property IDs
    const char * propertyName(PropertyID) const;


    // The library we're populating, plus the current TT and TS
    TerrainLibrary * _terrainLibrary;
    TerrainTypePtr _currentTT;
    TerrainSeamPtr _currentTS;
    std::unordered_map<TerrainSeamPtr, bool> _initializedTSs;
}


/*---------------------------------------------------------------------------*
 | Overall file format, section declarations/formats
 *---------------------------------------------------------------------------*/
// Entire file format (call this as the start rule)
sectionList [TerrainLibrary * lib]:
    { _terrainLibrary = lib; }      // Set this as our TL object
    { _terrainLibrary != NULL }?    // Make sure it isn't NULL!
    (blankLine)* ( terrainTypeSection
                 | terrainSeamSection )* EOF ;

// A TerrainType declaration, followed by one or more properties
terrainTypeSection { std::string tt; }:
    t:LEFT_SBRACKET TERRAIN_TYPE COLON tt=string RIGHT_SBRACKET EOL
    { beginTerrainTypeSection(t, tt); }
            ( blankLine
            | terrainColor
            | terrainSample )*
    { endSection(); } ;

// A TerrainSeam declaration, followed by one or more properties
terrainSeamSection { std::string tt1, tt2; }:
    t:LEFT_SBRACKET TERRAIN_SEAM COLON
        tt1=string AMPERSAND tt2=string RIGHT_SBRACKET EOL
    { beginTerrainSeamSection(t, tt1, tt2); }
            ( blankLine
            | smoothness
            | aspectRatio )*
    { endSection(); } ;


/*---------------------------------------------------------------------------*
 | TerrainType properties
 *---------------------------------------------------------------------------*/
// color = <R, G, B, A>
terrainColor { Color c; }:
    t:COLOR ASSIGN c=color EOL
    { assignColorProperty(t, TTColor, c); } ;

// sample = name
terrainSample { std::string s; }:
    t:SAMPLE ASSIGN s=string EOL
    { assignStringProperty(t, TTSample, s); } ;


/*---------------------------------------------------------------------------*
 | TerrainSeam properties
 *---------------------------------------------------------------------------*/
// Smoothness = 0.5
smoothness { scalar_t s; }:
    t:SMOOTHNESS ASSIGN s=nFraction EOL
    { assignScalarProperty(t, TSSmoothness, s); } ;

// Aspect Ratio = 1.0
aspectRatio { scalar_t s; }:
    t:ASPECT RATIO ASSIGN s=fraction EOL
    { assignScalarProperty(t, TSAspectRatio, s); } ;


/*---------------------------------------------------------------------------*
 | Dummy rule
 *---------------------------------------------------------------------------*/
// This is just to make the generated code compile...thx, Antlr 2.7.5!
antlrDummyRule: ANTLR_DUMMY_TOKEN ;

// A string (a single word, perhaps with embedded non-breaking spaces, or a
// quoted string)
string returns [std::string s]:
    ( q:QUOTED_STRING       { s += q->getText();  }
    | ( i1:ID               { s += i1->getText(); }
      | n1:NUMBER           { s += n1->getText(); }
        ( NBSP              { s += " ";           }
            ( i2:ID         { s += i2->getText(); }
            | n2:NUMBER     { s += n2->getText(); }
            )
         )*
      )+
    ) ;


// An untranslated filesystem path
path returns [std::string p]:
    ( s:QUOTED_STRING   { p += s->getText(); }
    | i:ID              { p += i->getText(); }
    | n:NUMBER          { p += n->getText(); }
    | c:COLON           { p += c->getText(); }
    | d:DOT             { p += d->getText(); }
    | b:BACKSLASH       { p += b->getText(); }
    | f:FORESLASH       { p += f->getText(); }
    | NBSP              { p += " "; }
    )+ ;


// A UNIX-style ('/'-separated) path
unixPath returns [std::string p]:
    p=path
    {   // Transliterate all '\'s into '/'s
        std::transform(p.begin(), p.end(), p.begin(), DOSToUNIX());
    } ;


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


// A color specified with RGB components between 0.0 and 1.0
color returns [Color c]:
    { scalar_t r, g, b, a; }
    LEFT_ABRACKET
        r=nFraction COMMA g=nFraction COMMA b=nFraction COMMA a=nFraction
    RIGHT_ABRACKET

    // Create a Color object from components
    { typedef Color::scalar_t c_scalar_t;
      c = Color(c_scalar_t(r), c_scalar_t(g),
                c_scalar_t(b), c_scalar_t(a)); } ;


// An unsigned integer in decimal notation
integer returns [int value]: n:NUMBER { value = atoi(n->getText().c_str()); } ;



/**
 * The TerrainLibraryLexer class creates tokens from the characters in
 * the .ttl file.
 */
class TerrainLibraryLexer extends Lexer;
options {
    exportVocab = TerrainLibrary;
    testLiterals = false;           // Don't usually look for literals
    caseSensitive = false;          // We're not picky...
    caseSensitiveLiterals = false;  // ...about identifiers either
    charVocabulary = '\3'..'\377';  // Stick to ASCII, please
    k = 2;
}

// Keywords
tokens {
    // Section type declarations
    TERRAIN_TYPE = "terraintype" ;
    TERRAIN_SEAM = "terrainseam" ;

    // TerrainType section property keywords
    COLOR       = "color" ;
    SAMPLE      = "sample" ;

    // TerrainSeam section property keywords
    SMOOTHNESS  = "smoothness" ;
    ASPECT      = "aspect" ;
    RATIO       = "ratio" ;
}


ID options { paraphrase = "id"; testLiterals = true; } :
            ( LETTER | '_' ) ( LETTER | DIGIT | '_' )* ;

// Dummy lexical token (antlr 2.7.5 won't compile this w/o at least one rule)
ANTLR_DUMMY_TOKEN : "%#%dummy%#%" ;

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
//ID options { paraphrase = "name"; testLiterals = true; } :
//            ( LETTER | '_' ) ( LETTER | DIGIT | '_' )* ;

// A string in quotes
QUOTED_STRING options { paraphrase = "quoted string"; } :
          ( DQUOTE (~'"')* DQUOTE )
        | ( SQUOTE (~'\'')* SQUOTE ) ;
//          ( DQUOTE s:(~S)* DQUOTE ) { $setText(s->getText()); }
//        | ( SQUOTE t:(.)* SQUOTE ) { $setText(t->getText()); } ;
