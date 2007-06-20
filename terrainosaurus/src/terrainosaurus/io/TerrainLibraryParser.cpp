/* $ANTLR 2.7.5 (20050613): "TerrainLibrary.g" -> "TerrainLibraryParser.cpp"$ */
#include "TerrainLibraryParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
ANTLR_BEGIN_NAMESPACE(terrainosaurus)
#line 1 "TerrainLibrary.g"
#line 8 "TerrainLibraryParser.cpp"
TerrainLibraryParser::TerrainLibraryParser(antlr::TokenBuffer& tokenBuf, int k)
: ::terrainosaurus::INIParser(tokenBuf,k)
{
}

TerrainLibraryParser::TerrainLibraryParser(antlr::TokenBuffer& tokenBuf)
: ::terrainosaurus::INIParser(tokenBuf,2)
{
}

TerrainLibraryParser::TerrainLibraryParser(antlr::TokenStream& lexer, int k)
: ::terrainosaurus::INIParser(lexer,k)
{
}

TerrainLibraryParser::TerrainLibraryParser(antlr::TokenStream& lexer)
: ::terrainosaurus::INIParser(lexer,2)
{
}

TerrainLibraryParser::TerrainLibraryParser(const antlr::ParserSharedInputState& state)
: ::terrainosaurus::INIParser(state,2)
{
}

void TerrainLibraryParser::sectionList(
	TerrainLibrary * lib
) {
	
#line 122 "TerrainLibrary.g"
	_terrainLibrary = lib;
#line 40 "TerrainLibraryParser.cpp"
	if (!( _terrainLibrary != NULL ))
		throw antlr::SemanticException(" _terrainLibrary != NULL ");
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == EOL)) {
			blankLine();
		}
		else {
			goto _loop3;
		}
		
	}
	_loop3:;
	} // ( ... )*
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LEFT_SBRACKET) && (LA(2) == TERRAIN_TYPE)) {
			terrainTypeSection();
		}
		else if ((LA(1) == LEFT_SBRACKET) && (LA(2) == TERRAIN_SEAM)) {
			terrainSeamSection();
		}
		else {
			goto _loop5;
		}
		
	}
	_loop5:;
	} // ( ... )*
	match(antlr::Token::EOF_TYPE);
}

void TerrainLibraryParser::blankLine() {
	
	match(EOL);
}

void TerrainLibraryParser::terrainTypeSection() {
	antlr::RefToken  t = antlr::nullToken;
#line 128 "TerrainLibrary.g"
	std::string tt;
#line 82 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(LEFT_SBRACKET);
	match(TERRAIN_TYPE);
	match(COLON);
	tt=string();
	match(RIGHT_SBRACKET);
	match(EOL);
#line 130 "TerrainLibrary.g"
	beginTerrainTypeSection(t, tt);
#line 93 "TerrainLibraryParser.cpp"
	{ // ( ... )*
	for (;;) {
		switch ( LA(1)) {
		case EOL:
		{
			blankLine();
			break;
		}
		case COLOR:
		{
			terrainColor();
			break;
		}
		case SAMPLE:
		{
			terrainSample();
			break;
		}
		default:
		{
			goto _loop8;
		}
		}
	}
	_loop8:;
	} // ( ... )*
#line 134 "TerrainLibrary.g"
	endSection();
#line 122 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::terrainSeamSection() {
	antlr::RefToken  t = antlr::nullToken;
#line 137 "TerrainLibrary.g"
	std::string tt1, tt2;
#line 129 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(LEFT_SBRACKET);
	match(TERRAIN_SEAM);
	match(COLON);
	tt1=string();
	match(AMPERSAND);
	tt2=string();
	match(RIGHT_SBRACKET);
	match(EOL);
#line 140 "TerrainLibrary.g"
	beginTerrainSeamSection(t, tt1, tt2);
#line 142 "TerrainLibraryParser.cpp"
	{ // ( ... )*
	for (;;) {
		switch ( LA(1)) {
		case EOL:
		{
			blankLine();
			break;
		}
		case SMOOTHNESS:
		{
			smoothness();
			break;
		}
		case ASPECT:
		{
			aspectRatio();
			break;
		}
		default:
		{
			goto _loop11;
		}
		}
	}
	_loop11:;
	} // ( ... )*
#line 144 "TerrainLibrary.g"
	endSection();
#line 171 "TerrainLibraryParser.cpp"
}

