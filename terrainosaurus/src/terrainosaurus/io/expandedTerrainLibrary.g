header "pre_include_hpp" {
    // Import library configuration
    #include <terrainosaurus/terrainosaurus-common.h>

    // Forward declarations
    namespace terrainosaurus {
        class TerrainLibraryParser;
        class TerrainLibraryLexer;
    };

    // Import parser superclass and supergrammer definitions
    #include "INIParser.hpp"
    #include "CommonParser.hpp"

    // Import TerrainLibrary and related object definitions
    #include <terrainosaurus/data/TerrainLibrary.hpp>

    // Import augmented enumeration mechanism
    #include <inca/util/Enumeration.hpp>
}
options {
	language= "Cpp";
	namespace= "terrainosaurus";
	namespaceStd= "std";
	namespaceAntlr= "antlr";
	genHashLines= true;
}

class TerrainLibraryParser extends Parser("::terrainosaurus::INIParser");

options {
	exportVocab= TerrainLibrary;
	importVocab= Common;
	defaultErrorHandler=false;
	k= 2;
}

{
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
    stl_ext::hash_map<TerrainSeamPtr, bool> _initializedTSs;
}
sectionList[TerrainLibrary * lib] :{ _terrainLibrary = lib; }      // Set this as our TL object
    { _terrainLibrary != NULL }?    // Make sure it isn't NULL!
    (blankLine)* ( terrainTypeSection
                 | terrainSeamSection )* EOF ;

terrainTypeSection { std::string tt; }
:t:LEFT_SBRACKET TERRAIN_TYPE COLON tt=string RIGHT_SBRACKET EOL
    { beginTerrainTypeSection(t, tt); }
    terrainColor
            ( blankLine
            | terrainColor
            | terrainSample )*
    { endSection(); } ;

terrainSeamSection { std::string tt1, tt2; }
:t:LEFT_SBRACKET TERRAIN_SEAM COLON
        tt1=string AMPERSAND tt2=string RIGHT_SBRACKET EOL
    { beginTerrainSeamSection(t, tt1, tt2); }
            ( blankLine
            | smoothness
            | aspectRatio )*
    { endSection(); } ;

terrainColor { Color c; }
:t:COLOR ASSIGN c=color EOL
    { assignColorProperty(t, TTColor, c); } ;

terrainSample { std::string s; }
:t:SAMPLE ASSIGN s=string EOL
    { assignStringProperty(t, TTSample, s); } ;

smoothness { scalar_t s; }
:t:SMOOTHNESS ASSIGN s=nFraction EOL
    { assignScalarProperty(t, TSSmoothness, s); } ;

aspectRatio { scalar_t s; }
:t:ASPECT RATIO ASSIGN s=fraction EOL
    { assignScalarProperty(t, TSAspectRatio, s); } ;

antlrDummyRule :ANTLR_DUMMY_TOKEN ;

// inherited from grammar CommonParser
string returns [std::string s]:( q:QUOTED_STRING       { std::string tmp = q->getText();
                              s += tmp.substr(1, tmp.length() - 2); }
    | ( i1:ID               { s += i1->getText(); }
      | n1:NUMBER           { s += n1->getText(); }
        ( NBSP              { s += " ";           }
            ( i2:ID         { s += i2->getText(); }
            | n2:NUMBER     { s += n2->getText(); }
            )
         )*
      )+
    ) ;

// inherited from grammar CommonParser
path returns [std::string p]:( s:QUOTED_STRING   { p += s->getText(); }
    | i:ID              { p += i->getText(); }
    | n:NUMBER          { p += n->getText(); }
    | c:COLON           { p += c->getText(); }
    | d:DOT             { p += d->getText(); }
    | b:BACKSLASH       { p += b->getText(); }
    | f:FORESLASH       { p += f->getText(); }
    | NBSP              { p += " "; }
    )+ ;

// inherited from grammar CommonParser
unixPath returns [std::string p]:p=path
    {   // Transliterate all '\'s into '/'s
        std::transform(p.begin(), p.end(), p.begin(), DOSToUNIX());
    } ;

// inherited from grammar CommonParser
blankLine :EOL ;

