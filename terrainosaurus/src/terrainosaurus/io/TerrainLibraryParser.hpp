#ifndef INC_TerrainLibraryParser_hpp_
#define INC_TerrainLibraryParser_hpp_

#line 20 "TerrainLibrary.g"

    // Import library configuration
    #include <terrainosaurus/terrainosaurus-common.h>

    // Forward declarations
    namespace terrainosaurus {
        class TerrainLibraryLexer;
        class TerrainLibraryParser;
    };

    // Import TerrainLibrary and related object definitions 
    #include "../data/TerrainLibrary.hpp"
    
    // Import container definitions
    #include <inca/util/hash_container>

#line 22 "TerrainLibraryParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.3: "TerrainLibrary.g" -> "TerrainLibraryParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "TerrainLibraryTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

ANTLR_BEGIN_NAMESPACE(terrainosaurus)
/**
 * The TerrainLibraryParser class processes tokens from a .ttl file.
 */
class CUSTOM_API TerrainLibraryParser : public antlr::LLkParser, public TerrainLibraryTokenTypes
{
#line 58 "TerrainLibrary.g"

/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // This enumerates all the properties that we can check/set. The
    // functions that check/set them implicitly work on the "current"
    // TerrainType or TerrainSeam.
    enum PropertyType {
        TTColor             = 0x0001,
        TSNumChromosomes    = 0x0002,
        TSSmoothness        = 0x0004,
        TSMutationRatio     = 0x0008,
        TSCrossoverRatio    = 0x0010,
        TSSelectionRatio    = 0x0020,
        TSAspectRatio       = 0x0040,
    };


/*---------------------------------------------------------------------------*
 | Helper function definitions (these help keep the parser grammar cleaner)
 | These are implemented in TerrainLibraryParser-functions.cpp.
 *---------------------------------------------------------------------------*/
protected:
    // These create new TTs and TSs and set properties on them (implicity
    // modifying the most recent TT or TS). They will throw exceptions if
    // an attempt is made to set/create something that has already been
    // set/created, thus effectively preventing duplicate entries
    void createTerrainType(antlr::RefToken tt);
    void createTerrainSeam(antlr::RefToken tt1, antlr::RefToken tt2);
    void endRecord(antlr::RefToken t);
    void addTerrainSample(const std::string & path, int line);
    void setColorProperty(PropertyType p, const Color &c, int line);
    void setScalarProperty(PropertyType p, scalar_t s, int line);
    void setIntegerProperty(PropertyType p, int i, int line);


    // The library we're populating, plus the current TT and TS, and a
    // record of which properties we've set on the current object
    TerrainLibrary * library;
    TerrainTypePtr currentTT;
    TerrainSeamPtr currentTS;
    stl_ext::hash_map<TerrainSeamPtr, bool> initializedTSs;
    unsigned int setProperties;
#line 36 "TerrainLibraryParser.hpp"
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
	public: void recordList(
		TerrainLibrary * lib
	);
	public: void blankLine();
	public: void terrainTypeRecord();
	public: void terrainSeamRecord();
	public: void terrainTypeDeclaration();
	public: void terrainColor();
	public: void terrainSample();
	public: void terrainSeamDeclaration();
	public: void numChromosomes();
	public: void smoothness();
	public: void mutationRatio();
	public: void crossoverRatio();
	public: void selectionRatio();
	public: void aspectRatio();
	public: Color  color();
	public: string  filename();
	public: int  integer();
	public: scalar_t  nFraction();
	public: scalar_t  fraction();
	public: scalar_t  scalar();
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
	static const int NUM_TOKENS = 39;
#else
	enum {
		NUM_TOKENS = 39
	};
#endif
	
};

ANTLR_END_NAMESPACE
#endif /*INC_TerrainLibraryParser_hpp_*/
