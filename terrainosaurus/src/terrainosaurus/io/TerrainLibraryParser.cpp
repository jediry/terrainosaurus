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
	
#line 143 "TerrainLibrary.g"
	library = lib;
#line 40 "TerrainLibraryParser.cpp"
#line 144 "TerrainLibrary.g"
	inGlobal = false;
#line 43 "TerrainLibraryParser.cpp"
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
		else if ((LA(1) == OPEN_SBRACKET) && (LA(2) == GLOBAL)) {
			globalSectionRecord();
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
			goto _loop11;
		}
		}
	}
	_loop11:;
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
		case POPULATION:
		{
			populationSize();
			break;
		}
		case EVOLUTION:
		{
			evolutionCycles();
			break;
		}
		case SELECTION:
		{
			selectionRatio();
			break;
		}
		case CROSSOVER:
		{
			crossoverRatio();
			break;
		}
		case MUTATION:
		{
			mutationRatio();
			break;
		}
		default:
		{
			goto _loop14;
		}
		}
	}
	_loop14:;
	} // ( ... )*
}

void TerrainLibraryParser::globalSectionRecord() {
	
	globalSectionDeclaration();
	{ // ( ... )*
	for (;;) {
		switch ( LA(1)) {
		case EOL:
		{
			blankLine();
			break;
		}
		case POPULATION:
		{
			populationSize();
			break;
		}
		case EVOLUTION:
		{
			evolutionCycles();
			break;
		}
		case SELECTION:
		{
			selectionRatio();
			break;
		}
		case ELITE:
		{
			eliteRatio();
			break;
		}
		default:
			if ((LA(1) == MUTATION) && (LA(2) == PROBABILITY)) {
				mutationProbability();
			}
			else if ((LA(1) == MUTATION) && (LA(2) == RATIO)) {
				mutationRatio();
			}
			else if ((LA(1) == CROSSOVER) && (LA(2) == PROBABILITY)) {
				crossoverProbability();
			}
			else if ((LA(1) == CROSSOVER) && (LA(2) == RATIO)) {
				crossoverRatio();
			}
			else if ((LA(1) == MAX) && (LA(2) == CROSSOVER)) {
				maxCrossoverWidth();
			}
			else if ((LA(1) == MAX) && (LA(2) == JITTER)) {
				maxJitterPixels();
			}
			else if ((LA(1) == MAX) && (LA(2) == SCALE)) {
				maxScaleFactor();
			}
			else if ((LA(1) == MAX) && (LA(2) == OFFSET)) {
				maxOffsetAmount();
			}
		else {
			goto _loop8;
		}
		}
	}
	_loop8:;
	} // ( ... )*
}

