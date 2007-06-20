#ifndef INC_MapLexer_hpp_
#define INC_MapLexer_hpp_

#line 23 "Map.g"

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

#line 22 "MapLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.5 (20050613): "Map.g" -> "MapLexer.hpp"$ */
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
#line 37 "MapLexer.hpp"
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
	protected: void mWS_CHAR(bool _createToken);
	protected: void mEOL_CHAR(bool _createToken);
	protected: void mDQUOTE(bool _createToken);
	protected: void mSQUOTE(bool _createToken);
	protected: void mSPACE(bool _createToken);
	public: void mEOL(bool _createToken);
	public: void mCOMMENT(bool _createToken);
	public: void mWS(bool _createToken);
	public: void mLEFT_ABRACKET(bool _createToken);
	public: void mRIGHT_ABRACKET(bool _createToken);
	public: void mLEFT_SBRACKET(bool _createToken);
	public: void mRIGHT_SBRACKET(bool _createToken);
	public: void mCOMMA(bool _createToken);
	public: void mASSIGN(bool _createToken);
	public: void mPERCENT(bool _createToken);
	public: void mCOLON(bool _createToken);
	public: void mAMPERSAND(bool _createToken);
	public: void mDOT(bool _createToken);
	public: void mBACKSLASH(bool _createToken);
	public: void mFORESLASH(bool _createToken);
	public: void mNBSP(bool _createToken);
	public: void mSIGN(bool _createToken);
	public: void mNUMBER(bool _createToken);
	public: void mNAME(bool _createToken);
	public: void mQUOTED_STRING(bool _createToken);
	public: void mANTLR_DUMMY_TOKEN(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const antlr::BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const antlr::BitSet _tokenSet_2;
};

ANTLR_END_NAMESPACE
#endif /*INC_MapLexer_hpp_*/
