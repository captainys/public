/* ////////////////////////////////////////////////////////////

File Name: fsguipopupmenu.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>


#ifdef _WIN32
	#ifndef _WINSOCKAPI_
		// Prevent inclusion of winsock.h
		#define _WINSOCKAPI_
		#include <windows.h>
		#undef _WINSOCKAPI_
	#else
		// Too late.  Just include it.
		#include <windows.h>
	#endif
#else  // Assuming UNIX
	#include <sys/time.h>
#endif



#include <ysclass.h>

#include <fssimplewindow.h>
#include "fsguiclipboard.h"

#include <ysbitmap.h>

#include "fsguipopupmenu.h"
#include "fsguidialog.h"



#include <time.h>




FsGuiPopUpMenuItem::FsGuiPopUpMenuItem()
{
	subMenu=NULL;
	owner=NULL;
	Initialize();
}

FsGuiPopUpMenuItem::~FsGuiPopUpMenuItem()
{
	if(subMenu!=NULL)
	{
		delete subMenu;
	}
}

void FsGuiPopUpMenuItem::Initialize(void)
{
	id=0;
	fsKey=FSKEY_NULL;
	checked=YSFALSE;
	isText=YSTRUE;
	drawingType=FSGUI_DRAWINGTYPE_NORMAL;

	wStr.Set(NULL);
	rightAlignedWStr.Set(NULL);

	msgWid=0;
	msgHei=0;
	rightMsgWid=0;
	rightMsgHei=0;


	x0=0;
	y0=0;
	wid=0;
	hei=0;

	bmp.CleanUp();
	rightAlignedBmp.CleanUp();

	if(subMenu!=NULL)
	{
		delete subMenu;
		subMenu=NULL;
	}

	selectionCallBack=NULL;
	selectionCallBackParam=NULL;
}

FsGuiPopUpMenu *FsGuiPopUpMenuItem::AddSubMenu(int left,int top)
{
	if(subMenu==NULL)
	{
		subMenu=new FsGuiPopUpMenu;
		subMenu->x0=left;
		subMenu->y0=top;
		subMenu->lastX=left;
		subMenu->lastY=top;
		subMenu->owner=this;
	}
	return subMenu;
}

FsGuiPopUpMenu *FsGuiPopUpMenuItem::AddSubMenu(void)
{
	if(NULL!=owner)
	{
		return AddSubMenu(owner->x0+owner->wid,y0);
	}
	else
	{
		return AddSubMenu(x0+wid,y0);
	}
}

FsGuiPopUpMenu *FsGuiPopUpMenuItem::GetSubMenu(void)
{
	return subMenu;
}

const FsGuiPopUpMenu *FsGuiPopUpMenuItem::GetSubMenu(void) const
{
	return subMenu;
}

const wchar_t *FsGuiPopUpMenuItem::GetString(void) const
{
	return wStr;
}

/* static */ void FsGuiPopUpMenuItem::SetCheck(FsGuiPopUpMenuItem *item,YSBOOL checked)
{
	if(nullptr!=item)
	{
		item->SetCheck(checked);
	}
}

void FsGuiPopUpMenuItem::SetCheck(YSBOOL checked)
{
	this->checked=checked;
}

/* static */ const YSBOOL FsGuiPopUpMenuItem::GetCheck(const FsGuiPopUpMenuItem *itm)
{
	if(nullptr!=itm)
	{
		return itm->GetCheck();
	}
	return YSFALSE;
}

const YSBOOL FsGuiPopUpMenuItem::GetCheck(void) const
{
	return checked;
}

void FsGuiPopUpMenuItem::InvertCheck(void)
{
	YsFlip(checked);
}

const int FsGuiPopUpMenuItem::GetId(void) const
{
	return id;
}

FsGuiPopUpMenu::FsGuiPopUpMenu() : itemList(itemAlloc)
{
	Initialize();
}

FsGuiPopUpMenu::~FsGuiPopUpMenu()
{
}

void FsGuiPopUpMenu::SetIsPullDownMenu(YSBOOL isPulLDown)
{
	pullDown=isPulLDown;
}

YSBOOL FsGuiPopUpMenu::IsPullDownMenu(void) const
{
	return pullDown;
}

void FsGuiPopUpMenu::SetNeedRedraw(YSBOOL needRedraw)
{
	this->needRedraw=needRedraw;
}

YSBOOL FsGuiPopUpMenu::NeedRedraw(void) const
{
	if(NULL!=GetModalDialog() && YSTRUE==GetModalDialog()->NeedRedraw())
	{
		return YSTRUE;
	}

	return needRedraw;
}

void FsGuiPopUpMenu::Initialize(void)
{
	ClearStringToIdentTable();

	itemList.CleanUp();

	x0=0;
	y0=0;
	wid=0;
	hei=0;
	pullDown=YSFALSE;

	lastX=0;
	lastY=0;
	lastItem=NULL;

	bgCol=defBgCol;
	fgCol=defFgCol;

	activeBgCol=defActiveBgCol;
	activeFgCol=defActiveFgCol;

	frameCol=defFrameCol;

	pMx=0;
	pMy=0;
	pLb=YSFALSE;
	pMb=YSFALSE;
	pRb=YSFALSE;

	needRedraw=YSTRUE;

	mouseOn=NULL;
	openSubMenu=NULL;
	owner=NULL;
}

