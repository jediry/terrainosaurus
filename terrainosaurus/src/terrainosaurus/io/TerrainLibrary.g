/*
 * File: TerrainLibrary.g
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file specifies an ANTLR grammar for the Terrainosaurus
 *      terrain library (.ttl) data file format. This is probably a
 *      to-be-short-lived hack.
 *
 *      To start the parsing process, the "recordList" rule in the generated
 *      parser should be called.
 */


// C++ Header section
header "pre_include_hpp" {
    // Import library configuration
    #include <terrainosaurus/terrainosaurus-common.h>

    // Forward declarations
    namespace terrainosaurus {
        class TerrainLibraryLexer;
        class TerrainLibraryParser;
    };

    // Import TerrainLibrary and related object definitions
    #include <terrainosaurus/data/TerrainLibrary.hpp>

    // Import container definitions
    #include <inca/util/hash_container>

    // Import augmented enumeration mechanism
//    #include <inca/util/Enumeration.hpp>

    // This enumerates all the properties that we can check/set. The
    // functions that check/set them implicitly work on the "current"
    // TerrainType or TerrainSeam.
/*    ENUMV ( TTLPropertyType,
          ( ( TTColor,             1 ),
          ( ( TSNumChromosomes,    2 ),
          ( ( TSSmoothness,        4 ),
          ( ( TSMutationRatio,     8 ),
          ( ( TSCrossoverRatio,    16 ),
          ( ( TSSelectionRatio,    32 ),
          ( ( TSAspectRatio,       64 ),
              BOOST_PP_NIL ))))))));*/
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
class TerrainLibraryParser extends Parser;
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
    enum PropertyType {
        // Properties for TerrainType
        TTColor                 = 0x00001,

        // Properties for the TerrainSeam GA
        TSSmoothness            = 0x00002,
        TSAspectRatio           = 0x00004,

        // General GA parameters
        GAEvolutionCycles       = 0x00100,
        GAPopulationSize        = 0x00200,
        GAEliteRatio            = 0x00400,
        GASelectionRatio        = 0x00800,
        GAMutationRatio         = 0x01000,
        GACrossoverRatio        = 0x02000,
        GAMutationProbability   = 0x04000,
        GACrossoverProbability  = 0x08000,

        // Properties for the heightfield GA
        HFMaxCrossoverWidth     = 0x10000,
        HFMaxJitterPixels       = 0x20000,
        HFMaxScaleFactor        = 0x40000,
        HFMaxOffsetAmount       = 0x80000,
    };


/*---------------------------------------------------------------------------*
 | Helper function definitions (these help keep the parser grammar cleaner)
 | These are implemented in TerrainLibraryParser-functions.cpp.
 *---------------------------------------------------------------------------*/
protected:
    // These create new TTs and TSs and set properties on them (implicity
    // modifying the most recent TT or TS). They will throw exceptions if
    // an attempt is made to set/create something that has already been
    // set/created, thus effectively preventing duplicate entries
    void beginGlobalSection(antlr::RefToken tt);
    void createTerrainType(antlr::RefToken tt);
    void createTerrainSeam(antlr::RefToken tt1, antlr::RefToken tt2);
    void endRecord(antlr::RefToken t);
    void addTerrainSample(const std::string & path, int line);
    void setColorProperty(PropertyType p, const Color &c, int line);
    void setScalarProperty(PropertyType p, scalar_t s, int line);
    void setIntegerProperty(PropertyType p, int i, int line);


    // The library we're populating, plus the current TT and TS, and a
    // record of which properties we've set on the current object
    TerrainLibrary * library;
    bool inGlobal;
    TerrainTypePtr currentTT;
    TerrainSeamPtr currentTS;
    stl_ext::hash_map<TerrainSeamPtr, bool> initializedTSs;
    unsigned int setProperties;
}


/*---------------------------------------------------------------------------*
 | Overall file format, record declarations/formats
 *---------------------------------------------------------------------------*/
// Entire file format (call this as the start rule)
recordList [TerrainLibrary * lib]:
    { library = lib; }      // Set this as the library we're gonna use
    { inGlobal = false; }   // Start out NOT in the global section
    { library != NULL }?    // Make sure it isn't NULL!
    (blankLine)* ( terrainTypeRecord
                 | terrainSeamRecord
                 | globalSectionRecord )* EOF ;

// A globals section declaration, followed by one or more properties
globalSectionRecord:
    globalSectionDeclaration ( blankLine
                             | populationSize
                             | evolutionCycles
                             | selectionRatio
                             | eliteRatio
                             | mutationProbability
                             | mutationRatio
                             | crossoverProbability
                             | crossoverRatio
                             | maxCrossoverWidth
                             | maxJitterPixels
                             | maxScaleFactor
                             | maxOffsetAmount )* ;

// A TerrainType declaration, followed by one or more properties
terrainTypeRecord:
    terrainTypeDeclaration ( blankLine
                           | terrainColor
                           | terrainSample )* ;

