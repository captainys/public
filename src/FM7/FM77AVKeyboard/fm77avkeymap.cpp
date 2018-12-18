#include "fm77avkeymap.h"
#include "fm77avkey.h"

FM77AVKeyMap::FM77AVKeyMap()
{
	for(auto & m : map)
	{
		m=AVKEY_NULL;
	}
	MakeUS101KeyMap();
}

void FM77AVKeyMap::MakeUS101KeyMap(void)
{
	map[FSKEY_SPACE]=           AVKEY_MID_SPACE;
	map[FSKEY_0]=               AVKEY_0;
	map[FSKEY_1]=               AVKEY_1;
	map[FSKEY_2]=               AVKEY_2;
	map[FSKEY_3]=               AVKEY_3;
	map[FSKEY_4]=               AVKEY_4;
	map[FSKEY_5]=               AVKEY_5;
	map[FSKEY_6]=               AVKEY_6;
	map[FSKEY_7]=               AVKEY_7;
	map[FSKEY_8]=               AVKEY_8;
	map[FSKEY_9]=               AVKEY_9;
	map[FSKEY_A]=               AVKEY_A;
	map[FSKEY_B]=               AVKEY_B;
	map[FSKEY_C]=               AVKEY_C;
	map[FSKEY_D]=               AVKEY_D;
	map[FSKEY_E]=               AVKEY_E;
	map[FSKEY_F]=               AVKEY_F;
	map[FSKEY_G]=               AVKEY_G;
	map[FSKEY_H]=               AVKEY_H;
	map[FSKEY_I]=               AVKEY_I;
	map[FSKEY_J]=               AVKEY_J;
	map[FSKEY_K]=               AVKEY_K;
	map[FSKEY_L]=               AVKEY_L;
	map[FSKEY_M]=               AVKEY_M;
	map[FSKEY_N]=               AVKEY_N;
	map[FSKEY_O]=               AVKEY_O;
	map[FSKEY_P]=               AVKEY_P;
	map[FSKEY_Q]=               AVKEY_Q;
	map[FSKEY_R]=               AVKEY_R;
	map[FSKEY_S]=               AVKEY_S;
	map[FSKEY_T]=               AVKEY_T;
	map[FSKEY_U]=               AVKEY_U;
	map[FSKEY_V]=               AVKEY_V;
	map[FSKEY_W]=               AVKEY_W;
	map[FSKEY_X]=               AVKEY_X;
	map[FSKEY_Y]=               AVKEY_Y;
	map[FSKEY_Z]=               AVKEY_Z;
	map[FSKEY_ESC]=             AVKEY_BREAK;
	map[FSKEY_F1]=              AVKEY_PF1;
	map[FSKEY_F2]=              AVKEY_PF2;
	map[FSKEY_F3]=              AVKEY_PF3;
	map[FSKEY_F4]=              AVKEY_PF4;
	map[FSKEY_F5]=              AVKEY_PF5;
	map[FSKEY_F6]=              AVKEY_PF6;
	map[FSKEY_F7]=              AVKEY_PF7;
	map[FSKEY_F8]=              AVKEY_PF8;
	map[FSKEY_F9]=              AVKEY_PF9;
	map[FSKEY_F10]=             AVKEY_PF10;
	map[FSKEY_F11]=             AVKEY_LEFT_SPACE;
	map[FSKEY_F12]=             AVKEY_RIGHT_SPACE;
	map[FSKEY_PRINTSCRN]=       AVKEY_NULL;
	map[FSKEY_CAPSLOCK]=        AVKEY_CAPS;
	map[FSKEY_SCROLLLOCK]=      AVKEY_NULL;
	map[FSKEY_PAUSEBREAK]=      AVKEY_NULL;
	map[FSKEY_BS]=              AVKEY_BACKSPACE;
	map[FSKEY_TAB]=             AVKEY_TAB;
	map[FSKEY_ENTER]=           AVKEY_RETURN;
	map[FSKEY_SHIFT]=           AVKEY_NULL;
	map[FSKEY_CTRL]=            AVKEY_NULL;
	map[FSKEY_ALT]=             AVKEY_NULL;
	map[FSKEY_INS]=             AVKEY_INS;
	map[FSKEY_DEL]=             AVKEY_DEL;
	map[FSKEY_HOME]=            AVKEY_HOME;
	map[FSKEY_END]=             AVKEY_DUP;
	map[FSKEY_PAGEUP]=          AVKEY_NULL;
	map[FSKEY_PAGEDOWN]=        AVKEY_NULL;
	map[FSKEY_UP]=              AVKEY_UP;
	map[FSKEY_DOWN]=            AVKEY_DOWN;
	map[FSKEY_LEFT]=            AVKEY_LEFT;
	map[FSKEY_RIGHT]=           AVKEY_RIGHT;
	map[FSKEY_NUMLOCK]=         AVKEY_NULL;
	map[FSKEY_TILDA]=           AVKEY_ESC;
	map[FSKEY_MINUS]=           AVKEY_MINUS;
	map[FSKEY_PLUS]=            AVKEY_HAT;
	map[FSKEY_LBRACKET]=        AVKEY_AT;
	map[FSKEY_RBRACKET]=        AVKEY_LEFT_SQUARE_BRACKET;
	map[FSKEY_BACKSLASH]=       AVKEY_YEN;
	map[FSKEY_SEMICOLON]=       AVKEY_SEMICOLON;
	map[FSKEY_SINGLEQUOTE]=     AVKEY_COLON;
	map[FSKEY_COMMA]=           AVKEY_COMMA;
	map[FSKEY_DOT]=             AVKEY_DOT;
	map[FSKEY_SLASH]=           AVKEY_SLASH;
	map[FSKEY_TEN0]=            AVKEY_NUM_0;
	map[FSKEY_TEN1]=            AVKEY_NUM_1;
	map[FSKEY_TEN2]=            AVKEY_NUM_2;
	map[FSKEY_TEN3]=            AVKEY_NUM_3;
	map[FSKEY_TEN4]=            AVKEY_NUM_4;
	map[FSKEY_TEN5]=            AVKEY_NUM_5;
	map[FSKEY_TEN6]=            AVKEY_NUM_6;
	map[FSKEY_TEN7]=            AVKEY_NUM_7;
	map[FSKEY_TEN8]=            AVKEY_NUM_8;
	map[FSKEY_TEN9]=            AVKEY_NUM_9;
	map[FSKEY_TENDOT]=          AVKEY_NUM_DOT;
	map[FSKEY_TENSLASH]=        AVKEY_NUM_SLASH;
	map[FSKEY_TENSTAR]=         AVKEY_NUM_STAR;
	map[FSKEY_TENMINUS]=        AVKEY_NUM_MINUS;
	map[FSKEY_TENPLUS]=         AVKEY_NUM_PLUS;
	map[FSKEY_TENENTER]=        AVKEY_NUM_RETURN;
	map[FSKEY_WHEELUP]=         AVKEY_NULL;
	map[FSKEY_WHEELDOWN]=       AVKEY_NULL;

	map[FSKEY_CONVERT]=         AVKEY_LEFT_SPACE;
	map[FSKEY_NONCONVERT]=      AVKEY_RIGHT_SPACE;

	map[FSKEY_LEFT_CTRL]=       AVKEY_CTRL;
	map[FSKEY_RIGHT_CTRL]=      AVKEY_GRAPH;
	map[FSKEY_LEFT_SHIFT]=      AVKEY_LEFT_SHIFT;
	map[FSKEY_RIGHT_SHIFT]=     AVKEY_RIGHT_SHIFT;
	map[FSKEY_LEFT_ALT]=        AVKEY_NULL;
	map[FSKEY_RIGHT_ALT]=       AVKEY_NULL;
}

