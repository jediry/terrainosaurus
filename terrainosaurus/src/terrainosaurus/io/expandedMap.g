header "pre_include_hpp" {
    // Import library configuration
    #include <terrainosaurus/terrainosaurus-common.h>

    // Forward declarations
    namespace terrainosaurus {
        class MapLexer;
        class MapParser;
    };

    // Import Map and related object definitions 
    #include "../data/Map.hpp"

    // Import parser superclass and supergrammer definitions
    #include "CommonParser.hpp"
}
options {
	language= "Cpp";
	namespace= "terrainosaurus";
	namespaceStd= "std";
	namespaceAntlr= "antlr";
	genHashLines= true;
}

class MapParser extends Parser;

options {
	exportVocab= Map;
	defaultErrorHandler=false;
	k= 2;
	importVocab=Common;
}

{
/*---------------------------------------------------------------------------*
 | Helper function definitions (these help keep the parser grammar cleaner)
 | These are implemented in MapParser-functions.cpp.
 *---------------------------------------------------------------------------*/
protected:
    void createVertex(scalar_t x, scalar_t y, antlr::RefToken pos);
    void setTerrainType(const std::string & tt, antlr::RefToken pos);
    void beginFace(antlr::RefToken pos);
    void addVertex(IDType id, antlr::RefToken pos);
    void endFace();

    // The map we're creating, plus the in-progress face (if any) and the
    // current TerrainType
    Map * map;
    Map::VertexPtrList vertices;
    TerrainTypePtr currentTT;
}
terrainMap[Map * m] :{ map = m; }            // Set this as the map we're gonna use
    { map != NULL }?        // Make sure it isn't NULL!
    { currentTT = map->terrainLibrary->terrainType(0); }    // Default TT
    ( vertexRecord
    | faceRecord
    | terrainTypeRecord
    | blankLine )* EOF ;

vertexRecord :{ scalar_t x, y; }
    t:VERTEX x=scalar y=scalar EOL
    { createVertex(x, y, t); } ;

faceRecord :{ IDType id; }
    f:FACE { beginFace(f); } ( id=integer { addVertex(id, f); } )+ EOL
    { endFace(); } ;

terrainTypeRecord :TERRAIN_TYPE id:NAME EOL
    { setTerrainType(id->getText(), id); } ;

antlrDummyRule :LEFT_SBRACKET ANTLR_DUMMY_TOKEN RIGHT_SBRACKET;

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

class MapLexer extends Lexer;

options {
	exportVocab= Map;
	testLiterals= false;
	caseSensitive= false;
	caseSensitiveLiterals= false;
	charVocabulary= '\3'..'\377';
	k= 2;
	importVocab=Common;
}

tokens {
    // Record type declarations
    VERTEX = "v" ;
    FACE   = "f" ;
    TERRAIN_TYPE = "tt" ;
}
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


