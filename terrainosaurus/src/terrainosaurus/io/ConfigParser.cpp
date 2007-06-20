/* $ANTLR 2.7.5 (20050613): "/expandedConfig.g" -> "ConfigParser.cpp"$ */
#include "ConfigParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
ANTLR_BEGIN_NAMESPACE(terrainosaurus)
#line 1 "/expandedConfig.g"
#line 8 "ConfigParser.cpp"
ConfigParser::ConfigParser(antlr::TokenBuffer& tokenBuf, int k)
: ::terrainosaurus::INIParser(tokenBuf,k)
{
}

ConfigParser::ConfigParser(antlr::TokenBuffer& tokenBuf)
: ::terrainosaurus::INIParser(tokenBuf,3)
{
}

ConfigParser::ConfigParser(antlr::TokenStream& lexer, int k)
: ::terrainosaurus::INIParser(lexer,k)
{
}

ConfigParser::ConfigParser(antlr::TokenStream& lexer)
: ::terrainosaurus::INIParser(lexer,3)
{
}

ConfigParser::ConfigParser(const antlr::ParserSharedInputState& state)
: ::terrainosaurus::INIParser(state,3)
{
}

void ConfigParser::sectionList(
	TerrainosaurusApplication * app
) {
#line 90 "/expandedConfig.g"
	_application = app;
#line 39 "ConfigParser.cpp"
	
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
	applicationSection();
	boundaryGASection();
	heightfieldGASection();
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LEFT_SBRACKET)) {
			erosionSection();
		}
		else {
			goto _loop5;
		}
		
	}
	_loop5:;
	} // ( ... )*
	match(antlr::Token::EOF_TYPE);
}

void ConfigParser::blankLine() {
	
	match(EOL);
}

void ConfigParser::applicationSection() {
	antlr::RefToken  t = antlr::nullToken;
	
	t = LT(1);
	match(LEFT_SBRACKET);
	match(APPLICATION);
	match(RIGHT_SBRACKET);
	match(EOL);
#line 97 "/expandedConfig.g"
	beginApplicationSection(t);
#line 86 "ConfigParser.cpp"
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
			cacheDirectory();
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
#line 100 "/expandedConfig.g"
	endSection();
#line 110 "ConfigParser.cpp"
}

void ConfigParser::boundaryGASection() {
	antlr::RefToken  t = antlr::nullToken;
	
	t = LT(1);
    INCA_DEBUG("Token type: " << tokenNames[LA(1)])
	match(LEFT_SBRACKET);
    INCA_DEBUG("Token type: " << tokenNames[LA(1)])
	match(BOUNDARY);
    INCA_DEBUG("Token type: " << tokenNames[LA(1)])
	match(GA);
    INCA_DEBUG("Token type: " << tokenNames[LA(1)])
	match(RIGHT_SBRACKET);
    INCA_DEBUG("Token type: " << tokenNames[LA(1)])
	match(EOL);
#line 103 "/expandedConfig.g"
	beginBoundaryGASection(t);
#line 124 "ConfigParser.cpp"
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
		case ELITISM:
		{
			elitismRatio();
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
		else {
			goto _loop11;
		}
		}
	}
	_loop11:;
	} // ( ... )*
#line 113 "/expandedConfig.g"
	endSection();
#line 175 "ConfigParser.cpp"
}

void ConfigParser::heightfieldGASection() {
	antlr::RefToken  t = antlr::nullToken;
	
	t = LT(1);
	match(LEFT_SBRACKET);
	match(HEIGHTFIELD);
	match(GA);
	match(RIGHT_SBRACKET);
	match(EOL);
#line 116 "/expandedConfig.g"
	beginHeightfieldGASection(t);
#line 189 "ConfigParser.cpp"
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
		case ELITISM:
		{
			elitismRatio();
			break;
		}
		case GENE:
		{
			geneSize();
			break;
		}
		case OVERLAP:
		{
			overlapFactor();
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
			else if ((LA(1) == MAX) && (LA(2) == VERTICAL) && (LA(3) == SCALE)) {
				maxVerticalScale();
			}
			else if ((LA(1) == MAX) && (LA(2) == VERTICAL) && (LA(3) == OFFSET)) {
				maxVerticalOffset();
			}
		else {
			goto _loop14;
		}
		}
	}
	_loop14:;
	} // ( ... )*