void TerrainLibraryParser::globalSectionDeclaration() {
	antlr::RefToken  n = antlr::nullToken;
	
	n = LT(1);
	match(OPEN_SBRACKET);
	match(GLOBAL);
	match(CLOSE_SBRACKET);
	match(EOL);
#line 186 "TerrainLibrary.g"
	beginGlobalSection(n);
#line 246 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::populationSize() {
	antlr::RefToken  t = antlr::nullToken;
#line 206 "TerrainLibrary.g"
	int n;
#line 253 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(POPULATION);
	match(SIZE);
	match(ASSIGN);
	n=integer();
	match(EOL);
#line 208 "TerrainLibrary.g"
	setIntegerProperty(GAPopulationSize, n, t->getLine());
#line 263 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::evolutionCycles() {
	antlr::RefToken  t = antlr::nullToken;
#line 211 "TerrainLibrary.g"
	int n;
#line 270 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(EVOLUTION);
	match(CYCLES);
	match(ASSIGN);
	n=integer();
	match(EOL);
#line 213 "TerrainLibrary.g"
	setIntegerProperty(GAEvolutionCycles, n, t->getLine());
#line 280 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::selectionRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 216 "TerrainLibrary.g"
	scalar_t r;
#line 287 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(SELECTION);
	match(RATIO);
	match(ASSIGN);
	r=fraction();
	match(EOL);
#line 218 "TerrainLibrary.g"
	setScalarProperty(GASelectionRatio, r, t->getLine());
#line 297 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::eliteRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 221 "TerrainLibrary.g"
	scalar_t r;
#line 304 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(ELITE);
	match(RATIO);
	match(ASSIGN);
	r=fraction();
	match(EOL);
#line 223 "TerrainLibrary.g"
	setScalarProperty(GAEliteRatio, r, t->getLine());
#line 314 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::mutationProbability() {
	antlr::RefToken  t = antlr::nullToken;
#line 236 "TerrainLibrary.g"
	scalar_t p;
#line 321 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(MUTATION);
	match(PROBABILITY);
	match(ASSIGN);
	p=fraction();
	match(EOL);
#line 238 "TerrainLibrary.g"
	setScalarProperty(GAMutationProbability, p, t->getLine());
#line 331 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::mutationRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 241 "TerrainLibrary.g"
	scalar_t r;
#line 338 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(MUTATION);
	match(RATIO);
	match(ASSIGN);
	r=fraction();
	match(EOL);
#line 243 "TerrainLibrary.g"
	setScalarProperty(GAMutationRatio, r, t->getLine());
#line 348 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::crossoverProbability() {
	antlr::RefToken  t = antlr::nullToken;
#line 226 "TerrainLibrary.g"
	scalar_t p;
#line 355 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(CROSSOVER);
	match(PROBABILITY);
	match(ASSIGN);
	p=fraction();
	match(EOL);
#line 228 "TerrainLibrary.g"
	setScalarProperty(GACrossoverProbability, p, t->getLine());
#line 365 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::crossoverRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 231 "TerrainLibrary.g"
	scalar_t r;
#line 372 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(CROSSOVER);
	match(RATIO);
	match(ASSIGN);
	r=fraction();
	match(EOL);
#line 233 "TerrainLibrary.g"
	setScalarProperty(GACrossoverRatio, r, t->getLine());
#line 382 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::maxCrossoverWidth() {
	antlr::RefToken  t = antlr::nullToken;
#line 250 "TerrainLibrary.g"
	int n;
#line 389 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(MAX);
	match(CROSSOVER);
	match(WIDTH);
	match(ASSIGN);
	n=integer();
	match(EOL);
#line 252 "TerrainLibrary.g"
	setIntegerProperty(HFMaxCrossoverWidth, n, t->getLine());
#line 400 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::maxJitterPixels() {
	antlr::RefToken  t = antlr::nullToken;
#line 255 "TerrainLibrary.g"
	int n;
#line 407 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(MAX);
	match(JITTER);
	match(PIXELS);
	match(ASSIGN);
	n=integer();
	match(EOL);
#line 257 "TerrainLibrary.g"
	setIntegerProperty(HFMaxJitterPixels, n, t->getLine());
#line 418 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::maxScaleFactor() {
	antlr::RefToken  t = antlr::nullToken;
#line 260 "TerrainLibrary.g"
	scalar_t f;
#line 425 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(MAX);
	match(SCALE);
	match(FACTOR);
	match(ASSIGN);
	f=scalar();
	match(EOL);
#line 262 "TerrainLibrary.g"
	setScalarProperty(HFMaxScaleFactor, f, t->getLine());
#line 436 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::maxOffsetAmount() {
	antlr::RefToken  t = antlr::nullToken;
#line 265 "TerrainLibrary.g"
	scalar_t a;
#line 443 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(MAX);
	match(OFFSET);
	match(AMOUNT);
	match(ASSIGN);
	a=scalar();
	match(EOL);
#line 267 "TerrainLibrary.g"
	setScalarProperty(HFMaxOffsetAmount, a, t->getLine());
#line 454 "TerrainLibraryParser.cpp"
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
#line 191 "TerrainLibrary.g"
	createTerrainType(n);
#line 469 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::terrainColor() {
	antlr::RefToken  t = antlr::nullToken;
#line 274 "TerrainLibrary.g"
	Color c;
#line 476 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(COLOR);
	match(ASSIGN);
	c=color();
	match(EOL);
#line 276 "TerrainLibrary.g"
	setColorProperty(TTColor, c, t->getLine());
#line 485 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::terrainSample() {
	antlr::RefToken  t = antlr::nullToken;
	
#line 279 "TerrainLibrary.g"
	string s;
#line 493 "TerrainLibraryParser.cpp"
	t = LT(1);
	match(SAMPLE);
	match(ASSIGN);
	s=filename();
	match(EOL);
#line 281 "TerrainLibrary.g"
	addTerrainSample(s, t->getLine());
#line 501 "TerrainLibraryParser.cpp"
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
#line 196 "TerrainLibrary.g"
	createTerrainSeam(n1, n2);
#line 520 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::smoothness() {
	antlr::RefToken  t = antlr::nullToken;
#line 288 "TerrainLibrary.g"
	scalar_t n;
#line 527 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(SMOOTHNESS);
	match(ASSIGN);
	n=nFraction();
	match(EOL);
#line 290 "TerrainLibrary.g"
	setScalarProperty(TSSmoothness, n, t->getLine());
#line 536 "TerrainLibraryParser.cpp"
}

void TerrainLibraryParser::aspectRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 293 "TerrainLibrary.g"
	scalar_t n;
#line 543 "TerrainLibraryParser.cpp"
	
	t = LT(1);
	match(ASPECT);
	match(RATIO);
	match(ASSIGN);
	n=fraction();
	match(EOL);
#line 295 "TerrainLibrary.g"
	setScalarProperty(TSAspectRatio, n, t->getLine());
#line 553 "TerrainLibraryParser.cpp"
}

int  TerrainLibraryParser::integer() {
#line 342 "TerrainLibrary.g"
	int value;
#line 559 "TerrainLibraryParser.cpp"
	antlr::RefToken  n = antlr::nullToken;
	
	n = LT(1);
	match(NUMBER);
#line 342 "TerrainLibrary.g"
	value = atoi(n->getText().c_str());
#line 566 "TerrainLibraryParser.cpp"
	return value;
}

