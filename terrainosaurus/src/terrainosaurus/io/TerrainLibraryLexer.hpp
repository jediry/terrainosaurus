#ifndef INC_TerrainLibraryLexer_hpp_
#define INC_TerrainLibraryLexer_hpp_

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

#line 31 "TerrainLibraryLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.5 (20050613): "TerrainLibrary.g" -> "TerrainLibraryLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "TerrainLibraryTokenTypes.hpp"
#include <antlr/CharScanner.hpp>
ANTLR_BEGIN_NAMESPACE(terrainosaurus)
/**
 * The TerrainLibraryLexer class creates tokens from the characters in
 * the .ttl file.
 */
class CUSTOM_API TerrainLibraryLexer : public antlr::CharScanner, public TerrainLibraryTokenTypes
{
#line 1 "TerrainLibrary.g"
#line 46 "TerrainLibraryLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return false;
	}
public:
	TerrainLibraryLexer(std::istream& in);
	TerrainLibraryLexer(antlr::InputBuffer& ib);
	TerrainLibraryLexer(const antlr::LexerSharedInputState& state);
	antlr::RefToken nextToken();
	public: void mID(bool _createToken);
	protected: void mLETTER(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	public: void mANTLR_DUMMY_TOKEN(bool _createToken);
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
	public: void mQUOTED_STRING(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const antlr::BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const antlr::BitSet _tokenSet_2;
};

ANTLR_END_NAMESPACE
#endif /*INC_TerrainLibraryLexer_hpp_*/