#line 131 "/expandedConfig.g"
	endSection();
#line 259 "ConfigParser.cpp"
}

void ConfigParser::erosionSection() {
	antlr::RefToken  t = antlr::nullToken;
	
	t = LT(1);
	match(LEFT_SBRACKET);
	match(EROSION);
	match(RIGHT_SBRACKET);
	match(EOL);
#line 134 "/expandedConfig.g"
	beginErosionSection(t);
#line 272 "ConfigParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == EOL)) {
			blankLine();
		}
		else {
			goto _loop17;
		}
		
	}
	_loop17:;
	} // ( ... )*
#line 136 "/expandedConfig.g"
	endSection();
#line 287 "ConfigParser.cpp"
}

void ConfigParser::cacheDirectory() {
	antlr::RefToken  t = antlr::nullToken;
#line 138 "/expandedConfig.g"
	std::string s;
#line 294 "ConfigParser.cpp"
	
	t = LT(1);
	match(POPULATION);
	match(SIZE);
	match(ASSIGN);
	s=unixPath();
	match(EOL);
#line 140 "/expandedConfig.g"
	assignStringProperty(t, CacheDirectory, s);
#line 304 "ConfigParser.cpp"
}

void ConfigParser::populationSize() {
	antlr::RefToken  t = antlr::nullToken;
#line 142 "/expandedConfig.g"
	int n;
#line 311 "ConfigParser.cpp"
	
	t = LT(1);
	match(POPULATION);
	match(SIZE);
	match(ASSIGN);
	n=integer();
	match(EOL);
#line 144 "/expandedConfig.g"
	assignIntegerProperty(t, PopulationSize, n);
#line 321 "ConfigParser.cpp"
}

void ConfigParser::evolutionCycles() {
	antlr::RefToken  t = antlr::nullToken;
#line 146 "/expandedConfig.g"
	int n;
#line 328 "ConfigParser.cpp"
	
	t = LT(1);
	match(EVOLUTION);
	match(CYCLES);
	match(ASSIGN);
	n=integer();
	match(EOL);
#line 148 "/expandedConfig.g"
	assignIntegerProperty(t, EvolutionCycles, n);
#line 338 "ConfigParser.cpp"
}

void ConfigParser::selectionRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 150 "/expandedConfig.g"
	scalar_t r;
#line 345 "ConfigParser.cpp"
	
	t = LT(1);
	match(SELECTION);
	match(RATIO);
	match(ASSIGN);
	r=fraction();
	match(EOL);
#line 152 "/expandedConfig.g"
	assignScalarProperty(t, SelectionRatio, r);
#line 355 "ConfigParser.cpp"
}

void ConfigParser::elitismRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 154 "/expandedConfig.g"
	scalar_t r;
#line 362 "ConfigParser.cpp"
	
	t = LT(1);
	match(ELITISM);
	match(RATIO);
	match(ASSIGN);
	r=fraction();
	match(EOL);
#line 156 "/expandedConfig.g"
	assignScalarProperty(t, ElitismRatio, r);
#line 372 "ConfigParser.cpp"
}

void ConfigParser::mutationProbability() {
	antlr::RefToken  t = antlr::nullToken;
#line 166 "/expandedConfig.g"
	scalar_t p;
#line 379 "ConfigParser.cpp"
	
	t = LT(1);
	match(MUTATION);
	match(PROBABILITY);
	match(ASSIGN);
	p=fraction();
	match(EOL);
#line 168 "/expandedConfig.g"
	assignScalarProperty(t, MutationProbability, p);
#line 389 "ConfigParser.cpp"
}

void ConfigParser::mutationRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 170 "/expandedConfig.g"
	scalar_t r;
