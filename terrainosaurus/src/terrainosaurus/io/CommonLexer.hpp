#ifndef INC_CommonLexer_hpp_
#define INC_CommonLexer_hpp_

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
    

#line 27 "CommonLexer.hpp"
#include <antlr/config.hpp>
/* $ANTLR 2.7.5 (20050613): "CommonGrammar.g" -> "CommonLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "CommonTokenTypes.hpp"
#include <antlr/CharScanner.hpp>
ANTLR_BEGIN_NAMESPACE(terrainosaurus)
class CUSTOM_API CommonLexer : public antlr::CharScanner, public CommonTokenTypes
{
#line 1 "CommonGrammar.g"
#line 38 "CommonLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return false;
	}
public:
	CommonLexer(std::istream& in);
	CommonLexer(antlr::InputBuffer& ib);
	CommonLexer(const antlr::LexerSharedInputState& state);
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
#endif /*INC_CommonLexer_hpp_*/
