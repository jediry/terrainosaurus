#ifndef INC_MapTokenTypes_hpp_
#define INC_MapTokenTypes_hpp_

ANTLR_BEGIN_NAMESPACE(terrainosaurus)
/* $ANTLR 2.7.5 (20050613): "Map.g" -> "MapTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API MapTokenTypes {
#endif
	enum {
		EOF_ = 1,
		VERTEX = 4,
		EOL = 5,
		FACE = 6,
		TERRAIN_TYPE = 7,
		NAME = 8,
		NUMBER = 9,
		DOT = 10,
		LITERAL_e = 11,
		SIGN = 12,
		PERCENT = 13,
		DIGIT = 14,
		LETTER = 15,
		WS_CHAR = 16,
		EOL_CHAR = 17,
		DQUOTE = 18,
		SQUOTE = 19,
		SPACE = 20,
		COMMENT = 21,
		WS = 22,
		LEFT_ABRACKET = 23,
		RIGHT_ABRACKET = 24,
		LEFT_SBRACKET = 25,
		RIGHT_SBRACKET = 26,
		COMMA = 27,
		ASSIGN = 28,
		COLON = 29,
		AMPERSAND = 30,
		BACKSLASH = 31,
		FORESLASH = 32,
		NBSP = 33,
		QUOTED_STRING = 34,
		ANTLR_DUMMY_TOKEN = 35,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
#endif /*INC_MapTokenTypes_hpp_*/