#line 396 "ConfigParser.cpp"
	
	t = LT(1);
	match(MUTATION);
	match(RATIO);
	match(ASSIGN);
	r=fraction();
	match(EOL);
#line 172 "/expandedConfig.g"
	assignScalarProperty(t, MutationRatio, r);
#line 406 "ConfigParser.cpp"
}

void ConfigParser::crossoverProbability() {
	antlr::RefToken  t = antlr::nullToken;
#line 158 "/expandedConfig.g"
	scalar_t p;
#line 413 "ConfigParser.cpp"
	
	t = LT(1);
	match(CROSSOVER);
	match(PROBABILITY);
	match(ASSIGN);
	p=fraction();
	match(EOL);
#line 160 "/expandedConfig.g"
	assignScalarProperty(t, CrossoverProbability, p);
#line 423 "ConfigParser.cpp"
}

void ConfigParser::crossoverRatio() {
	antlr::RefToken  t = antlr::nullToken;
#line 162 "/expandedConfig.g"
	scalar_t r;
#line 430 "ConfigParser.cpp"
	
	t = LT(1);
	match(CROSSOVER);
	match(RATIO);
	match(ASSIGN);
	r=fraction();
	match(EOL);
#line 164 "/expandedConfig.g"
	assignScalarProperty(t, CrossoverRatio, r);
#line 440 "ConfigParser.cpp"
}

void ConfigParser::geneSize() {
	antlr::RefToken  t = antlr::nullToken;
#line 174 "/expandedConfig.g"
	int n;
#line 447 "ConfigParser.cpp"
	
	t = LT(1);
	match(GENE);
	match(SIZE);
	match(ASSIGN);
	n=integer();
	match(EOL);
#line 176 "/expandedConfig.g"
	assignIntegerProperty(t, GeneSize, n);
#line 457 "ConfigParser.cpp"
}

void ConfigParser::overlapFactor() {
	antlr::RefToken  t = antlr::nullToken;
#line 178 "/expandedConfig.g"
	scalar_t f;
#line 464 "ConfigParser.cpp"
	
	t = LT(1);
	match(OVERLAP);
	match(FACTOR);
	match(ASSIGN);
	f=fraction();
	match(EOL);
#line 180 "/expandedConfig.g"
	assignScalarProperty(t, OverlapFactor, f);
#line 474 "ConfigParser.cpp"
}

void ConfigParser::maxCrossoverWidth() {
	antlr::RefToken  t = antlr::nullToken;
#line 182 "/expandedConfig.g"
	int n;
#line 481 "ConfigParser.cpp"
	
	t = LT(1);
	match(MAX);
	match(CROSSOVER);
	match(WIDTH);
	match(ASSIGN);
	n=integer();
	match(EOL);
#line 184 "/expandedConfig.g"
	assignIntegerProperty(t, MaxCrossoverWidth, n);
#line 492 "ConfigParser.cpp"
}

void ConfigParser::maxVerticalScale() {
	antlr::RefToken  t = antlr::nullToken;
#line 186 "/expandedConfig.g"
	scalar_t s;
#line 499 "ConfigParser.cpp"
	
	t = LT(1);
	match(MAX);
	match(VERTICAL);
	match(SCALE);
	match(ASSIGN);
	s=scalar();
	match(EOL);
#line 188 "/expandedConfig.g"
	assignScalarProperty(t, MaxVerticalScale, s);
#line 510 "ConfigParser.cpp"
}

void ConfigParser::maxVerticalOffset() {
	antlr::RefToken  t = antlr::nullToken;
#line 190 "/expandedConfig.g"
	scalar_t s;
#line 517 "ConfigParser.cpp"
	
	t = LT(1);
	match(MAX);
	match(VERTICAL);
	match(OFFSET);
	match(ASSIGN);
	s=scalar();
	match(EOL);
#line 192 "/expandedConfig.g"
	assignScalarProperty(t, MaxVerticalOffset, s);
#line 528 "ConfigParser.cpp"
}

