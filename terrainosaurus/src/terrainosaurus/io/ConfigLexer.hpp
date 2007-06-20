#ifndef INC_ConfigLexer_hpp_
#define INC_ConfigLexer_hpp_

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

#line 26 "ConfigLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.5 (20050613): "/expandedConfig.g" -> "ConfigLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "ConfigTokenTypes.hpp"
#include <antlr/CharScanner.hpp>
ANTLR_BEGIN_NAMESPACE(terrainosaurus)
class CUSTOM_API ConfigLexer : public antlr::CharScanner, public ConfigTokenTypes
{
#line 1 "/expandedConfig.g"
#line 37 "ConfigLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return false;
	}
public:
	ConfigLexer(std::istream& in);
	ConfigLexer(antlr::InputBuffer& ib);
	ConfigLexer(const antlr::LexerSharedInputState& state);
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
	public: void mAND(bool _createToken);
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
#endif /*INC_ConfigLexer_hpp_*/
