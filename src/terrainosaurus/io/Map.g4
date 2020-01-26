/* -*- antlr -*-
 *
 * File: Map.g4
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

grammar Map;

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
 | Overall file format, record declarations/formats
 *---------------------------------------------------------------------------*/
// Entire file format (call this as the start rule)
terrainMap:
    ( vertexRecord
    | faceRecord
    | terrainTypeRecord
    | blankLine )* EOF
    ;

// A single-line vertex declaration with x and y coordinates
vertexRecord:
    'v' x=scalar y=scalar EOL;

// A single-line face declaration with the list of vertices
faceRecord:
    'f' ( integer )+ EOL;

// A single-line terrain-type declaration (applies hereafter, until changed)
terrainTypeRecord:
    'tt' ID EOL;
