#ifndef INC_MapParser_hpp_
#define INC_MapParser_hpp_

#line 22 "Map.g"

    // Import library configuration
    #include <terrainosaurus/terrainosaurus-common.h>

    // Forward declarations
    namespace terrainosaurus {
        class MapLexer;
        class MapParser;
    };

    // Import Map and related object definitions 
    #include "../data/Map.hpp"

#line 19 "MapParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.4: "Map.g" -> "MapParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "MapTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

ANTLR_BEGIN_NAMESPACE(terrainosaurus)
/**
 * The MapParser class processes tokens from a .map file.
 */
class CUSTOM_API MapParser : public antlr::LLkParser, public MapTokenTypes
{
#line 57 "Map.g"

/*---------------------------------------------------------------------------*
 | Helper function definitions (these help keep the parser grammar cleaner)
 | These are implemented in MapParser-functions.cpp.
 *---------------------------------------------------------------------------*/
protected:
    void createVertex(scalar_t x, scalar_t y, antlr::RefToken pos);
    void setTerrainType(const string &tt, antlr::RefToken pos);
    void beginFace(antlr::RefToken pos);
    void addVertex(IDType id, antlr::RefToken pos);
    void endFace();

    // The map we're creating, plus the in-progress face (if any) and the
    // current TerrainType
    Map * map;
    Map::VertexPtrList vertices;
    TerrainTypePtr currentTT;
#line 33 "MapParser.hpp"
public:
	void initializeASTFactory( antlr::ASTFactory& factory );
protected:
	MapParser(antlr::TokenBuffer& tokenBuf, int k);
public:
	MapParser(antlr::TokenBuffer& tokenBuf);
protected:
	MapParser(antlr::TokenStream& lexer, int k);
public:
	MapParser(antlr::TokenStream& lexer);
	MapParser(const antlr::ParserSharedInputState& state);
	int getNumTokens() const
	{
		return MapParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return MapParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return MapParser::tokenNames;
	}
	public: void terrainMap(
		Map * m
	);
	public: void vertexRecord();
	public: void faceRecord();
	public: void terrainTypeRecord();
	public: void blankLine();
	public: scalar_t  scalar();
	public: int  integer();
	public: scalar_t  fraction();
	public: scalar_t  nFraction();
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
	static const int NUM_TOKENS = 16;
#else
	enum {
		NUM_TOKENS = 16
	};
#endif
	
};

ANTLR_END_NAMESPACE
#endif /*INC_MapParser_hpp_*/