// inherited from grammar CommonParser
fraction returns [scalar_t s]:w:NUMBER ( DOT f:NUMBER )? ( "e" (sg:SIGN)? e:NUMBER )? (p:PERCENT)?
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

// inherited from grammar CommonParser
scalar returns [scalar_t s]:(sg:SIGN)? s=fraction
    {
        // If we have a sign token and it is negative, then negate 's'
        if (sg != NULL && sg->getText() == "-")
            s *= -1;
    } ;

// inherited from grammar CommonParser
nFraction returns [scalar_t s]:s=fraction
    { s >= scalar_t(0) && s <= scalar_t(1) }? ;

// inherited from grammar CommonParser
color returns [Color c]:{ scalar_t r, g, b, a; }
    LEFT_ABRACKET
        r=nFraction COMMA g=nFraction COMMA b=nFraction COMMA a=nFraction
    RIGHT_ABRACKET

    // Create a Color object from components
    { typedef Color::scalar_t c_scalar_t;
      c = Color(c_scalar_t(r), c_scalar_t(g),
                c_scalar_t(b), c_scalar_t(a)); } ;

// inherited from grammar CommonParser
integer returns [int value]:n:NUMBER { value = atoi(n->getText().c_str()); } ;

class TerrainLibraryLexer extends Lexer;

options {
	exportVocab= TerrainLibrary;
	importVocab= Common;
	testLiterals= false;
	caseSensitive= false;
	caseSensitiveLiterals= false;
	charVocabulary= '\3'..'\377';
	k= 2;
}

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
ID 
options {
	paraphrase= "id";
	testLiterals= true;
}
:( LETTER | '_' ) ( LETTER | DIGIT | '_' )* ;

ANTLR_DUMMY_TOKEN :"%#%dummy%#%" ;

// inherited from grammar CommonLexer
protected DIGIT :( '0'..'9' ) ;

// inherited from grammar CommonLexer
protected LETTER :( 'a'..'z' ) ;

// inherited from grammar CommonLexer
protected WS_CHAR :( ' ' | '\t' ) ;

// inherited from grammar CommonLexer
protected EOL_CHAR :( "\r\n" | '\r' | '\n' ) ;

// inherited from grammar CommonLexer
protected DQUOTE :'"'   { $setText(""); } ;

// inherited from grammar CommonLexer
protected SQUOTE :'\''  { $setText(""); } ;

// inherited from grammar CommonLexer
protected SPACE :' ' ;

// inherited from grammar CommonLexer
EOL 
options {
	paraphrase= "end of line";
}
:EOL_CHAR  { newline(); } ;

// inherited from grammar CommonLexer
COMMENT 
options {
	paraphrase= "comment";
}
:'#' ( ~('\r' | '\n') )* EOL_CHAR  { newline(); $setType(EOL); } ;

// inherited from grammar CommonLexer
WS :( WS_CHAR )+                     { $setType(antlr::Token::SKIP); } ;

// inherited from grammar CommonLexer
LEFT_ABRACKET :'<' ;

// inherited from grammar CommonLexer
RIGHT_ABRACKET :'>' ;

// inherited from grammar CommonLexer
LEFT_SBRACKET :'[' ;

// inherited from grammar CommonLexer
RIGHT_SBRACKET :']' ;

// inherited from grammar CommonLexer
COMMA :',' ;

// inherited from grammar CommonLexer
ASSIGN :'=' ;

// inherited from grammar CommonLexer
PERCENT :'%' ;

// inherited from grammar CommonLexer
COLON :':' ;

// inherited from grammar CommonLexer
AND :'&' ;

// inherited from grammar CommonLexer
DOT :'.' ;

// inherited from grammar CommonLexer
BACKSLASH :'\\' ;

// inherited from grammar CommonLexer
FORESLASH :'/' ;

// inherited from grammar CommonLexer
NBSP :"\\ " ;

// inherited from grammar CommonLexer
SIGN :( '+' | '-' ) ;

// inherited from grammar CommonLexer
NUMBER 
options {
	paraphrase= "number";
}
:(DIGIT)+ ;

// inherited from grammar CommonLexer
QUOTED_STRING 
options {
	paraphrase= "quoted string";
}
:( DQUOTE (~'"')* DQUOTE )
        | ( SQUOTE (~'\'')* SQUOTE ) ;


