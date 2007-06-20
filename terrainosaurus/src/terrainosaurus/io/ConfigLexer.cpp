/* $ANTLR 2.7.5 (20050613): "/expandedConfig.g" -> "ConfigLexer.cpp"$ */
#include "ConfigLexer.hpp"
#include <antlr/CharBuffer.hpp>
#include <antlr/TokenStreamException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/CharStreamException.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/NoViableAltForCharException.hpp>

ANTLR_BEGIN_NAMESPACE(terrainosaurus)
#line 1 "/expandedConfig.g"
#line 13 "ConfigLexer.cpp"
ConfigLexer::ConfigLexer(std::istream& in)
	: antlr::CharScanner(new antlr::CharBuffer(in),false)
{
	initLiterals();
}

ConfigLexer::ConfigLexer(antlr::InputBuffer& ib)
	: antlr::CharScanner(ib,false)
{
	initLiterals();
}

ConfigLexer::ConfigLexer(const antlr::LexerSharedInputState& state)
	: antlr::CharScanner(state,false)
{
	initLiterals();
}

void ConfigLexer::initLiterals()
{
	literals["size"] = 40;
	literals["ratio"] = 48;
	literals["factor"] = 54;
	literals["cycles"] = 42;
	literals["vertical"] = 51;
	literals["cache"] = 37;
	literals["boundary"] = 34;
	literals["directory"] = 38;
	literals["scale"] = 53;
	literals["crossover"] = 46;
	literals["e"] = 13;
	literals["population"] = 39;
	literals["heightfield"] = 33;
	literals["erosion"] = 36;
	literals["width"] = 56;
	literals["elitism"] = 44;
	literals["gene"] = 49;
	literals["probability"] = 47;
	literals["offset"] = 52;
	literals["mutation"] = 45;
	literals["overlap"] = 50;
	literals["ga"] = 35;
	literals["max"] = 55;
	literals["selection"] = 43;
	literals["evolution"] = 41;
	literals["application"] = 32;
}