// A TerrainSeam declaration, followed by one or more properties
terrainSeamRecord:
    terrainSeamDeclaration  ( blankLine
                            | smoothness
                            | aspectRatio
                            | populationSize
                            | evolutionCycles
                            | selectionRatio
                            | crossoverRatio
                            | mutationRatio )* ;

// [Global]
globalSectionDeclaration:
    n:OPEN_SBRACKET GLOBAL CLOSE_SBRACKET EOL
    { beginGlobalSection(n); } ;

// [TerrainType: Snow]
terrainTypeDeclaration:
    OPEN_SBRACKET TERRAIN_TYPE COLON n:NAME CLOSE_SBRACKET EOL
    { createTerrainType(n); } ;

// [TerrainSeam: Snow & Grass]
terrainSeamDeclaration:
    OPEN_SBRACKET TERRAIN_SEAM COLON n1:NAME AND n2:NAME CLOSE_SBRACKET EOL
    { createTerrainSeam(n1, n2); } ;

// A line with nothing important on it
blankLine: EOL ;


/*---------------------------------------------------------------------------*
 | Genetic algorithm parameters
 *---------------------------------------------------------------------------*/
// population size = n
populationSize { int n; }:
    t:POPULATION SIZE ASSIGN n=integer EOL
    { setIntegerProperty(GAPopulationSize, n, t->getLine()); } ;

// evolution cycles = n
evolutionCycles { int n; }:
    t:EVOLUTION CYCLES ASSIGN n=integer EOL
    { setIntegerProperty(GAEvolutionCycles, n, t->getLine()); } ;

// selection ratio = r
selectionRatio { scalar_t r; }:
    t:SELECTION RATIO ASSIGN r=fraction EOL
    { setScalarProperty(GASelectionRatio, r, t->getLine()); } ;

// selection ratio = r
eliteRatio { scalar_t r; }:
    t:ELITE RATIO ASSIGN r=fraction EOL
    { setScalarProperty(GAEliteRatio, r, t->getLine()); } ;

// crossover probability = p
crossoverProbability { scalar_t p; }:
    t:CROSSOVER PROBABILITY ASSIGN p=fraction EOL
    { setScalarProperty(GACrossoverProbability, p, t->getLine()); } ;

// crossover ratio = r
crossoverRatio { scalar_t r; }:
    t:CROSSOVER RATIO ASSIGN r=fraction EOL
    { setScalarProperty(GACrossoverRatio, r, t->getLine()); } ;

// mutation probability = p
mutationProbability { scalar_t p; }:
    t:MUTATION PROBABILITY ASSIGN p=fraction EOL
    { setScalarProperty(GAMutationProbability, p, t->getLine()); } ;

// mutation ratio = r
mutationRatio { scalar_t r; }:
    t:MUTATION RATIO ASSIGN r=fraction EOL
    { setScalarProperty(GAMutationRatio, r, t->getLine()); } ;


/*---------------------------------------------------------------------------*
 | Heightfield GA limits
 *---------------------------------------------------------------------------*/
// max crossover width = n
maxCrossoverWidth { int n; }:
    t:MAX CROSSOVER WIDTH ASSIGN n=integer EOL
    { setIntegerProperty(HFMaxCrossoverWidth, n, t->getLine()); } ;

// max jitter pixels = n
maxJitterPixels { int n; }:
    t:MAX JITTER PIXELS ASSIGN n=integer EOL
    { setIntegerProperty(HFMaxJitterPixels, n, t->getLine()); } ;

// max scale factor = f
maxScaleFactor { scalar_t f; }:
    t:MAX SCALE FACTOR ASSIGN f=scalar EOL
    { setScalarProperty(HFMaxScaleFactor, f, t->getLine()); } ;

// max scale factor = f
maxOffsetAmount { scalar_t a; }:
    t:MAX OFFSET AMOUNT ASSIGN a=scalar EOL
    { setScalarProperty(HFMaxOffsetAmount, a, t->getLine()); } ;


/*---------------------------------------------------------------------------*
 | TerrainType properties
 *---------------------------------------------------------------------------*/
// color = <R, G, B, A>
terrainColor { Color c; }:
    t:COLOR ASSIGN c=color EOL
    { setColorProperty(TTColor, c, t->getLine()); } ;

// sample = filename.dem
terrainSample: { std::string s; }
    t:SAMPLE ASSIGN s=filename EOL
    { addTerrainSample(s, t->getLine()); } ;


/*---------------------------------------------------------------------------*
 | TerrainSeam properties
 *---------------------------------------------------------------------------*/
// Smoothness = 0.5
smoothness { scalar_t n; }:
    t:SMOOTHNESS ASSIGN n=nFraction EOL
    { setScalarProperty(TSSmoothness, n, t->getLine()); } ;

