#ifndef INC_MapLexer_hpp_
#define INC_MapLexer_hpp_

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

#line 19 "MapLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.4: "Map.g" -> "MapLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "MapTokenTypes.hpp"
#include <antlr/CharScanner.hpp>
ANTLR_BEGIN_NAMESPACE(terrainosaurus)
/**
 * The MapLexer class creates tokens from the characters in
 * the .ttl file.
 */
class CUSTOM_API MapLexer : public antlr::CharScanner, public MapTokenTypes
{
#line 1 "Map.g"
#line 34 "MapLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return false;
	}
public:
	MapLexer(std::istream& in);
	MapLexer(antlr::InputBuffer& ib);
	MapLexer(const antlr::LexerSharedInputState& state);
	antlr::RefToken nextToken();
	protected: void mDIGIT(bool _createToken);
	protected: void mLETTER(bool _createToken);
	public: void mEOL(bool _createToken);
	public: void mCOMMENT(bool _createToken);
	public: void mWS(bool _createToken);
	public: void mDOT(bool _createToken);
	public: void mNUMBER(bool _createToken);
	public: void mSIGN(bool _createToken);
	public: void mNAME(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
};

ANTLR_END_NAMESPACE
#endif /*INC_MapLexer_hpp_*/