antlr::RefToken ConfigLexer::nextToken()
{
	antlr::RefToken theRetToken;
	for (;;) {
		antlr::RefToken theRetToken;
		int _ttype = antlr::Token::INVALID_TYPE;
		resetText();
		try {   // for lexical and char stream error handling
			switch ( LA(1)) {
			case 0x5f /* '_' */ :
			case 0x61 /* 'a' */ :
			case 0x62 /* 'b' */ :
			case 0x63 /* 'c' */ :
			case 0x64 /* 'd' */ :
			case 0x65 /* 'e' */ :
			case 0x66 /* 'f' */ :
			case 0x67 /* 'g' */ :
			case 0x68 /* 'h' */ :
			case 0x69 /* 'i' */ :
			case 0x6a /* 'j' */ :
			case 0x6b /* 'k' */ :
			case 0x6c /* 'l' */ :
			case 0x6d /* 'm' */ :
			case 0x6e /* 'n' */ :
			case 0x6f /* 'o' */ :
			case 0x70 /* 'p' */ :
			case 0x71 /* 'q' */ :
			case 0x72 /* 'r' */ :
			case 0x73 /* 's' */ :
			case 0x74 /* 't' */ :
			case 0x75 /* 'u' */ :
			case 0x76 /* 'v' */ :
			case 0x77 /* 'w' */ :
			case 0x78 /* 'x' */ :
			case 0x79 /* 'y' */ :
			case 0x7a /* 'z' */ :
			{
				mID(true);
				theRetToken=_returnToken;
				break;
			}
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
				if ((LA(1) == 0x25 /* '%' */ ) && (LA(2) == 0x23 /* '#' */ )) {
					mANTLR_DUMMY_TOKEN(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x5c /* '\\' */ ) && (LA(2) == 0x20 /* ' ' */ )) {
					mNBSP(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x25 /* '%' */ ) && (true)) {
					mPERCENT(true);
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

void ConfigLexer::mID(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = ID;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x61 /* 'a' */ :
	case 0x62 /* 'b' */ :
	case 0x63 /* 'c' */ :
	case 0x64 /* 'd' */ :
	case 0x65 /* 'e' */ :
	case 0x66 /* 'f' */ :
	case 0x67 /* 'g' */ :
	case 0x68 /* 'h' */ :
	case 0x69 /* 'i' */ :
	case 0x6a /* 'j' */ :
	case 0x6b /* 'k' */ :
	case 0x6c /* 'l' */ :
	case 0x6d /* 'm' */ :
	case 0x6e /* 'n' */ :
	case 0x6f /* 'o' */ :
	case 0x70 /* 'p' */ :
	case 0x71 /* 'q' */ :
	case 0x72 /* 'r' */ :
	case 0x73 /* 's' */ :
	case 0x74 /* 't' */ :
	case 0x75 /* 'u' */ :
	case 0x76 /* 'v' */ :
	case 0x77 /* 'w' */ :
	case 0x78 /* 'x' */ :
	case 0x79 /* 'y' */ :
	case 0x7a /* 'z' */ :
	{
		mLETTER(false);
		break;
	}
	case 0x5f /* '_' */ :
	{
		match('_' /* charlit */ );
		break;
	}
	default:
	{
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}
	}
	{ // ( ... )*
	for (;;) {
		switch ( LA(1)) {
		case 0x61 /* 'a' */ :
		case 0x62 /* 'b' */ :
		case 0x63 /* 'c' */ :
		case 0x64 /* 'd' */ :
		case 0x65 /* 'e' */ :
		case 0x66 /* 'f' */ :
		case 0x67 /* 'g' */ :
		case 0x68 /* 'h' */ :
		case 0x69 /* 'i' */ :
		case 0x6a /* 'j' */ :
		case 0x6b /* 'k' */ :
		case 0x6c /* 'l' */ :
		case 0x6d /* 'm' */ :
		case 0x6e /* 'n' */ :
		case 0x6f /* 'o' */ :
		case 0x70 /* 'p' */ :
		case 0x71 /* 'q' */ :
		case 0x72 /* 'r' */ :
		case 0x73 /* 's' */ :
		case 0x74 /* 't' */ :
		case 0x75 /* 'u' */ :
		case 0x76 /* 'v' */ :
		case 0x77 /* 'w' */ :
		case 0x78 /* 'x' */ :
		case 0x79 /* 'y' */ :
		case 0x7a /* 'z' */ :
		{
			mLETTER(false);
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
			mDIGIT(false);
			break;
		}
		case 0x5f /* '_' */ :
		{
			match('_' /* charlit */ );
			break;
		}
		default:
		{
			goto _loop58;
		}
		}
	}
	_loop58:;
	} // ( ... )*
	_ttype = testLiteralsTable(_ttype);
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void ConfigLexer::mLETTER(bool _createToken) {
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

void ConfigLexer::mDIGIT(bool _createToken) {
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

void ConfigLexer::mANTLR_DUMMY_TOKEN(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = ANTLR_DUMMY_TOKEN;
	std::string::size_type _saveIndex;
	
	match("%#%dummy%#%");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void ConfigLexer::mWS_CHAR(bool _createToken) {
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

void ConfigLexer::mEOL_CHAR(bool _createToken) {
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

void ConfigLexer::mDQUOTE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = DQUOTE;
	std::string::size_type _saveIndex;
	
	match('\"' /* charlit */ );
#line 346 "/expandedConfig.g"
	{ text.erase(_begin); text += ""; };
#line 488 "ConfigLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void ConfigLexer::mSQUOTE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = SQUOTE;
	std::string::size_type _saveIndex;
	
	match('\'' /* charlit */ );
#line 349 "/expandedConfig.g"
	{ text.erase(_begin); text += ""; };
#line 505 "ConfigLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void ConfigLexer::mSPACE(bool _createToken) {
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

void ConfigLexer::mEOL(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = EOL;
	std::string::size_type _saveIndex;
	
	mEOL_CHAR(false);
#line 359 "/expandedConfig.g"
	newline();
#line 536 "ConfigLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void ConfigLexer::mCOMMENT(bool _createToken) {
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
			goto _loop75;
		}
		
	}
	_loop75:;
	} // ( ... )*
	mEOL_CHAR(false);
#line 366 "/expandedConfig.g"
	newline(); _ttype = EOL;
#line 568 "ConfigLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void ConfigLexer::mWS(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = WS;
	std::string::size_type _saveIndex;
	
	{ // ( ... )+
	int _cnt78=0;
	for (;;) {
		if ((LA(1) == 0x9 /* '\t' */  || LA(1) == 0x20 /* ' ' */ )) {
			mWS_CHAR(false);
		}
		else {
			if ( _cnt78>=1 ) { goto _loop78; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt78++;
	}
	_loop78:;
	}  // ( ... )+
#line 369 "/expandedConfig.g"
	_ttype = antlr::Token::SKIP;
#line 598 "ConfigLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void ConfigLexer::mLEFT_ABRACKET(bool _createToken) {
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

void ConfigLexer::mRIGHT_ABRACKET(bool _createToken) {
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

void ConfigLexer::mLEFT_SBRACKET(bool _createToken) {
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

void ConfigLexer::mRIGHT_SBRACKET(bool _createToken) {
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

void ConfigLexer::mCOMMA(bool _createToken) {
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

void ConfigLexer::mASSIGN(bool _createToken) {
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

void ConfigLexer::mPERCENT(bool _createToken) {
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

void ConfigLexer::mCOLON(bool _createToken) {
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

void ConfigLexer::mAND(bool _createToken) {
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

void ConfigLexer::mDOT(bool _createToken) {
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

void ConfigLexer::mBACKSLASH(bool _createToken) {
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

void ConfigLexer::mFORESLASH(bool _createToken) {
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

void ConfigLexer::mNBSP(bool _createToken) {
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

void ConfigLexer::mSIGN(bool _createToken) {
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

void ConfigLexer::mNUMBER(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = NUMBER;
	std::string::size_type _saveIndex;
	
	{ // ( ... )+
	int _cnt96=0;
	for (;;) {
		if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
			mDIGIT(false);
		}
		else {
			if ( _cnt96>=1 ) { goto _loop96; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt96++;
	}
	_loop96:;
	}  // ( ... )+
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void ConfigLexer::mQUOTED_STRING(bool _createToken) {
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
				goto _loop100;
			}
			
		}
		_loop100:;
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
				goto _loop103;
			}
			
		}
		_loop103:;
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


const unsigned long ConfigLexer::_tokenSet_0_data_[] = { 4294958072UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xc 0xe 0xf 0x10 0x11 0x12 0x13 0x14 
// 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # $ % 
// & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 
const antlr::BitSet ConfigLexer::_tokenSet_0(_tokenSet_0_data_,16);
const unsigned long ConfigLexer::_tokenSet_1_data_[] = { 4294967288UL, 4294967291UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! # $ 
// % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 
const antlr::BitSet ConfigLexer::_tokenSet_1(_tokenSet_1_data_,16);
const unsigned long ConfigLexer::_tokenSet_2_data_[] = { 4294967288UL, 4294967167UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # 
// $ % & ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 
const antlr::BitSet ConfigLexer::_tokenSet_2(_tokenSet_2_data_,16);

ANTLR_END_NAMESPACE