// Aspect Ratio = 1.0
aspectRatio { scalar_t n; }:
    t:ASPECT RATIO ASSIGN n=fraction EOL
    { setScalarProperty(TSAspectRatio, n, t->getLine()); } ;


/*---------------------------------------------------------------------------*
 | Complex value types
 *---------------------------------------------------------------------------*/
// A real number, specified in standard notation (e.g. -3.42342)
scalar returns [scalar_t s]:
    (sg:SIGN)? s=fraction
    {
        // If we have a sign token and it is negative, then negate 's'
        if (sg != NULL && sg->getText() == "-")
            s *= -1;
    } ;


// A positive fractional value, >= 0.0
fraction returns [scalar_t s]:
    w:NUMBER ( DOT f:NUMBER )?
    {   std::string tmp(w->getText());      // Construct a composite string
        if (f != NULL) {                    // If we have a fractional part,
            tmp += '.';                     // then add it in
            tmp += f->getText();
        }
        s = scalar_t(atof(tmp.c_str()));
    } ;


// A normalized fractional value, within [0.0, 1.0]
nFraction returns [scalar_t s]:
    s=fraction
    { s >= 0.0 && s <= 1.0 }? ; // Enforce s in [0.0, 1.0]


// A color specified with RGB components between 0.0 and 1.0
color returns [Color c]:
    { scalar_t r, g, b, a; }
    OPEN_ABRACKET
        r=nFraction COMMA g=nFraction COMMA b=nFraction COMMA a=nFraction
    CLOSE_ABRACKET

    // Create a Color object from components
    { typedef Color::scalar_t c_scalar_t;
      c = Color(c_scalar_t(r), c_scalar_t(g),
                c_scalar_t(b), c_scalar_t(a)); } ;

// An unsigned integer in decimal notation
integer returns [int value]: n:NUMBER { value = atoi(n->getText().c_str()); } ;


// A file path
filename returns [std::string path]:
    ( q:QUOTED_STRING           { path += q->getText(); }
    | n:NAME                    { path += n->getText(); }
    | i:NUMBER                  { path += i->getText(); }
    | c:COLON                   { path += c->getText(); }
    | d:DOT                     { path += d->getText(); }
    | ( BACKSLASH | FORESLASH ) { path += "/"; }
    | NBSP                      { path += " "; }
    )+ ;


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
    k = 30;
}

// Keywords
tokens {
    // Record type declarations
    TERRAIN_TYPE = "terraintype" ;
    TERRAIN_SEAM = "terrainseam" ;
    GLOBAL       = "global" ;

    // TerrainType property keywords
    COLOR       = "color" ;
    SAMPLE      = "sample" ;

    // TerrainSeam property keywords
    SMOOTHNESS  = "smoothness" ;
    ASPECT      = "aspect" ;

    // Genetic algorithm parameter keywords
    POPULATION  = "population" ;
    SIZE        = "size" ;
    EVOLUTION   = "evolution" ;
    CYCLES      = "cycles" ;
    MUTATION    = "mutation" ;
    CROSSOVER   = "crossover" ;
    SELECTION   = "selection" ;
    ELITE       = "elite" ;
    RATIO       = "ratio" ;
    PROBABILITY = "probability" ;

    // Heightfield cross/mutate limit keywords
    MAX     = "max" ;
    WIDTH   = "width" ;
    SCALE   = "scale" ;
    FACTOR  = "factor" ;
    OFFSET  = "offset" ;
    AMOUNT  = "amount" ;
    JITTER  = "jitter" ;
    PIXELS  = "pixels" ;
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
    '#' ( ~('\r' | '\n') )* EOL_CHAR  { $setType(EOL); } ;
WS : ( WS_CHAR )+                     { $setType(antlr::Token::SKIP); } ;

// Delimiters
OPEN_ABRACKET   : "<" ;
CLOSE_ABRACKET  : ">" ;
OPEN_SBRACKET   : "[" ;
CLOSE_SBRACKET  : "]" ;
COMMA           : "," ;
ASSIGN          : "=" ;
COLON           : ":" ;
AND             : "&" ;
DOT             : "." ;
BACKSLASH       : '\\' ;
FORESLASH       : '/' ;
NBSP            : "\\ " ;

// An unsigned integral number
NUMBER  options { paraphrase = "number"; } : (DIGIT)+ ;

// A positive or negative sign
SIGN  : ( '+' | '-' ) ;

// An identifier
NAME    options { paraphrase = "name"; testLiterals = true; } :
            ( LETTER | '_' ) ( LETTER | DIGIT | '_' )* ;

// A string in quotes
QUOTED_STRING options { paraphrase = "quoted string"; } :
          ( DQUOTE (~'"')* DQUOTE )
        | ( SQUOTE (~'\'')* SQUOTE ) ;
//          ( DQUOTE s:(~S)* DQUOTE ) { $setText(s->getText()); }
//        | ( SQUOTE t:(.)* SQUOTE ) { $setText(t->getText()); } ;