std::string  ConfigParser::unixPath() {
#line 221 "/expandedConfig.g"
	std::string p;
#line 534 "ConfigParser.cpp"
	
	p=path();
#line 222 "/expandedConfig.g"
	// Transliterate all '\'s into '/'s
	std::transform(p.begin(), p.end(), p.begin(), DOSToUNIX());
	
#line 541 "ConfigParser.cpp"
	return p;
}

int  ConfigParser::integer() {
#line 272 "/expandedConfig.g"
	int value;
#line 548 "ConfigParser.cpp"
	antlr::RefToken  n = antlr::nullToken;
	
	n = LT(1);
	match(NUMBER);
#line 272 "/expandedConfig.g"
	value = atoi(n->getText().c_str());
#line 555 "ConfigParser.cpp"
	return value;
}

scalar_t  ConfigParser::fraction() {
#line 230 "/expandedConfig.g"
	scalar_t s;
#line 562 "ConfigParser.cpp"
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
#line 231 "/expandedConfig.g"
	
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
	
#line 671 "ConfigParser.cpp"
	return s;
}

scalar_t  ConfigParser::scalar() {
#line 249 "/expandedConfig.g"
	scalar_t s;
#line 678 "ConfigParser.cpp"
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
#line 250 "/expandedConfig.g"
	
	// If we have a sign token and it is negative, then negate 's'
	if (sg != NULL && sg->getText() == "-")
	s *= -1;
	
#line 706 "ConfigParser.cpp"
	return s;
}

void ConfigParser::antlrDummyRule() {
	
	match(ANTLR_DUMMY_TOKEN);
}

