/* $ANTLR 2.7.5 (20050201): "Map.g" -> "MapParser.cpp"$ */
#include "MapParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
ANTLR_BEGIN_NAMESPACE(terrainosaurus)
#line 1 "Map.g"
#line 8 "MapParser.cpp"
MapParser::MapParser(antlr::TokenBuffer& tokenBuf, int k)
: antlr::LLkParser(tokenBuf,k)
{
}

MapParser::MapParser(antlr::TokenBuffer& tokenBuf)
: antlr::LLkParser(tokenBuf,2)
{
}

MapParser::MapParser(antlr::TokenStream& lexer, int k)
: antlr::LLkParser(lexer,k)
{
}

MapParser::MapParser(antlr::TokenStream& lexer)
: antlr::LLkParser(lexer,2)
{
}

MapParser::MapParser(const antlr::ParserSharedInputState& state)
: antlr::LLkParser(state,2)
{
}

void MapParser::terrainMap(
	Map * m
) {
	
#line 82 "Map.g"
	map = m;
#line 40 "MapParser.cpp"
	if (!( map != NULL ))
		throw antlr::SemanticException(" map != NULL ");
#line 84 "Map.g"
	currentTT = map->terrainLibrary->terrainType(0);
#line 45 "MapParser.cpp"
	{ // ( ... )*
	for (;;) {
		switch ( LA(1)) {
		case VERTEX:
		{
			vertexRecord();
			break;
		}
		case FACE:
		{
			faceRecord();
			break;
		}
		case TERRAIN_TYPE:
		{
			terrainTypeRecord();
			break;
		}
		case EOL:
		{
			blankLine();
			break;
		}
		default:
		{
			goto _loop3;
		}
		}
	}
	_loop3:;
	} // ( ... )*
	match(antlr::Token::EOF_TYPE);
}

void MapParser::vertexRecord() {
	antlr::RefToken  t = antlr::nullToken;
	
#line 92 "Map.g"
	scalar_t x, y;
#line 85 "MapParser.cpp"
	t = LT(1);
	match(VERTEX);
	x=scalar();
	y=scalar();
	match(EOL);
#line 94 "Map.g"
	createVertex(x, y, t);
#line 93 "MapParser.cpp"
}

void MapParser::faceRecord() {
	antlr::RefToken  f = antlr::nullToken;
	
#line 98 "Map.g"
	IDType id;
#line 101 "MapParser.cpp"
	f = LT(1);
	match(FACE);
#line 99 "Map.g"
	beginFace(f);
#line 106 "MapParser.cpp"
	{ // ( ... )+
	int _cnt7=0;
	for (;;) {
		if ((LA(1) == NUMBER)) {
			id=integer();
#line 99 "Map.g"
			addVertex(id, f);
#line 114 "MapParser.cpp"
		}
		else {
			if ( _cnt7>=1 ) { goto _loop7; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt7++;
	}
	_loop7:;
	}  // ( ... )+
	match(EOL);
#line 100 "Map.g"
	endFace();
#line 127 "MapParser.cpp"
}

void MapParser::terrainTypeRecord() {
	antlr::RefToken  id = antlr::nullToken;
	
	match(TERRAIN_TYPE);
	id = LT(1);
	match(NAME);
	match(EOL);
#line 105 "Map.g"
	setTerrainType(id->getText(), id);
#line 139 "MapParser.cpp"
}

void MapParser::blankLine() {
	
	match(EOL);
}

scalar_t  MapParser::scalar() {
#line 115 "Map.g"
	scalar_t s;
#line 150 "MapParser.cpp"
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
#line 117 "Map.g"
	
	// If we have a sign token and it is negative, then negate 's'
	if (sg != NULL && sg->getText() == "-")
	s *= -1;
	
#line 178 "MapParser.cpp"
	return s;
}

int  MapParser::integer() {
#line 143 "Map.g"
	int value;
#line 185 "MapParser.cpp"
	antlr::RefToken  n = antlr::nullToken;
	
	n = LT(1);
	match(NUMBER);
#line 143 "Map.g"
	value = atoi(n->getText().c_str());
#line 192 "MapParser.cpp"
	return value;
}

scalar_t  MapParser::fraction() {
#line 125 "Map.g"
	scalar_t s;
#line 199 "MapParser.cpp"
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
	case SIGN:
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
#line 127 "Map.g"
	std::string tmp(w->getText());      // Construct a composite string
	if (f != NULL) {                    // If we have a fractional part,
	tmp += '.';                     // then add it in
	tmp += f->getText();
	}
	s = (scalar_t)atof(tmp.c_str());
	
#line 235 "MapParser.cpp"
	return s;
}

scalar_t  MapParser::nFraction() {
#line 137 "Map.g"
	scalar_t s;
#line 242 "MapParser.cpp"
	
	s=fraction();
	if (!( s >= 0.0 && s <= 1.0 ))
		throw antlr::SemanticException(" s >= 0.0 && s <= 1.0 ");
	return s;
}

void MapParser::initializeASTFactory( antlr::ASTFactory& )
{
}
const char* MapParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"\"v\"",
	"end of line",
	"\"f\"",
	"\"tt\"",
	"name",
	"SIGN",
	"number",
	".",
	"digit",
	"letter",
	"comment",
	"WS",
	0
};



ANTLR_END_NAMESPACE