scalar_t  TerrainLibraryParser::fraction() {
#line 312 "TerrainLibrary.g"
	scalar_t s;
#line 573 "TerrainLibraryParser.cpp"
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
#line 314 "TerrainLibrary.g"
	string tmp(w->getText());           // Construct a composite string
	if (f != NULL) {                    // If we have a fractional part,
	tmp += '.';                     // then add it in
	tmp += f->getText();
	}
	s = scalar_t(atof(tmp.c_str()));
	
#line 608 "TerrainLibraryParser.cpp"
	return s;
}

scalar_t  TerrainLibraryParser::scalar() {
#line 302 "TerrainLibrary.g"
	scalar_t s;
#line 615 "TerrainLibraryParser.cpp"
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
#line 304 "TerrainLibrary.g"
	
	// If we have a sign token and it is negative, then negate 's'
	if (sg != NULL && sg->getText() == "-")
	s *= -1;
	
#line 643 "TerrainLibraryParser.cpp"
	return s;
}

Color  TerrainLibraryParser::color() {
#line 330 "TerrainLibrary.g"
	Color c;
#line 650 "TerrainLibraryParser.cpp"
	
#line 331 "TerrainLibrary.g"
	scalar_t r, g, b, a;
#line 654 "TerrainLibraryParser.cpp"
	match(OPEN_ABRACKET);
	r=nFraction();
	match(COMMA);
	g=nFraction();
	match(COMMA);
	b=nFraction();
	match(COMMA);
	a=nFraction();
	match(CLOSE_ABRACKET);
#line 337 "TerrainLibrary.g"
	typedef Color::scalar_t c_scalar_t;
	c = Color(c_scalar_t(r), c_scalar_t(g),
	c_scalar_t(b), c_scalar_t(a));
#line 668 "TerrainLibraryParser.cpp"
	return c;
}

string  TerrainLibraryParser::filename() {
#line 346 "TerrainLibrary.g"
	string path;
#line 675 "TerrainLibraryParser.cpp"
	antlr::RefToken  q = antlr::nullToken;
	antlr::RefToken  n = antlr::nullToken;
	antlr::RefToken  i = antlr::nullToken;
	antlr::RefToken  c = antlr::nullToken;
	antlr::RefToken  d = antlr::nullToken;
	
	{ // ( ... )+
	int _cnt45=0;
	for (;;) {
		switch ( LA(1)) {
		case QUOTED_STRING:
		{
			q = LT(1);
			match(QUOTED_STRING);
#line 347 "TerrainLibrary.g"
			path += q->getText();
#line 692 "TerrainLibraryParser.cpp"
			break;
		}
		case NAME:
		{
			n = LT(1);
			match(NAME);
#line 348 "TerrainLibrary.g"
			path += n->getText();
#line 701 "TerrainLibraryParser.cpp"
			break;
		}
		case NUMBER:
		{
			i = LT(1);
			match(NUMBER);
#line 349 "TerrainLibrary.g"
			path += i->getText();
#line 710 "TerrainLibraryParser.cpp"
			break;
		}
		case COLON:
		{
			c = LT(1);
			match(COLON);
#line 350 "TerrainLibrary.g"
			path += c->getText();
#line 719 "TerrainLibraryParser.cpp"
			break;
		}
		case DOT:
		{
			d = LT(1);
			match(DOT);
#line 351 "TerrainLibrary.g"
			path += d->getText();
#line 728 "TerrainLibraryParser.cpp"
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
#line 352 "TerrainLibrary.g"
			path += "/";
#line 754 "TerrainLibraryParser.cpp"
			break;
		}
		case NBSP:
		{
			match(NBSP);
#line 353 "TerrainLibrary.g"
			path += " ";
#line 762 "TerrainLibraryParser.cpp"
			break;
		}
		default:
		{
			if ( _cnt45>=1 ) { goto _loop45; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		}
		_cnt45++;
	}
	_loop45:;
	}  // ( ... )+
	return path;
}

scalar_t  TerrainLibraryParser::nFraction() {
#line 324 "TerrainLibrary.g"
	scalar_t s;
#line 780 "TerrainLibraryParser.cpp"
	
	s=fraction();
	if (!( s >= 0.0 && s <= 1.0 ))
		throw antlr::SemanticException(" s >= 0.0 && s <= 1.0 ");
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
	"\"global\"",
	"CLOSE_SBRACKET",
	"end of line",
	"\"terraintype\"",
	"COLON",
	"name",
	"\"terrainseam\"",
	"AND",
	"\"population\"",
	"\"size\"",
	"ASSIGN",
	"\"evolution\"",
	"\"cycles\"",
	"\"selection\"",
	"\"ratio\"",
	"\"elite\"",
	"\"crossover\"",
	"\"probability\"",
	"\"mutation\"",
	"\"max\"",
	"\"width\"",
	"\"jitter\"",
	"\"pixels\"",
	"\"scale\"",
	"\"factor\"",
	"\"offset\"",
	"\"amount\"",
	"\"color\"",
	"\"sample\"",
	"\"smoothness\"",
	"\"aspect\"",
	"SIGN",
	"number",
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
