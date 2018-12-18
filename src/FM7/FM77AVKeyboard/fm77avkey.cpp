#include <vector>
#include <string.h>
#include "fm77avkey.h"

typedef struct KeyCodeAndLabelStruct
{
	int code;
	const char *label;
} KeyCodeAndLabel;

static KeyCodeAndLabel bitPattern[]=
{
	{AVKEY_BREAK				,"101010101010101001001010110101"},
	{AVKEY_PF1					,"101010101010101001001010101001"},
	{AVKEY_PF2					,"101010101010101001001001010101"},
	{AVKEY_PF3					,"101010101010101001001001001001"},
	{AVKEY_PF4					,"101010101001010110110110110101"},
	{AVKEY_PF5					,"101010101001010110110110101001"},
	{AVKEY_PF6					,"101010101001010110110101010101"},
	{AVKEY_PF7					,"101010101001010110110101001001"},
	{AVKEY_PF8					,"101010101001010110101010110101"},
	{AVKEY_PF9					,"101010101001010110101010101001"},
	{AVKEY_PF10					,"101010101001010110101001010101"},
	{AVKEY_EL					,"101010101010110101010110101001"},
	{AVKEY_CLS					,"101010101010110101010101010101"},
	{AVKEY_DUP					,"101010101010110101001010110101"},
	{AVKEY_HOME					,"101010101010110101001001010101"},
	{AVKEY_INS					,"101010101010110101010110110101"},
	{AVKEY_DEL					,"101010101010110101010101001001"},
	{AVKEY_LEFT					,"101010101010110101001001001001"},
	{AVKEY_RIGHT				,"101010101010101010110110101001"},
	{AVKEY_UP					,"101010101010110101001010101001"},
	{AVKEY_DOWN					,"101010101010101010110110110101"},

	{AVKEY_ESC					,"101010110110110110110110101001"},
	{AVKEY_0					,"101010110110110101010101001001"},
	{AVKEY_1					,"101010110110110110110101010101"},
	{AVKEY_2					,"101010110110110110110101001001"},
	{AVKEY_3					,"101010110110110110101010110101"},
	{AVKEY_4					,"101010110110110110101010101001"},
	{AVKEY_5					,"101010110110110110101001010101"},
	{AVKEY_6					,"101010110110110110101001001001"},
	{AVKEY_7					,"101010110110110101010110110101"},
	{AVKEY_8					,"101010110110110101010110101001"},
	{AVKEY_9					,"101010110110110101010101010101"},
	{AVKEY_MINUS				,"101010110110110101001010110101"},
	{AVKEY_HAT					,"101010110110110101001010101001"},
	{AVKEY_YEN					,"101010110110110101001001010101"},
	{AVKEY_BACKSPACE			,"101010110110110101001001001001"},

	{AVKEY_TAB					,"101010110110101010110110110101"},
	{AVKEY_Q					,"101010110110101010110110101001"},
	{AVKEY_W					,"101010110110101010110101010101"},
	{AVKEY_E					,"101010110110101010110101001001"},
	{AVKEY_R					,"101010110110101010101010110101"},
	{AVKEY_T					,"101010110110101010101010101001"},
	{AVKEY_Y					,"101010110110101010101001010101"},
	{AVKEY_U					,"101010110110101010101001001001"},
	{AVKEY_I					,"101010110110101001010110110101"},
	{AVKEY_O					,"101010110110101001010110101001"},
	{AVKEY_P					,"101010110110101001010101010101"},
	{AVKEY_AT					,"101010110110101001010101001001"},
	{AVKEY_LEFT_SQUARE_BRACKET	,"101010110110101001001010110101"},
	{AVKEY_RETURN				,"101010110110101001001010101001"},

	{AVKEY_CTRL					,"101010101010101010110101010101"},
	{AVKEY_A					,"101010110110101001001001010101"},
	{AVKEY_S					,"101010110110101001001001001001"},
	{AVKEY_D					,"101010110101010110110110110101"},
	{AVKEY_F					,"101010110101010110110110101001"},
	{AVKEY_G					,"101010110101010110110101010101"},
	{AVKEY_H					,"101010110101010110110101001001"},
	{AVKEY_J					,"101010110101010110101010110101"},
	{AVKEY_K					,"101010110101010110101010101001"},
	{AVKEY_L					,"101010110101010110101001010101"},
	{AVKEY_SEMICOLON			,"101010110101010110101001001001"},
	{AVKEY_COLON				,"101010110101010101010110110101"},
	{AVKEY_RIGHT_SQUARE_BRACKET	,"101010110101010101010110101001"},

	{AVKEY_LEFT_SHIFT			,"101010101010101010110101001001"},
	{AVKEY_Z					,"101010110101010101010101010101"},
	{AVKEY_X					,"101010110101010101010101001001"},
	{AVKEY_C					,"101010110101010101001010110101"},
	{AVKEY_V					,"101010110101010101001010101001"},
	{AVKEY_B					,"101010110101010101001001010101"},
	{AVKEY_N					,"101010110101010101001001001001"},
	{AVKEY_M					,"101010110101001010110110110101"},
	{AVKEY_COMMA				,"101010110101001010110110101001"},
	{AVKEY_DOT					,"101010110101001010110101010101"},
	{AVKEY_SLASH				,"101010110101001010110101001001"},
	{AVKEY_DOUBLE_QUOTE			,"101010110101001010101010110101"},
	{AVKEY_RIGHT_SHIFT			,"101010101010101010101010110101"},

	{AVKEY_CAPS					,"101010101010101010101010101001"},
	{AVKEY_GRAPH				,"101010101010101010101001010101"},
	{AVKEY_LEFT_SPACE			,"101010101010101010101001001001"},
	{AVKEY_MID_SPACE			,"101010101010101001010110110101"},
	{AVKEY_RIGHT_SPACE			,"101010110101001010101010101001"},
	{AVKEY_KANA					,"101010101010101001010101010101"},

	{AVKEY_NUM_STAR				,"101010110101001010101001010101"},
	{AVKEY_NUM_SLASH			,"101010110101001010101001001001"},
	{AVKEY_NUM_PLUS				,"101010110101001001010110110101"},
	{AVKEY_NUM_MINUS			,"101010110101001001010110101001"},
	{AVKEY_NUM_EQUAL			,"101010110101001001001010101001"},
	{AVKEY_NUM_COMMA			,"101010101010110110110110101001"},
	{AVKEY_NUM_RETURN			,"101010101010110110101010101001"},
	{AVKEY_NUM_DOT				,"101010101010110110101001001001"},
	{AVKEY_NUM_0				,"101010101010110110101001010101"},
	{AVKEY_NUM_1				,"101010101010110110110101010101"},
	{AVKEY_NUM_2				,"101010101010110110110101001001"},
	{AVKEY_NUM_3				,"101010101010110110101010110101"},
	{AVKEY_NUM_4				,"101010110101001001001001010101"},
	{AVKEY_NUM_5				,"101010110101001001001001001001"},
	{AVKEY_NUM_6				,"101010101010110110110110110101"},
	{AVKEY_NUM_7				,"101010110101001001010101010101"},
	{AVKEY_NUM_8				,"101010110101001001010101001001"},
	{AVKEY_NUM_9				,"101010110101001001001010110101"},
};