std::string  TerrainLibraryParser::string() {
#line 183 "TerrainLibrary.g"
	std::string s;
#line 177 "TerrainLibraryParser.cpp"
	antlr::RefToken  q = antlr::nullToken;
	antlr::RefToken  i1 = antlr::nullToken;
	antlr::RefToken  n1 = antlr::nullToken;
	antlr::RefToken  i2 = antlr::nullToken;
	antlr::RefToken  n2 = antlr::nullToken;
	
	{
	switch ( LA(1)) {
	case QUOTED_STRING:
	{
		q = LT(1);
		match(QUOTED_STRING);
#line 184 "TerrainLibrary.g"
		s += q->getText();
#line 192 "TerrainLibraryParser.cpp"
		break;
	}
	case ID:
	case NUMBER:
	{
		{ // ( ... )+
		int _cnt23=0;
		for (;;) {
			switch ( LA(1)) {
			case ID:
			{
				i1 = LT(1);
				match(ID);
#line 185 "TerrainLibrary.g"
				s += i1->getText();
#line 208 "TerrainLibraryParser.cpp"
				break;
			}
			case NUMBER:
			{
				n1 = LT(1);
				match(NUMBER);
#line 186 "TerrainLibrary.g"
				s += n1->getText();
#line 217 "TerrainLibraryParser.cpp"
				{ // ( ... )*
				for (;;) {
					if ((LA(1) == NBSP)) {
						match(NBSP);
#line 187 "TerrainLibrary.g"
						s += " ";
#line 224 "TerrainLibraryParser.cpp"
						{
						switch ( LA(1)) {
						case ID:
						{
							i2 = LT(1);
							match(ID);
#line 188 "TerrainLibrary.g"
							s += i2->getText();
#line 233 "TerrainLibraryParser.cpp"
							break;
						}
						case NUMBER:
						{
							n2 = LT(1);
							match(NUMBER);
#line 189 "TerrainLibrary.g"
							s += n2->getText();
#line 242 "TerrainLibraryParser.cpp"
							break;
						}
						default:
						{
							throw antlr::NoViableAltException(LT(1), getFilename());
						}
						}
						}
					}
					else {
						goto _loop22;
					}
					
				}
				_loop22:;
				} // ( ... )*
				break;
			}
			default:
			{
				if ( _cnt23>=1 ) { goto _loop23; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt23++;
		}
		_loop23:;
		}  // ( ... )+
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	return s;
}

void TerrainLibraryParser::terrainColor() {
	antlr::RefToken  t = antlr::nullToken;
#line 151 "TerrainLibrary.g"
	Color c;
#line 285 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(COLOR);
	match(ASSIGN);
	c=color();
	match(EOL);
#line 153 "TerrainLibrary.g"
	assignColorProperty(t, TTColor, c);
#line 294 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::terrainSample() {
	antlr::RefToken  t = antlr::nullToken;
#line 156 "TerrainLibrary.g"
	std::string s;
#line 301 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(SAMPLE);
	match(ASSIGN);
	s=string();
	match(EOL);
#line 158 "TerrainLibrary.g"
	assignStringProperty(t, TTSample, s);
#line 310 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::smoothness() {
	antlr::RefToken  t = antlr::nullToken;
#line 165 "TerrainLibrary.g"
	scalar_t s;
#line 317 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(SMOOTHNESS);
	match(ASSIGN);
	s=nFraction();
	match(EOL);
#line 167 "TerrainLibrary.g"
	assignScalarProperty(t, TSSmoothness, s);
#line 326 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::aspectRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 170 "TerrainLibrary.g"
	scalar_t s;
#line 333 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(ASPECT);
	match(RATIO);
	match(ASSIGN);
	s=fraction();
	match(EOL);
#line 172 "TerrainLibrary.g"
	assignScalarProperty(t, TSAspectRatio, s);
#line 343 "TerrainLibraryParser.cpp"
}

Color  TerrainLibraryParser::color() {
#line 265 "TerrainLibrary.g"
	Color c;
#line 349 "TerrainLibraryParser.cpp"
	
#line 266 "TerrainLibrary.g"
	scalar_t r, g, b, a;
#line 353 "TerrainLibraryParser.cpp"
	match(LEFT_ABRACKET);
	r=nFraction();
	match(COMMA);
	g=nFraction();
	match(COMMA);
	b=nFraction();
	match(COMMA);
	a=nFraction();
	match(RIGHT_ABRACKET);
#line 272 "TerrainLibrary.g"
	typedef Color::scalar_t c_scalar_t;
	c = Color(c_scalar_t(r), c_scalar_t(g),
	c_scalar_t(b), c_scalar_t(a));
#line 367 "TerrainLibraryParser.cpp"
	return c;
}

scalar_t  TerrainLibraryParser::nFraction() {
#line 259 "TerrainLibrary.g"
	scalar_t s;
#line 374 "TerrainLibraryParser.cpp"
	
	s=fraction();
	if (!( s >= scalar_t(0) && s <= scalar_t(1) ))
		throw antlr::SemanticException(" s >= scalar_t(0) && s <= scalar_t(1) ");
	return s;
}

scalar_t  TerrainLibraryParser::fraction() {
#line 226 "TerrainLibrary.g"
	scalar_t s;
#line 385 "TerrainLibraryParser.cpp"
	antlr::RefToken  w = antlr::nullToken;
	antlr::RefToken  f = antlr::nullToken;
	antlr::RefToken  sg = antlr::nullToken;
	antlr::RefToken  e = antlr::nullToken;
	antlr::RefToken  p = antlr::nullToken;
	
	w = LT(1);
	match(NUMBER);
	{
	switch ( LA(1)) {
	case DOT:
	{
		match(DOT);
		f = LT(1);
		match(NUMBER);
		break;
	}
	case antlr::Token::EOF_TYPE:
	case EOL:
	case LITERAL_e:
	case PERCENT:
	case COMMA:
	case RIGHT_ABRACKET:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	switch ( LA(1)) {
	case LITERAL_e:
	{
		match(LITERAL_e);
		{
		switch ( LA(1)) {
		case SIGN:
		{
			sg = LT(1);
			match(SIGN);
			break;
		}
		case NUMBER:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		e = LT(1);
		match(NUMBER);
		break;
	}
	case antlr::Token::EOF_TYPE:
	case EOL:
	case PERCENT:
	case COMMA:
	case RIGHT_ABRACKET:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	switch ( LA(1)) {
	case PERCENT:
	{
		p = LT(1);
		match(PERCENT);
		break;
	}
	case antlr::Token::EOF_TYPE:
	case EOL:
	case COMMA:
	case RIGHT_ABRACKET:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 228 "TerrainLibrary.g"
	
	// First, convert the floating point number
	std::string tmp(w->getText());          // Whole # part
	tmp += ".";                             // Decimal point
	if (f != NULL)  tmp += f->getText();    // Fractional part
	else            tmp += "0";             // Implicit zero
	if (e != NULL) {                        // Exponent
	tmp += "e";
	if (sg != NULL) tmp += sg->getText();
	tmp += e->getText();
	}
	s = scalar_t(atof(tmp.c_str()));
	
	// If this is a percent, convert to normal decimal by dividing by 100
	if (p != NULL)  s /= 100;
	
#line 497 "TerrainLibraryParser.cpp"
	return s;
}

void TerrainLibraryParser::antlrDummyRule() {
	
	match(ANTLR_DUMMY_TOKEN);
}

std::string  TerrainLibraryParser::path() {
#line 197 "TerrainLibrary.g"
	std::string p;
#line 509 "TerrainLibraryParser.cpp"
	antlr::RefToken  s = antlr::nullToken;
	antlr::RefToken  i = antlr::nullToken;
	antlr::RefToken  n = antlr::nullToken;
	antlr::RefToken  c = antlr::nullToken;
	antlr::RefToken  d = antlr::nullToken;
	antlr::RefToken  b = antlr::nullToken;
	antlr::RefToken  f = antlr::nullToken;
	
	{ // ( ... )+
	int _cnt26=0;
	for (;;) {
		switch ( LA(1)) {
		case QUOTED_STRING:
		{
			s = LT(1);
			match(QUOTED_STRING);
#line 198 "TerrainLibrary.g"
			p += s->getText();
#line 528 "TerrainLibraryParser.cpp"
			break;
		}
		case ID:
		{
			i = LT(1);
			match(ID);
#line 199 "TerrainLibrary.g"
			p += i->getText();
#line 537 "TerrainLibraryParser.cpp"
			break;
		}
		case NUMBER:
		{
			n = LT(1);
			match(NUMBER);
#line 200 "TerrainLibrary.g"
			p += n->getText();
#line 546 "TerrainLibraryParser.cpp"
			break;
		}
		case COLON:
		{
			c = LT(1);
			match(COLON);
#line 201 "TerrainLibrary.g"
			p += c->getText();
#line 555 "TerrainLibraryParser.cpp"
			break;
		}
		case DOT:
		{
			d = LT(1);
			match(DOT);
#line 202 "TerrainLibrary.g"
			p += d->getText();
#line 564 "TerrainLibraryParser.cpp"
			break;
		}
		case BACKSLASH:
		{
			b = LT(1);
			match(BACKSLASH);
#line 203 "TerrainLibrary.g"
			p += b->getText();
#line 573 "TerrainLibraryParser.cpp"
			break;
		}
		case FORESLASH:
		{
			f = LT(1);
			match(FORESLASH);
#line 204 "TerrainLibrary.g"
			p += f->getText();
#line 582 "TerrainLibraryParser.cpp"
			break;
		}
		case NBSP:
		{
			match(NBSP);
#line 205 "TerrainLibrary.g"
			p += " ";
#line 590 "TerrainLibraryParser.cpp"
			break;
		}
		default:
		{
			if ( _cnt26>=1 ) { goto _loop26; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		}
		_cnt26++;
	}
	_loop26:;
	}  // ( ... )+
	return p;
}

std::string  TerrainLibraryParser::unixPath() {
#line 210 "TerrainLibrary.g"
	std::string p;
#line 608 "TerrainLibraryParser.cpp"
	
	p=path();
#line 212 "TerrainLibrary.g"
	// Transliterate all '\'s into '/'s
	std::transform(p.begin(), p.end(), p.begin(), DOSToUNIX());
	
#line 615 "TerrainLibraryParser.cpp"
	return p;
}

scalar_t  TerrainLibraryParser::scalar() {
#line 248 "TerrainLibrary.g"
	scalar_t s;
#line 622 "TerrainLibraryParser.cpp"
	antlr::RefToken  sg = antlr::nullToken;
	
	{
	switch ( LA(1)) {
	case SIGN:
	{
		sg = LT(1);
		match(SIGN);
		break;
	}
	case NUMBER:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	s=fraction();
#line 250 "TerrainLibrary.g"
	
	// If we have a sign token and it is negative, then negate 's'
	if (sg != NULL && sg->getText() == "-")
	s *= -1;
	
#line 650 "TerrainLibraryParser.cpp"
	return s;
}

int  TerrainLibraryParser::integer() {
#line 278 "TerrainLibrary.g"
	int value;
#line 657 "TerrainLibraryParser.cpp"
	antlr::RefToken  n = antlr::nullToken;
	
	n = LT(1);
	match(NUMBER);
#line 278 "TerrainLibrary.g"
	value = atoi(n->getText().c_str());
#line 664 "TerrainLibraryParser.cpp"
	return value;
}

void TerrainLibraryParser::initializeASTFactory( antlr::ASTFactory& )
{
}
const char* TerrainLibraryParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"LEFT_SBRACKET",
	"\"terraintype\"",
	"COLON",
	"RIGHT_SBRACKET",
	"end of line",
	"\"terrainseam\"",
	"AMPERSAND",
	"\"color\"",
	"ASSIGN",
	"\"sample\"",
	"\"smoothness\"",
	"\"aspect\"",
	"\"ratio\"",
	"ANTLR_DUMMY_TOKEN",
	"quoted string",
	"id",
	"number",
	"NBSP",
	"DOT",
	"BACKSLASH",
	"FORESLASH",
	"\"e\"",
	"SIGN",
	"PERCENT",
	"LEFT_ABRACKET",
	"COMMA",
	"RIGHT_ABRACKET",
	"DIGIT",
	"LETTER",
	"WS_CHAR",
	"EOL_CHAR",
	"DQUOTE",
	"SQUOTE",
	"SPACE",
	"comment",
	"WS",
	0
};



ANTLR_END_NAMESPACE
