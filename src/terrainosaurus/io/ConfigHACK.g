/* -*- antlr -*-
 *
 * File: Config.g
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file specifies an ANTLR grammar for the Terrainosaurus
 *      configuration (.conf) file format, which is based on the Windows
 *      .ini file format.
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
        class ConfigParser;
        class ConfigLexer;
    };

    // Import parser superclass and supergrammar definition
    #include <terrainosaurus/io/INIParser.hpp>
    #include "CommonParser.hpp"
    
    // Import application definition
    #include <terrainosaurus/TerrainosaurusApplication.hpp>

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
 * The ConfigParser class processes tokens from a .conf file.
 */
class ConfigParser extends Parser("::terrainosaurus::INIParser");
options {
    exportVocab = Config;
    defaultErrorHandler=false;
    k = 3;
}

/* class terrainosaurus::ConfigParser (internal functions) */ {
/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:

    // This enumerates all the properties that we can check/set. Not all of
    // them are valid in every section.
    ENUM ( PropertyType,
        /* Application parameters */
        ( CacheDirectory,

        /* General GA parameters */
        ( EvolutionCycles,    
        ( PopulationSize,     
        ( ElitismRatio,         
        ( SelectionRatio,     
        ( MutationRatio,      
        ( CrossoverRatio,     
        ( MutationProbability,
        ( CrossoverProbability,

        /* Properties for the heightfield GA */
        ( GeneSize,         
        ( OverlapFactor,
        ( MaxCrossoverWidth,
        ( MaxVerticalScale, 
        ( MaxVerticalOffset,
          EOL )))))))))))))));


/*---------------------------------------------------------------------------*
 | Helper function definitions (these help keep the parser grammar cleaner)
 | These are implemented in ConfigParser-functions.cpp.
 *---------------------------------------------------------------------------*/
protected:
    // These begin & end file sections, and assign properties within sections.
    void beginApplicationSection(antlr::RefToken t);
    void beginHeightfieldGASection(antlr::RefToken t);
    void beginBoundaryGASection(antlr::RefToken t);
    void beginErosionSection(antlr::RefToken t);
    void endSection();
    void assignColorProperty(antlr::RefToken t, PropertyType p, const Color & c);
    void assignScalarProperty(antlr::RefToken t, PropertyType p, scalar_t s);
    void assignIntegerProperty(antlr::RefToken t, PropertyType p, int i);
    void assignStringProperty(antlr::RefToken t, PropertyType p, const std::string & s);
    
    // This provides a symbolic interpretation of integer property IDs
    const char * propertyName(PropertyID) const;
    
    // The application object receiving settings from the file
    TerrainosaurusApplication * _application;
}


/*---------------------------------------------------------------------------*
 | File sections
 *---------------------------------------------------------------------------*/
// Entire file format (call this rule first)
sectionList[TerrainosaurusApplication * app] { _application = app; }:
    (blankLine)*  applicationSection
                  boundaryGASection
                  heightfieldGASection
                  ( erosionSection )* EOF ;

// [Application] section
applicationSection :
    t:LEFT_SBRACKET APPLICATION RIGHT_SBRACKET EOL
    { beginApplicationSection(t); }
        ( blankLine
        | cacheDirectory )*
    { endSection(); } ;

// [Boundary GA] section
boundaryGASection :
    t:LEFT_SBRACKET BOUNDARY GA RIGHT_SBRACKET EOL
    { beginBoundaryGASection(t); }
        ( blankLine
        | populationSize
        | evolutionCycles
        | selectionRatio
        | elitismRatio
        | mutationProbability
        | mutationRatio
        | crossoverProbability
        | crossoverRatio )*
    { endSection(); } ;

// [Heightfield GA] section
heightfieldGASection :
    t:LEFT_SBRACKET HEIGHTFIELD GA RIGHT_SBRACKET EOL
    { beginHeightfieldGASection(t); }
        ( blankLine
        | populationSize
        | evolutionCycles
        | selectionRatio
        | elitismRatio
        | mutationProbability
        | mutationRatio
        | crossoverProbability
        | crossoverRatio
        | geneSize
        | overlapFactor
        | maxCrossoverWidth
        | maxVerticalScale
        | maxVerticalOffset )*
    { endSection(); } ;

// [Erosion] section
erosionSection :
    t:LEFT_SBRACKET EROSION RIGHT_SBRACKET EOL
    { beginErosionSection(t); }
        ( blankLine )*
    { endSection(); } ;


/*---------------------------------------------------------------------------*
 | Application parameters
 *---------------------------------------------------------------------------*/
// cache directory = path
cacheDirectory { std::string s; }:
    t:POPULATION SIZE ASSIGN s=unixPath EOL
    { assignStringProperty(t, CacheDirectory, s); } ;


/*---------------------------------------------------------------------------*
 | Genetic algorithm parameters (common to HF and B GAs)
 *---------------------------------------------------------------------------*/
// population size = n
populationSize { int n; }:
    t:POPULATION SIZE ASSIGN n=integer EOL
    { assignIntegerProperty(t, PopulationSize, n); } ;