void FsGuiPopUpMenu::FitSubMenuToWindow(FsGuiPopUpMenu *subMenu)
{
	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);
	if(YSTRUE==IsPullDownMenu())
	{
		if(winWid<subMenu->x0+subMenu->wid)
		{
			MoveSubMenu(subMenu,winWid-subMenu->wid,subMenu->y0);
		}
	}
	else
	{
		if(winWid<subMenu->x0+subMenu->wid)
		{
			// Use this->x0-subMenu->wid instead of winWid-subMenu->wid to avoid accidental menu selection by
			// immediately releasing the mouse button after opening the sub-menu.
			MoveSubMenu(subMenu,this->x0-subMenu->wid,subMenu->y0);
		}
	}
	if(0>subMenu->x0)
	{
		MoveSubMenu(subMenu,0,subMenu->y0);
	}
}

const FsGuiPopUpMenuItem *FsGuiPopUpMenu::FindMenuItem(int ident) const
{
	return FindMenuItem(this,ident);
}

const FsGuiPopUpMenuItem *FsGuiPopUpMenu::FindMenuItem(const FsGuiPopUpMenu *menuPtr,int ident) const
{
	for(auto &item : itemList)
	{
		auto subMenu=item.GetSubMenu();
		if(nullptr!=subMenu)
		{
			auto found=FindMenuItem(subMenu,ident);
			if(nullptr!=found)
			{
				return found;
			}
		}
		else if(item.GetId()==ident)
		{
			return &item;
		}
	}
	return nullptr;
}

FsGuiPopUpMenuItem *FsGuiPopUpMenu::FindMenuItem(int ident)
{
	return FindMenuItem(this,ident);
}

FsGuiPopUpMenuItem *FsGuiPopUpMenu::FindMenuItem(FsGuiPopUpMenu *menuPtr,int ident)
{
	for(auto &item : menuPtr->itemList)
	{
		auto subMenu=item.GetSubMenu();
		if(nullptr!=subMenu)
		{
			auto found=FindMenuItem(subMenu,ident);
			if(nullptr!=found)
			{
				return found;
			}
		}
		else if(item.GetId()==ident)
		{
			return &item;
		}
	}
	return nullptr;
}

void FsGuiPopUpMenu::CleanUp(void)
{
	itemList.CleanUp();
	lastX=x0;
	lastY=y0;

	pMx=0;
	pMy=0;
	pLb=YSFALSE;
	pMb=YSFALSE;
	pRb=YSFALSE;

	needRedraw=YSTRUE;

	mouseOn=NULL;
	openSubMenu=NULL;
}

YSRESULT FsGuiPopUpMenu::FitPullDownMenu(int wid)
{
	if(YSTRUE==pullDown)
	{
		int x0,y0,nextY0;
		int totalWid=0,totalHei=0;

		x0=0;
		y0=0;
		nextY0=0;
		for(auto &item : itemList)
		{
			if(0<x0 && wid<x0+item.wid)
			{
				x0=0;
				y0=nextY0;
			}

			const int dx=x0-item.x0;
			const int dy=y0-item.y0;

			item.x0=x0;
			item.y0=y0;

			if(NULL!=item.subMenu)
			{
				MoveSubMenuByOffset(item.subMenu,dx,dy);
			}

			nextY0=YsGreater(nextY0,y0+item.hei);
			x0+=item.wid;

			totalWid=YsGreater(totalWid,x0);
			totalHei=YsGreater(totalHei,nextY0);
		}

		this->wid=totalWid;
		this->hei=totalHei;

		return YSOK;
	}
	return YSERR;
}

void FsGuiPopUpMenu::MoveSubMenu(FsGuiPopUpMenu *subMenu,int x0,int y0)
{
	const int dx=x0-subMenu->x0;
	const int dy=y0-subMenu->y0;
	MoveSubMenuByOffset(subMenu,dx,dy);
}

void FsGuiPopUpMenu::MoveSubMenuByOffset(FsGuiPopUpMenu *subMenu,int dx,int dy)
{
	subMenu->x0+=dx;
	subMenu->y0+=dy;

	for(auto &item : subMenu->itemList)
	{
		item.x0+=dx;
		item.y0+=dy;

		if(NULL!=item.subMenu)
		{
			MoveSubMenuByOffset(item.subMenu,dx,dy);
		}
	}
}

FsGuiPopUpMenuItem *FsGuiPopUpMenu::GetLastSelectedItem(void)
{
	FsGuiPopUpMenuItem *ret=lastItem;
	lastItem=NULL;
	return ret;
}

