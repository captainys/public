/* ////////////////////////////////////////////////////////////

File Name: fsguicanvas.cpp
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

#include <ysclass.h>

#include "fsguiclipboard.h"

#include <ysbitmap.h>

#include "fsgui.h"
#include "fsguicommondrawing.h"
#include "fsguistatusbar.h"


FsGuiCanvas::FsGuiCanvas()
{
	Initialize();
}

void FsGuiCanvas::Initialize(void)
{
	ClearStringToIdentTable();

	mainMenu=NULL;
	activeDlg=NULL;
	topStatusBar=NULL;
	bottomStatusBar=NULL;
	dlgArray.Clear();
	prevWid=100;
	prevHei=100;

	LButtonDownCallBack=nullptr;
	LButtonUpCallBack=nullptr;
	MButtonDownCallBack=nullptr;
	MButtonUpCallBack=nullptr;
	RButtonDownCallBack=nullptr;
	RButtonUpCallBack=nullptr;
	MouseMoveCallBack=nullptr;
}

FsGuiCanvas::~FsGuiCanvas()
{
	/* Memo: 2015/03/18
	The following two lines will delete all the self-destructive dialogs before the application
	closes.  However, most of the modeless dialogs are not owned by the FsGuiCanvas, it
	can cause crash because some dialogs might be deleted in the destructor of the application.
	The assumption is that this function is called just before the program closes, 
	the impact of the last-minute memory leak is small.

	The potential problem can be if the application closes one child-class of FsGuiCanvas and 
	start another child-class of FsGuiCanvas, which is possible.  To avoid memory leak, the 
	destructor of the child-class of FsGuiCanvas must call the following two functions 
	before deleting its own dialogs.

	Probably I should experiment with shared_pointer.

	RemoveDialogAll();
	PerformScheduledDeletion();
	*/
}

void FsGuiCanvas::SetMainMenu(class FsGuiPopUpMenu *mainMenu)
{
	this->mainMenu=mainMenu;
	if(NULL!=mainMenu)
	{
		mainMenu->SetParent(this);
	}
}

void FsGuiCanvas::RemoveMainMenu(void)
{
	SetMainMenu(NULL);
}

void FsGuiCanvas::SetTopStatusBar(class FsGuiStatusBar *topBar)
{
	topStatusBar=topBar;
}

class FsGuiStatusBar *FsGuiCanvas::GetTopStatusBar(void) const
{
	return topStatusBar;
}

void FsGuiCanvas::SetBottomStatusBar(class FsGuiStatusBar *bottomBar)
{
	bottomStatusBar=bottomBar;
}

class FsGuiStatusBar *FsGuiCanvas::GetBottomStatusBar(void) const
{
	return bottomStatusBar;
}

class FsGuiPopUpMenu *FsGuiCanvas::GetMainMenu(void) const
{
	return mainMenu;
}

