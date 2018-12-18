#ifndef FM77AVKEY_IS_INCLUDED
#define FM77AVKEY_IS_INCLUDED
/* { */

#include <vector>
#include <string>

enum
{
	AVKEY_NULL,

	AVKEY_BREAK,
	AVKEY_PF1,
	AVKEY_PF2,
	AVKEY_PF3,
	AVKEY_PF4,
	AVKEY_PF5,
	AVKEY_PF6,
	AVKEY_PF7,
	AVKEY_PF8,
	AVKEY_PF9,
	AVKEY_PF10,
	AVKEY_EL,
	AVKEY_CLS,
	AVKEY_DUP,
	AVKEY_HOME,
	AVKEY_INS,
	AVKEY_DEL,
	AVKEY_LEFT,
	AVKEY_RIGHT,
	AVKEY_UP,
	AVKEY_DOWN,

	AVKEY_ESC,
	AVKEY_0,
	AVKEY_1,
	AVKEY_2,
	AVKEY_3,
	AVKEY_4,
	AVKEY_5,
	AVKEY_6,
	AVKEY_7,
	AVKEY_8,
	AVKEY_9,
	AVKEY_MINUS,
	AVKEY_HAT,
	AVKEY_YEN,
	AVKEY_BACKSPACE,

	AVKEY_TAB,
	AVKEY_Q,
	AVKEY_W,
	AVKEY_E,
	AVKEY_R,
	AVKEY_T,
	AVKEY_Y,
	AVKEY_U,
	AVKEY_I,
	AVKEY_O,
	AVKEY_P,
	AVKEY_AT,
	AVKEY_LEFT_SQUARE_BRACKET,
	AVKEY_RETURN,

	AVKEY_CTRL,
	AVKEY_A,
	AVKEY_S,
	AVKEY_D,
	AVKEY_F,
	AVKEY_G,
	AVKEY_H,
	AVKEY_J,
	AVKEY_K,
	AVKEY_L,
	AVKEY_SEMICOLON,
	AVKEY_COLON,
	AVKEY_RIGHT_SQUARE_BRACKET,

	AVKEY_LEFT_SHIFT,
	AVKEY_Z,
	AVKEY_X,
	AVKEY_C,
	AVKEY_V,
	AVKEY_B,
	AVKEY_N,
	AVKEY_M,
	AVKEY_COMMA,
	AVKEY_DOT,
	AVKEY_SLASH,
	AVKEY_DOUBLE_QUOTE,
	AVKEY_RIGHT_SHIFT,

	AVKEY_CAPS,
	AVKEY_GRAPH,
	AVKEY_LEFT_SPACE,
	AVKEY_MID_SPACE,
	AVKEY_RIGHT_SPACE,
	AVKEY_KANA,

	AVKEY_NUM_STAR,
	AVKEY_NUM_SLASH,
	AVKEY_NUM_PLUS,
	AVKEY_NUM_MINUS,
	AVKEY_NUM_EQUAL,
	AVKEY_NUM_COMMA,
	AVKEY_NUM_RETURN,
	AVKEY_NUM_DOT,
	AVKEY_NUM_0,
	AVKEY_NUM_1,
	AVKEY_NUM_2,
	AVKEY_NUM_3,
	AVKEY_NUM_4,
	AVKEY_NUM_5,
	AVKEY_NUM_6,
	AVKEY_NUM_7,
	AVKEY_NUM_8,
	AVKEY_NUM_9,

AVKEY_NUM_KEYCODE
};

typedef struct
{
	int keyCode;
	bool shift,ctrl,graph;
} FM77AVKeyCombination;

class FM77AVKeyEvent
{
public:
	bool pressEvent; // If false, it is a release event.
	int keyCode;

	FM77AVKeyEvent(){};
	FM77AVKeyEvent(bool pressEvent,int keyCode)
	{
		this->pressEvent=pressEvent;
		this->keyCode=keyCode;
	}
};

/*! Translates a label to key code.  For auto-typing, str[] can be terminated with 0, 0x0d, 0x0a, or '\\'.
*/
int FM77AVKeyLabelToKeyCode(const char str[]);

FM77AVKeyCombination FM77AVTranslateCharToCode(char c);

const char *FM77AVGetKeyPress30BitPattern(int fm77avkey);
std::string FM77AVGetKeyRelease30BitPattern(int fm77avkey);

std::string FM77AVMake30BitPattern(const FM77AVKeyEvent keyEvent);

unsigned int FM77AVGet1stByteIn30bitEncoding(const char code30[]);
void FM77AVPut1stByteIn30bitEncoding(char code30[],unsigned int byteData);

void FM77AV30bitTo40bit(char code40[41],const char code30[]);

/* } */
#endif