static KeyCodeAndLabel keyCodeLabelTable[]=
{
	{AVKEY_NULL,                     "NULL"},

	{AVKEY_BREAK,                    "BREAK"},
	{AVKEY_PF1,                      "PF1"},
	{AVKEY_PF2,                      "PF2"},
	{AVKEY_PF3,                      "PF3"},
	{AVKEY_PF4,                      "PF4"},
	{AVKEY_PF5,                      "PF5"},
	{AVKEY_PF6,                      "PF6"},
	{AVKEY_PF7,                      "PF7"},
	{AVKEY_PF8,                      "PF8"},
	{AVKEY_PF9,                      "PF9"},
	{AVKEY_PF10,                     "PF10"},
	{AVKEY_EL,                       "EL"},
	{AVKEY_CLS,                      "CLS"},
	{AVKEY_DUP,                      "DUP"},
	{AVKEY_HOME,                     "HOME"},
	{AVKEY_INS,                      "INS"},
	{AVKEY_DEL,                      "DEL"},
	{AVKEY_LEFT,                     "LEFT"},
	{AVKEY_RIGHT,                    "RIGHT"},
	{AVKEY_UP,                       "UP"},
	{AVKEY_DOWN,                     "DOWN"},

	{AVKEY_ESC,                      "ESC"},
	{AVKEY_0,                        "0"},
	{AVKEY_1,                        "1"},
	{AVKEY_2,                        "2"},
	{AVKEY_3,                        "3"},
	{AVKEY_4,                        "4"},
	{AVKEY_5,                        "5"},
	{AVKEY_6,                        "6"},
	{AVKEY_7,                        "7"},
	{AVKEY_8,                        "8"},
	{AVKEY_9,                        "9"},
	{AVKEY_MINUS,                    "MINUS"},
	{AVKEY_HAT,                      "HAT"},
	{AVKEY_YEN,                      "YEN"},
	{AVKEY_BACKSPACE,                "BACKSPACE"},
                                    
	{AVKEY_TAB,                      "TAB"},
	{AVKEY_Q,                        "Q"},
	{AVKEY_W,                        "W"},
	{AVKEY_E,                        "E"},
	{AVKEY_R,                        "R"},
	{AVKEY_T,                        "T"},
	{AVKEY_Y,                        "Y"},
	{AVKEY_U,                        "U"},
	{AVKEY_I,                        "I"},
	{AVKEY_O,                        "O"},
	{AVKEY_P,                        "P"},
	{AVKEY_AT,                       "AT"},
	{AVKEY_LEFT_SQUARE_BRACKET,      "LEFT_SQUARE_BRACKET"},
	{AVKEY_RETURN,                   "RETURN"},
                                    
	{AVKEY_CTRL,                     "CTRL"},
	{AVKEY_A,                        "A"},
	{AVKEY_S,                        "S"},
	{AVKEY_D,                        "D"},
	{AVKEY_F,                        "F"},
	{AVKEY_G,                        "G"},
	{AVKEY_H,                        "H"},
	{AVKEY_J,                        "J"},
	{AVKEY_K,                        "K"},
	{AVKEY_L,                        "L"},
	{AVKEY_SEMICOLON,                "SEMICOLON"},
	{AVKEY_COLON,                    "COLON"},
	{AVKEY_RIGHT_SQUARE_BRACKET,     "RIGHT_SQUARE_BRACKET"},
                                    
	{AVKEY_LEFT_SHIFT,               "LEFT_SHIFT"},
	{AVKEY_Z,                        "Z"},
	{AVKEY_X,                        "X"},
	{AVKEY_C,                        "C"},
	{AVKEY_V,                        "V"},
	{AVKEY_B,                        "B"},
	{AVKEY_N,                        "N"},
	{AVKEY_M,                        "M"},
	{AVKEY_COMMA,                    "COMMA"},
	{AVKEY_DOT,                      "DOT"},
	{AVKEY_SLASH,                    "SLASH"},
	{AVKEY_DOUBLE_QUOTE,             "DOUBLE_QUOTE"},
	{AVKEY_RIGHT_SHIFT,              "RIGHT_SHIFT"},
                                    
	{AVKEY_CAPS,                     "CAPS"},
	{AVKEY_GRAPH,                    "GRAPH"},
	{AVKEY_LEFT_SPACE,               "LEFT_SPACE"},
	{AVKEY_MID_SPACE,                "MID_SPACE"},
	{AVKEY_RIGHT_SPACE,              "RIGHT_SPACE"},
	{AVKEY_KANA,                     "KANA"},
                                    
	{AVKEY_NUM_STAR,                 "NUM_STAR"},
	{AVKEY_NUM_SLASH,                "NUM_SLASH"},
	{AVKEY_NUM_PLUS,                 "NUM_PLUS"},
	{AVKEY_NUM_MINUS,                "NUM_MINUS"},
	{AVKEY_NUM_EQUAL,                "NUM_EQUAL"},
	{AVKEY_NUM_COMMA,                "NUM_COMMA"},
	{AVKEY_NUM_RETURN,               "NUM_RETURN"},
	{AVKEY_NUM_DOT,                  "NUM_DOT"},
	{AVKEY_NUM_0,                    "NUM_0"},
	{AVKEY_NUM_1,                    "NUM_1"},
	{AVKEY_NUM_2,                    "NUM_2"},
	{AVKEY_NUM_3,                    "NUM_3"},
	{AVKEY_NUM_4,                    "NUM_4"},
	{AVKEY_NUM_5,                    "NUM_5"},
	{AVKEY_NUM_6,                    "NUM_6"},
	{AVKEY_NUM_7,                    "NUM_7"},
	{AVKEY_NUM_8,                    "NUM_8"},
	{AVKEY_NUM_9,                    "NUM_9"},

	{0,NULL}
};