YSRESULT FsGuiCanvas::AddDialog(class FsGuiDialog *dlg)
{
	if(NULL!=dlg && YSTRUE!=dlgArray.IsIncluded(dlg))
	{
		dlgArray.Append(dlg);
		dlg->SetParent(this);
		dlg->OnAttach();
		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiCanvas::RemoveDialog(class FsGuiDialog *dlg)
{
	for(YSSIZE_T i=0; i<dlgArray.GetN(); ++i)
	{
		if(dlg==dlgArray[i])
		{
			if(dlg==activeDlg)
			{
				if(0<i)
				{
					activeDlg=dlgArray[i-1];
				}
				else if(i<dlgArray.GetN()-1)
				{
					activeDlg=dlgArray[i+1];
				}
				else
				{
					activeDlg=NULL;
				}
				FsDisableIME();
			}
			dlgArray.Delete(i);
			SetNeedRedraw(YSTRUE);
			dlg->OnDetach();

			if(YSTRUE==dlg->IsSelfDestructiveDialog())
			{
				scheduledForDeletion.Append(dlg);
			}

			return YSOK;
		}
	}
	return YSERR;
}

void FsGuiCanvas::RemoveAllNonPermanentDialog(void)
{
	for(auto i : dlgArray.ReverseIndex())
	{
		if(YSTRUE!=dlgArray[i]->IsPermanent())
		{
			RemoveDialog(dlgArray[i]);
		}
	}
}

void FsGuiCanvas::RemoveDialogAll(void)
{
	if(NULL!=activeDlg)
	{
		activeDlg=NULL;
		FsDisableIME();
	}
	while(0<dlgArray.GetN())
	{
		RemoveDialog(dlgArray[0]);
	}
	SetNeedRedraw(YSTRUE);
}

void FsGuiCanvas::ArrangeDialog(void)
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	int topLeftX0,topLeftY0;
	int topRightX0,topRightY0;
	int bottomLeftX0,bottomLeftY0;
	int bottomRightX0,bottomRightY0;

	topLeftX0=0;
	topRightX0=wid;
	if(GetMainMenu()!=NULL)
	{
		topLeftY0=GetMainMenu()->GetHeight();
		topRightY0=GetMainMenu()->GetHeight();
	}
	else
	{
		topLeftY0=0;
		topRightY0=0;
	}
	if(NULL!=GetTopStatusBar())
	{
		topLeftY0+=GetTopStatusBar()->GetHeight();
		topRightY0+=GetTopStatusBar()->GetHeight();
	}


	bottomLeftX0=0;
	bottomLeftY0=hei;
	bottomRightX0=wid;
	bottomRightY0=hei;

	if(NULL!=GetBottomStatusBar())
	{
		bottomLeftY0-=GetBottomStatusBar()->GetHeight();
		bottomRightY0-=GetBottomStatusBar()->GetHeight();
	}



	YsArray <FsGuiDialog *> allDialog;
	for(int i=0; i<GetNumDialog(); ++i)
	{
		allDialog.Append(GetDialog(i));
	}
	if(NULL!=GetModalDialog())
	{
		allDialog.Append(GetModalDialog());
	}



	int totalCenterCenterHeight=0;
	for(auto dlg : allDialog)
	{
		switch(dlg->GetArrangeType())
		{
		default:
			break;
		case FSDIALOG_ARRANGE_TOP_LEFT:
			break;
		case FSDIALOG_ARRANGE_TOP_RIGHT:
			break;
		case FSDIALOG_ARRANGE_BOTTOM_LEFT:
			break;
		case FSDIALOG_ARRANGE_BOTTOM_RIGHT:
			break;
		case FSDIALOG_ARRANGE_CENTER_CENTER:
			totalCenterCenterHeight+=dlg->GetHeight();
			break;
		}
	}


	int centerX=wid/2;
	int centerCenterY0=hei/2-totalCenterCenterHeight/2;


	for(auto dlg : allDialog)
	{
		switch(dlg->GetArrangeType())
		{
		default:
			break;
		case FSDIALOG_ARRANGE_TOP_LEFT:
			dlg->Move(topLeftX0,topLeftY0);
			topLeftY0+=dlg->GetHeight();
			break;
		case FSDIALOG_ARRANGE_TOP_RIGHT:
			dlg->Move(topRightX0-dlg->GetWidth(),topRightY0);
			topRightY0+=dlg->GetHeight();
			break;
		case FSDIALOG_ARRANGE_BOTTOM_LEFT:
			dlg->Move(bottomLeftX0,bottomLeftY0-dlg->GetHeight());
			bottomLeftY0-=dlg->GetHeight();
			break;
		case FSDIALOG_ARRANGE_BOTTOM_RIGHT:
			dlg->Move(bottomRightX0-dlg->GetWidth(),bottomRightY0-dlg->GetHeight());
			bottomRightY0-=dlg->GetHeight();
			break;
		case FSDIALOG_ARRANGE_CENTER_CENTER:
			dlg->Move(centerX-dlg->GetWidth()/2,centerCenterY0);
			centerCenterY0+=dlg->GetHeight();
			break;
		case FSDIALOG_ARRANGE_TOP_CENTER:
			dlg->Move(centerX-dlg->GetWidth()/2,topLeftY0);
			topLeftY0+=dlg->GetHeight();
			break;
		case FSDIALOG_ARRANGE_BOTTOM_CENTER:
			dlg->Move(centerX-dlg->GetWidth()/2,bottomRightY0-dlg->GetHeight());
			bottomRightY0-=dlg->GetHeight();
			break;
		}
	}

	SetNeedRedraw(YSTRUE);
}

void FsGuiCanvas::KillDialogFocusAll(void)
{
	for(int i=0; i<GetNumDialog(); ++i)
	{
		GetDialog(i)->SetFocus(NULL);
	}
	FsGuiDialog *activeModalDlg=GetActiveModalDialog();
	if(NULL!=activeModalDlg)
	{
		activeModalDlg->SetFocus(NULL);
	}

	SetNeedRedraw(YSTRUE);
}

YSSIZE_T FsGuiCanvas::GetNumDialog(void) const
{
	return dlgArray.GetN();
}

class FsGuiDialog *FsGuiCanvas::GetDialog(YSSIZE_T index) const
{
	if(YSTRUE==dlgArray.IsInRange(index))
	{
		return dlgArray[index];
	}
	return NULL;
}

YSBOOL FsGuiCanvas::IsDialogUsed(const FsGuiDialog *dlg) const
{
	for(YSSIZE_T dlgIdx=0; dlgIdx<GetNumDialog(); ++dlgIdx)
	{
		if(dlg==GetDialog(dlgIdx))
		{
			return YSTRUE;
		}
	}

	for(FsGuiDialog *modalDlg=this->GetModalDialog(); NULL!=modalDlg; modalDlg=modalDlg->GetModalDialog())
	{
		if(dlg==modalDlg)
		{
			return YSTRUE;
		}
	}

	return YSFALSE;
}

FsGuiDialog *FsGuiCanvas::GetActiveModalDialog(void) const
{
	if(NULL!=this->GetModalDialog())
	{
		FsGuiDialog *dlg=this->GetModalDialog();
		while(NULL!=dlg->GetModalDialog())
		{
			dlg=dlg->GetModalDialog();
		}
		return dlg;
	}

	for(YSSIZE_T i=0; i<dlgArray.GetN(); ++i)
	{
		FsGuiDialog *dlg=dlgArray[i]->GetModalDialog();
		if(NULL!=dlg)
		{
			while(NULL!=dlg->GetModalDialog())
			{
				dlg=dlg->GetModalDialog();
			}
			return dlg;
		}
	}
	if(NULL!=mainMenu && NULL!=mainMenu->GetModalDialog())
	{
		FsGuiDialog *dlg=mainMenu->GetModalDialog();
		while(NULL!=dlg->GetModalDialog())
		{
			dlg=dlg->GetModalDialog();
		}
		return dlg;
	}

	return NULL;
}

FsGuiDialog *FsGuiCanvas::GetActiveDialog(void) const
{
	return activeDlg;
}

YSRESULT FsGuiCanvas::SetActiveDialog(const class FsGuiDialog *newActiveDlg)
{
	if(NULL==newActiveDlg)
	{
		activeDlg=NULL;
		return YSOK;
	}

	for(int i=0; i<dlgArray.GetN(); ++i)
	{
		if(dlgArray[i]==newActiveDlg)
		{
			activeDlg=dlgArray[i];
			return YSOK;
		}
	}

	return YSERR;
}

void FsGuiCanvas::Interval(void)
{
	static bool busy=false;
	if(true==busy)
	{
		return;
	}

	busy=true;

	this->OnInterval();

	FsGuiDialog *modalDlg=GetActiveModalDialog();
	if(NULL!=modalDlg)
	{
		modalDlg->Interval();
	}
	else
	{
		for(YSSIZE_T i=dlgArray.GetN()-1; 0<=i; --i)
		{
			dlgArray[i]->Interval();
		}
	}

	// If file-dialog is set, open one and then call
	//     virtual void OnFileSelected(int fileDialogIdent,int nFile,const YsWString fileName[]);
	//
	PerformScheduledDeletion();

	busy=false;
}

YSRESULT FsGuiCanvas::SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	PerformScheduledDeletion();

	YSBOOL eventTaken=YSFALSE;
	int prevMx=this->prevMx;
	int prevMy=this->prevMy;
	YSBOOL prevLb=this->prevLb;
	YSBOOL prevMb=this->prevMb;
	YSBOOL prevRb=this->prevRb;

	this->prevMx=mx;
	this->prevMy=my;
	this->prevLb=lb;
	this->prevMx=mx;
	this->prevRb=rb;



	FsGuiDialog *modalDlg=GetActiveModalDialog();
	if(NULL!=modalDlg)
	{
		return modalDlg->SetMouseState(lb,mb,rb,mx,my);
	}

	if(NULL!=mainMenu && YSOK==mainMenu->SetMouseState(lb,mb,rb,mx,my))
	{
		ForgetPreviousMouseButtonState();
		return YSOK;
	}

	if(NULL!=mainMenu && YSTRUE==mainMenu->IsMouseOnMenu(mx,my))
	{
		eventTaken=YSTRUE;
	}


	{
		auto activeDlgBackUp=activeDlg;
		activeDlg=nullptr;

		for(YSSIZE_T i=dlgArray.GetN()-1; 0<=i; --i)
		{
			auto dlgPtr=dlgArray[i];  // This dialog may be closed inside SetMouseState.  Therefore, the pointer must be saved.

			// 2017/07/14 Button-CallBack function might change activeDlg.
			//            How can I detect it?

			if(YSOK==dlgPtr->SetMouseState(lb,mb,rb,mx,my))
			{
				if(nullptr!=activeDlg && YSTRUE==dlgArray.IsIncluded(activeDlg))
				{
					// It means that the mouse call-back function updated activeDlg.
					// If it is the case, don't update activeDlg to the responded dialog.
				}
				else if(YSTRUE==dlgArray.IsIncluded(dlgPtr))  // Make sure the dialog was not closed inside SetMouseState.
				{
					activeDlg=dlgPtr;
				}
				return YSOK;
			}
			if(YSTRUE==dlgPtr->IsMouseOnDialog(mx,my))
			{
				activeDlg=activeDlgBackUp;
				return YSOK;
			}
		}

		activeDlg=activeDlgBackUp;
	}


	if(prevLb!=YSTRUE && lb==YSTRUE && 0==prevTouch.GetN())  // Don't fire LButtonDown if any touch is on.
	{
		if(true==(bool)LButtonDownCallBack && YSOK==LButtonDownCallBack(FsGuiMouseButtonSet(lb,mb,rb),YsVec2i(mx,my)))
		{
			eventTaken=YSTRUE;
		}
		if(YSOK==OnLButtonDown(lb,mb,rb,mx,my))
		{
			eventTaken=YSTRUE;
		}
	}
	if(prevMb!=YSTRUE && mb==YSTRUE)
	{
		if(true==(bool)MButtonDownCallBack && YSOK==MButtonDownCallBack(FsGuiMouseButtonSet(lb,mb,rb),YsVec2i(mx,my)))
		{
			eventTaken=YSTRUE;
		}
		if(YSOK==OnMButtonDown(lb,mb,rb,mx,my))
		{
			eventTaken=YSTRUE;
		}
	}
	if(prevRb!=YSTRUE && rb==YSTRUE)
	{
		if(true==(bool)RButtonDownCallBack && YSOK==RButtonDownCallBack(FsGuiMouseButtonSet(lb,mb,rb),YsVec2i(mx,my)))
		{
			eventTaken=YSTRUE;
		}
		if(YSOK==OnRButtonDown(lb,mb,rb,mx,my))
		{
			eventTaken=YSTRUE;
		}
	}

	if(prevMx!=mx || prevMy!=my)
	{
		// Experimental 2018/10/30
		// Ignore mouse move if touch is on.
		// Win32 WM_TOUCH is extremely problematic because it emulates LButton, but it also sends WM_TOUCH.
		// There is a possibility mouse move is sensed as well as touch movement.
		if(0==prevTouchState.GetNumCurrentTouch())
		{
			if(true==(bool)MouseMoveCallBack && YSOK==MouseMoveCallBack(FsGuiMouseButtonSet(lb,mb,rb),YsVec2i(mx,my),YsVec2i(prevMx,prevMy)))
			{
				eventTaken=YSTRUE;
			}
			if(YSOK==OnMouseMove(lb,mb,rb,mx,my,prevMx,prevMy))
			{
				eventTaken=YSTRUE;
			}
		}
	}

	if(prevLb==YSTRUE && lb!=YSTRUE && 0==prevTouch.GetN())  // Don't fire LButtonUp if a touch is on.
	{
		if(true==(bool)LButtonUpCallBack && YSOK==LButtonUpCallBack(FsGuiMouseButtonSet(lb,mb,rb),YsVec2i(mx,my)))
		{
			eventTaken=YSTRUE;
		}
		if(YSOK==OnLButtonUp(lb,mb,rb,mx,my))
		{
			eventTaken=YSTRUE;
		}
	}
	if(prevMb==YSTRUE && mb!=YSTRUE)
	{
		if(true==(bool)MButtonUpCallBack && YSOK==MButtonUpCallBack(FsGuiMouseButtonSet(lb,mb,rb),YsVec2i(mx,my)))
		{
			eventTaken=YSTRUE;
		}
		if(YSOK==OnMButtonUp(lb,mb,rb,mx,my))
		{
			eventTaken=YSTRUE;
		}
	}
	if(prevRb==YSTRUE && rb!=YSTRUE)
	{
		if(true==(bool)RButtonUpCallBack && YSOK==RButtonUpCallBack(FsGuiMouseButtonSet(lb,mb,rb),YsVec2i(mx,my)))
		{
			eventTaken=YSTRUE;
		}
		if(YSOK==OnRButtonUp(lb,mb,rb,mx,my))
		{
			eventTaken=YSTRUE;
		}
	}

	if(YSTRUE==eventTaken)
	{
		return YSOK;
	}


	return YSERR;
}

