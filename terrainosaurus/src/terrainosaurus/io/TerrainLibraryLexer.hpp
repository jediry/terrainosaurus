#ifndef INC_TerrainLibraryLexer_hpp_
#define INC_TerrainLibraryLexer_hpp_

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

#line 22 "TerrainLibraryLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.3: "TerrainLibrary.g" -> "TerrainLibraryLexer.hpp"$ */
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
#line 37 "TerrainLibraryLexer.hpp"
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
	protected: void mDIGIT(bool _createToken);
	protected: void mLETTER(bool _createToken);
	protected: void mWS_CHAR(bool _createToken);
	protected: void mEOL_CHAR(bool _createToken);
	protected: void mDOUBLE_QUOTE(bool _createToken);
	public: void mEOL(bool _createToken);
	public: void mCOMMENT(bool _createToken);
	public: void mWS(bool _createToken);
	public: void mOPEN_ABRACKET(bool _createToken);
	public: void mCLOSE_ABRACKET(bool _createToken);
	public: void mOPEN_SBRACKET(bool _createToken);
	public: void mCLOSE_SBRACKET(bool _createToken);
	public: void mCOMMA(bool _createToken);
	public: void mASSIGN(bool _createToken);
	public: void mCOLON(bool _createToken);
	public: void mAND(bool _createToken);
	public: void mDOT(bool _createToken);
	public: void mBACKSLASH(bool _createToken);
	public: void mFORESLASH(bool _createToken);
	public: void mNUMBER(bool _createToken);
	public: void mSIGN(bool _createToken);
	public: void mNAME(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
};

ANTLR_END_NAMESPACE
#endif /*INC_TerrainLibraryLexer_hpp_*/