void FM77AVKeyMap::ZXCForLMRSpzce(void)
{
	map[FSKEY_C]=               AVKEY_RIGHT_SPACE;
	map[FSKEY_X]=               AVKEY_MID_SPACE;
	map[FSKEY_Z]=               AVKEY_LEFT_SPACE;
}

void FM77AVKeyMap::ZXCForZXC(void)
{
	map[FSKEY_C]=               AVKEY_RIGHT_SPACE;
	map[FSKEY_X]=               AVKEY_MID_SPACE;
	map[FSKEY_Z]=               AVKEY_LEFT_SPACE;
}

void FM77AVKeyMap::ArrowFor2468(void)
{
	map[FSKEY_UP]=              AVKEY_NUM_8;
	map[FSKEY_DOWN]=            AVKEY_NUM_2;
	map[FSKEY_LEFT]=            AVKEY_NUM_4;
	map[FSKEY_RIGHT]=           AVKEY_NUM_6;
}
void FM77AVKeyMap::ArrowForArrow(void)
{
	map[FSKEY_UP]=              AVKEY_UP;
	map[FSKEY_DOWN]=            AVKEY_DOWN;
	map[FSKEY_LEFT]=            AVKEY_LEFT;
	map[FSKEY_RIGHT]=           AVKEY_RIGHT;
}
void FM77AVKeyMap::ArrowForNone(void)
{
	map[FSKEY_UP]=              AVKEY_NULL;
	map[FSKEY_DOWN]=            AVKEY_NULL;
	map[FSKEY_LEFT]=            AVKEY_NULL;
	map[FSKEY_RIGHT]=           AVKEY_NULL;
}