YSRESULT FsGuiCanvas::SetTouchState(YSSIZE_T nTouch,const class FsVec2i touch[])
{
	PerformScheduledDeletion();

	YSBOOL eventTaken=YSFALSE;

	auto prevTouch=this->prevTouch;
	auto newTouch=ConvertTouch(nTouch,touch);
	this->prevTouch=newTouch;



	FsGuiDialog *modalDlg=GetActiveModalDialog();
	if(NULL!=modalDlg)
	{
		return modalDlg->SetTouchState(nTouch,touch);
	}

	if(NULL!=mainMenu && YSOK==mainMenu->SetTouchState(nTouch,touch))
	{
		return YSOK;
	}

	if(NULL!=mainMenu)
	{
		for(YSSIZE_T i=0; i<nTouch; ++i)
		{
			if(YSTRUE==mainMenu->IsMouseOnMenu(touch[i].x(),touch[i].y()))
			{
				eventTaken=YSTRUE;
			}
		}
	}

	for(YSSIZE_T i=dlgArray.GetN()-1; 0<=i; --i)
	{
		auto dlgPtr=dlgArray[i];  // This dialog may be closed inside SetMouseState.  Therefore, the pointer must be saved.
		if(YSOK==dlgPtr->SetTouchState(nTouch,touch))
		{
			if(YSTRUE==dlgArray.IsIncluded(dlgPtr))  // Make sure the dialog was not closed inside SetMouseState.
			{
				activeDlg=dlgPtr;
			}
			return YSOK;
		}
		for(YSSIZE_T i=0; i<nTouch; ++i)
		{
			if(YSTRUE==dlgPtr->IsMouseOnDialog(touch[i].x(),touch[i].y()))
			{
				return YSOK;
			}
		}
	}


	auto trans=MatchTouch(newTouch,prevTouch);
	{
		YSBOOL changed=YSFALSE;
		if(prevTouchState.movement.size()!=trans.movement.size() ||
		   prevTouchState.newTouch.size()!=trans.newTouch.size() ||
		   prevTouchState.endedTouch.size()!=trans.endedTouch.size())
		{
			changed=YSTRUE;
		}
		else
		{
			for(auto m : trans.movement)
			{
				if(m.from!=m.to)
				{
					changed=YSTRUE;
					break;
				}
			}
			for(auto idx : trans.newTouch.AllIndex())
			{
				if(trans.newTouch[idx]!=prevTouchState.newTouch[idx])
				{
					changed=YSTRUE;
					break;
				}
			}
			for(auto idx : trans.endedTouch.AllIndex())
			{
				if(trans.endedTouch[idx]!=prevTouchState.endedTouch[idx])
				{
					changed=YSTRUE;
					break;
				}
			}
		}
		if(YSTRUE==changed)
		{
			if(true==(bool)TouchStateChangeCallBack && YSOK==TouchStateChangeCallBack(trans))
			{
				eventTaken=YSTRUE;
			}
			if(YSOK==OnTouchStateChange(trans))
			{
				eventTaken=YSTRUE;
			}
		}
		prevTouchState=trans;
	}


	if(YSTRUE==eventTaken)
	{
		return YSOK;
	}

	return YSERR;
}

