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

    // Import augmented enumeration mechanism
//    #include <inca/util/Enumeration.hpp>

    // This enumerates all the properties that we can check/set. The
    // functions that check/set them implicitly work on the "current"
    // TerrainType or TerrainSeam.
/*    ENUMV ( TTLPropertyType,
          ( ( TTColor,             1 ),
          ( ( TSNumChromosomes,    2 ),
          ( ( TSSmoothness,        4 ),
          ( ( TSMutationRatio,     8 ),
          ( ( TSCrossoverRatio,    16 ),
          ( ( TSSelectionRatio,    32 ),
          ( ( TSAspectRatio,       64 ),
              BOOST_PP_NIL ))))))));*/

#line 38 "TerrainLibraryParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.4: "TerrainLibrary.g" -> "TerrainLibraryParser.hpp"$ */
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
#line 74 "TerrainLibrary.g"

/*---------------------------------------------------------------------------*
 | Type definitions
 *---------------------------------------------------------------------------*/
public:
    // This enumerates all the properties that we can check/set. The
    // functions that check/set them implicitly work on the "current"
    // TerrainType or TerrainSeam.
    enum PropertyType {
        // Properties for TerrainType
        TTColor                 = 0x00001,

        // Properties for the TerrainSeam GA
        TSSmoothness            = 0x00002,
        TSAspectRatio           = 0x00004,

        // General GA parameters
        GAEvolutionCycles       = 0x00100,
        GAPopulationSize        = 0x00200,
        GAEliteRatio            = 0x00400,
        GASelectionRatio        = 0x00800,
        GAMutationRatio         = 0x01000,
        GACrossoverRatio        = 0x02000,
        GAMutationProbability   = 0x04000,
        GACrossoverProbability  = 0x08000,

        // Properties for the heightfield GA
        HFMaxCrossoverWidth     = 0x10000,
        HFMaxJitterPixels       = 0x20000,
        HFMaxScaleFactor        = 0x40000,
        HFMaxOffsetAmount       = 0x80000,
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
    void beginGlobalSection(antlr::RefToken tt);
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
    bool inGlobal;
    TerrainTypePtr currentTT;
    TerrainSeamPtr currentTS;
    stl_ext::hash_map<TerrainSeamPtr, bool> initializedTSs;
    unsigned int setProperties;
#line 52 "TerrainLibraryParser.hpp"
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
	public: void globalSectionRecord();
	public: void globalSectionDeclaration();
	public: void populationSize();
	public: void evolutionCycles();
	public: void selectionRatio();
	public: void eliteRatio();
	public: void mutationProbability();
	public: void mutationRatio();
	public: void crossoverProbability();
	public: void crossoverRatio();
	public: void maxCrossoverWidth();
	public: void maxJitterPixels();
	public: void maxScaleFactor();
	public: void maxOffsetAmount();
	public: void terrainTypeDeclaration();
	public: void terrainColor();
	public: void terrainSample();
	public: void terrainSeamDeclaration();
	public: void smoothness();
	public: void aspectRatio();
	public: int  integer();
	public: scalar_t  fraction();
	public: scalar_t  scalar();
	public: Color  color();
	public: string  filename();
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
	static const int NUM_TOKENS = 55;
#else
	enum {
		NUM_TOKENS = 55
	};
#endif
	
};

ANTLR_END_NAMESPACE
#endif /*INC_TerrainLibraryParser_hpp_*/