template <class CHARTYPE,class STRTYPE>
static void FsDecomposeStringByTab(STRTYPE &left,STRTYPE &right,const CHARTYPE src[])
{
	const CHARTYPE empty[1]={0};

	left.Set(empty);
	right.Set(empty);

	int i;
	for(i=0; 0!=src[i]; i++)
	{
		if('\t'==src[i])
		{
			i++;
			break;
		}
		left.Append(src[i]);
	}

	right.Set(src+i);
}

FsGuiPopUpMenuItem *FsGuiPopUpMenu::AddTextItem(int menuId,int fsKey,const char label[],void (*callback)(void *,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *),void *contextPtr)
{
	YsWString wLabel,wLabelLeft,wLabelRight;

	if(NULL!=label)
	{
		wLabel.SetUTF8String(label);
	}
	return AddTextItem(menuId,fsKey,wLabel,callback,contextPtr);
}

FsGuiPopUpMenuItem *FsGuiPopUpMenu::AddTextItem(int menuId,int fsKey,const wchar_t wLabel[],void (*callback)(void *,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *),void *contextPtr)
{
	YsListItem <FsGuiPopUpMenuItem> *newItem=itemList.Create();
	newItem->dat.Initialize();
	newItem->dat.selectionCallBack=callback;
	newItem->dat.selectionCallBackParam=contextPtr;

	YsWString wLabelLeft,wLabelRight;

	if(NULL!=wLabel)
	{
		FsDecomposeStringByTab <wchar_t,YsWString> (wLabelLeft,wLabelRight,wLabel);
	}
	else
	{
		const wchar_t space[]={' ',0}; // Workaround to cope with 64-bit GCC bug in Mac OSX
		wLabelLeft.Set(space);
		wLabelRight.Set(NULL);
	}

	if(FSKEY_NULL!=fsKey)
	{
		const char *keyLabel=FsKeyCodeToString(fsKey);
		if(NULL!=keyLabel)
		{
			wLabelLeft.Append('(');
			for(int i=0; 0!=keyLabel[i]; ++i)
			{
				wLabelLeft.Append(keyLabel[i]);
			}
			wLabelLeft.Append(')');
		}
	}

	YsBitmap bmpL;
	if(YSOK==RenderUnicodeString(bmpL,wLabelLeft,defFgCol,defBgCol) ||
	   YSOK==RenderAsciiString(bmpL,wLabelLeft,defFgCol,defBgCol))
	{
		newItem->dat.bmp.SetAndBreakUp(bmpL,defBitmapWidthLimit,defBitmapHeightLimit);
		if(0<wLabelRight.Strlen())
		{
			YsBitmap bmpR;
			if(YSOK==RenderUnicodeString(bmpR,wLabelRight,defFgCol,defBgCol) ||
			   YSOK==RenderAsciiString(bmpR,wLabelRight,defFgCol,defBgCol))
			{
				newItem->dat.rightAlignedBmp.SetAndBreakUp(bmpR,defBitmapWidthLimit,defBitmapHeightLimit);
			}
		}
	}
	else
	{
		bmpL.PrepareBitmap(8,8);
		newItem->dat.bmp.SetAndBreakUp(bmpL,8,8); // At least crash must be prevented.
	}

	const int msgWid=newItem->dat.bmp.GetWidth()+newItem->dat.rightAlignedBmp.GetWidth();
	const int msgHei=YsGreater(newItem->dat.bmp.GetHeight(),newItem->dat.rightAlignedBmp.GetHeight());

	const int rightMsgWid=newItem->dat.rightAlignedBmp.GetWidth();
	const int rightMsgHei=newItem->dat.rightAlignedBmp.GetHeight();

	newItem->dat.isText=YSTRUE;
	newItem->dat.drawingType=FSGUI_DRAWINGTYPE_NORMAL;

	SetUpMenuItem(&newItem->dat,menuId,fsKey,wLabel,msgWid,msgHei,rightMsgWid,rightMsgHei);
	return &newItem->dat;
}

FsGuiPopUpMenuItem *FsGuiPopUpMenu::AddBmpItem(int menuId,int fsKey,const YsBitmap &bmp,const wchar_t altLabel[],void (*callback)(void *,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *),void *contextPtr)
{
	YsListItem <FsGuiPopUpMenuItem> *newItem;

	newItem=itemList.Create();
	newItem->dat.Initialize();
	newItem->dat.selectionCallBack=callback;
	newItem->dat.selectionCallBackParam=contextPtr;
	newItem->dat.bmp.SetAndBreakUp(bmp,defBitmapWidthLimit,defBitmapHeightLimit);

	const int msgWid=newItem->dat.bmp.GetWidth();
	const int msgHei=newItem->dat.bmp.GetHeight();

	newItem->dat.isText=YSFALSE;

	SetUpMenuItem(&newItem->dat,menuId,fsKey,altLabel,msgWid,msgHei,0,0);
	return &newItem->dat;
}