/* virtual */ YSRESULT FsGuiCanvas::ProcessCommand(const YsString &str)
{
	printf(">> %s\n",str.Txt());

	auto args=str.Argv();
	if(0<args.GetN())
	{
		if(0==args[0].STRCMP("CLICK") && 3<=args.GetN())
		{
			auto target=FindTarget(args[1].c_str());
			if(nullptr!=target)
			{
				target->VirtualClick(args[2]);
				for(auto dlgPtr : dlgArray)
				{
					if(target==dlgPtr)
					{
						SetActiveDialog(dlgPtr);
					}
				}
			}
		}
		else if(0==args[0].STRCMP("CHECK") && 3<=args.size())
		{
			auto target=FindTarget(args[1].c_str());
			if(nullptr!=target)
			{
				target->VirtualCheck(args[2]);
				for(auto dlgPtr : dlgArray)
				{
					if(target==dlgPtr)
					{
						SetActiveDialog(dlgPtr);
					}
				}
			}
		}
		else if(0==args[0].STRCMP("UNCHECK") && 3<=args.size())
		{
			auto target=FindTarget(args[1].c_str());
			if(nullptr!=target)
			{
				target->VirtualUncheck(args[2]);
				for(auto dlgPtr : dlgArray)
				{
					if(target==dlgPtr)
					{
						SetActiveDialog(dlgPtr);
					}
				}
			}
		}
		else if(0==args[0].STRCMP("SELECT") && 4<=args.size())
		{
			auto target=FindTarget(args[1].c_str());
			if(nullptr!=target)
			{
				target->VirtualSelect(args[2],args[3]);
			}
		}
		else if(0==args[0].STRCMP("SELECTN") && 4<=args.size())
		{
			auto target=FindTarget(args[1].c_str());
			if(nullptr!=target)
			{
				target->VirtualSelect(args[2],atoi(args[3]));
			}
		}
		else if(0==args[0].STRCMP("KEYIN") && 2<=args.GetN())
		{
			auto fsKey=FsStringToKeyCode(args[1]);
			YSBOOL shift=YSFALSE,ctrl=YSFALSE,alt=YSFALSE;
			for(int i=2; i<args.GetN(); ++i)
			{
				if(0==args[i].STRCMP("SHIFT"))
				{
					shift=YSTRUE;
				}
				else if(0==args[i].STRCMP("CTRL") || 0==args[i].STRCMP("CONTROL"))
				{
					ctrl=YSTRUE;
				}
				else if(0==args[i].STRCMP("ALT"))
				{
					alt=YSTRUE;
				}
			}
			KeyIn(fsKey,shift,ctrl,alt);
			return YSOK;
		}
		else if(0==args[0].STRCMP("TYPE") && 2<=args.GetN())
		{
			for(auto c : args[1])
			{
				CharIn(c);
			}
		}
		else if(0==args[0].STRCMP("CLOSEMODAL"))
		{
			int returnCode=0;
			if(2<=args.size())
			{
				returnCode=atoi(args[1].c_str());
			}
			auto dlg=GetActiveModalDialog();
			if(nullptr!=dlg)
			{
				dlg->CloseModalDialog(returnCode);
			}
		}
		return FsGuiControlBase::ProcessCommand(str);
	}
	return YSOK;
}

