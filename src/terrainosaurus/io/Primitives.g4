/* -*- antlr -*-
 *
 * File: Primitives.g4
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file specifies an ANTLR grammar for reusable rules shared by
 *      multiple grammars used by Terrainosaurus.
 *
 */

grammar Primitives;


// A line with nothing important on it
blankLine: EOL ;


// An untranslated filesystem path
path:
    ( QUOTED_STRING
    | ID
    | NUMBER
    | COLON
    | DOT
    | BACKSLASH
    | FORESLASH
    | NBSP
    )+ ;


// A string (a single word, perhaps with embedded non-breaking spaces, or a
// quoted string)
string:
    ( QUOTED_STRING
    | ( ID
      | NUMBER
        ( NBSP+
            ( ID
            | NUMBER
            )
         )*
      )+
    ) ;


// A positive real number, specified in any of the following formats:
//      standard decimal notation (1.234)
//      percent notation (123.4%)
//      scientific notation (0.1234e1)
// These may be combined (e.g., 1234.0e-1%)
fraction returns [double value]:
    ( NUMBER ( DOT NUMBER )? ( 'e' SIGN? NUMBER )? ) { $value = atof($text.c_str()); }
    ( PERCENT { $value *= 100.0; } )?
    ;


// A positive or negative real number, with the same format as the 'fraction'
// type (see preceding 'fraction' rule).
scalar returns [double value]:
    (sg=SIGN)? f=fraction
    {
        // If we have a sign token and it is negative, then negate 's'
        $value = $f.value;
        if ($sg != nullptr && $sg.text == "-")
            $value *= -1;
    } ;


// A positive real number, clamped to the range [0.0, 1.0], with the same
// format as the 'fraction' type (see preceding 'fraction' rule).
nFraction returns [double value]:
    f=fraction
    { $value = $f.value; }
    { $f.value >= 0 && $f.value <= 1 }?   // Enforce s in [0.0, 1.0]
    ;


// A color specified with RGB components between 0.0 and 1.0
color returns [terrainosaurus::Color value]:
    LEFT_ABRACKET
        r=nFraction COMMA g=nFraction COMMA b=nFraction COMMA a=nFraction
    RIGHT_ABRACKET

    // Create a Color object from components
    { typedef terrainosaurus::Color::scalar_t c_scalar_t;
      $value = terrainosaurus::Color(c_scalar_t($r.value), c_scalar_t($g.value),
                                     c_scalar_t($b.value), c_scalar_t($a.value));
    }
    ;


// An unsigned integer in decimal notation
integer returns [int value]: NUMBER { $value = atoi($text.c_str()); } ;

// Keywords

ID: ( LETTER | '_' ) ( LETTER | DIGIT | '_' )* ;

// Primitive character classes
fragment DIGIT     : ( '0'..'9' ) ;
fragment LETTER    : ( 'a'..'z' ) | ( 'A'..'Z' ) ;
fragment WS_CHAR   : ( ' ' | '\t' ) ;
fragment EOL_CHAR  : ( '\r\n' | '\r' | '\n' ) ;
fragment DQUOTE    : '"' ;
fragment SQUOTE    : '\'' ;
fragment SPACE     : ' ' ;

// Comments and whitespace
EOL: EOL_CHAR ;
COMMENT: '#' ( ~[\r\n] )* -> skip;
WS : ( WS_CHAR )+ -> skip;

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
NBSP            : '\\ ' ;

// A positive or negative sign
SIGN  : ( '+' | '-' ) ;

// An unsigned integral number
NUMBER: (DIGIT)+ ;

// A string in quotes
QUOTED_STRING:
        ( ( DQUOTE (~'"')* DQUOTE )
        | ( SQUOTE (~'\'')* SQUOTE ) ) { { auto s = getText(); setText(s.substr(1, s.length() - 2)); } } ;