void FsGuiPopUpMenu::SetUpMenuItem(
    FsGuiPopUpMenuItem *item,int id,int fsKey,const wchar_t label[],int msgWid,int msgHei,int rightMsgWid,int rightMsgHei)
{
	item->id=id;
	item->fsKey=fsKey;
	item->wStr.Set(label);

	item->msgWid=msgWid;
	item->msgHei=msgHei;
	item->rightMsgWid=rightMsgWid;
	item->rightMsgHei=rightMsgHei;

	item->x0=lastX;
	item->y0=lastY;

	item->owner=this;

	if(pullDown==YSTRUE)
	{
		item->wid=msgWid+defHSpaceUnit;
		item->hei=msgHei+defVSpaceUnit/4;

		lastX+=item->wid;
		wid+=item->wid;
		hei=YsGreater(hei,item->hei);

		item->AddSubMenu(item->x0,item->y0+hei);
	}
	else
	{
		const int leftSpace=defHSpaceUnit+defHAnnotation;
		const int rightSpace=defHSpaceUnit*2+defHAnnotation;
		item->wid=msgWid+leftSpace+rightSpace;
		item->hei=msgHei+defVSpaceUnit/4;

		lastY+=item->hei;
		wid=YsGreater(wid,item->wid);
		hei=YsGreater(hei,(lastY-y0)+defVSpaceUnit/4);
	}

}

FsGuiPopUpMenuItem *FsGuiPopUpMenu::FindItemInRange(int mx,int my) const
{
	YsListItem <FsGuiPopUpMenuItem> *ptr;
	if(pullDown==YSTRUE)
	{
		if(y0<=my && my<y0+hei)
		{
			ptr=NULL;
			while(NULL!=(ptr=itemList.FindNext(ptr)))
			{
				if(ptr->dat.x0<=mx && mx<ptr->dat.x0+ptr->dat.wid &&
				   ptr->dat.y0<=my && my<ptr->dat.y0+ptr->dat.hei)
				{
					return &ptr->dat;
				}
			}
		}
	}
	else
	{
		if(x0<=mx && mx<x0+wid)
		{
			ptr=NULL;
			while(NULL!=(ptr=itemList.FindNext(ptr)))
			{
				if(ptr->dat.y0<=my && my<ptr->dat.y0+ptr->dat.hei)
				{
					return &ptr->dat;
				}
			}
		}
	}
	return NULL;
}

YSBOOL FsGuiPopUpMenu::IsMouseOnOpenSubMenu(int mx,int my) const
{
	auto openItem=GetOpenSubMenu();
	while(NULL!=openItem)
	{
		auto openSubMenu=openItem->GetSubMenu();
		if(NULL!=openSubMenu && 
		   openSubMenu->x0<=mx && mx<openSubMenu->x0+openSubMenu->wid &&
		   openSubMenu->y0<=my && my<openSubMenu->y0+openSubMenu->hei)
		{
			return YSTRUE;
		}
		openItem=openSubMenu->GetOpenSubMenu();
	}
	return YSFALSE;
}

FsGuiPopUpMenu *FsGuiPopUpMenu::GetTopMostMenu(void)
{
	FsGuiPopUpMenu *ptr;
	ptr=this;
	while(NULL!=ptr)
	{
		if(ptr->owner!=NULL && ptr->owner->owner!=NULL)
		{
			ptr=ptr->owner->owner;
		}
		else
		{
			return ptr;
		}
	}
	return NULL;
}

int FsGuiPopUpMenu::GetWidth(void) const
{
	return wid;
}

int FsGuiPopUpMenu::GetHeight(void) const
{
	return hei;
}

void FsGuiPopUpMenu::SetWidth(int wid)
{
	this->wid=wid;
}

const YsColor &FsGuiPopUpMenu::GetBackgroundColor(void) const
{
	return bgCol;
}

void FsGuiPopUpMenu::SetBackgroundColor(const YsColor &newBackgroundColor)
{
	if(bgCol!=newBackgroundColor)
	{
		bgCol=newBackgroundColor;
		needRedraw=YSTRUE;
	}
}

YSBOOL FsGuiPopUpMenu::IsMouseOnMenu(int mx,int my) const
{
	if(NULL!=FindItemInRange(mx,my))
	{
		return YSTRUE;
	}
	else if(NULL!=openSubMenu && NULL!=openSubMenu->GetSubMenu())
	{
		return openSubMenu->GetSubMenu()->IsMouseOnMenu(mx,my);
	}
	return YSFALSE;
}

YSRESULT FsGuiPopUpMenu::SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	YSRESULT processed;

	if((mx!=pMx || my!=pMy) && 0==prevTouch.GetN())
	{
		SetMouseOnItem(mx,my);
	}

	processed=YSERR;

	FsGuiPopUpMenuItem *onItem=NULL;
	if(pLb!=YSTRUE && lb==YSTRUE && 0==prevTouch.GetN())
	{
		processed=MouseButtonPressEvent(onItem,mx,my);
		if(onItem==NULL)
		{
			CloseSubMenu();
		}
		else
		{
			processed=YSOK; // The message must not be passed to the client.  2011/08/18
		}
	}
	else if(pLb==YSTRUE && lb!=YSTRUE && 0==prevTouch.GetN())
	{
		processed=MouseButtonReleaseEvent(onItem,mx,my);
	}
	else if((mx!=pMx || my!=pMy) && 0==prevTouch.GetN())
	{
		if(pullDown==YSTRUE && openSubMenu!=NULL)
		{
			if(mouseOn!=NULL && mouseOn!=openSubMenu)
			{
				auto nextOpenSubMenu=mouseOn;
				CloseSubMenu(); // This function will nullify mouseOn.  That's why nextOpenSubMenu must remember what it was.
				OpenSubMenu(nextOpenSubMenu);
			}
		}
	}

	pLb=lb;
	pMb=mb;
	pRb=rb;
	pMx=mx;
	pMy=my;

	return processed;
}