FsGuiControlBase *FsGuiCanvas::FindTarget(const char labelIn[]) const
{
	YsString label(labelIn);
	if(0==label.Strcmp("$mainMenu"))
	{
		if(nullptr!=GetMainMenu())
		{
			return GetMainMenu();
		}
	}
	else if(0==label.Strcmp("$activeDialog"))
	{
		if(nullptr!=GetActiveDialog())
		{
			return GetActiveDialog();
		}
	}
	else if(0==label.Strcmp("$activeModalDialog"))
	{
		if(nullptr!=GetActiveModalDialog())
		{
			return GetActiveModalDialog();
		}
	}
	else
	{
		if(nullptr!=GetActiveModalDialog() &&
		   0==GetActiveModalDialog()->GetIdent().Strcmp(label))
		{
			return GetActiveModalDialog();
		}
		for(auto dlgPtr : dlgArray)
		{
			if(0==dlgPtr->GetIdent().Strcmp(label))
			{
				return dlgPtr;
			}
		}
	}
	printf("Target [%s] not found.\n",labelIn);
	return nullptr;
}

void FsGuiCanvas::ForgetPreviousMouseButtonState(void)
{
	for(YSSIZE_T i=dlgArray.GetN()-1; 0<=i; --i)
	{
		dlgArray[i]->ForgetPreviousMouseButtonState();
	}
}


