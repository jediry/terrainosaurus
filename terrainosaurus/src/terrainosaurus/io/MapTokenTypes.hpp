#ifndef INC_MapTokenTypes_hpp_
#define INC_MapTokenTypes_hpp_

ANTLR_BEGIN_NAMESPACE(terrainosaurus)
/* $ANTLR 2.7.5 (20050201): "Map.g" -> "MapTokenTypes.hpp"$ */

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
		SIGN = 9,
		NUMBER = 10,
		DOT = 11,
		DIGIT = 12,
		LETTER = 13,
		COMMENT = 14,
		WS = 15,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
#endif /*INC_MapTokenTypes_hpp_*/
