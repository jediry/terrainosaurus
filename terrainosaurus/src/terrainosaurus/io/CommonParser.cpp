/* $ANTLR 2.7.5 (20050613): "CommonGrammar.g" -> "CommonParser.cpp"$ */
#include "CommonParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
ANTLR_BEGIN_NAMESPACE(terrainosaurus)
#line 1 "CommonGrammar.g"
#line 8 "CommonParser.cpp"
CommonParser::CommonParser(antlr::TokenBuffer& tokenBuf, int k)
: antlr::LLkParser(tokenBuf,k)
{
}

CommonParser::CommonParser(antlr::TokenBuffer& tokenBuf)
: antlr::LLkParser(tokenBuf,2)
{
}

CommonParser::CommonParser(antlr::TokenStream& lexer, int k)
: antlr::LLkParser(lexer,k)
{
}

CommonParser::CommonParser(antlr::TokenStream& lexer)
: antlr::LLkParser(lexer,2)
{
}

CommonParser::CommonParser(const antlr::ParserSharedInputState& state)
: antlr::LLkParser(state,2)
{
}

std::string  CommonParser::string() {
#line 64 "CommonGrammar.g"
	std::string s;
#line 37 "CommonParser.cpp"
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
#line 65 "CommonGrammar.g"
		std::string tmp = q->getText();
		s += tmp.substr(1, tmp.length() - 2);
#line 53 "CommonParser.cpp"
		break;
	}
	case ID:
	case NUMBER:
	{
		{ // ( ... )+
		int _cnt7=0;
		for (;;) {
			switch ( LA(1)) {
			case ID:
			{
				i1 = LT(1);
				match(ID);
#line 67 "CommonGrammar.g"
				s += i1->getText();
#line 69 "CommonParser.cpp"
				break;
			}
			case NUMBER:
			{
				n1 = LT(1);
				match(NUMBER);
#line 68 "CommonGrammar.g"
				s += n1->getText();
#line 78 "CommonParser.cpp"
				{ // ( ... )*
				for (;;) {
					if ((LA(1) == NBSP)) {
						match(NBSP);
#line 69 "CommonGrammar.g"
						s += " ";
#line 85 "CommonParser.cpp"
						{
						switch ( LA(1)) {
						case ID:
						{
							i2 = LT(1);
							match(ID);
#line 70 "CommonGrammar.g"
							s += i2->getText();
#line 94 "CommonParser.cpp"
							break;
						}
						case NUMBER:
						{
							n2 = LT(1);
							match(NUMBER);
#line 71 "CommonGrammar.g"
							s += n2->getText();
#line 103 "CommonParser.cpp"
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
						goto _loop6;
					}
					
				}
				_loop6:;
				} // ( ... )*
				break;
			}
			default:
			{
				if ( _cnt7>=1 ) { goto _loop7; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt7++;
		}
		_loop7:;
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

std::string  CommonParser::path() {
#line 79 "CommonGrammar.g"
	std::string p;
#line 145 "CommonParser.cpp"
	antlr::RefToken  s = antlr::nullToken;
	antlr::RefToken  i = antlr::nullToken;
	antlr::RefToken  n = antlr::nullToken;
	antlr::RefToken  c = antlr::nullToken;
	antlr::RefToken  d = antlr::nullToken;
	antlr::RefToken  b = antlr::nullToken;
	antlr::RefToken  f = antlr::nullToken;
	
	{ // ( ... )+
	int _cnt10=0;
	for (;;) {
		switch ( LA(1)) {
		case QUOTED_STRING:
		{
			s = LT(1);
			match(QUOTED_STRING);
#line 80 "CommonGrammar.g"
			p += s->getText();
#line 164 "CommonParser.cpp"
			break;
		}
		case ID:
		{
			i = LT(1);
			match(ID);
#line 81 "CommonGrammar.g"
			p += i->getText();
#line 173 "CommonParser.cpp"
			break;
		}
		case NUMBER:
		{
			n = LT(1);
			match(NUMBER);
#line 82 "CommonGrammar.g"
			p += n->getText();
#line 182 "CommonParser.cpp"
			break;
		}
		case COLON:
		{
			c = LT(1);
			match(COLON);
#line 83 "CommonGrammar.g"
			p += c->getText();
#line 191 "CommonParser.cpp"
			break;
		}
		case DOT:
		{
			d = LT(1);
			match(DOT);
#line 84 "CommonGrammar.g"
			p += d->getText();
#line 200 "CommonParser.cpp"
			break;
		}
		case BACKSLASH:
		{
			b = LT(1);
			match(BACKSLASH);
#line 85 "CommonGrammar.g"
			p += b->getText();
#line 209 "CommonParser.cpp"
			break;
		}
		case FORESLASH:
		{
			f = LT(1);
			match(FORESLASH);
#line 86 "CommonGrammar.g"
			p += f->getText();
#line 218 "CommonParser.cpp"
			break;
		}
		case NBSP:
		{
			match(NBSP);
#line 87 "CommonGrammar.g"
			p += " ";
#line 226 "CommonParser.cpp"
			break;
		}
		default:
		{
			if ( _cnt10>=1 ) { goto _loop10; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		}
		_cnt10++;
	}
	_loop10:;
	}  // ( ... )+
	return p;
}

std::string  CommonParser::unixPath() {
#line 92 "CommonGrammar.g"
	std::string p;
#line 244 "CommonParser.cpp"
	
	p=path();
#line 94 "CommonGrammar.g"
	// Transliterate all '\'s into '/'s
	std::transform(p.begin(), p.end(), p.begin(), DOSToUNIX());
	
#line 251 "CommonParser.cpp"
	return p;
}

void CommonParser::blankLine() {
	
	match(EOL);
}

scalar_t  CommonParser::fraction() {
#line 108 "CommonGrammar.g"
	scalar_t s;
#line 263 "CommonParser.cpp"
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
#line 110 "CommonGrammar.g"
	
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
	
#line 372 "CommonParser.cpp"
	return s;
}

scalar_t  CommonParser::scalar() {
#line 130 "CommonGrammar.g"
	scalar_t s;
#line 379 "CommonParser.cpp"
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
#line 132 "CommonGrammar.g"
	
	// If we have a sign token and it is negative, then negate 's'
	if (sg != NULL && sg->getText() == "-")
	s *= -1;
	
#line 407 "CommonParser.cpp"
	return s;
}

scalar_t  CommonParser::nFraction() {
#line 141 "CommonGrammar.g"
	scalar_t s;
#line 414 "CommonParser.cpp"
	
	s=fraction();
	if (!( s >= scalar_t(0) && s <= scalar_t(1) ))
		throw antlr::SemanticException(" s >= scalar_t(0) && s <= scalar_t(1) ");
	return s;
}

Color  CommonParser::color() {
#line 147 "CommonGrammar.g"
	Color c;
#line 425 "CommonParser.cpp"
	
#line 148 "CommonGrammar.g"
	scalar_t r, g, b, a;
#line 429 "CommonParser.cpp"
	match(LEFT_ABRACKET);
	r=nFraction();
	match(COMMA);
	g=nFraction();
	match(COMMA);
	b=nFraction();
	match(COMMA);
	a=nFraction();
	match(RIGHT_ABRACKET);
#line 154 "CommonGrammar.g"
	typedef Color::scalar_t c_scalar_t;
	c = Color(c_scalar_t(r), c_scalar_t(g),
	c_scalar_t(b), c_scalar_t(a));
#line 443 "CommonParser.cpp"
	return c;
}

int  CommonParser::integer() {
#line 160 "CommonGrammar.g"
	int value;
#line 450 "CommonParser.cpp"
	antlr::RefToken  n = antlr::nullToken;
	
	n = LT(1);
	match(NUMBER);
#line 160 "CommonGrammar.g"
	value = atoi(n->getText().c_str());
#line 457 "CommonParser.cpp"
	return value;
}

void CommonParser::initializeASTFactory( antlr::ASTFactory& )
{
}
const char* CommonParser::tokenNames[] = {
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
	0
};



ANTLR_END_NAMESPACE