int FM77AVKeyLabelToKeyCode(const char str[])
{
	for(auto &l : keyCodeLabelTable)
	{
		if(nullptr!=l.label)
		{
			int len=strlen(l.label);
			if(0==strncmp(str,l.label,len) && (0==str[len] || '\\'==str[len] || 0x0d==str[len] || 0x0a==str[len]))
			{
				return l.code;
			}
		}
	}
	return 0;
}


typedef struct KeyTranslationTableStruct
{
	char c;
	FM77AVKeyCombination comb;
} KeyTranslationTable;

static KeyTranslationTable keyTranslationTable[]=
{
	{'\t',{AVKEY_TAB,           false,false,false}},
	{' ', {AVKEY_MID_SPACE,     false,false,false}},		// 32
	{'!', {AVKEY_1,             true,false,false}},
	{'\"',{AVKEY_DOUBLE_QUOTE,  false,false,false}},
	{'#', {AVKEY_3,             true,false,false}},
	{'$', {AVKEY_4,             true,false,false}},
	{'%', {AVKEY_5,             true,false,false}},
	{'&', {AVKEY_6,             true,false,false}},
	{'\'',{AVKEY_7,             true,false,false}},
	{'(', {AVKEY_8,             true,false,false}},
	{')', {AVKEY_9,             true,false,false}},
	{'*', {AVKEY_NUM_STAR,      false,false,false}},
	{'+', {AVKEY_NUM_PLUS,      false,false,false}},
	{',', {AVKEY_NUM_COMMA,     false,false,false}},
	{'-', {AVKEY_NUM_MINUS,     false,false,false}},
	{'.', {AVKEY_NUM_DOT,       false,false,false}},
	{'/', {AVKEY_NUM_SLASH,     false,false,false}},
	{'0', {AVKEY_NUM_0,         false,false,false}},		// 48
	{'1', {AVKEY_NUM_1,         false,false,false}},
	{'2', {AVKEY_NUM_2,         false,false,false}},
	{'3', {AVKEY_NUM_3,         false,false,false}},
	{'4', {AVKEY_NUM_4,         false,false,false}},
	{'5', {AVKEY_NUM_5,         false,false,false}},
	{'6', {AVKEY_NUM_6,         false,false,false}},
	{'7', {AVKEY_NUM_7,         false,false,false}},
	{'8', {AVKEY_NUM_8,         false,false,false}},
	{'9', {AVKEY_NUM_9,         false,false,false}},
	{':', {AVKEY_COLON,         false,false,false}},
	{';', {AVKEY_SEMICOLON,     false,false,false}},
{'<', {AVKEY_COMMA,true,false,false}},
	{'=', {AVKEY_NUM_EQUAL,     false,false,false}},
{'>', {AVKEY_DOT,true,false,false}},
	{'?', {AVKEY_SLASH,         true,false,false}},
	{'@', {AVKEY_AT,            false,false,false}},		//64
	{'A', {AVKEY_A,             true,false,false}},
	{'B', {AVKEY_B,             true,false,false}},
	{'C', {AVKEY_C,             true,false,false}},
	{'D', {AVKEY_D,             true,false,false}},
	{'E', {AVKEY_E,             true,false,false}},
	{'F', {AVKEY_F,             true,false,false}},
	{'G', {AVKEY_G,             true,false,false}},
	{'H', {AVKEY_H,             true,false,false}},
	{'I', {AVKEY_I,             true,false,false}},
	{'J', {AVKEY_J,             true,false,false}},
	{'K', {AVKEY_K,             true,false,false}},
	{'L', {AVKEY_L,             true,false,false}},
	{'M', {AVKEY_M,             true,false,false}},
	{'N', {AVKEY_N,             true,false,false}},
	{'O', {AVKEY_O,             true,false,false}},
	{'P', {AVKEY_P,             true,false,false}},			//80
	{'Q', {AVKEY_Q,             true,false,false}},
	{'R', {AVKEY_R,             true,false,false}},
	{'S', {AVKEY_S,             true,false,false}},
	{'T', {AVKEY_T,             true,false,false}},
	{'U', {AVKEY_U,             true,false,false}},
	{'V', {AVKEY_V,             true,false,false}},
	{'W', {AVKEY_W,             true,false,false}},
	{'X', {AVKEY_X,             true,false,false}},
	{'Y', {AVKEY_Y,             true,false,false}},
	{'Z', {AVKEY_Z,             true,false,false}},
	{'[', {AVKEY_LEFT_SQUARE_BRACKET,false,false,false}},
	{'\\',{AVKEY_YEN,           false,false,false}},
	{']', {AVKEY_RIGHT_SQUARE_BRACKET,false,false,false}},
	{'^', {AVKEY_HAT,           false,false,false}},
{'_', {AVKEY_DOUBLE_QUOTE,true,false,false}},
{'`', {AVKEY_AT,true,false,false}},  // 96
	{'a', {AVKEY_A,             false,false,false}},
	{'b', {AVKEY_B,             false,false,false}},
	{'c', {AVKEY_C,             false,false,false}},
	{'d', {AVKEY_D,             false,false,false}},
	{'e', {AVKEY_E,             false,false,false}},
	{'f', {AVKEY_F,             false,false,false}},
	{'g', {AVKEY_G,             false,false,false}},
	{'h', {AVKEY_H,             false,false,false}},
	{'i', {AVKEY_I,             false,false,false}},
	{'j', {AVKEY_J,             false,false,false}},
	{'k', {AVKEY_K,             false,false,false}},
	{'l', {AVKEY_L,             false,false,false}},
	{'m', {AVKEY_M,             false,false,false}},
	{'n', {AVKEY_N,             false,false,false}},
	{'o', {AVKEY_O,             false,false,false}},
	{'p', {AVKEY_P,             false,false,false}},
	{'q', {AVKEY_Q,             false,false,false}},
	{'r', {AVKEY_R,             false,false,false}},
	{'s', {AVKEY_S,             false,false,false}},
	{'t', {AVKEY_T,             false,false,false}},
	{'u', {AVKEY_U,             false,false,false}},
	{'v', {AVKEY_V,             false,false,false}},
	{'w', {AVKEY_W,             false,false,false}},
	{'x', {AVKEY_X,             false,false,false}},
	{'y', {AVKEY_Y,             false,false,false}},
	{'z', {AVKEY_Z,             false,false,false}},
{'{', {AVKEY_LEFT_SQUARE_BRACKET,true,false,false}},
	{'|', {AVKEY_YEN,           true,false,false}},
{'}', {AVKEY_RIGHT_SQUARE_BRACKET,true,false,false}},
	{'~', {AVKEY_HAT,           true,false,false}},


/*	{'', {AVKEY_,             false,false,false}},
	{'', {AVKEY_,             false,false,false}},
*/
};