YSRESULT FsGuiCanvas::KeyIn(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	PerformScheduledDeletion();

	// 2013/09/21
	//   From the requirement of the new polygon editor.
	//     Overrided OnKeyDown must first be tested, otherwise, space key is passed to the view-control dialog.
	//     But, key stroke for the menu must have the priority.  Otherwise, sub-menu cannot be selected by short-cut keys.
	//
	//     However, if FsGuiTextBox is active, all the key strokes must be sent to the dialog, therefore, activeDialog has the priority.
	//
	//     In conclusion, the priority changes depending on the state of the user-interface objects.
	//
	//     Modal dialog always has the top priority.
	//     The main menu takes the key if a sub-menu is open.
	//
	//     The active dialog has the priority over the canvas if MayEatThisKeyStroke returns YSTRUE.
	//     Otherwise, the canvas has the priority over the active dialog.

	FsGuiDialog *modalDlg=GetActiveModalDialog();
	if(NULL!=modalDlg)
	{
		return modalDlg->KeyIn(fsKey,shift,ctrl,alt);;
	}

	if(NULL!=mainMenu && NULL!=mainMenu->GetOpenSubMenu())  // If a sub-menu is open, the main menu takes the key.
	{
		if(NULL!=mainMenu && YSOK==mainMenu->KeyIn(fsKey,shift,ctrl,alt))
		{
			return YSOK;
		}
	}
	else // The main menu has the last priority
	{
		if(NULL!=activeDlg && YSTRUE==activeDlg->MayEatThisKeyStroke(fsKey,shift,ctrl,alt)) // Active dialog has the first priority
		{
			if(NULL!=activeDlg && YSOK==activeDlg->KeyIn(fsKey,shift,ctrl,alt))
			{
				return YSOK;
			}
			if(YSOK==OnKeyDown(fsKey,shift,ctrl,alt))
			{
				return YSOK;
			}
		}
		else // Canvas has the first priority
		{
			auto shortCut=FindShortCut(fsKey,shift,ctrl,alt);
			if(nullptr!=shortCut)
			{
				shortCut->callback();
				return YSOK;
			}
			if(YSOK==OnKeyDown(fsKey,shift,ctrl,alt))
			{
				return YSOK;
			}
			if(NULL!=activeDlg && YSOK==activeDlg->KeyIn(fsKey,shift,ctrl,alt))
			{
				return YSOK;
			}
		}

		if(NULL!=mainMenu && YSOK==mainMenu->KeyIn(fsKey,shift,ctrl,alt))
		{
			return YSOK;
		}
	}

	return YSERR;
}

