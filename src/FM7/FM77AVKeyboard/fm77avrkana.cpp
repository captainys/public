#include <string.h>

#include "fm77avrkana.h"


static FM77AVRKanaMap rKanaTable[]=
{
	{"a",	false,	{{AVKEY_3,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"xa",	false,	{{AVKEY_3,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"la",	false,	{{AVKEY_3,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"i",	false,	{{AVKEY_E,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"yi",	false,	{{AVKEY_E,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"xi",	false,	{{AVKEY_E,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"li",	false,	{{AVKEY_E,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"u",	false,	{{AVKEY_4,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"wu",	false,	{{AVKEY_4,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"xu",	false,	{{AVKEY_4,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"lu",	false,	{{AVKEY_4,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"e",	false,	{{AVKEY_5,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ye",	false,	{{AVKEY_5,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"xe",	false,	{{AVKEY_5,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"le",	false,	{{AVKEY_5,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"o",	false,	{{AVKEY_6,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"xo",	false,	{{AVKEY_6,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"lo",	false,	{{AVKEY_6,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"ka",	false,	{{AVKEY_T,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ki",	false,	{{AVKEY_G,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ku",	false,	{{AVKEY_H,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ke",	false,	{{AVKEY_COLON,	false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ko",	false,	{{AVKEY_B,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"kya",	false,	{{AVKEY_G,		false,false,false},	{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"kyi",	false,	{{AVKEY_G,		false,false,false},	{AVKEY_E,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"kyu",	false,	{{AVKEY_G,		false,false,false},	{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"kye",	false,	{{AVKEY_G,		false,false,false},	{AVKEY_5,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"kyo",	false,	{{AVKEY_G,		false,false,false},	{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"ga",	false,	{{AVKEY_T,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"gi",	false,	{{AVKEY_G,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"gu",	false,	{{AVKEY_H,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ge",	false,	{{AVKEY_COLON,	false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"go",	false,	{{AVKEY_B,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"sa",	false,	{{AVKEY_X,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"si",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"shi",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"su",	false,	{{AVKEY_R,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"se",	false,	{{AVKEY_P,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"so",	false,	{{AVKEY_C,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"sha",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"sya",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"shu",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"syu",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"she",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_5,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"sye",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_5,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"sho",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"syo",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"ja",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_7,		true,false,false}}},
	{"jya",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_7,		true,false,false}}},
	{"ju",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_8,		true,false,false}}},
	{"jyu",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_8,		true,false,false}}},
	{"je",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_5,		true,false,false}}},
	{"jye",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_5,		true,false,false}}},
	{"jo",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_9,		true,false,false}}},
	{"jyo",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_9,		true,false,false}}},

	{"za",	false,	{{AVKEY_X,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"zi",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ji",	false,	{{AVKEY_D,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"zu",	false,	{{AVKEY_R,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ze",	false,	{{AVKEY_P,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"zo",	false,	{{AVKEY_C,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"ta",	false,	{{AVKEY_Q,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ti",	false,	{{AVKEY_A,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"chi",	false,	{{AVKEY_A,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"tu",	false,	{{AVKEY_Z,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"tsu",	false,	{{AVKEY_Z,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"te",	false,	{{AVKEY_W,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"to",	false,	{{AVKEY_S,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"tya",	false,	{{AVKEY_A,		false,false,false},	{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"tyi",	false,	{{AVKEY_A,		false,false,false},	{AVKEY_E,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"thi",	false,	{{AVKEY_W,		false,false,false},	{AVKEY_E,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"tyu",	false,	{{AVKEY_A,		false,false,false},	{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"tyo",	false,	{{AVKEY_A,		false,false,false},	{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"cha",	false,	{{AVKEY_A,		false,false,false},	{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"chu",	false,	{{AVKEY_A,		false,false,false},	{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"cho",	false,	{{AVKEY_A,		false,false,false},	{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"ltu",	false,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"xtu",	false,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"da",	false,	{{AVKEY_Q,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"di",	false,	{{AVKEY_A,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"du",	false,	{{AVKEY_Z,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"de",	false,	{{AVKEY_W,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"do",	false,	{{AVKEY_S,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"dha",	false,	{{AVKEY_Q,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"dhi",	false,	{{AVKEY_W,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_E,		true,false,false}}},
	{"dhu",	false,	{{AVKEY_S,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_4,		true,false,false}}},
	{"dhe",	false,	{{AVKEY_W,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"dho",	false,	{{AVKEY_S,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"dya",	false,	{{AVKEY_W,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_7,		true,false,false}}},
	{"dyi",	false,	{{AVKEY_W,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_E,		true,false,false}}},
	{"dyu",	false,	{{AVKEY_W,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_8,		true,false,false}}},
	{"dye",	false,	{{AVKEY_W,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_5,		true,false,false}}},
	{"dyo",	false,	{{AVKEY_W,		false,false,false},	{AVKEY_AT,  	false,false,false},	{AVKEY_9,		true,false,false}}},

	{"na",	false,	{{AVKEY_U,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ni",	false,	{{AVKEY_I,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nu",	false,	{{AVKEY_1,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ne",	false,	{{AVKEY_COMMA,	false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"no",	false,	{{AVKEY_K,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"nya",	false,	{{AVKEY_I,		false,false,false},	{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nyi",	false,	{{AVKEY_I,		false,false,false},	{AVKEY_E,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nyu",	false,	{{AVKEY_1,		false,false,false},	{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nye",	false,	{{AVKEY_COMMA,	false,false,false},	{AVKEY_5,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nyo",	false,	{{AVKEY_K,		false,false,false},	{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"ha",	false,	{{AVKEY_F,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"hi",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"hu",	false,	{{AVKEY_2,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"he",	false,	{{AVKEY_HAT,	false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ho",	false,	{{AVKEY_MINUS,	false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"pa",	false,	{{AVKEY_F,		false,false,false},	{AVKEY_LEFT_SQUARE_BRACKET,false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"pi",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_LEFT_SQUARE_BRACKET,false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"pu",	false,	{{AVKEY_2,		false,false,false},	{AVKEY_LEFT_SQUARE_BRACKET,false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"pe",	false,	{{AVKEY_HAT,	false,false,false},	{AVKEY_LEFT_SQUARE_BRACKET,false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"po",	false,	{{AVKEY_MINUS,	false,false,false},	{AVKEY_LEFT_SQUARE_BRACKET,false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"pya",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_LEFT_SQUARE_BRACKET,false,false,false},	{AVKEY_7,		true,false,false}}},
	{"pyi",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_LEFT_SQUARE_BRACKET,false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"pyu",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_LEFT_SQUARE_BRACKET,false,false,false},	{AVKEY_8,		true,false,false}}},
	{"pye",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_LEFT_SQUARE_BRACKET,false,false,false},	{AVKEY_5,		true,false,false}}},
	{"pyo",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_LEFT_SQUARE_BRACKET,false,false,false},	{AVKEY_9,		true,false,false}}},

	{"hya",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"hyu",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"hyo",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"ba",	false,	{{AVKEY_F,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"bi",	false,	{{AVKEY_V,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"bu",	false,	{{AVKEY_2,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"be",	false,	{{AVKEY_HAT,	false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"bo",	false,	{{AVKEY_MINUS,	false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"fa",	false,	{{AVKEY_2,		false,false,false},	{AVKEY_3,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"fi",	false,	{{AVKEY_2,		false,false,false},	{AVKEY_E,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"fu",	false,	{{AVKEY_2,		false,false,false},	{AVKEY_NULL,	true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"fe",	false,	{{AVKEY_2,		false,false,false},	{AVKEY_5,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"fo",	false,	{{AVKEY_2,		false,false,false},	{AVKEY_6,		true,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"va",	false,	{{AVKEY_4,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_3,		true,false,false}}},
	{"vi",	false,	{{AVKEY_4,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_E,		true,false,false}}},
	{"vu",	false,	{{AVKEY_4,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_4,		true,false,false}}},
	{"ve",	false,	{{AVKEY_4,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_5,		true,false,false}}},
	{"vo",	false,	{{AVKEY_4,		false,false,false},	{AVKEY_AT,		false,false,false},	{AVKEY_6,		true,false,false}}},

	{"ma",	false,	{{AVKEY_J,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"mi",	false,	{{AVKEY_N,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"mu",	false,	{{AVKEY_RIGHT_SQUARE_BRACKET,false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"me",	false,	{{AVKEY_SLASH,	false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"mo",	false,	{{AVKEY_M,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"mya",	false,	{{AVKEY_N,		false,false,false},	{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"myi",	false,	{{AVKEY_N,		false,false,false},	{AVKEY_E,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"myu",	false,	{{AVKEY_N,		false,false,false},	{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"mye",	false,	{{AVKEY_N,		false,false,false},	{AVKEY_5,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"myo",	false,	{{AVKEY_N,		false,false,false},	{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"ya",	false,	{{AVKEY_7,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"yu",	false,	{{AVKEY_8,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"yo",	false,	{{AVKEY_9,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"lya",	false,	{{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"lyu",	false,	{{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"lyo",	false,	{{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"xya",	false,	{{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"xyu",	false,	{{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"xyo",	false,	{{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"ra",	false,	{{AVKEY_O,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ri",	false,	{{AVKEY_L,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ru",	false,	{{AVKEY_DOT,	false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"re",	false,	{{AVKEY_SEMICOLON,false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ro",	false,	{{AVKEY_DOUBLE_QUOTE,false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},  // This key doesn't exist in the US keyboard.  Therefore it is impossible to type Katakana-RO.

	{"rya",	false,	{{AVKEY_L,		false,false,false},	{AVKEY_7,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ryi",	false,	{{AVKEY_L,		false,false,false},	{AVKEY_E,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ryu",	false,	{{AVKEY_L,		false,false,false},	{AVKEY_8,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"rye",	false,	{{AVKEY_L,		false,false,false},	{AVKEY_5,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ryo",	false,	{{AVKEY_L,		false,false,false},	{AVKEY_9,		true,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"wa",	false,	{{AVKEY_0,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"wi",	false,	{{AVKEY_4,		false,false,false},	{AVKEY_E,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"we",	false,	{{AVKEY_4,		false,false,false},	{AVKEY_5,		true,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"wo",	false,	{{AVKEY_0,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"nn",	false,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"-",	false,	{{AVKEY_YEN,	false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},


	// Small TSU
	{"kk",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"gg",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"yy",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ss",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"jj",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"zz",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"tt",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"dd",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"hh",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"pp",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"bb",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ff",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"vv",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"mm",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"rr",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ww",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"cc",	true,	{{AVKEY_Z,		true,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},

	{"nk",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ng",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"ns",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nj",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nz",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nt",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nd",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nh",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"np",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nb",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nf",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nv",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nm",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nr",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
	{"nw",	true,	{{AVKEY_Y,		false,false,false},	{AVKEY_NULL,	false,false,false},	{AVKEY_NULL,	false,false,false}}},
};

FM77AVRKanaMap FM77AVFindKeyCombinationForRomaji(const char romaji[])
{
	for(auto &r : rKanaTable)
	{
		if(0==strcmp(romaji,r.inputStr))
		{
			return r;
		}

		char inputStr[16];
		strcpy(inputStr,r.inputStr);
		for(int i=0; 0!=inputStr[i]; ++i)
		{
			if('a'<=inputStr[i] && inputStr[i]<='z')
			{
				inputStr[i]=inputStr[i]+'A'-'a';
			}
		}

		if(0==strcmp(romaji,inputStr))
		{
			return r;
		}
	}

	FM77AVRKanaMap empty;
	empty.inputStr="";
	empty.comb[0].keyCode=AVKEY_NULL;
	empty.comb[1].keyCode=AVKEY_NULL;
	empty.comb[2].keyCode=AVKEY_NULL;
	return empty;
}

