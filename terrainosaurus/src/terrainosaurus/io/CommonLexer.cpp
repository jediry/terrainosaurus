/* $ANTLR 2.7.5 (20050613): "CommonGrammar.g" -> "CommonLexer.cpp"$ */
#include "CommonLexer.hpp"
#include <antlr/CharBuffer.hpp>
#include <antlr/TokenStreamException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/CharStreamException.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/NoViableAltForCharException.hpp>

ANTLR_BEGIN_NAMESPACE(terrainosaurus)
#line 1 "CommonGrammar.g"
#line 13 "CommonLexer.cpp"
CommonLexer::CommonLexer(std::istream& in)
	: antlr::CharScanner(new antlr::CharBuffer(in),false)
{
	initLiterals();
}

CommonLexer::CommonLexer(antlr::InputBuffer& ib)
	: antlr::CharScanner(ib,false)
{
	initLiterals();
}

CommonLexer::CommonLexer(const antlr::LexerSharedInputState& state)
	: antlr::CharScanner(state,false)
{
	initLiterals();
}

void CommonLexer::initLiterals()
{
	literals["e"] = 13;
}

antlr::RefToken CommonLexer::nextToken()
{
	antlr::RefToken theRetToken;
	for (;;) {
		antlr::RefToken theRetToken;
		int _ttype = antlr::Token::INVALID_TYPE;
		resetText();
		try {   // for lexical and char stream error handling
			switch ( LA(1)) {
			case 0xa /* '\n' */ :
			case 0xd /* '\r' */ :
			{
				mEOL(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x23 /* '#' */ :
			{
				mCOMMENT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x9 /* '\t' */ :
			case 0x20 /* ' ' */ :
			{
				mWS(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3c /* '<' */ :
			{
				mLEFT_ABRACKET(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3e /* '>' */ :
			{
				mRIGHT_ABRACKET(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5b /* '[' */ :
			{
				mLEFT_SBRACKET(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5d /* ']' */ :
			{
				mRIGHT_SBRACKET(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2c /* ',' */ :
			{
				mCOMMA(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3d /* '=' */ :
			{
				mASSIGN(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x25 /* '%' */ :
			{
				mPERCENT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3a /* ':' */ :
			{
				mCOLON(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x26 /* '&' */ :
			{
				mAND(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2e /* '.' */ :
			{
				mDOT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2f /* '/' */ :
			{
				mFORESLASH(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2b /* '+' */ :
			case 0x2d /* '-' */ :
			{
				mSIGN(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x30 /* '0' */ :
			case 0x31 /* '1' */ :
			case 0x32 /* '2' */ :
			case 0x33 /* '3' */ :
			case 0x34 /* '4' */ :
			case 0x35 /* '5' */ :
			case 0x36 /* '6' */ :
			case 0x37 /* '7' */ :
			case 0x38 /* '8' */ :
			case 0x39 /* '9' */ :
			{
				mNUMBER(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x22 /* '\"' */ :
			case 0x27 /* '\'' */ :
			{
				mQUOTED_STRING(true);
				theRetToken=_returnToken;
				break;
			}
			default:
				if ((LA(1) == 0x5c /* '\\' */ ) && (LA(2) == 0x20 /* ' ' */ )) {
					mNBSP(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x5c /* '\\' */ ) && (true)) {
					mBACKSLASH(true);
					theRetToken=_returnToken;
				}
			else {
				if (LA(1)==EOF_CHAR)
				{
					uponEOF();
					_returnToken = makeToken(antlr::Token::EOF_TYPE);
				}
				else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			}
			if ( !_returnToken )
				goto tryAgain; // found SKIP token

			_ttype = _returnToken->getType();
			_returnToken->setType(_ttype);
			return _returnToken;
		}
		catch (antlr::RecognitionException& e) {
				throw antlr::TokenStreamRecognitionException(e);
		}
		catch (antlr::CharStreamIOException& csie) {
			throw antlr::TokenStreamIOException(csie.io);
		}
		catch (antlr::CharStreamException& cse) {
			throw antlr::TokenStreamException(cse.getMessage());
		}
tryAgain:;
	}
}

void CommonLexer::mDIGIT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = DIGIT;
	std::string::size_type _saveIndex;
	
	{
	matchRange('0','9');
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mLETTER(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LETTER;
	std::string::size_type _saveIndex;
	
	{
	matchRange('a','z');
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mWS_CHAR(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = WS_CHAR;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x20 /* ' ' */ :
	{
		match(' ' /* charlit */ );
		break;
	}
	case 0x9 /* '\t' */ :
	{
		match('\t' /* charlit */ );
		break;
	}
	default:
	{
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mEOL_CHAR(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = EOL_CHAR;
	std::string::size_type _saveIndex;
	
	{
	if ((LA(1) == 0xd /* '\r' */ ) && (LA(2) == 0xa /* '\n' */ )) {
		match("\r\n");
	}
	else if ((LA(1) == 0xd /* '\r' */ ) && (true)) {
		match('\r' /* charlit */ );
	}
	else if ((LA(1) == 0xa /* '\n' */ )) {
		match('\n' /* charlit */ );
	}
	else {
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mDQUOTE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = DQUOTE;
	std::string::size_type _saveIndex;
	
	match('\"' /* charlit */ );
#line 184 "CommonGrammar.g"
	{ text.erase(_begin); text += ""; };
#line 298 "CommonLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mSQUOTE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = SQUOTE;
	std::string::size_type _saveIndex;
	
	match('\'' /* charlit */ );
#line 185 "CommonGrammar.g"
	{ text.erase(_begin); text += ""; };
#line 315 "CommonLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mSPACE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = SPACE;
	std::string::size_type _saveIndex;
	
	match(' ' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mEOL(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = EOL;
	std::string::size_type _saveIndex;
	
	mEOL_CHAR(false);
#line 189 "CommonGrammar.g"
	newline();
#line 346 "CommonLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mCOMMENT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = COMMENT;
	std::string::size_type _saveIndex;
	
	match('#' /* charlit */ );
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			{
			match(_tokenSet_0);
			}
		}
		else {
			goto _loop38;
		}
		
	}
	_loop38:;
	} // ( ... )*
	mEOL_CHAR(false);
#line 191 "CommonGrammar.g"
	newline(); _ttype = EOL;
#line 378 "CommonLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mWS(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = WS;
	std::string::size_type _saveIndex;
	
	{ // ( ... )+
	int _cnt41=0;
	for (;;) {
		if ((LA(1) == 0x9 /* '\t' */  || LA(1) == 0x20 /* ' ' */ )) {
			mWS_CHAR(false);
		}
		else {
			if ( _cnt41>=1 ) { goto _loop41; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt41++;
	}
	_loop41:;
	}  // ( ... )+
#line 192 "CommonGrammar.g"
	_ttype = antlr::Token::SKIP;
#line 408 "CommonLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mLEFT_ABRACKET(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LEFT_ABRACKET;
	std::string::size_type _saveIndex;
	
	match('<' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mRIGHT_ABRACKET(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = RIGHT_ABRACKET;
	std::string::size_type _saveIndex;
	
	match('>' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mLEFT_SBRACKET(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LEFT_SBRACKET;
	std::string::size_type _saveIndex;
	
	match('[' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mRIGHT_SBRACKET(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = RIGHT_SBRACKET;
	std::string::size_type _saveIndex;
	
	match(']' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mCOMMA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = COMMA;
	std::string::size_type _saveIndex;
	
	match(',' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mASSIGN(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = ASSIGN;
	std::string::size_type _saveIndex;
	
	match('=' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mPERCENT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = PERCENT;
	std::string::size_type _saveIndex;
	
	match('%' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mCOLON(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = COLON;
	std::string::size_type _saveIndex;
	
	match(':' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mAND(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = AND;
	std::string::size_type _saveIndex;
	
	match('&' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mDOT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = DOT;
	std::string::size_type _saveIndex;
	
	match('.' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mBACKSLASH(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = BACKSLASH;
	std::string::size_type _saveIndex;
	
	match('\\' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mFORESLASH(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = FORESLASH;
	std::string::size_type _saveIndex;
	
	match('/' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mNBSP(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = NBSP;
	std::string::size_type _saveIndex;
	
	match("\\ ");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mSIGN(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = SIGN;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x2b /* '+' */ :
	{
		match('+' /* charlit */ );
		break;
	}
	case 0x2d /* '-' */ :
	{
		match('-' /* charlit */ );
		break;
	}
	default:
	{
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mNUMBER(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = NUMBER;
	std::string::size_type _saveIndex;
	
	{ // ( ... )+
	int _cnt59=0;
	for (;;) {
		if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
			mDIGIT(false);
		}
		else {
			if ( _cnt59>=1 ) { goto _loop59; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt59++;
	}
	_loop59:;
	}  // ( ... )+
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CommonLexer::mQUOTED_STRING(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = QUOTED_STRING;
	std::string::size_type _saveIndex;
	
	switch ( LA(1)) {
	case 0x22 /* '\"' */ :
	{
		{
		mDQUOTE(false);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_1.member(LA(1)))) {
				matchNot('\"' /* charlit */ );
			}
			else {
				goto _loop63;
			}
			
		}
		_loop63:;
		} // ( ... )*
		mDQUOTE(false);
		}
		break;
	}
	case 0x27 /* '\'' */ :
	{
		{
		mSQUOTE(false);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_2.member(LA(1)))) {
				matchNot('\'' /* charlit */ );
			}
			else {
				goto _loop66;
			}
			
		}
		_loop66:;
		} // ( ... )*
		mSQUOTE(false);
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}


const unsigned long CommonLexer::_tokenSet_0_data_[] = { 4294958072UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xc 0xe 0xf 0x10 0x11 0x12 0x13 0x14 
// 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f 
const antlr::BitSet CommonLexer::_tokenSet_0(_tokenSet_0_data_,16);
const unsigned long CommonLexer::_tokenSet_1_data_[] = { 4294967288UL, 4294967291UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f 
const antlr::BitSet CommonLexer::_tokenSet_1(_tokenSet_1_data_,16);
const unsigned long CommonLexer::_tokenSet_2_data_[] = { 4294967288UL, 4294967167UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f 
const antlr::BitSet CommonLexer::_tokenSet_2(_tokenSet_2_data_,16);

ANTLR_END_NAMESPACE
