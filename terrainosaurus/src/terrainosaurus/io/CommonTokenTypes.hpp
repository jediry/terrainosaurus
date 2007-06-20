#ifndef INC_CommonTokenTypes_hpp_
#define INC_CommonTokenTypes_hpp_

ANTLR_BEGIN_NAMESPACE(terrainosaurus)
/* $ANTLR 2.7.5 (20050613): "CommonGrammar.g" -> "CommonTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API CommonTokenTypes {
#endif
	enum {
		EOF_ = 1,
		QUOTED_STRING = 4,
		ID = 5,
		NUMBER = 6,
		NBSP = 7,
		COLON = 8,
		DOT = 9,
		BACKSLASH = 10,
		FORESLASH = 11,
		EOL = 12,
		LITERAL_e = 13,
		SIGN = 14,
		PERCENT = 15,
		LEFT_ABRACKET = 16,
		COMMA = 17,
		RIGHT_ABRACKET = 18,
		DIGIT = 19,
		LETTER = 20,
		WS_CHAR = 21,
		EOL_CHAR = 22,
		DQUOTE = 23,
		SQUOTE = 24,
		SPACE = 25,
		COMMENT = 26,
		WS = 27,
		LEFT_SBRACKET = 28,
		RIGHT_SBRACKET = 29,
		ASSIGN = 30,
		AND = 31,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
#endif /*INC_CommonTokenTypes_hpp_*/
