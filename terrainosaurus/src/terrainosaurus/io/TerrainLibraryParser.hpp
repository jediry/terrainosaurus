#ifndef INC_TerrainLibraryParser_hpp_
#define INC_TerrainLibraryParser_hpp_

#line 21 "TerrainLibrary.g"

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


    // Include STL string algorithms
    #include <algorithm>


    // Import TerrainLibrary and related object definitions
    #include <terrainosaurus/data/TerrainLibrary.hpp>

    // Import augmented enumeration mechanism
    #include <inca/util/Enumeration.hpp>

#line 31 "TerrainLibraryParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.5 (20050613): "TerrainLibrary.g" -> "TerrainLibraryParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "TerrainLibraryTokenTypes.hpp"

// Include correct superclass header with a header statement for example:
// header "post_include_hpp" {
// #include "::terrainosaurus::INIParser.hpp"
// }
// Or....
// header {
// #include "::terrainosaurus::INIParser.hpp"
// }


ANTLR_BEGIN_NAMESPACE(terrainosaurus)
/**
 * The TerrainLibraryParser class processes tokens from a .ttl file.
 */
class CUSTOM_API TerrainLibraryParser : public ::terrainosaurus::INIParser, public TerrainLibraryTokenTypes
{
#line 68 "TerrainLibrary.g"

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
#line 54 "TerrainLibraryParser.hpp"
public:
	void initializeASTFactory( antlr::ASTFactory& factory );
protected:
	TerrainLibraryParser(antlr::TokenBuffer& tokenBuf, int k);
public:
	TerrainLibraryParser(antlr::TokenBuffer& tokenBuf);
protected:
	TerrainLibraryParser(antlr::TokenStream& lexer, int k);
public:
	TerrainLibraryParser(antlr::TokenStream& lexer);
	TerrainLibraryParser(const antlr::ParserSharedInputState& state);
	int getNumTokens() const
	{
		return TerrainLibraryParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return TerrainLibraryParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return TerrainLibraryParser::tokenNames;
	}
	public: void sectionList(
		TerrainLibrary * lib
	);
	public: void blankLine();
	public: void terrainTypeSection();
	public: void terrainSeamSection();
	public: std::string  string();
	public: void terrainColor();
	public: void terrainSample();
	public: void smoothness();
	public: void aspectRatio();
	public: Color  color();
	public: scalar_t  nFraction();
	public: scalar_t  fraction();
	public: void antlrDummyRule();
	public: std::string  path();
	public: std::string  unixPath();
	public: scalar_t  scalar();
	public: int  integer();
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
	static const int NUM_TOKENS = 40;
#else
	enum {
		NUM_TOKENS = 40
	};
#endif
	
};

ANTLR_END_NAMESPACE
#endif /*INC_TerrainLibraryParser_hpp_*/