YSRESULT FsGuiPopUpMenu::SetTouchState(YSSIZE_T nTouch,const class FsVec2i touch[])
{
	auto prevTouch=this->prevTouch;
	auto newTouch=ConvertTouch(nTouch,touch);
	auto trans=MatchTouch(newTouch,prevTouch);
	this->prevTouch=newTouch;

	YSRESULT processed=YSERR;

	if(1==trans.movement.GetN())
	{
		SetMouseOnItem(trans.movement[0].to.xi(),trans.movement[0].to.yi());
	}

	processed=YSERR;

	FsGuiPopUpMenuItem *onItem=NULL;
	if(pLb!=YSTRUE && 1==trans.newTouch.GetN())
	{
		processed=MouseButtonPressEvent(onItem,trans.newTouch[0].xi(),trans.newTouch[0].yi());
		if(onItem==NULL)
		{
			CloseSubMenu();
		}
		else
		{
			processed=YSOK; // The message must not be passed to the client.  2011/08/18
		}
	}
	else if(pLb!=YSTRUE && 1==trans.endedTouch.GetN())
	{
		processed=MouseButtonReleaseEvent(onItem,trans.endedTouch[0].xi(),trans.endedTouch[0].yi());
	}
	else if(pLb!=YSTRUE && 1==trans.movement.GetN())
	{
		if(pullDown==YSTRUE && openSubMenu!=NULL)
		{
			if(mouseOn!=NULL && mouseOn!=openSubMenu)
			{
				auto nextOpenSubMenu=mouseOn;
				CloseSubMenu(); // This function will nullify mouseOn.  That's why nextOpenSubMenu must remember what it was.
				OpenSubMenu(nextOpenSubMenu);
			}
		}
	}

	return processed;
}

void FsGuiPopUpMenu::SetMouseOnItem(int mx,int my)
{
	FsGuiPopUpMenuItem *prevMouseOn,*newMouseOn;

	prevMouseOn=mouseOn;
	newMouseOn=FindItemInRange(mx,my);

	// 2014/11/14 Now pull-down menu allows to expand multi-rows. >>
	// If the mouse cursor is on one of the open sub menus, it should be kept open when the cursor 
	// drift onto another pull-down menu.
	if(YSTRUE==IsMouseOnOpenSubMenu(mx,my))
	{
		newMouseOn=NULL;
	}
	// 2014/11/14 Now pull-down menu allows to expand multi-rows. <<

	if(newMouseOn!=NULL && prevMouseOn!=newMouseOn)
	{
		mouseOn=newMouseOn;
		GetTopMostMenu()->needRedraw=YSTRUE;
	}
	if(pullDown==YSTRUE && prevMouseOn!=newMouseOn && newMouseOn==NULL)
	{
		mouseOn=NULL;
		GetTopMostMenu()->needRedraw=YSTRUE;
	}

	if(openSubMenu!=NULL)
	{
		openSubMenu->GetSubMenu()->SetMouseOnItem(mx,my);
	}
}

void FsGuiPopUpMenu::OpenSubMenu(FsGuiPopUpMenuItem *item)
{
	if(openSubMenu!=NULL)
	{
		CloseSubMenu();
	}
	if(NULL!=item && NULL!=item->subMenu)
	{
		FitSubMenuToWindow(item->subMenu);
	}
	openSubMenu=item;
	mouseOn=item;
	if(0<openSubMenu->subMenu->itemList.GetN())
	{
		openSubMenu->subMenu->mouseOn=&(openSubMenu->subMenu->itemList.FindNext(NULL)->dat);
	}
	else
	{
		openSubMenu->subMenu->mouseOn=NULL;
	}
	GetTopMostMenu()->OnOpenSubMenu(item);
	GetTopMostMenu()->needRedraw=YSTRUE;
}

FsGuiPopUpMenuItem *FsGuiPopUpMenu::GetOpenSubMenu(void)
{
	return openSubMenu;
}

const FsGuiPopUpMenuItem *FsGuiPopUpMenu::GetOpenSubMenu(void) const
{
	return openSubMenu;
}

void FsGuiPopUpMenu::CloseSubMenu(void)
{
	if(openSubMenu!=NULL && openSubMenu->GetSubMenu()!=NULL)
	{
		GetTopMostMenu()->OnCloseSubMenu(openSubMenu);
		openSubMenu->GetSubMenu()->CloseSubMenu();
		GetTopMostMenu()->needRedraw=YSTRUE;
	}
	mouseOn=NULL;
	openSubMenu=NULL;
}

