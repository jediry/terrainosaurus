/* $ANTLR 2.7.4: "TerrainLibrary.g" -> "TerrainLibraryParser.cpp"$ */
#include "TerrainLibraryParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
ANTLR_BEGIN_NAMESPACE(terrainosaurus)
#line 1 "TerrainLibrary.g"
#line 8 "TerrainLibraryParser.cpp"
TerrainLibraryParser::TerrainLibraryParser(antlr::TokenBuffer& tokenBuf, int k)
: antlr::LLkParser(tokenBuf,k)
{
}

TerrainLibraryParser::TerrainLibraryParser(antlr::TokenBuffer& tokenBuf)
: antlr::LLkParser(tokenBuf,2)
{
}

TerrainLibraryParser::TerrainLibraryParser(antlr::TokenStream& lexer, int k)
: antlr::LLkParser(lexer,k)
{
}

TerrainLibraryParser::TerrainLibraryParser(antlr::TokenStream& lexer)
: antlr::LLkParser(lexer,2)
{
}

TerrainLibraryParser::TerrainLibraryParser(const antlr::ParserSharedInputState& state)
: antlr::LLkParser(state,2)
{
}

void TerrainLibraryParser::recordList(
	TerrainLibrary * lib
) {
	
#line 127 "TerrainLibrary.g"
	library = lib;
#line 40 "TerrainLibraryParser.cpp"
	if (!( library != NULL ))
		throw antlr::SemanticException(" library != NULL ");
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
		if ((LA(1) == OPEN_SBRACKET) && (LA(2) == TERRAIN_TYPE)) {
			terrainTypeRecord();
		}
		else if ((LA(1) == OPEN_SBRACKET) && (LA(2) == TERRAIN_SEAM)) {
			terrainSeamRecord();
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

void TerrainLibraryParser::terrainTypeRecord() {
	
	terrainTypeDeclaration();
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
}

void TerrainLibraryParser::terrainSeamRecord() {
	
	terrainSeamDeclaration();
	{ // ( ... )*
	for (;;) {
		switch ( LA(1)) {
		case EOL:
		{
			blankLine();
			break;
		}
		case NUMBER:
		{
			numChromosomes();
			break;
		}
		case SMOOTHNESS:
		{
			smoothness();
			break;
		}
		case MUTATION:
		{
			mutationRatio();
			break;
		}
		case CROSSOVER:
		{
			crossoverRatio();
			break;
		}
		case SELECTION:
		{
			selectionRatio();
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
}

void TerrainLibraryParser::terrainTypeDeclaration() {
	antlr::RefToken  n = antlr::nullToken;
	
	match(OPEN_SBRACKET);
	match(TERRAIN_TYPE);
	match(COLON);
	n = LT(1);
	match(NAME);
	match(CLOSE_SBRACKET);
	match(EOL);
#line 150 "TerrainLibrary.g"
	createTerrainType(n);
#line 172 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::terrainColor() {
	antlr::RefToken  t = antlr::nullToken;
#line 165 "TerrainLibrary.g"
	Color c;
#line 179 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(COLOR);
	match(ASSIGN);
	c=color();
	match(EOL);
#line 167 "TerrainLibrary.g"
	setColorProperty(TTColor, c, t->getLine());
#line 188 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::terrainSample() {
	antlr::RefToken  t = antlr::nullToken;
	
#line 170 "TerrainLibrary.g"
	string s;
#line 196 "TerrainLibraryParser.cpp"
	t = LT(1);
	match(SAMPLE);
	match(ASSIGN);
	s=filename();
	match(EOL);
#line 172 "TerrainLibrary.g"
	addTerrainSample(s, t->getLine());
#line 204 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::terrainSeamDeclaration() {
	antlr::RefToken  n1 = antlr::nullToken;
	antlr::RefToken  n2 = antlr::nullToken;
	
	match(OPEN_SBRACKET);
	match(TERRAIN_SEAM);
	match(COLON);
	n1 = LT(1);
	match(NAME);
	match(AND);
	n2 = LT(1);
	match(NAME);
	match(CLOSE_SBRACKET);
	match(EOL);
#line 155 "TerrainLibrary.g"
	createTerrainSeam(n1, n2);
#line 223 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::numChromosomes() {
	antlr::RefToken  t = antlr::nullToken;
#line 179 "TerrainLibrary.g"
	int n;
#line 230 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(NUMBER);
	match(OF);
	match(CHROMOSOMES);
	match(ASSIGN);
	n=integer();
	match(EOL);
#line 181 "TerrainLibrary.g"
	setIntegerProperty(TSNumChromosomes, n, t->getLine());
#line 241 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::smoothness() {
	antlr::RefToken  t = antlr::nullToken;
#line 185 "TerrainLibrary.g"
	scalar_t n;
#line 248 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(SMOOTHNESS);
	match(ASSIGN);
	n=nFraction();
	match(EOL);
#line 187 "TerrainLibrary.g"
	setScalarProperty(TSSmoothness, n, t->getLine());
#line 257 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::mutationRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 191 "TerrainLibrary.g"
	scalar_t n;
#line 264 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(MUTATION);
	match(RATIO);
	match(ASSIGN);
	n=nFraction();
	match(EOL);
#line 193 "TerrainLibrary.g"
	setScalarProperty(TSMutationRatio, n, t->getLine());
#line 274 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::crossoverRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 197 "TerrainLibrary.g"
	scalar_t n;
#line 281 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(CROSSOVER);
	match(RATIO);
	match(ASSIGN);
	n=nFraction();
	match(EOL);
#line 199 "TerrainLibrary.g"
	setScalarProperty(TSCrossoverRatio, n, t->getLine());
#line 291 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::selectionRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 203 "TerrainLibrary.g"
	scalar_t n;
#line 298 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(SELECTION);
	match(RATIO);
	match(ASSIGN);
	n=nFraction();
	match(EOL);
#line 205 "TerrainLibrary.g"
	setScalarProperty(TSSelectionRatio, n, t->getLine());
#line 308 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::aspectRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 209 "TerrainLibrary.g"
	scalar_t n;
#line 315 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(ASPECT);
	match(RATIO);
	match(ASSIGN);
	n=fraction();
	match(EOL);
#line 211 "TerrainLibrary.g"
	setScalarProperty(TSAspectRatio, n, t->getLine());
#line 325 "TerrainLibraryParser.cpp"
}

Color  TerrainLibraryParser::color() {
#line 246 "TerrainLibrary.g"
	Color c;
#line 331 "TerrainLibraryParser.cpp"
	
#line 247 "TerrainLibrary.g"
	scalar_t r, g, b, a;
#line 335 "TerrainLibraryParser.cpp"
	match(OPEN_ABRACKET);
	r=nFraction();
	match(COMMA);
	g=nFraction();
	match(COMMA);
	b=nFraction();
	match(COMMA);
	a=nFraction();
	match(CLOSE_ABRACKET);
#line 253 "TerrainLibrary.g"
	typedef Color::scalar_t c_scalar_t;
	c = Color(c_scalar_t(r), c_scalar_t(g),
	c_scalar_t(b), c_scalar_t(a));
#line 349 "TerrainLibraryParser.cpp"
	return c;
}

string  TerrainLibraryParser::filename() {
#line 262 "TerrainLibrary.g"
	string path;
#line 356 "TerrainLibraryParser.cpp"
	antlr::RefToken  q = antlr::nullToken;
	antlr::RefToken  n = antlr::nullToken;
	antlr::RefToken  i = antlr::nullToken;
	antlr::RefToken  c = antlr::nullToken;
	antlr::RefToken  d = antlr::nullToken;
	
	{ // ( ... )+
	int _cnt33=0;
	for (;;) {
		switch ( LA(1)) {
		case QUOTED_STRING:
		{
			q = LT(1);
			match(QUOTED_STRING);
#line 263 "TerrainLibrary.g"
			path += q->getText();
#line 373 "TerrainLibraryParser.cpp"
			break;
		}
		case NAME:
		{
			n = LT(1);
			match(NAME);
#line 264 "TerrainLibrary.g"
			path += n->getText();
#line 382 "TerrainLibraryParser.cpp"
			break;
		}
		case NUMBER:
		{
			i = LT(1);
			match(NUMBER);
#line 265 "TerrainLibrary.g"
			path += i->getText();
#line 391 "TerrainLibraryParser.cpp"
			break;
		}
		case COLON:
		{
			c = LT(1);
			match(COLON);
#line 266 "TerrainLibrary.g"
			path += c->getText();
#line 400 "TerrainLibraryParser.cpp"
			break;
		}
		case DOT:
		{
			d = LT(1);
			match(DOT);
#line 267 "TerrainLibrary.g"
			path += d->getText();
#line 409 "TerrainLibraryParser.cpp"
			break;
		}
		case BACKSLASH:
		case FORESLASH:
		{
			{
			switch ( LA(1)) {
			case BACKSLASH:
			{
				match(BACKSLASH);
				break;
			}
			case FORESLASH:
			{
				match(FORESLASH);
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 268 "TerrainLibrary.g"
			path += "/";
#line 435 "TerrainLibraryParser.cpp"
			break;
		}
		case NBSP:
		{
			match(NBSP);
#line 269 "TerrainLibrary.g"
			path += " ";
#line 443 "TerrainLibraryParser.cpp"
			break;
		}
		default:
		{
			if ( _cnt33>=1 ) { goto _loop33; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		}
		_cnt33++;
	}
	_loop33:;
	}  // ( ... )+
	return path;
}

int  TerrainLibraryParser::integer() {
#line 258 "TerrainLibrary.g"
	int value;
#line 461 "TerrainLibraryParser.cpp"
	antlr::RefToken  n = antlr::nullToken;
	
	n = LT(1);
	match(NUMBER);
#line 258 "TerrainLibrary.g"
	value = atoi(n->getText().c_str());
#line 468 "TerrainLibraryParser.cpp"
	return value;
}

scalar_t  TerrainLibraryParser::nFraction() {
#line 240 "TerrainLibrary.g"
	scalar_t s;
#line 475 "TerrainLibraryParser.cpp"
	
	s=fraction();
	if (!( s >= 0.0 && s <= 1.0 ))
		throw antlr::SemanticException(" s >= 0.0 && s <= 1.0 ");
	return s;
}

scalar_t  TerrainLibraryParser::fraction() {
#line 228 "TerrainLibrary.g"
	scalar_t s;
#line 486 "TerrainLibraryParser.cpp"
	antlr::RefToken  w = antlr::nullToken;
	antlr::RefToken  f = antlr::nullToken;
	
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
	case COMMA:
	case CLOSE_ABRACKET:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 230 "TerrainLibrary.g"
	string tmp(w->getText());           // Construct a composite string
	if (f != NULL) {                    // If we have a fractional part,
	tmp += '.';                     // then add it in
	tmp += f->getText();
	}
	s = scalar_t(atof(tmp.c_str()));
	
#line 522 "TerrainLibraryParser.cpp"
	return s;
}

scalar_t  TerrainLibraryParser::scalar() {
#line 218 "TerrainLibrary.g"
	scalar_t s;
#line 529 "TerrainLibraryParser.cpp"
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
#line 220 "TerrainLibrary.g"
	
	// If we have a sign token and it is negative, then negate 's'
	if (sg != NULL && sg->getText() == "-")
	s *= -1;
	
#line 557 "TerrainLibraryParser.cpp"
	return s;
}

void TerrainLibraryParser::initializeASTFactory( antlr::ASTFactory& )
{
}
const char* TerrainLibraryParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"OPEN_SBRACKET",
	"\"terraintype\"",
	"COLON",
	"name",
	"CLOSE_SBRACKET",
	"end of line",
	"\"terrainseam\"",
	"AND",
	"\"color\"",
	"ASSIGN",
	"\"sample\"",
	"\"number\"",
	"\"of\"",
	"\"chromosomes\"",
	"\"smoothness\"",
	"\"mutation\"",
	"\"ratio\"",
	"\"crossover\"",
	"\"selection\"",
	"\"aspect\"",
	"SIGN",
	"DOT",
	"OPEN_ABRACKET",
	"COMMA",
	"CLOSE_ABRACKET",
	"quoted string",
	"BACKSLASH",
	"FORESLASH",
	"NBSP",
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
