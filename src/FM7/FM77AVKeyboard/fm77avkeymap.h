#ifndef FM77AVKEYMAP_IS_INCLUDED
#define FM77AVKEYMAP_IS_INCLUDED
/* { */

#include <fssimplewindow.h>
#include "fm77avkey.h"

class FM77AVKeyMap
{
public:
	int map[FSKEY_NUM_KEYCODE];

	FM77AVKeyMap();
	void MakeUS101KeyMap(void);
	void ZXCForLMRSpzce(void);
	void ZXCForZXC(void);
	void ArrowFor2468(void);
	void ArrowForArrow(void);
	void ArrowForNone(void);
};



/*! For mapping AVKEY_? to modern keyboard.
*/
class FM77AVReverseKeyMap
{
public:
	class ModernKey
	{
	public:
		int fskey;
		const char *label;
	};

	ModernKey map[AVKEY_NUM_KEYCODE];

	FM77AVReverseKeyMap();
	void MakeUS101KeyMap(void);
	void MakeJISKeyMap(void);
	void Map(int avkey,int fskey,const char *keyLabel);

	const char *GetKeyLabel(int avkey);
};

/* } */
#endif