void FsGuiPopUpMenu::CloseAllSubMenu(void)
{
	if(openSubMenu!=NULL)
	{
		openSubMenu->GetSubMenu()->CloseAllSubMenu();
		CloseSubMenu();
	}
}

YSRESULT FsGuiPopUpMenu::MouseButtonPressEvent(FsGuiPopUpMenuItem *&onItem,int mx,int my)
{
	FsGuiPopUpMenuItem *item=FindItemInRange(mx,my);
	if(item!=NULL)
	{
		onItem=item;
	}

	YSBOOL mouseOnOpenSubMenu=YSFALSE;
	FsGuiPopUpMenuItem *openItem=openSubMenu;
	while(NULL!=openItem)
	{
		FsGuiPopUpMenu *subMenu=openItem->GetSubMenu();
		if(subMenu->x0<=mx && mx<subMenu->x0+subMenu->wid &&
		   subMenu->y0<=my && my<subMenu->y0+subMenu->hei)
		{
			mouseOnOpenSubMenu=YSTRUE;
			break;
		}
		openItem=subMenu->openSubMenu;
	}

	if(YSTRUE!=mouseOnOpenSubMenu && item!=NULL && item->GetSubMenu()!=NULL)
	{
		// Don't CloseSubMenu() here.  It will nullify openSubMenu.
		if(item==openSubMenu)  // Close it.
		{
			CloseSubMenu();
			return YSOK;
		}
		else  // Close current open sub menu and open new one.
		{
			CloseSubMenu();
			OpenSubMenu(item);
			return YSOK;
		}
	}

	if(openSubMenu!=NULL && openSubMenu->GetSubMenu()!=NULL)
	{
		if(openSubMenu->GetSubMenu()->MouseButtonPressEvent(onItem,mx,my)==YSOK)
		{
			return YSOK;
		}
	}

	return YSERR;
}

YSRESULT FsGuiPopUpMenu::MouseButtonReleaseEvent(FsGuiPopUpMenuItem *&onItem,int mx,int my)
{
	if(openSubMenu!=NULL && openSubMenu->GetSubMenu()!=NULL)
	{
		if(openSubMenu->GetSubMenu()->MouseButtonReleaseEvent(onItem,mx,my)==YSOK)
		{
			return YSOK;
		}
	}

	FsGuiPopUpMenuItem *item;
	item=FindItemInRange(mx,my);
	if(item!=NULL)
	{
		onItem=item;
		if(item->GetSubMenu()==NULL)
		{
			ProcessClick(item);
		}
		return YSOK;
	}

	return YSERR;
}