/* void FM77AVKeyMap::MakeXXXMap(void)
{
	map[FSKEY_SPACE]=           AVKEY_
	map[FSKEY_0]=               AVKEY_
	map[FSKEY_1]=               AVKEY_
	map[FSKEY_2]=               AVKEY_
	map[FSKEY_3]=               AVKEY_
	map[FSKEY_4]=               AVKEY_
	map[FSKEY_5]=               AVKEY_
	map[FSKEY_6]=               AVKEY_
	map[FSKEY_7]=               AVKEY_
	map[FSKEY_8]=               AVKEY_
	map[FSKEY_9]=               AVKEY_
	map[FSKEY_A]=               AVKEY_
	map[FSKEY_B]=               AVKEY_
	map[FSKEY_C]=               AVKEY_
	map[FSKEY_D]=               AVKEY_
	map[FSKEY_E]=               AVKEY_
	map[FSKEY_F]=               AVKEY_
	map[FSKEY_G]=               AVKEY_
	map[FSKEY_H]=               AVKEY_
	map[FSKEY_I]=               AVKEY_
	map[FSKEY_J]=               AVKEY_
	map[FSKEY_K]=               AVKEY_
	map[FSKEY_L]=               AVKEY_
	map[FSKEY_M]=               AVKEY_
	map[FSKEY_N]=               AVKEY_
	map[FSKEY_O]=               AVKEY_
	map[FSKEY_P]=               AVKEY_
	map[FSKEY_Q]=               AVKEY_
	map[FSKEY_R]=               AVKEY_
	map[FSKEY_S]=               AVKEY_
	map[FSKEY_T]=               AVKEY_
	map[FSKEY_U]=               AVKEY_
	map[FSKEY_V]=               AVKEY_
	map[FSKEY_W]=               AVKEY_
	map[FSKEY_X]=               AVKEY_
	map[FSKEY_Y]=               AVKEY_
	map[FSKEY_Z]=               AVKEY_
	map[FSKEY_ESC]=             AVKEY_
	map[FSKEY_F1]=              AVKEY_
	map[FSKEY_F2]=              AVKEY_
	map[FSKEY_F3]=              AVKEY_
	map[FSKEY_F4]=              AVKEY_
	map[FSKEY_F5]=              AVKEY_
	map[FSKEY_F6]=              AVKEY_
	map[FSKEY_F7]=              AVKEY_
	map[FSKEY_F8]=              AVKEY_
	map[FSKEY_F9]=              AVKEY_
	map[FSKEY_F10]=             AVKEY_
	map[FSKEY_F11]=             AVKEY_
	map[FSKEY_F12]=             AVKEY_
	map[FSKEY_PRINTSCRN]=       AVKEY_
	map[FSKEY_CAPSLOCK]=        AVKEY_
	map[FSKEY_SCROLLLOCK]=      AVKEY_
	map[FSKEY_PAUSEBREAK]=      AVKEY_
	map[FSKEY_BS]=              AVKEY_
	map[FSKEY_TAB]=             AVKEY_
	map[FSKEY_ENTER]=           AVKEY_
	map[FSKEY_SHIFT]=           AVKEY_
	map[FSKEY_CTRL]=            AVKEY_
	map[FSKEY_ALT]=             AVKEY_
	map[FSKEY_INS]=             AVKEY_
	map[FSKEY_DEL]=             AVKEY_
	map[FSKEY_HOME]=            AVKEY_
	map[FSKEY_END]=             AVKEY_
	map[FSKEY_PAGEUP]=          AVKEY_
	map[FSKEY_PAGEDOWN]=        AVKEY_
	map[FSKEY_UP]=              AVKEY_
	map[FSKEY_DOWN]=            AVKEY_
	map[FSKEY_LEFT]=            AVKEY_
	map[FSKEY_RIGHT]=           AVKEY_
	map[FSKEY_NUMLOCK]=         AVKEY_
	map[FSKEY_TILDA]=           AVKEY_
	map[FSKEY_MINUS]=           AVKEY_
	map[FSKEY_PLUS]=            AVKEY_
	map[FSKEY_LBRACKET]=        AVKEY_
	map[FSKEY_RBRACKET]=        AVKEY_
	map[FSKEY_BACKSLASH]=       AVKEY_
	map[FSKEY_SEMICOLON]=       AVKEY_
	map[FSKEY_SINGLEQUOTE]=     AVKEY_
	map[FSKEY_COMMA]=           AVKEY_
	map[FSKEY_DOT]=             AVKEY_
	map[FSKEY_SLASH]=           AVKEY_
	map[FSKEY_TEN0]=            AVKEY_
	map[FSKEY_TEN1]=            AVKEY_
	map[FSKEY_TEN2]=            AVKEY_
	map[FSKEY_TEN3]=            AVKEY_
	map[FSKEY_TEN4]=            AVKEY_
	map[FSKEY_TEN5]=            AVKEY_
	map[FSKEY_TEN6]=            AVKEY_
	map[FSKEY_TEN7]=            AVKEY_
	map[FSKEY_TEN8]=            AVKEY_
	map[FSKEY_TEN9]=            AVKEY_
	map[FSKEY_TENDOT]=          AVKEY_
	map[FSKEY_TENSLASH]=        AVKEY_
	map[FSKEY_TENSTAR]=         AVKEY_
	map[FSKEY_TENMINUS]=        AVKEY_
	map[FSKEY_TENPLUS]=         AVKEY_
	map[FSKEY_TENENTER]=        AVKEY_
	map[FSKEY_WHEELUP]=         AVKEY_
	map[FSKEY_WHEELDOWN]=       AVKEY_

	map[FSKEY_CONVERT]=         AVKEY_
	map[FSKEY_NONCONVERT]=      AVKEY_

	map[FSKEY_LEFT_CTRL]=       AVKEY_
	map[FSKEY_RIGHT_CTRL]=      AVKEY_
	map[FSKEY_LEFT_SHIFT]=      AVKEY_
	map[FSKEY_RIGHT_SHIFT]=     AVKEY_
	map[FSKEY_LEFT_ALT]=        AVKEY_
	map[FSKEY_RIGHT_ALT]=       AVKEY_
} */


