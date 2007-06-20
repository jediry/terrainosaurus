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
options {
	language= "Cpp";
	namespace= "terrainosaurus";
	namespaceStd= "std";
	namespaceAntlr= "antlr";
	genHashLines= true;
}

class ConfigParser extends Parser("::terrainosaurus::INIParser");

options {
	exportVocab= Config;
	defaultErrorHandler=false;
	k= 3;
	importVocab=Common;
}

{
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
sectionList[TerrainosaurusApplication * app] { _application = app; }
:(blankLine)*  applicationSection
                  boundaryGASection
                  heightfieldGASection
                  ( erosionSection )* EOF ;

applicationSection :t:LEFT_SBRACKET APPLICATION RIGHT_SBRACKET EOL
    { beginApplicationSection(t); }
        ( blankLine
        | cacheDirectory )*
    { endSection(); } ;

boundaryGASection :t:LEFT_SBRACKET BOUNDARY GA RIGHT_SBRACKET EOL
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

heightfieldGASection :t:LEFT_SBRACKET HEIGHTFIELD GA RIGHT_SBRACKET EOL
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

erosionSection :t:LEFT_SBRACKET EROSION RIGHT_SBRACKET EOL
    { beginErosionSection(t); }
        ( blankLine )*
    { endSection(); } ;

cacheDirectory { std::string s; }
:t:POPULATION SIZE ASSIGN s=unixPath EOL
    { assignStringProperty(t, CacheDirectory, s); } ;

populationSize { int n; }
:t:POPULATION SIZE ASSIGN n=integer EOL
    { assignIntegerProperty(t, PopulationSize, n); } ;

evolutionCycles { int n; }
:t:EVOLUTION CYCLES ASSIGN n=integer EOL
    { assignIntegerProperty(t, EvolutionCycles, n); } ;

selectionRatio { scalar_t r; }
:t:SELECTION RATIO ASSIGN r=fraction EOL
    { assignScalarProperty(t, SelectionRatio, r); } ;

elitismRatio { scalar_t r; }
:t:ELITISM RATIO ASSIGN r=fraction EOL
    { assignScalarProperty(t, ElitismRatio, r); } ;

crossoverProbability { scalar_t p; }
:t:CROSSOVER PROBABILITY ASSIGN p=fraction EOL
    { assignScalarProperty(t, CrossoverProbability, p); } ;

crossoverRatio { scalar_t r; }
:t:CROSSOVER RATIO ASSIGN r=fraction EOL
    { assignScalarProperty(t, CrossoverRatio, r); } ;

mutationProbability { scalar_t p; }
:t:MUTATION PROBABILITY ASSIGN p=fraction EOL
    { assignScalarProperty(t, MutationProbability, p); } ;

mutationRatio { scalar_t r; }
:t:MUTATION RATIO ASSIGN r=fraction EOL
    { assignScalarProperty(t, MutationRatio, r); } ;

geneSize { int n; }
:t:GENE SIZE ASSIGN n=integer EOL
    { assignIntegerProperty(t, GeneSize, n); } ;

overlapFactor { scalar_t f; }
:t:OVERLAP FACTOR ASSIGN f=fraction EOL
    { assignScalarProperty(t, OverlapFactor, f); } ;

maxCrossoverWidth { int n; }
:t:MAX CROSSOVER WIDTH ASSIGN n=integer EOL
    { assignIntegerProperty(t, MaxCrossoverWidth, n); } ;

maxVerticalScale { scalar_t s; }
:t:MAX VERTICAL SCALE ASSIGN s=scalar EOL
    { assignScalarProperty(t, MaxVerticalScale, s); } ;

maxVerticalOffset { scalar_t s; }
:t:MAX VERTICAL OFFSET ASSIGN s=scalar EOL
    { assignScalarProperty(t, MaxVerticalOffset, s); } ;

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

class ConfigLexer extends Lexer;

options {
	exportVocab= Config;
	testLiterals= false;
	caseSensitive= false;
	caseSensitiveLiterals= false;
	charVocabulary= '\3'..'\377';
	k= 2;
	importVocab=Common;
}

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


