/* $ANTLR 2.7.3: "TerrainLibrary.g" -> "TerrainLibraryLexer.cpp"$ */
#include "TerrainLibraryLexer.hpp"
#include <antlr/CharBuffer.hpp>
#include <antlr/TokenStreamException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/CharStreamException.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/NoViableAltForCharException.hpp>

ANTLR_BEGIN_NAMESPACE(terrainosaurus)
#line 1 "TerrainLibrary.g"
#line 13 "TerrainLibraryLexer.cpp"
TerrainLibraryLexer::TerrainLibraryLexer(std::istream& in)
	: antlr::CharScanner(new antlr::CharBuffer(in),false)
{
	initLiterals();
}

TerrainLibraryLexer::TerrainLibraryLexer(antlr::InputBuffer& ib)
	: antlr::CharScanner(ib,false)
{
	initLiterals();
}

TerrainLibraryLexer::TerrainLibraryLexer(const antlr::LexerSharedInputState& state)
	: antlr::CharScanner(state,false)
{
	initLiterals();
}

void TerrainLibraryLexer::initLiterals()
{
	literals["terraintype"] = 5;
	literals["color"] = 12;
	literals["sample"] = 14;
	literals["of"] = 16;
	literals["number"] = 15;
	literals["chromosomes"] = 17;
	literals["aspect"] = 23;
	literals["mutation"] = 19;
	literals["smoothness"] = 18;
	literals["ratio"] = 20;
	literals["selection"] = 22;
	literals["terrainseam"] = 10;
	literals["crossover"] = 21;
}

antlr::RefToken TerrainLibraryLexer::nextToken()
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
				mOPEN_ABRACKET(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3e /* '>' */ :
			{
				mCLOSE_ABRACKET(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5b /* '[' */ :
			{
				mOPEN_SBRACKET(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5d /* ']' */ :
			{
				mCLOSE_SBRACKET(true);
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
			case 0x5c /* '\\' */ :
			{
				mBACKSLASH(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2f /* '/' */ :
			{
				mFORESLASH(true);
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
			case 0x2b /* '+' */ :
			case 0x2d /* '-' */ :
			{
				mSIGN(true);
				theRetToken=_returnToken;
				break;
			}
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
				mNAME(true);
				theRetToken=_returnToken;
				break;
			}
			default:
			{
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

void TerrainLibraryLexer::mDIGIT(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = DIGIT;
	int _saveIndex;
	
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

void TerrainLibraryLexer::mLETTER(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = LETTER;
	int _saveIndex;
	
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

void TerrainLibraryLexer::mWS_CHAR(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = WS_CHAR;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x20 /* ' ' */ :
	{
		match(' ');
		break;
	}
	case 0x9 /* '\t' */ :
	{
		match('\t');
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

void TerrainLibraryLexer::mEOL_CHAR(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = EOL_CHAR;
	int _saveIndex;
	
	{
	if ((LA(1) == 0xd /* '\r' */ ) && (LA(2) == 0xa /* '\n' */ )) {
		match("\r\n");
	}
	else if ((LA(1) == 0xd /* '\r' */ ) && (true)) {
		match('\r');
	}
	else if ((LA(1) == 0xa /* '\n' */ )) {
		match('\n');
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

void TerrainLibraryLexer::mDOUBLE_QUOTE(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = DOUBLE_QUOTE;
	int _saveIndex;
	
	match('"');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mEOL(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = EOL;
	int _saveIndex;
	
	mEOL_CHAR(false);
#line 299 "TerrainLibrary.g"
	newline();
#line 341 "TerrainLibraryLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mCOMMENT(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = COMMENT;
	int _saveIndex;
	
	match('#');
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			{
			match(_tokenSet_0);
			}
		}
		else {
			goto _loop47;
		}
		
	}
	_loop47:;
	} // ( ... )*
	mEOL_CHAR(false);
#line 301 "TerrainLibrary.g"
	_ttype = EOL;
#line 373 "TerrainLibraryLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mWS(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = WS;
	int _saveIndex;
	
	{ // ( ... )+
	int _cnt50=0;
	for (;;) {
		if ((LA(1) == 0x9 /* '\t' */  || LA(1) == 0x20 /* ' ' */ )) {
			mWS_CHAR(false);
		}
		else {
			if ( _cnt50>=1 ) { goto _loop50; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt50++;
	}
	_loop50:;
	}  // ( ... )+
#line 302 "TerrainLibrary.g"
	_ttype = antlr::Token::SKIP;
#line 403 "TerrainLibraryLexer.cpp"
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mOPEN_ABRACKET(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = OPEN_ABRACKET;
	int _saveIndex;
	
	match("<");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mCLOSE_ABRACKET(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CLOSE_ABRACKET;
	int _saveIndex;
	
	match(">");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mOPEN_SBRACKET(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = OPEN_SBRACKET;
	int _saveIndex;
	
	match("[");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mCLOSE_SBRACKET(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CLOSE_SBRACKET;
	int _saveIndex;
	
	match("]");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mCOMMA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = COMMA;
	int _saveIndex;
	
	match(",");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mASSIGN(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = ASSIGN;
	int _saveIndex;
	
	match("=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mCOLON(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = COLON;
	int _saveIndex;
	
	match(":");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mAND(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = AND;
	int _saveIndex;
	
	match("&");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mDOT(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = DOT;
	int _saveIndex;
	
	match(".");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mBACKSLASH(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = BACKSLASH;
	int _saveIndex;
	
	match('\\');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mFORESLASH(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = FORESLASH;
	int _saveIndex;
	
	match('/');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mNUMBER(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = NUMBER;
	int _saveIndex;
	
	{ // ( ... )+
	int _cnt64=0;
	for (;;) {
		if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
			mDIGIT(false);
		}
		else {
			if ( _cnt64>=1 ) { goto _loop64; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt64++;
	}
	_loop64:;
	}  // ( ... )+
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void TerrainLibraryLexer::mSIGN(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = SIGN;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x2b /* '+' */ :
	{
		match('+');
		break;
	}
	case 0x2d /* '-' */ :
	{
		match('-');
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

void TerrainLibraryLexer::mNAME(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = NAME;
	int _saveIndex;
	
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
		match('_');
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
			match('_');
			break;
		}
		default:
		{
			goto _loop70;
		}
		}
	}
	_loop70:;
	} // ( ... )*
	_ttype = testLiteralsTable(_ttype);
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}


const unsigned long TerrainLibraryLexer::_tokenSet_0_data_[] = { 4294958072UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// NULL_TREE_LOOKAHEAD OPEN_SBRACKET "terraintype" COLON NAME CLOSE_SBRACKET 
// EOL AND "color" "sample" "number" "of" "chromosomes" "smoothness" "mutation" 
// "ratio" "crossover" "selection" "aspect" SIGN DOT OPEN_ABRACKET COMMA 
// CLOSE_ABRACKET BACKSLASH FORESLASH SPACE DIGIT LETTER WS_CHAR EOL_CHAR 
// DOUBLE_QUOTE COMMENT WS 
const antlr::BitSet TerrainLibraryLexer::_tokenSet_0(_tokenSet_0_data_,16);

ANTLR_END_NAMESPACE