YSRESULT FsGuiPopUpMenu::KeyIn(int fsKey,YSBOOL /*shift*/,YSBOOL /*ctrl*/,YSBOOL alt)
{
	FsGuiPopUpMenu *subMenu;
	switch(fsKey)
	{
	case FSKEY_NULL:
		return YSERR;
	case FSKEY_ALT:
		if(NULL!=openSubMenu)
		{
			YsArray <FsGuiPopUpMenuItem *> openSubMenuStack;
			for(FsGuiPopUpMenuItem *subMenu=openSubMenu; NULL!=subMenu; subMenu=subMenu->GetSubMenu()->openSubMenu)
			{
				openSubMenuStack.Append(subMenu);
			}
			for(YSSIZE_T idx=openSubMenuStack.GetN()-1; 0<=idx; --idx)
			{
				openSubMenuStack[idx]->GetSubMenu()->CloseSubMenu();
			}
			CloseSubMenu();
			GetTopMostMenu()->needRedraw=YSTRUE;
			return YSOK;
		}
		break;
	case FSKEY_ESC:
		{
			subMenu=this;
			while(subMenu!=NULL && subMenu->openSubMenu!=NULL)
			{
				if(subMenu->openSubMenu->GetSubMenu()!=NULL &&
				   subMenu->openSubMenu->GetSubMenu()->openSubMenu==NULL)
				{
					subMenu->CloseSubMenu();
					GetTopMostMenu()->needRedraw=YSTRUE;
					return YSOK;
				}
				else
				{
					subMenu=subMenu->openSubMenu->GetSubMenu();
				}
			}
		}
		break;
	case FSKEY_UP:
		{
			if(openSubMenu!=NULL)
			{
				subMenu=openSubMenu->subMenu;
				while(subMenu!=NULL)
				{
					if(subMenu->openSubMenu==NULL)
					{
						if(subMenu->mouseOn!=NULL)
						{
							YsListItem <FsGuiPopUpMenuItem> *ptr,*newMouseOn;
							ptr=NULL;
							while(NULL!=(ptr=subMenu->itemList.FindNext(ptr)))
							{
								if(subMenu->mouseOn==&ptr->dat)
								{
									newMouseOn=subMenu->itemList.FindPrev(ptr);
									if(newMouseOn==NULL)
									{
										newMouseOn=subMenu->itemList.FindPrev(NULL);
									}
									if(newMouseOn!=NULL)
									{
										subMenu->mouseOn=&newMouseOn->dat;
										GetTopMostMenu()->needRedraw=YSTRUE;
										break;
									}
								}
							}
							return YSOK;
						}
						else // if(subMenu->mouseOn==NULL)
						{
							YsListItem <FsGuiPopUpMenuItem> *newMouseOn;
							newMouseOn=subMenu->itemList.FindPrev(NULL);
							if(newMouseOn!=NULL)
							{
								subMenu->mouseOn=&newMouseOn->dat;
								GetTopMostMenu()->needRedraw=YSTRUE;
							}
						}
						break;
					}
					else // if(subMenu->openSubMenu!=NULL)
					{
						subMenu=subMenu->openSubMenu->GetSubMenu();
					}
				}
			}
		}
		break;
	case FSKEY_DOWN:
		{
			if(openSubMenu!=NULL)
			{
				subMenu=openSubMenu->subMenu;
				while(subMenu!=NULL)
				{
					if(subMenu->openSubMenu==NULL)
					{
						if(subMenu->mouseOn!=NULL)
						{
							YsListItem <FsGuiPopUpMenuItem> *ptr,*newMouseOn;
							ptr=NULL;
							while(NULL!=(ptr=subMenu->itemList.FindNext(ptr)))
							{
								if(subMenu->mouseOn==&ptr->dat)
								{
									newMouseOn=subMenu->itemList.FindNext(ptr);
									if(newMouseOn==NULL)
									{
										newMouseOn=subMenu->itemList.FindNext(NULL);
									}
									if(newMouseOn!=NULL)
									{
										subMenu->mouseOn=&newMouseOn->dat;
										GetTopMostMenu()->needRedraw=YSTRUE;
										break;
									}
								}
							}
							return YSOK;
						}
						else // if(subMenu->mouseOn==NULL)
						{
							YsListItem <FsGuiPopUpMenuItem> *newMouseOn;
							newMouseOn=subMenu->itemList.FindNext(NULL);
							if(newMouseOn!=NULL)
							{
								subMenu->mouseOn=&newMouseOn->dat;
								GetTopMostMenu()->needRedraw=YSTRUE;
							}
						}
						break;
					}
					else // if(subMenu->openSubMenu!=NULL)
					{
						subMenu=subMenu->openSubMenu->GetSubMenu();
					}
				}
			}
		}
		break;
	case FSKEY_RIGHT:
		{
			if(openSubMenu!=NULL)
			{
				subMenu=openSubMenu->subMenu;
				while(subMenu!=NULL)
				{
					if(subMenu->openSubMenu==NULL)
					{
						if(subMenu->mouseOn!=NULL && subMenu->mouseOn->GetSubMenu()!=NULL)
						{
							subMenu->OpenSubMenu(subMenu->mouseOn);
							GetTopMostMenu()->needRedraw=YSTRUE;
							return YSOK;
						}
						break;
					}
					else // if(subMenu->openSubMenu!=NULL)
					{
						subMenu=subMenu->openSubMenu->GetSubMenu();
					}
				}

				if(pullDown==YSTRUE)
				{
					YsListItem <FsGuiPopUpMenuItem> *ptr,*newOpenSubMenu;
					ptr=NULL;
					while(NULL!=(ptr=itemList.FindNext(ptr)))
					{
						if(openSubMenu->subMenu==ptr->dat.subMenu)
						{
							newOpenSubMenu=itemList.FindNext(ptr);
							if(newOpenSubMenu==NULL)
							{
								newOpenSubMenu=itemList.FindNext(NULL);
							}
							if(newOpenSubMenu!=NULL)
							{
								openSubMenu->subMenu->CloseSubMenu();
								OpenSubMenu(&newOpenSubMenu->dat);
							}
							break;
						}
					}
				}
			}
		}
		break;
	case FSKEY_LEFT:
		{
			if(openSubMenu!=NULL)
			{
				subMenu=openSubMenu->subMenu;
				while(subMenu!=NULL && subMenu->openSubMenu!=NULL)
				{
					if(subMenu->openSubMenu->GetSubMenu()!=NULL &&
					   subMenu->openSubMenu->GetSubMenu()->openSubMenu==NULL)
					{
						subMenu->CloseSubMenu();
						GetTopMostMenu()->needRedraw=YSTRUE;
						return YSOK;
					}
					else
					{
						subMenu=subMenu->openSubMenu->GetSubMenu();
					}
				}

				if(pullDown==YSTRUE)
				{
					YsListItem <FsGuiPopUpMenuItem> *ptr,*newOpenSubMenu;
					ptr=NULL;
					while(NULL!=(ptr=itemList.FindNext(ptr)))
					{
						if(openSubMenu->subMenu==ptr->dat.subMenu)
						{
							newOpenSubMenu=itemList.FindPrev(ptr);
							if(newOpenSubMenu==NULL)
							{
								newOpenSubMenu=itemList.FindPrev(NULL);
							}
							if(newOpenSubMenu!=NULL)
							{
								openSubMenu->subMenu->CloseSubMenu();
								OpenSubMenu(&newOpenSubMenu->dat);
							}
							break;
						}
					}
				}
			}
		}
		break;
	case FSKEY_ENTER:
		if(pullDown==YSTRUE && openSubMenu!=NULL)
		{
			subMenu=openSubMenu->subMenu;
			while(subMenu!=NULL)
			{
				if(subMenu->openSubMenu==NULL)
				{
					if(subMenu->mouseOn!=NULL)
					{
						if(subMenu->mouseOn->GetSubMenu()!=NULL)
						{
							subMenu->OpenSubMenu(subMenu->mouseOn);
							return YSOK;
						}
						else
						{
							ProcessClick(subMenu->mouseOn);
							return YSOK;
						}
					}
					break;
				}
				else // if(subMenu->openSubMenu!=NULL)
				{
					subMenu=subMenu->openSubMenu->GetSubMenu();
				}
			}
		}
		break;
	}

	subMenu=this;
	while(subMenu!=NULL && subMenu->openSubMenu!=NULL)
	{
		subMenu=subMenu->openSubMenu->GetSubMenu();
	}

	if(subMenu!=NULL)
	{
		if(subMenu==GetTopMostMenu() && YSTRUE==subMenu->IsPullDownMenu() && YSTRUE!=alt)
		{
			// Require ALT key for opening the top-level pull-down menu.
			return YSERR;
		}

		YsArray <YsListItem <FsGuiPopUpMenuItem> *> candidateArray;

		for(YsListItem <FsGuiPopUpMenuItem> *ptr=NULL; NULL!=(ptr=subMenu->itemList.FindNext(ptr)); )
		{
			if(ptr->dat.fsKey==fsKey)
			{
				candidateArray.Append(ptr);
			}
		}

		if(1==candidateArray.GetN())
		{
			if(NULL!=candidateArray[0]->dat.GetSubMenu())
			{
				subMenu->OpenSubMenu(&candidateArray[0]->dat);
				GetTopMostMenu()->needRedraw=YSTRUE;
			}
			else
			{
				ProcessClick(&candidateArray[0]->dat);
			}
			return YSOK;
		}
		else if(1<candidateArray.GetN())
		{
			int nowSelected=-1;
			for(int i=0; i<candidateArray.GetN(); i++)
			{
				if(subMenu->mouseOn==&candidateArray[i]->dat)
				{
					nowSelected=i;
					break;
				}
			}
			int nextSelection=(nowSelected+1)%candidateArray.GetN();
			subMenu->mouseOn=&candidateArray[nextSelection]->dat;
			if(NULL!=candidateArray[nextSelection]->dat.GetSubMenu())
			{
				subMenu->OpenSubMenu(&candidateArray[nextSelection]->dat);
			}

			GetTopMostMenu()->needRedraw=YSTRUE;
		}
	}

	return YSERR;
}

