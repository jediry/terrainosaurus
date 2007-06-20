#ifndef INC_ConfigParser_hpp_
#define INC_ConfigParser_hpp_

#line 1 "/expandedConfig.g"

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

#line 26 "ConfigParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.5 (20050613): "/expandedConfig.g" -> "ConfigParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "ConfigTokenTypes.hpp"

// Include correct superclass header with a header statement for example:
// header "post_include_hpp" {
// #include "::terrainosaurus::INIParser.hpp"
// }
// Or....
// header {
// #include "::terrainosaurus::INIParser.hpp"
// }


ANTLR_BEGIN_NAMESPACE(terrainosaurus)
class CUSTOM_API ConfigParser : public ::terrainosaurus::INIParser, public ConfigTokenTypes
{
#line 38 "/expandedConfig.g"

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
#line 46 "ConfigParser.hpp"
public:
	void initializeASTFactory( antlr::ASTFactory& factory );
protected:
	ConfigParser(antlr::TokenBuffer& tokenBuf, int k);
public:
	ConfigParser(antlr::TokenBuffer& tokenBuf);
protected:
	ConfigParser(antlr::TokenStream& lexer, int k);
public:
	ConfigParser(antlr::TokenStream& lexer);
	ConfigParser(const antlr::ParserSharedInputState& state);
	int getNumTokens() const
	{
		return ConfigParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return ConfigParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return ConfigParser::tokenNames;
	}
	public: void sectionList(
		TerrainosaurusApplication * app
	);
	public: void blankLine();
	public: void applicationSection();
	public: void boundaryGASection();
	public: void heightfieldGASection();
	public: void erosionSection();
	public: void cacheDirectory();
	public: void populationSize();
	public: void evolutionCycles();
	public: void selectionRatio();
	public: void elitismRatio();
	public: void mutationProbability();
	public: void mutationRatio();
	public: void crossoverProbability();
	public: void crossoverRatio();
	public: void geneSize();
	public: void overlapFactor();
	public: void maxCrossoverWidth();
	public: void maxVerticalScale();
	public: void maxVerticalOffset();
	public: std::string  unixPath();
	public: int  integer();
	public: scalar_t  fraction();
	public: scalar_t  scalar();
	public: void antlrDummyRule();
	public: std::string  string();
	public: std::string  path();
	public: scalar_t  nFraction();
	public: Color  color();
public:
	antlr::RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	antlr::RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 56;
#else
	enum {
		NUM_TOKENS = 56
	};
#endif
	
};

ANTLR_END_NAMESPACE
#endif /*INC_ConfigParser_hpp_*/
