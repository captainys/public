#include "fssimplewindow.h"
#include <stdio.h>



int main(void)
{
	FsOpenWindow(0,0,800,600,0);

	printf("Press and hold SPACE key to force the mouse cursor to be the center of the window.\n");

	for(;;)
	{
		FsPollDevice();
		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}

		int wid,hei;
		FsGetWindowSize(wid,hei);
		if(FsGetKeyState(FSKEY_SPACE))
		{
			FsSetMousePosition(wid/2,hei/2);
		}
	}

	return 0;
}
