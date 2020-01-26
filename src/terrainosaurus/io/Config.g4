/* -*- antlr -*-
 *
 * File: Config.g4
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

grammar Config;

import Primitives;

// Include precompiled header in each .cpp file
@lexer::preinclude {
    #include <terrainosaurus/precomp.h>
}

@parser::preinclude {
    #include <terrainosaurus/precomp.h>
}

@parser::listenerpreinclude {
    #include <terrainosaurus/precomp.h>
}

@parser::baselistenerpreinclude {
    #include <terrainosaurus/precomp.h>
}

// Global options section
options {
    language = Cpp;               // Create C++ output
}


/*---------------------------------------------------------------------------*
 | File sections
 *---------------------------------------------------------------------------*/
// Entire file format (call this rule first)
sectionList:
    (blankLine)*  applicationSection
                  boundaryGASection
                  heightfieldGASection EOF
    ;

// [Application] section
applicationSection:
    '[' 'Application' ']' EOL
        ( blankLine
        | cacheDirectoryAssignment )*
    ;

// [Boundary GA] section
boundaryGASection :
    '[' 'Boundary' 'GA' ']' EOL
        ( blankLine
        | populationSizeAssignment
        | evolutionCyclesAssignment
        | selectionRatioAssignment
        | elitismRatioAssignment
        | mutationProbabilityAssignment
        | mutationRatioAssignment
        | crossoverProbabilityAssignment
        | crossoverRatioAssignment )*
    ;

// [Heightfield GA] section
heightfieldGASection :
    '[' 'Heightfield' 'GA' ']' EOL
        ( blankLine
        | populationSizeAssignment
        | evolutionCyclesAssignment
        | selectionRatioAssignment
        | elitismRatioAssignment
        | mutationProbabilityAssignment
        | mutationRatioAssignment
        | crossoverProbabilityAssignment
        | crossoverRatioAssignment
        | geneSizeAssignment
        | overlapFactorAssignment
        | maxCrossoverWidthAssignment
        | maxVerticalScaleAssignment
        | maxVerticalOffsetAssignment )*
    ;


/*---------------------------------------------------------------------------*
 | Application parameters
 *---------------------------------------------------------------------------*/
// cache directory = path
cacheDirectoryAssignment:
    'cache' 'directory' '=' path EOL;


/*---------------------------------------------------------------------------*
 | Genetic algorithm parameters (common to HF and B GAs)
 *---------------------------------------------------------------------------*/
// population size = n
populationSizeAssignment returns [int value]:
    'population' 'size' '=' integer EOL { $value = $integer.value; };

// evolution cycles = n
evolutionCyclesAssignment returns [int value]:
    'evolution' 'cycles' '=' integer EOL { $value = $integer.value; };

// selection ratio = r
selectionRatioAssignment returns [double value]:
    'selection' 'ratio' '=' fraction EOL { $value = $fraction.value; };

// elitism ratio = r
elitismRatioAssignment returns [double value]:
    'elitism' 'ratio' '=' fraction EOL { $value = $fraction.value; };

// crossover probability = p
crossoverProbabilityAssignment returns [double value]:
    'crossover' 'probability' '=' fraction EOL { $value = $fraction.value; };

// crossover ratio = r
crossoverRatioAssignment returns [double value]:
    'crossover' 'ratio' '=' fraction EOL { $value = $fraction.value; };

// mutation probability = p
mutationProbabilityAssignment returns [double value]:
    'mutation' 'probability' '=' fraction EOL { $value = $fraction.value; };

// mutation ratio = r
mutationRatioAssignment returns [double value]:
    'mutation' 'ratio' '=' fraction EOL { $value = $fraction.value; };


/*---------------------------------------------------------------------------*
 | Boundary GA parameters
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 | Heightfield GA parameters
 *---------------------------------------------------------------------------*/
// gene size = n
geneSizeAssignment returns [int value]:
    'gene' 'size' '=' integer EOL { $value = $integer.value; };

// overlap factor = f
overlapFactorAssignment returns [double value]:
    'overlap' 'factor' '=' fraction EOL { $value = $fraction.value; };

// max crossover width = n
maxCrossoverWidthAssignment returns [int value]:
    'max' 'crossover' 'width' '=' integer EOL { $value = $integer.value; };

// max vertical scale = s
maxVerticalScaleAssignment returns [double value]:
    'max' 'vertical' 'scale' '=' scalar EOL { $value = $scalar.value; };

// max vertical offset = s
maxVerticalOffsetAssignment returns [double value]:
    'max' 'vertical' 'offset' '=' scalar EOL { $value = $scalar.value; };