std::string  ConfigParser::string() {
#line 197 "/expandedConfig.g"
	std::string s;
#line 718 "ConfigParser.cpp"
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
#line 197 "/expandedConfig.g"
		std::string tmp = q->getText();
		s += tmp.substr(1, tmp.length() - 2);
#line 734 "ConfigParser.cpp"
		break;
	}
	case ID:
	case NUMBER:
	{
		{ // ( ... )+
		int _cnt39=0;
		for (;;) {
			switch ( LA(1)) {
			case ID:
			{
				i1 = LT(1);
				match(ID);
#line 199 "/expandedConfig.g"
				s += i1->getText();
#line 750 "ConfigParser.cpp"
				break;
			}
			case NUMBER:
			{
				n1 = LT(1);
				match(NUMBER);
#line 200 "/expandedConfig.g"
				s += n1->getText();
#line 759 "ConfigParser.cpp"
				{ // ( ... )*
				for (;;) {
					if ((LA(1) == NBSP)) {
						match(NBSP);
#line 201 "/expandedConfig.g"
						s += " ";
#line 766 "ConfigParser.cpp"
						{
						switch ( LA(1)) {
						case ID:
						{
							i2 = LT(1);
							match(ID);
#line 202 "/expandedConfig.g"
							s += i2->getText();
#line 775 "ConfigParser.cpp"
							break;
						}
						case NUMBER:
						{
							n2 = LT(1);
							match(NUMBER);
#line 203 "/expandedConfig.g"
							s += n2->getText();
#line 784 "ConfigParser.cpp"
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
						goto _loop38;
					}
					
				}
				_loop38:;
				} // ( ... )*
				break;
			}
			default:
			{
				if ( _cnt39>=1 ) { goto _loop39; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt39++;
		}
		_loop39:;
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

std::string  ConfigParser::path() {
#line 210 "/expandedConfig.g"
	std::string p;
#line 826 "ConfigParser.cpp"
	antlr::RefToken  s = antlr::nullToken;
	antlr::RefToken  i = antlr::nullToken;
	antlr::RefToken  n = antlr::nullToken;
	antlr::RefToken  c = antlr::nullToken;
	antlr::RefToken  d = antlr::nullToken;
	antlr::RefToken  b = antlr::nullToken;
	antlr::RefToken  f = antlr::nullToken;
	
	{ // ( ... )+
	int _cnt42=0;
	for (;;) {
		switch ( LA(1)) {
		case QUOTED_STRING:
		{
			s = LT(1);
			match(QUOTED_STRING);
#line 210 "/expandedConfig.g"
			p += s->getText();
#line 845 "ConfigParser.cpp"
			break;
		}
		case ID:
		{
			i = LT(1);
			match(ID);
#line 211 "/expandedConfig.g"
			p += i->getText();
#line 854 "ConfigParser.cpp"
			break;
		}
		case NUMBER:
		{
			n = LT(1);
			match(NUMBER);
#line 212 "/expandedConfig.g"
			p += n->getText();
#line 863 "ConfigParser.cpp"
			break;
		}
		case COLON:
		{
			c = LT(1);
			match(COLON);
#line 213 "/expandedConfig.g"
			p += c->getText();
#line 872 "ConfigParser.cpp"
			break;
		}
		case DOT:
		{
			d = LT(1);
			match(DOT);
#line 214 "/expandedConfig.g"
			p += d->getText();
#line 881 "ConfigParser.cpp"
			break;
		}
		case BACKSLASH:
		{
			b = LT(1);
			match(BACKSLASH);
#line 215 "/expandedConfig.g"
			p += b->getText();
#line 890 "ConfigParser.cpp"
			break;
		}
		case FORESLASH:
		{
			f = LT(1);
			match(FORESLASH);
#line 216 "/expandedConfig.g"
			p += f->getText();
#line 899 "ConfigParser.cpp"
			break;
		}
		case NBSP:
		{
			match(NBSP);
#line 217 "/expandedConfig.g"
			p += " ";
#line 907 "ConfigParser.cpp"
			break;
		}
		default:
		{
			if ( _cnt42>=1 ) { goto _loop42; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		}
		_cnt42++;
	}
	_loop42:;
	}  // ( ... )+
	return p;
}

scalar_t  ConfigParser::nFraction() {
#line 257 "/expandedConfig.g"
	scalar_t s;
#line 925 "ConfigParser.cpp"
	
	s=fraction();
	if (!( s >= scalar_t(0) && s <= scalar_t(1) ))
		throw antlr::SemanticException(" s >= scalar_t(0) && s <= scalar_t(1) ");
	return s;
}

Color  ConfigParser::color() {
#line 261 "/expandedConfig.g"
	Color c;
#line 936 "ConfigParser.cpp"
	
#line 261 "/expandedConfig.g"
	scalar_t r, g, b, a;
#line 940 "ConfigParser.cpp"
	match(LEFT_ABRACKET);
	r=nFraction();
	match(COMMA);
	g=nFraction();
	match(COMMA);
	b=nFraction();
	match(COMMA);
	a=nFraction();
	match(RIGHT_ABRACKET);
#line 267 "/expandedConfig.g"
	typedef Color::scalar_t c_scalar_t;
	c = Color(c_scalar_t(r), c_scalar_t(g),
	c_scalar_t(b), c_scalar_t(a));
#line 954 "ConfigParser.cpp"
	return c;
}

void ConfigParser::initializeASTFactory( antlr::ASTFactory& )
{
}
const char* ConfigParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"quoted string",
	"ID",
	"number",
	"NBSP",
	"COLON",
	"DOT",
	"BACKSLASH",
	"FORESLASH",
	"end of line",
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
	"LEFT_SBRACKET",
	"RIGHT_SBRACKET",
	"ASSIGN",
	"AND",
	"APPLICATION",
	"BOUNDARY",
	"GA",
	"HEIGHTFIELD",
	"EROSION",
	"POPULATION",
	"SIZE",
	"EVOLUTION",
	"CYCLES",
	"SELECTION",
	"RATIO",
	"ELITISM",
	"CROSSOVER",
	"PROBABILITY",
	"MUTATION",
	"GENE",
	"OVERLAP",
	"FACTOR",
	"MAX",
	"WIDTH",
	"VERTICAL",
	"SCALE",
	"OFFSET",
	"ANTLR_DUMMY_TOKEN",
	0
};



ANTLR_END_NAMESPACE