////////////////////////////////////////////////////////////



FM77AVReverseKeyMap::FM77AVReverseKeyMap()
{
	for(auto &m : map)
	{
		m.fskey=0;
		m.label="";
	}
}
void FM77AVReverseKeyMap::MakeUS101KeyMap(void)
{
	for(auto &m : map)
	{
		m.fskey=0;
		m.label="";
	}

	Map(AVKEY_MID_SPACE,			FSKEY_SPACE,           " ");
	Map(AVKEY_0,					FSKEY_0,               "0");
	Map(AVKEY_1,					FSKEY_1,               "1");
	Map(AVKEY_2,					FSKEY_2,               "2");
	Map(AVKEY_3,					FSKEY_3,               "3");
	Map(AVKEY_4,					FSKEY_4,               "4");
	Map(AVKEY_5,					FSKEY_5,               "5");
	Map(AVKEY_6,					FSKEY_6,               "6");
	Map(AVKEY_7,					FSKEY_7,               "7");
	Map(AVKEY_8,					FSKEY_8,               "8");
	Map(AVKEY_9,					FSKEY_9,               "9");
	Map(AVKEY_A,					FSKEY_A,               "A");
	Map(AVKEY_B,					FSKEY_B,               "B");
	Map(AVKEY_C,					FSKEY_C,               "C");
	Map(AVKEY_D,					FSKEY_D,               "D");
	Map(AVKEY_E,					FSKEY_E,               "E");
	Map(AVKEY_F,					FSKEY_F,               "F");
	Map(AVKEY_G,					FSKEY_G,               "G");
	Map(AVKEY_H,					FSKEY_H,               "H");
	Map(AVKEY_I,					FSKEY_I,               "I");
	Map(AVKEY_J,					FSKEY_J,               "J");
	Map(AVKEY_K,					FSKEY_K,               "K");
	Map(AVKEY_L,					FSKEY_L,               "L");
	Map(AVKEY_M,					FSKEY_M,               "M");
	Map(AVKEY_N,					FSKEY_N,               "N");
	Map(AVKEY_O,					FSKEY_O,               "O");
	Map(AVKEY_P,					FSKEY_P,               "P");
	Map(AVKEY_Q,					FSKEY_Q,               "Q");
	Map(AVKEY_R,					FSKEY_R,               "R");
	Map(AVKEY_S,					FSKEY_S,               "S");
	Map(AVKEY_T,					FSKEY_T,               "T");
	Map(AVKEY_U,					FSKEY_U,               "U");
	Map(AVKEY_V,					FSKEY_V,               "V");
	Map(AVKEY_W,					FSKEY_W,               "W");
	Map(AVKEY_X,					FSKEY_X,               "X");
	Map(AVKEY_Y,					FSKEY_Y,               "Y");
	Map(AVKEY_Z,					FSKEY_Z,               "Z");
	Map(AVKEY_BREAK,				FSKEY_ESC,             nullptr);
	Map(AVKEY_PF1,					FSKEY_F1,              nullptr);
	Map(AVKEY_PF2,					FSKEY_F2,              nullptr);
	Map(AVKEY_PF3,					FSKEY_F3,              nullptr);
	Map(AVKEY_PF4,					FSKEY_F4,              nullptr);
	Map(AVKEY_PF5,					FSKEY_F5,              nullptr);
	Map(AVKEY_PF6,					FSKEY_F6,              nullptr);
	Map(AVKEY_PF7,					FSKEY_F7,              nullptr);
	Map(AVKEY_PF8,					FSKEY_F8,              nullptr);
	Map(AVKEY_PF9,					FSKEY_F9,              nullptr);
	Map(AVKEY_PF10,					FSKEY_F10,             nullptr);
	Map(AVKEY_LEFT_SPACE,			FSKEY_F11,             nullptr);
	Map(AVKEY_RIGHT_SPACE,			FSKEY_F12,             nullptr);
	Map(AVKEY_CAPS,					FSKEY_CAPSLOCK,        nullptr);
	Map(AVKEY_BACKSPACE,			FSKEY_BS,              nullptr);
	Map(AVKEY_TAB,					FSKEY_TAB,             "\t");
	Map(AVKEY_RETURN,				FSKEY_ENTER,           "\n");
	Map(AVKEY_INS,					FSKEY_INS,             nullptr);
	Map(AVKEY_DEL,					FSKEY_DEL,             nullptr);
	Map(AVKEY_HOME,					FSKEY_HOME,            nullptr);
	Map(AVKEY_DUP,					FSKEY_END,             nullptr);
	Map(AVKEY_UP,					FSKEY_UP,              nullptr);
	Map(AVKEY_DOWN,					FSKEY_DOWN,            nullptr);
	Map(AVKEY_LEFT,					FSKEY_LEFT,            nullptr);
	Map(AVKEY_RIGHT,				FSKEY_RIGHT,           nullptr);
	Map(AVKEY_ESC,					FSKEY_TILDA,           nullptr);
	Map(AVKEY_MINUS,				FSKEY_MINUS,           "-");
	Map(AVKEY_HAT,					FSKEY_PLUS,            "=");
	Map(AVKEY_AT,					FSKEY_LBRACKET,        "[");
	Map(AVKEY_LEFT_SQUARE_BRACKET,	FSKEY_RBRACKET,        "]");
	Map(AVKEY_RIGHT_SQUARE_BRACKET,	FSKEY_NULL,		       "Not Exist!(])"); // This key doesn't exist in the US keyboard
	Map(AVKEY_YEN,					FSKEY_BACKSLASH,       "\\");
	Map(AVKEY_SEMICOLON,			FSKEY_SEMICOLON,       ";");
	Map(AVKEY_COLON,				FSKEY_SINGLEQUOTE,     "\'");
	Map(AVKEY_COMMA,				FSKEY_COMMA,           ",");
	Map(AVKEY_DOT,					FSKEY_DOT,             ".");
	Map(AVKEY_SLASH,				FSKEY_SLASH,           "/");
	Map(AVKEY_DOUBLE_QUOTE,			FSKEY_BACKSLASH,       "Not Exist!(\")");  // This key doesn't exist in the US keyboard.
	Map(AVKEY_NUM_0,				FSKEY_TEN0,            "0");
	Map(AVKEY_NUM_1,				FSKEY_TEN1,            "1");
	Map(AVKEY_NUM_2,				FSKEY_TEN2,            "2");
	Map(AVKEY_NUM_3,				FSKEY_TEN3,            "3");
	Map(AVKEY_NUM_4,				FSKEY_TEN4,            "4");
	Map(AVKEY_NUM_5,				FSKEY_TEN5,            "5");
	Map(AVKEY_NUM_6,				FSKEY_TEN6,            "6");
	Map(AVKEY_NUM_7,				FSKEY_TEN7,            "7");
	Map(AVKEY_NUM_8,				FSKEY_TEN8,            "8");
	Map(AVKEY_NUM_9,				FSKEY_TEN9,            "9");
	Map(AVKEY_NUM_DOT,				FSKEY_TENDOT,          ".");
	Map(AVKEY_NUM_SLASH,			FSKEY_TENSLASH,        "/");
	Map(AVKEY_NUM_STAR,				FSKEY_TENSTAR,         "*");
	Map(AVKEY_NUM_MINUS,			FSKEY_TENMINUS,        "-");
	Map(AVKEY_NUM_PLUS,				FSKEY_TENPLUS,         "+");
	Map(AVKEY_NUM_RETURN,			FSKEY_TENENTER,        "\n");
				
	Map(AVKEY_LEFT_SPACE,			FSKEY_CONVERT,         " ");
	Map(AVKEY_RIGHT_SPACE,			FSKEY_NONCONVERT,      " ");
				
	Map(AVKEY_CTRL,					FSKEY_LEFT_CTRL,       nullptr);
	Map(AVKEY_GRAPH,				FSKEY_RIGHT_CTRL,      nullptr);
	Map(AVKEY_LEFT_SHIFT,			FSKEY_LEFT_SHIFT,      nullptr);
	Map(AVKEY_RIGHT_SHIFT,			FSKEY_RIGHT_SHIFT,     nullptr);
}
void FM77AVReverseKeyMap::MakeJISKeyMap(void)
{
	for(auto &m : map)
	{
		m.fskey=0;
		m.label="";
	}

	for(auto &m : map)
	{
		m.fskey=0;
		m.label="";
	}

	Map(AVKEY_MID_SPACE,			FSKEY_SPACE,           " ");
	Map(AVKEY_0,					FSKEY_0,               "0");
	Map(AVKEY_1,					FSKEY_1,               "1");
	Map(AVKEY_2,					FSKEY_2,               "2");
	Map(AVKEY_3,					FSKEY_3,               "3");
	Map(AVKEY_4,					FSKEY_4,               "4");
	Map(AVKEY_5,					FSKEY_5,               "5");
	Map(AVKEY_6,					FSKEY_6,               "6");
	Map(AVKEY_7,					FSKEY_7,               "7");
	Map(AVKEY_8,					FSKEY_8,               "8");
	Map(AVKEY_9,					FSKEY_9,               "9");
	Map(AVKEY_A,					FSKEY_A,               "A");
	Map(AVKEY_B,					FSKEY_B,               "B");
	Map(AVKEY_C,					FSKEY_C,               "C");
	Map(AVKEY_D,					FSKEY_D,               "D");
	Map(AVKEY_E,					FSKEY_E,               "E");
	Map(AVKEY_F,					FSKEY_F,               "F");
	Map(AVKEY_G,					FSKEY_G,               "G");
	Map(AVKEY_H,					FSKEY_H,               "H");
	Map(AVKEY_I,					FSKEY_I,               "I");
	Map(AVKEY_J,					FSKEY_J,               "J");
	Map(AVKEY_K,					FSKEY_K,               "K");
	Map(AVKEY_L,					FSKEY_L,               "L");
	Map(AVKEY_M,					FSKEY_M,               "M");
	Map(AVKEY_N,					FSKEY_N,               "N");
	Map(AVKEY_O,					FSKEY_O,               "O");
	Map(AVKEY_P,					FSKEY_P,               "P");
	Map(AVKEY_Q,					FSKEY_Q,               "Q");
	Map(AVKEY_R,					FSKEY_R,               "R");
	Map(AVKEY_S,					FSKEY_S,               "S");
	Map(AVKEY_T,					FSKEY_T,               "T");
	Map(AVKEY_U,					FSKEY_U,               "U");
	Map(AVKEY_V,					FSKEY_V,               "V");
	Map(AVKEY_W,					FSKEY_W,               "W");
	Map(AVKEY_X,					FSKEY_X,               "X");
	Map(AVKEY_Y,					FSKEY_Y,               "Y");
	Map(AVKEY_Z,					FSKEY_Z,               "Z");
	Map(AVKEY_BREAK,				FSKEY_ESC,             nullptr);
	Map(AVKEY_PF1,					FSKEY_F1,              nullptr);
	Map(AVKEY_PF2,					FSKEY_F2,              nullptr);
	Map(AVKEY_PF3,					FSKEY_F3,              nullptr);
	Map(AVKEY_PF4,					FSKEY_F4,              nullptr);
	Map(AVKEY_PF5,					FSKEY_F5,              nullptr);
	Map(AVKEY_PF6,					FSKEY_F6,              nullptr);
	Map(AVKEY_PF7,					FSKEY_F7,              nullptr);
	Map(AVKEY_PF8,					FSKEY_F8,              nullptr);
	Map(AVKEY_PF9,					FSKEY_F9,              nullptr);
	Map(AVKEY_PF10,					FSKEY_F10,             nullptr);
	Map(AVKEY_LEFT_SPACE,			FSKEY_F11,             nullptr);
	Map(AVKEY_RIGHT_SPACE,			FSKEY_F12,             nullptr);
	Map(AVKEY_CAPS,					FSKEY_CAPSLOCK,        nullptr);
	Map(AVKEY_BACKSPACE,			FSKEY_BS,              nullptr);
	Map(AVKEY_TAB,					FSKEY_TAB,             "\t");
	Map(AVKEY_RETURN,				FSKEY_ENTER,           "\n");
	Map(AVKEY_INS,					FSKEY_INS,             nullptr);
	Map(AVKEY_DEL,					FSKEY_DEL,             nullptr);
	Map(AVKEY_HOME,					FSKEY_HOME,            nullptr);
	Map(AVKEY_DUP,					FSKEY_END,             nullptr);
	Map(AVKEY_UP,					FSKEY_UP,              nullptr);
	Map(AVKEY_DOWN,					FSKEY_DOWN,            nullptr);
	Map(AVKEY_LEFT,					FSKEY_LEFT,            nullptr);
	Map(AVKEY_RIGHT,				FSKEY_RIGHT,           nullptr);
	Map(AVKEY_ESC,					FSKEY_TILDA,           nullptr);
	Map(AVKEY_MINUS,				FSKEY_MINUS,           "-");
	Map(AVKEY_HAT,					FSKEY_PLUS,            "^");
	Map(AVKEY_AT,					FSKEY_LBRACKET,        "@");
	Map(AVKEY_LEFT_SQUARE_BRACKET,	FSKEY_RBRACKET,        "[");
	Map(AVKEY_RIGHT_SQUARE_BRACKET,	FSKEY_RBRACKET,        "]");
	Map(AVKEY_YEN,					FSKEY_BACKSLASH,       "\\(YEN)");
	Map(AVKEY_SEMICOLON,			FSKEY_SEMICOLON,       ";");
	Map(AVKEY_COLON,				FSKEY_SINGLEQUOTE,     ":");
	Map(AVKEY_COMMA,				FSKEY_COMMA,           ",");
	Map(AVKEY_DOT,					FSKEY_DOT,             ".");
	Map(AVKEY_SLASH,				FSKEY_SLASH,           "/");
	Map(AVKEY_DOUBLE_QUOTE,			FSKEY_BACKSLASH,       "\"(\\)");
	Map(AVKEY_NUM_0,				FSKEY_TEN0,            "0");
	Map(AVKEY_NUM_1,				FSKEY_TEN1,            "1");
	Map(AVKEY_NUM_2,				FSKEY_TEN2,            "2");
	Map(AVKEY_NUM_3,				FSKEY_TEN3,            "3");
	Map(AVKEY_NUM_4,				FSKEY_TEN4,            "4");
	Map(AVKEY_NUM_5,				FSKEY_TEN5,            "5");
	Map(AVKEY_NUM_6,				FSKEY_TEN6,            "6");
	Map(AVKEY_NUM_7,				FSKEY_TEN7,            "7");
	Map(AVKEY_NUM_8,				FSKEY_TEN8,            "8");
	Map(AVKEY_NUM_9,				FSKEY_TEN9,            "9");
	Map(AVKEY_NUM_DOT,				FSKEY_TENDOT,          ".");
	Map(AVKEY_NUM_SLASH,			FSKEY_TENSLASH,        "/");
	Map(AVKEY_NUM_STAR,				FSKEY_TENSTAR,         "*");
	Map(AVKEY_NUM_MINUS,			FSKEY_TENMINUS,        "-");
	Map(AVKEY_NUM_PLUS,				FSKEY_TENPLUS,         "+");
	Map(AVKEY_NUM_RETURN,			FSKEY_TENENTER,        "\n");
				
	Map(AVKEY_LEFT_SPACE,			FSKEY_CONVERT,         " ");
	Map(AVKEY_RIGHT_SPACE,			FSKEY_NONCONVERT,      " ");
				
	Map(AVKEY_CTRL,					FSKEY_LEFT_CTRL,       nullptr);
	Map(AVKEY_GRAPH,				FSKEY_RIGHT_CTRL,      nullptr);
	Map(AVKEY_LEFT_SHIFT,			FSKEY_LEFT_SHIFT,      nullptr);
	Map(AVKEY_RIGHT_SHIFT,			FSKEY_RIGHT_SHIFT,     nullptr);
}
void FM77AVReverseKeyMap::Map(int avkey,int fskey,const char *keyLabel)
{
	map[avkey].fskey=fskey;
	map[avkey].label=keyLabel;
}

const char *FM77AVReverseKeyMap::GetKeyLabel(int avkey)
{
	if(0<=avkey && avkey<AVKEY_NUM_KEYCODE)
	{
		return map[avkey].label;
	}
	return nullptr;
}