YSRESULT FsGuiCanvas::SetWindowSize(int newWid,int newHei,YSBOOL autoArrangeDialog)
{
	if(newWid!=prevWid || newHei!=prevHei)
	{
		if(NULL!=mainMenu)
		{
			mainMenu->FitPullDownMenu(newWid);
		}

		prevWid=newWid;
		prevHei=newHei;
		if(YSTRUE==autoArrangeDialog)
		{
			ArrangeDialog();
		}
		return YSOK;
	}
	return YSERR;
	}

YSRESULT FsGuiCanvas::CharIn(int c)
{
	FsGuiDialog *modalDlg=GetActiveModalDialog();
	if(NULL!=modalDlg)
	{
		return modalDlg->CharIn(c);
	}

	if(NULL!=activeDlg && YSOK==activeDlg->CharIn(c))
	{
		return YSOK;
	}

	if(YSOK==OnKeyChar(c))
	{
		return YSOK;
	}

	return YSERR;
}

void FsGuiCanvas::Show(void) const
{
	const FsGuiDialog *modalDlg=GetModalDialog();
	int wid,hei;
	FsGetWindowSize(wid,hei);

	if(NULL!=topStatusBar)
	{
		const int y0=(NULL==mainMenu ? 0 : mainMenu->GetHeight());
		topStatusBar->SetWidth(wid);
		topStatusBar->Show(y0);
	}
	if(NULL!=bottomStatusBar)
	{
		const int y0=hei-bottomStatusBar->GetHeight();
		bottomStatusBar->SetWidth(wid);
		bottomStatusBar->Show(y0);
	}

	for(YSSIZE_T i=0; i<dlgArray.GetN(); ++i)
	{
		dlgArray[i]->Show(modalDlg);
	}

	if(NULL!=mainMenu)
	{
		mainMenu->SetWidth(wid);
		mainMenu->Show(modalDlg);
	}
	if(NULL!=modalDlg)
	{
		FsGuiCommonDrawing::DrawMask(FsGuiObject::defFgCol,FsGuiObject::defBgCol,0.0,0.0,(double)wid,(double)hei);
		modalDlg->Show();
	}
}

YSBOOL FsGuiCanvas::NeedRedraw(void) const
{
	if(NULL!=GetModalDialog() && YSTRUE==GetModalDialog()->NeedRedraw())
	{
		return YSTRUE;
	}
	if(NULL!=mainMenu && YSTRUE==mainMenu->NeedRedraw())
	{
		return YSTRUE;
	}
	if(NULL!=GetTopStatusBar() && YSTRUE==GetTopStatusBar()->NeedRedraw())
	{
		return YSTRUE;
	}
	if(NULL!=GetBottomStatusBar() && YSTRUE==GetBottomStatusBar()->NeedRedraw())
	{
		return YSTRUE;
	}
	for(YSSIZE_T i=0; i<dlgArray.GetN(); ++i)
	{
		if(YSTRUE==dlgArray[i]->NeedRedraw())
		{
			return YSTRUE;
		}
	}
	return needRedraw;
}