/* virtual */ YSRESULT FsGuiPopUpMenu::VirtualClick(const YsString &identStr)
{
	auto item=FindMenuItem(FindIdent(identStr));
	if(nullptr!=item)
	{
		ProcessClick(item);
		return YSOK;
	}
	printf("Targe [%s] not found.\n",identStr.c_str());
	return YSERR;
}

void FsGuiPopUpMenu::ProcessClick(FsGuiPopUpMenuItem *item)
{
	if(nullptr!=item)
	{
		GetTopMostMenu()->CloseSubMenu();
		GetTopMostMenu()->lastItem=item;
		GetTopMostMenu()->needRedraw=YSTRUE;

		// Do nothing after OnSelectMenuItem and InvokeCallBackIfAssigned.
		// Menu itself may be re-built in a menu handler,
		// in which case 'this' pointer may become invalid.

		// 2014/03/16 OnSelectMenuItem must be called before InvokeCallBackIfAssigned
		//            OnSelectMenuItem can be used for debugging purpose by checking what
		//            menu is selected, and then InvokeCallBackIfAssigned will do the actual
		//            task.
		GetTopMostMenu()->OnSelectMenuItem(item);
		GetTopMostMenu()->InvokeCallBackIfAssigned(item);
	}
}

void FsGuiPopUpMenu::OnOpenSubMenu(FsGuiPopUpMenuItem *)
{
}

void FsGuiPopUpMenu::OnCloseSubMenu(FsGuiPopUpMenuItem *)
{
}

void FsGuiPopUpMenu::OnSelectMenuItem(FsGuiPopUpMenuItem *)
{
}

void FsGuiPopUpMenu::InvokeCallBackIfAssigned(FsGuiPopUpMenuItem *item)
{
	if(true==(bool)item->stdCallBack)
	{
		item->stdCallBack();
	}
	if(NULL!=item->selectionCallBack)
	{
		(*item->selectionCallBack)(item->selectionCallBackParam,this,item);
	}
}

