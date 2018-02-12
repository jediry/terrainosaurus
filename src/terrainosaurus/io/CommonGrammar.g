/* -*- antlr -*-
 *
 * \file    CommonGrammar.g
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file specifies token types used by in one or more of the
 *      the Terrainosaurus file formats, and is not intended to be used
 *      directly. The parsers and lexers for these formats should inherit from
 *      these grammars.
 */


// C++ Header section
header "pre_include_hpp" {
    // Import library configuration
    #include <terrainosaurus/terrainosaurus-common.h>

    // Forward declarations
    namespace terrainosaurus {
        class CommonParser;
        class CommonLexer;
        
        // std::transform(...) functor for converting to UNIX-style slashes
        struct DOSToUNIX {
            int operator()(int ch) {
                return ch == '\\' ? '/' : ch;
            }
        };
    };
    
    // Include STL string algorithms
    #include <algorithm>
    
}

header "pre_include_cpp" {
    // Include precompiled header
    #include <terrainosaurus/precomp.h>
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
 * The CommonParser grammar contains reusable token types.
 */
class CommonParser extends Parser;
options {
    exportVocab=Common;
    defaultErrorHandler=false;
    k = 2;
}

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



/*
 * The CommonLexer grammar contains token types used by other,
 * file-format-specific lexers.
 */
class CommonLexer extends Lexer;
options {
    exportVocab = Common;           // Export tokens for sub-grammars
    testLiterals = false;           // Don't usually look for literals
    caseSensitive = false;          // We're not picky...
    caseSensitiveLiterals = false;  // ...about identifiers either
    charVocabulary = '\3'..'\377';  // Stick to ASCII, please
    k = 2;                          // Grammar is indeterminate w/o this
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
//ID options { paraphrase = "name"; testLiterals = true; } :
//            ( LETTER | '_' ) ( LETTER | DIGIT | '_' )* ;

// A string in quotes
QUOTED_STRING options { paraphrase = "quoted string"; } :
          ( DQUOTE (~'"')* DQUOTE )
        | ( SQUOTE (~'\'')* SQUOTE ) ;
//          ( DQUOTE s:(~S)* DQUOTE ) { $setText(s->getText()); }
//        | ( SQUOTE t:(.)* SQUOTE ) { $setText(t->getText()); } ;