void FsGuiCanvas::SetNeedRedraw(YSBOOL needRedraw)
{
	this->needRedraw=needRedraw;
}

/* virtual */ void FsGuiCanvas::OnInterval(void)
{
}

YSRESULT FsGuiCanvas::OnKeyDown(int,YSBOOL,YSBOOL,YSBOOL)
{
	return YSERR;
}

YSRESULT FsGuiCanvas::OnKeyChar(int)
{
	return YSERR;
}

YSRESULT FsGuiCanvas::OnLButtonDown(YSBOOL,YSBOOL,YSBOOL,int,int)
{
	return YSERR;
}

YSRESULT FsGuiCanvas::OnMButtonDown(YSBOOL,YSBOOL,YSBOOL,int,int)
{
	return YSERR;
}

YSRESULT FsGuiCanvas::OnRButtonDown(YSBOOL,YSBOOL,YSBOOL,int,int)
{
	return YSERR;
}

YSRESULT FsGuiCanvas::OnMouseMove(YSBOOL,YSBOOL,YSBOOL,int,int,int,int)
{
	return YSERR;
}

/* virtual */ YSRESULT FsGuiCanvas::OnTouchStateChange(const TouchState &)
{
	return YSERR;
}

YSRESULT FsGuiCanvas::OnLButtonUp(YSBOOL,YSBOOL,YSBOOL,int,int)
{
	return YSERR;
}

YSRESULT FsGuiCanvas::OnMButtonUp(YSBOOL,YSBOOL,YSBOOL,int,int)
{
	return YSERR;
}

YSRESULT FsGuiCanvas::OnRButtonUp(YSBOOL,YSBOOL,YSBOOL,int,int)
{
	return YSERR;
}

void FsGuiCanvas::ClearLButtonDownCallBack(void)
{
	LButtonDownCallBack=nullptr;
}
void FsGuiCanvas::ClearLButtonUpCallBack(void)
{
	LButtonUpCallBack=nullptr;
}
void FsGuiCanvas::ClearMButtonDownCallBack(void)
{
	MButtonDownCallBack=nullptr;
}
void FsGuiCanvas::ClearMButtonUpCallBack(void)
{
	MButtonUpCallBack=nullptr;
}
void FsGuiCanvas::ClearRButtonDownCallBack(void)
{
	RButtonDownCallBack=nullptr;
}
void FsGuiCanvas::ClearRButtonUpCallBack(void)
{
	RButtonUpCallBack=nullptr;
}
void FsGuiCanvas::ClearMouseMoveCallBack(void)
{
	MouseMoveCallBack=nullptr;
}

void FsGuiCanvas::ClearTouchStateChangeCallBack(void)
{
	TouchStateChangeCallBack=nullptr;
}

const FsGuiCanvas::ShortCutKey *FsGuiCanvas::FindShortCut(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt) const
{
	for(auto &s : shortCutArray)
	{
		if(s.fsKey==fsKey && s.shift==shift && s.ctrl==ctrl && s.alt==alt)
		{
			return &s;
		}
	}
	return nullptr;
}

FsGuiCanvas::ShortCutKey *FsGuiCanvas::FindShortCut(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	for(auto &s : shortCutArray)
	{
		if(s.fsKey==fsKey && s.shift==shift && s.ctrl==ctrl && s.alt==alt)
		{
			return &s;
		}
	}
	return nullptr;
}

FsGuiCanvas::ShortCutKey *FsGuiCanvas::CreateShortCut(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	shortCutArray.Increment();
	shortCutArray.Last().fsKey=fsKey;
	shortCutArray.Last().shift=shift;
	shortCutArray.Last().ctrl=ctrl;
	shortCutArray.Last().alt=alt;
	return &shortCutArray.Last();
}

void FsGuiCanvas::PrintShortCutKeyWarning(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	printf("  Warning!  Shoft Cut Key %s (shift=%s,ctrl=%s,alt=%s) is already used.\n",
	   FsKeyCodeToString(fsKey),YsBoolToStr(shift),YsBoolToStr(ctrl),YsBoolToStr(alt));
}

void FsGuiCanvas::ClearKeyCallBack(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	for(auto idx : shortCutArray.ReverseIndex())
	{
		auto &s=shortCutArray[idx];
		if(s.fsKey==fsKey && s.shift==shift && s.ctrl==ctrl && s.alt==alt)
		{
			shortCutArray.Delete(idx);
		}
	}
}
