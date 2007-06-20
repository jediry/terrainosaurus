#ifndef INC_CommonParser_hpp_
#define INC_CommonParser_hpp_

#line 19 "CommonGrammar.g"

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
    

#line 27 "CommonParser.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.5 (20050613): "CommonGrammar.g" -> "CommonParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "CommonTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

ANTLR_BEGIN_NAMESPACE(terrainosaurus)
/**
 * The CommonParser grammar contains reusable token types.
 */
class CUSTOM_API CommonParser : public antlr::LLkParser, public CommonTokenTypes
{
#line 1 "CommonGrammar.g"
#line 41 "CommonParser.hpp"
public:
	void initializeASTFactory( antlr::ASTFactory& factory );
protected:
	CommonParser(antlr::TokenBuffer& tokenBuf, int k);
public:
	CommonParser(antlr::TokenBuffer& tokenBuf);
protected:
	CommonParser(antlr::TokenStream& lexer, int k);
public:
	CommonParser(antlr::TokenStream& lexer);
	CommonParser(const antlr::ParserSharedInputState& state);
	int getNumTokens() const
	{
		return CommonParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return CommonParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return CommonParser::tokenNames;
	}
	public: std::string  string();
	public: std::string  path();
	public: std::string  unixPath();
	public: void blankLine();
	public: scalar_t  fraction();
	public: scalar_t  scalar();
	public: scalar_t  nFraction();
	public: Color  color();
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
	static const int NUM_TOKENS = 32;
#else
	enum {
		NUM_TOKENS = 32
	};
#endif
	
};

ANTLR_END_NAMESPACE
#endif /*INC_CommonParser_hpp_*/
