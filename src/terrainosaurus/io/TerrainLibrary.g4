/* -*- antlr -*-
 *
 * File: TerrainLibrary.g4
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

grammar TerrainLibrary;

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
    language = Cpp;                 // Create C++ output
}


/*---------------------------------------------------------------------------*
 | Overall file format, section declarations/formats
 *---------------------------------------------------------------------------*/
// Entire file format (call this as the start rule)
sectionList:
    ( blankLine )*
    ( terrainTypeSection )*
    ( terrainSeamSection )*
    EOF
    ;

// A TerrainType declaration, followed by one or more properties
terrainTypeSection:
    '[' 'TerrainType' ':' string ']' EOL
            ( blankLine
            | terrainColorAssignment
            | terrainSampleAssignment )*
    ;

// A TerrainSeam declaration, followed by one or more properties
terrainSeamSection:
    '[' 'TerrainSeam' ':' string '&' string ']' EOL
            ( blankLine
            | smoothnessAssignment
            | aspectRatioAssignment )*
    ;


/*---------------------------------------------------------------------------*
 | TerrainType properties
 *---------------------------------------------------------------------------*/
// color = <R, G, B, A>
terrainColorAssignment returns [terrainosaurus::Color value]:
    'color' '=' color EOL { $value = $color.value; };

// sample = name
terrainSampleAssignment:
    'sample' '=' string EOL;


/*---------------------------------------------------------------------------*
 | TerrainSeam properties
 *---------------------------------------------------------------------------*/
// Smoothness = 0.5
smoothnessAssignment returns [double value]:
    'smoothness' '=' nFraction EOL { $value = $nFraction.value; };

// Aspect Ratio = 1.0
aspectRatioAssignment returns [double value]:
    'aspect' 'ratio' '=' fraction EOL { $value = $fraction.value; };