// evolution cycles = n
evolutionCycles { int n; }:
    t:EVOLUTION CYCLES ASSIGN n=integer EOL
    { assignIntegerProperty(t, EvolutionCycles, n); } ;

// selection ratio = r
selectionRatio { scalar_t r; }:
    t:SELECTION RATIO ASSIGN r=fraction EOL
    { assignScalarProperty(t, SelectionRatio, r); } ;

// elitism ratio = r
elitismRatio { scalar_t r; }:
    t:ELITISM RATIO ASSIGN r=fraction EOL
    { assignScalarProperty(t, ElitismRatio, r); } ;

// crossover probability = p
crossoverProbability { scalar_t p; }:
    t:CROSSOVER PROBABILITY ASSIGN p=fraction EOL
    { assignScalarProperty(t, CrossoverProbability, p); } ;

// crossover ratio = r
crossoverRatio { scalar_t r; }:
    t:CROSSOVER RATIO ASSIGN r=fraction EOL
    { assignScalarProperty(t, CrossoverRatio, r); } ;

// mutation probability = p
mutationProbability { scalar_t p; }:
    t:MUTATION PROBABILITY ASSIGN p=fraction EOL
    { assignScalarProperty(t, MutationProbability, p); } ;

// mutation ratio = r
mutationRatio { scalar_t r; }:
    t:MUTATION RATIO ASSIGN r=fraction EOL
    { assignScalarProperty(t, MutationRatio, r); } ;


/*---------------------------------------------------------------------------*
 | Boundary GA parameters
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 | Heightfield GA parameters
 *---------------------------------------------------------------------------*/
// gene size = n
geneSize { int n; }:
    t:GENE SIZE ASSIGN n=integer EOL
    { assignIntegerProperty(t, GeneSize, n); } ;

// overlap factor = f
overlapFactor { scalar_t f; }:
    t:OVERLAP FACTOR ASSIGN f=fraction EOL
    { assignScalarProperty(t, OverlapFactor, f); } ;

// max crossover width = n
maxCrossoverWidth { int n; }:
    t:MAX CROSSOVER WIDTH ASSIGN n=integer EOL
    { assignIntegerProperty(t, MaxCrossoverWidth, n); } ;

// max vertical scale = s
maxVerticalScale { scalar_t s; }:
    t:MAX VERTICAL SCALE ASSIGN s=scalar EOL
    { assignScalarProperty(t, MaxVerticalScale, s); } ;

// max vertical offset = s
maxVerticalOffset { scalar_t s; }:
    t:MAX VERTICAL OFFSET ASSIGN s=scalar EOL
    { assignScalarProperty(t, MaxVerticalOffset, s); } ;


/*---------------------------------------------------------------------------*
 | Erosion parameters
 *---------------------------------------------------------------------------*/
// Ben, add your property definitions here, and don't forget to add 'em to the
// 'erosionSection' rule above...

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



/*---------------------------------------------------------------------------*
 | Dummy rule
 *---------------------------------------------------------------------------*/
// This is just to make the generated code compile...thx, Antlr 2.7.5!
antlrDummyRule: ANTLR_DUMMY_TOKEN ;



/**
 * The ConfigLexer class creates tokens from the characters in a .conf file.
 * It uses the lexical token types defined in LexerCommon and adds the keywords
 * specific to the .conf file format.
 */
class ConfigLexer extends Lexer;
options {
    exportVocab = Config;
    testLiterals = false;           // Don't usually look for literals
    caseSensitive = false;          // We're not picky...
    caseSensitiveLiterals = false;  // ...about identifiers either
    charVocabulary = '\3'..'\377';  // Stick to ASCII, please
    k = 2;
}

// Keywords
tokens {
    // Section type declarations
    APPLICATION = "application" ;
    HEIGHTFIELD = "heightfield" ;
    BOUNDARY    = "boundary" ; 
    GA          = "ga" ;
    EROSION     = "erosion" ;

    // [Application] section property keywords
    CACHE       = "cache" ;
    DIRECTORY   = "directory" ;

    // [* GA] section property keywords
    POPULATION  = "population" ;
    SIZE        = "size" ;
    EVOLUTION   = "evolution" ;
    CYCLES      = "cycles" ;
    SELECTION   = "selection" ;
    ELITISM     = "elitism" ;
    MUTATION    = "mutation" ;
    CROSSOVER   = "crossover" ;
    PROBABILITY = "probability" ;
    RATIO       = "ratio" ;
    
    // [Heightfield GA] section property keywords
    GENE        = "gene" ;
    OVERLAP     = "overlap" ;
    VERTICAL    = "vertical" ;
    OFFSET      = "offset" ;
    SCALE       = "scale" ;
    FACTOR      = "factor" ;
    MAX         = "max" ;
    WIDTH       = "width" ;
    
    // [Boundary GA] section property keywords

    // [Erosion] section property keywords
}

ID options { paraphrase = "id"; testLiterals = true; } :
            ( LETTER | '_' ) ( LETTER | DIGIT | '_' )* ;

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

// Dummy lexical token (antlr 2.7.5 won't compile this w/o at least one rule)
ANTLR_DUMMY_TOKEN : "%#%dummy%#%" ;
