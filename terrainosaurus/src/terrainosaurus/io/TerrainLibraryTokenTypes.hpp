#ifndef INC_TerrainLibraryTokenTypes_hpp_
#define INC_TerrainLibraryTokenTypes_hpp_

ANTLR_BEGIN_NAMESPACE(terrainosaurus)
/* $ANTLR 2.7.3: "TerrainLibrary.g" -> "TerrainLibraryTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API TerrainLibraryTokenTypes {
#endif
	enum {
		EOF_ = 1,
		OPEN_SBRACKET = 4,
		TERRAIN_TYPE = 5,
		COLON = 6,
		NAME = 7,
		CLOSE_SBRACKET = 8,
		EOL = 9,
		TERRAIN_SEAM = 10,
		AND = 11,
		COLOR = 12,
		ASSIGN = 13,
		SAMPLE = 14,
		NUMBER = 15,
		OF = 16,
		CHROMOSOMES = 17,
		SMOOTHNESS = 18,
		MUTATION = 19,
		RATIO = 20,
		CROSSOVER = 21,
		SELECTION = 22,
		ASPECT = 23,
		SIGN = 24,
		DOT = 25,
		OPEN_ABRACKET = 26,
		COMMA = 27,
		CLOSE_ABRACKET = 28,
		BACKSLASH = 29,
		FORESLASH = 30,
		SPACE = 31,
		DIGIT = 32,
		LETTER = 33,
		WS_CHAR = 34,
		EOL_CHAR = 35,
		DOUBLE_QUOTE = 36,
		COMMENT = 37,
		WS = 38,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
#endif /*INC_TerrainLibraryTokenTypes_hpp_*/