#include "fsgui3dapp.h"

static FsGui3DMainCanvas *appPtr=nullptr;

/* static */ FsGui3DMainCanvas *FsGui3DMainCanvas::GetMainCanvas()
{
	if(nullptr==appPtr)
	{
		appPtr=new FsGui3DMainCanvas;
	}
	return appPtr;
}

/* static */ void FsGui3DMainCanvas::DeleteMainCanvas(void)
{
	if(nullptr!=appPtr)
	{
		delete appPtr;
		appPtr=nullptr;
	}
}