FM77AVKeyCombination FM77AVTranslateCharToCode(char c)
{
	if(0!=c)
	{
		printf("Translateing: %c\n",c);
	}

	for(auto &i : keyTranslationTable)
	{
		if(i.c==c)
		{
			return i.comb;
		}
	}
	FM77AVKeyCombination comb;
	comb.keyCode=AVKEY_NULL;
	return comb;
}

const char *FM77AVGetKeyPress30BitPattern(int fm77avkey)
{
	for(auto &i : bitPattern)
	{
		if(i.code==fm77avkey)
		{
			return i.label;
		}
	}
	return nullptr;
}

std::string FM77AVGetKeyRelease30BitPattern(int fm77avkey)
{
	std::string ptn;
	auto pressPtn=FM77AVGetKeyPress30BitPattern(fm77avkey);
	if(nullptr!=pressPtn)
	{
		/* Thanks to Mr. Kobayashi from Classic PC & Retro Game Club JAPAN
		   for calculation of the key-release code.
		*/

		auto firstByte=FM77AVGet1stByteIn30bitEncoding(pressPtn);
		firstByte-=0x60;
		firstByte&=255;

		char code30[31];
		for(int i=0; i<31; ++i)
		{
			code30[i]=pressPtn[i];
		}
		FM77AVPut1stByteIn30bitEncoding(code30,firstByte);
		ptn=code30;
	}
	return ptn;
}

