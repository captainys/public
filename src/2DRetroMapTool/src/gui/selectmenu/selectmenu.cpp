#include <ysclass.h>
#include <yscompilerwarning.h>
#include <ysgl.h>
#include <ysport.h>

#include "../fsgui3dapp.h"
#include <fsguifiledialog.h>



void FsGui3DMainCanvas::Select_SelectAll(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	auto fieldPtr=world.GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		for(auto mpHd : fieldPtr->AllMapPiece())
		{
			world.AddSelectedMapPiece(fdHd,mpHd);
		}
		for(auto muHd : fieldPtr->AllMarkUp())
		{
			world.AddSelectedMarkUp(fdHd,muHd);
		}
	}
	world.ReadyVbo(fdHd);
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::Select_UnselectAll(FsGuiPopUpMenuItem *)
{
	world.UnselectAllMapPiece(GetCurrentField());
	world.UnselectAllMarkUp(GetCurrentField());
	world.ReadyVbo(GetCurrentField());
	SetNeedRedraw(YSTRUE);
}

