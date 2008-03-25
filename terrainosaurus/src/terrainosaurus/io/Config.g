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
    #include "INIParser.hpp"
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
class ConfigParser extends CommonParser(::terrainosaurus::INIParser);
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
class ConfigLexer extends CommonLexer;
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

// Dummy lexical token (antlr 2.7.5 won't compile this w/o at least one rule)
ANTLR_DUMMY_TOKEN : "%#%dummy%#%" ;