std::string FM77AVMake30BitPattern(const FM77AVKeyEvent keyEvent)
{
	std::string str;

	if(true==keyEvent.pressEvent)
	{
		return FM77AVGetKeyPress30BitPattern(keyEvent.keyCode);
	}
	else
	{
		return FM77AVGetKeyRelease30BitPattern(keyEvent.keyCode);
	}
}

/* Repeat every 3rd of 4-byte groups.
   The raw pulse looks to be going 100us,125us,175us.  Thanks Mr. Kobayashi of Classic PC and Retro Gaming JAPAN!
   But, approximating it with 100us 100us 200us looks to work just fine.
*/
void FM77AV30bitTo40bit(char code40[41],const char code30[])
{
	int k=0;
	for(int i=0; i<30; i+=3)
	{
		code40[k++]=code30[i];
		code40[k++]=code30[i+1];
		code40[k++]=code30[i+2];
		code40[k++]=code30[i+2];
	}
	code40[40]=0;
	if(40!=k)
	{
		printf("Internal error!\n");
		printf("Did I count correctly?\n");
		exit(1);
	}
}

unsigned int FM77AVGet1stByteIn30bitEncoding(const char code30[])
{
	unsigned int sum=0;
	unsigned int bitShift=0x80;
	for(int i=4; i<12; ++i)
	{
		if('1'==code30[i])
		{
			sum+=bitShift;
		}
		bitShift>>=1;
	}
	return sum;
}

void FM77AVPut1stByteIn30bitEncoding(char code30[],unsigned int byteData)
{
	unsigned int bitShift=0x80;
	for(int i=4; i<12; ++i)
	{
		if(0!=(byteData&bitShift))
		{
			code30[i]='1';
		}
		else
		{
			code30[i]='0';
		}
		bitShift>>=1;
	}
}

