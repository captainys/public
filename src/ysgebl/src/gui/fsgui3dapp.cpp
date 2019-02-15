/* ////////////////////////////////////////////////////////////

File Name: fsgui3dapp.cpp
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

#include <ysdebug.h>
#include <ysport.h>

#include <fsguifiledialog.h>

#include <ysgl.h>
#include <ysglslcpp.h>

#include <ysshellextedit_orientationutil.h>
#include <ysshellext_sweeputil.h>
#include <ysshellext_thickeningutil.h>


#include "ysgebl_gui_editor_base.h"
#include "filename.h"
#include "filemenu/file_slideshowdialog.h"
#include "viewmenu/viewdialog.h"
#include "config/polycreconfig.h"
#include "config/configmenu.h"
#include "edit/edit_create_airfoil.h"
#include "refbmp/refbmp.h"
#include "refbmp/refbmpdialog.h"
#include "miscdlg.h"
#include "edit/edit_offsetdialog.h"
#include "edit/edit_sweepdialog.h"
#include "edit/edit_mirrorimagedialog.h"
#include "edit/edit_hemmingdialog.h"
#include "edit/edit_rounddialog.h"
#include "edit/edit_createprimitivedialog.h"
#include "edit/edit_projdialog.h"
#include "edit/edit_intersectiondialog.h"
#include "edit/edit_insertdialog.h"
#include "edit/edit_solidofrevolutiondialog.h"
#include "util/utildlg.h"
#include "dnm/dnmtreedialog.h"
#include "help/helpdlg.h"
#include "boolmenu/booldialog.h"
#include "sketch/sketch.h"
#include "ysgebl_gui_extension_base.h"
#include "viewmenu/viewdialog.h"

#include "textresource.h"




static int appSleepInterval=10;

////////////////////////////////////////////////////////////

void GeblGuiEditorBase::DrawingCacheState::Save(const YsShellDnmContainer <YsShellExtEditGui>::Node *shl)
{
	slHd=shl;
	whenCached=shl->GetSavePoint();
}

YSBOOL GeblGuiEditorBase::DrawingCacheState::IsModified(const YsShellDnmContainer <YsShellExtEditGui>::Node *shl) const
{
	if(slHd!=shl)
	{
		return YSTRUE;
	}
	if(NULL!=slHd)
	{
		return shl->IsModified(whenCached);
	}
	return YSFALSE;
}

////////////////////////////////////////////////////////////

void GeblGuiEditorBase::NonManifoldEdgeCache::CacheNonManifoldEdge(const YsShellDnmContainer <YsShellExtEditGui>::Node *shl)
{
	lineVtxBuf.CleanUp();
	lineColBuf.CleanUp();

	pointVtxBuf.CleanUp();
	pointPixOffsetBuf.CleanUp();
	pointColBuf.CleanUp();

	const YsColor color[8]=
	{
		YsColor(0.0,0.0,0.0),
		YsColor(0.0,0.0,1.0),
		YsColor(1.0,0.0,0.0),
		YsColor(1.0,0.0,1.0),
		YsColor(0.0,1.0,0.0),
		YsColor(0.0,1.0,1.0),
		YsColor(1.0,1.0,0.0),
		YsColor(1.0,1.0,1.0)
	};


	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl->MoveToNextEdge(edHd))
	{
		const int nEdPl=shl->GetNumPolygonUsingEdge(edHd);
		if(2!=nEdPl)
		{
			const int colorCode=YsBound(nEdPl,0,7);

			YsShellVertexHandle edVtHd[2];
			shl->GetEdge(edVtHd,edHd);

			YsVec3 edVtPos[2];
			shl->GetVertexPosition(edVtPos[0],edVtHd[0]);
			shl->GetVertexPosition(edVtPos[1],edVtHd[1]);

			lineVtxBuf.AddVertex(edVtPos[0]);
			lineColBuf.AddColor(color[colorCode]);
			lineVtxBuf.AddVertex(edVtPos[1]);
			lineColBuf.AddColor(color[colorCode]);

			for(int i=0; i<2; ++i)
			{
				pointVtxBuf.AddVertex(edVtPos[i]);
				pointColBuf.AddColor(color[colorCode]);
				pointPixOffsetBuf.AddPixOffset(-2.0f,-2.0f);
				pointVtxBuf.AddVertex(edVtPos[i]);
				pointColBuf.AddColor(color[colorCode]);
				pointPixOffsetBuf.AddPixOffset(-2.0f, 6.0f);
				pointVtxBuf.AddVertex(edVtPos[i]);
				pointColBuf.AddColor(color[colorCode]);
				pointPixOffsetBuf.AddPixOffset( 6.0f,-2.0f);
			}
		}
	}

	Save(shl);
}

////////////////////////////////////////////////////////////

GeblGuiEditorMainMenu::GeblGuiEditorMainMenu(class GeblGuiEditorBase *canvas)
{
	this->canvas=canvas;
}

void GeblGuiEditorMainMenu::OnSelectMenuItem(FsGuiPopUpMenuItem *item)
{
	canvas->OnSelectMenuItem(item);
}

GeblGuiEditorBase *GeblGuiEditorMainMenu::GetMainCanvas(void) const
{
	return canvas;
}

////////////////////////////////////////////////////////////

GeblGuiEditorBase::GeblGuiEditorBase() : threeDInterface(this),viewControlDlg(NULL)
{
	// To let GUI extension customize directory names, fileAndDirName must be ready before creating GUI extension.
	fileAndDirName=PolyCreFileName::Create();

	guiExtension=PolyCreCreateGuiExtension(*this);
	if(nullptr==guiExtension)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Cannot create guiExtension.\n");
		exit(1);
	}

	mustTerminate=YSFALSE;
	drawingMasterSwitch=YSTRUE;

	nextESCOnlyCancelDnmEditSubMode=YSFALSE;

	preSelectedMovingDirection=YsXVec();

	config=PolyCreConfig::Create();

	dnmCurrentState=-1;
	dnmEditMode=YSFALSE;

	SetDefaultViewOption();

	propIO.AddProperty(drawAxis,                         "DRAWAXIS");
	propIO.AddProperty(drawVertex,                       "DRAWVERTEX");
	propIO.AddProperty(drawPolygonEdge,                  "DRAWPOLYGONEDGE");
	propIO.AddProperty(drawPolygon,                      "DRAWPOLYGON");
	propIO.AddProperty(drawConstEdge,                    "DRAWCONSTEDGE");
	propIO.AddProperty(drawShrunkPolygon,                "DRAWSHRUNKPOLYGON");
	propIO.AddProperty(drawOtherShell,                   "DRAWOTHERSHELL");
	propIO.AddProperty(drawBackFaceInDifferentColor,     "DRAWBACKFACEINDIFFERENTCOLOR");
	propIO.AddProperty(drawApplyViewPortToAllShell,      "DRAWAPPLYVIEWPORTTOALLSHELL");
	propIO.AddProperty(drawHighlight,                    "DRAWHIGHLIGHT");
	propIO.AddProperty(drawOtherShellWireframe,          "DRAWOTHERSHELLWIREFRAME");
	propIO.AddProperty(drawOtherShellFill,               "DRAWOTHERSHELLFILL");
	propIO.AddProperty(drawDnmConnection,                "DRAWDNMCONNECTION");
	LoadViewPreference();


	actuallyDrawNonManifoldEdge=YSFALSE;
	backFaceColor.SetIntRGB(128,16,16);

	menuIdSource=0x78000000;
	MakeMainMenu();
	SetMenuCheck();

	AddViewControlDialog();
	drawEnv.AttachCanvas(this);
	ModeCleanUpCallBack=NULL;
	SpaceKeyCallBack=NULL;
	BackSpaceKeyCallBack=NULL;
	LButtonCallBack=NULL;
	RButtonCallBack=NULL;
	LButtonBoxCallBack=NULL;
	MouseMoveCallBack=NULL;
	lBtnDown=YSFALSE;
	lBtnDownMx=0;
	lBtnDownMy=0;
	needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_ALL;

	undoRedoRequireToClearUIMode=YSTRUE;
	deletionRequireToClearUIMode=YSTRUE;

	UIBeforeDrawCallBack=NULL;
	UIDrawCallBack3D=NULL;
	UIDrawCallBack2D=NULL;

	// [Modeless dialogs]
	crossSectionDlg=PolyCreCrossSectionDialog::Create(this);
	faceNameDlg=PolyCreSetFaceGroupNameDialog::Create(this);
	solidOfRevolutionDlg=PolyCreSolidOfRevolutionDialog::Create(this);
	dnmTreeDlg=PolyCreDnmTreeDialog::Create();
	angleSliderDlg=PolyCreAngleSliderDialog::Create(this);
	equilateralDlg=PolyCreEquilateralDialog::Create(this);
	slideShowDlg=PolyCreSlideShowDialog::Create(this);
	parallelogramDlg=PolyCreParallelogramDialog::Create(this);
	insertVtxDlg=PolyCreInsertVertexDialog::Create(this);
	boolDlg=PolyCreBoolDialog::Create(this);
	chopOffDlg=PolyCreChopOffDialog::Create(this);
	colorPaletteDlg=YsColorPaletteDialog::Create();
	refBmpStore=PolyCreReferenceBitmapStore::Create();
	thickeningDlg=PolyCreEditOffsetThickeningDialog::Create(this);
	parallelSweepWithPathDlg=PolyCreEditParallelSweepWithPathDialog::Create(this);
	sweepConstEdgeDlg=PolyCreEditSweepConstEdgeDialog::Create(this);
	mirrorImageDlg=PolyCreEditCreateMirrorImageDialog::Create();
	thickeningInfo=YsShellExt_ThickeningInfo::Create();
	hemmingDlg=PolyCreHemmingDialog::Create(this);
	roundDlg=PolyCreRoundDialog::Create(this);
	createPrimitiveDlg=PolyCreCreatePrimitiveDialog::Create(this);
	projectionDlg=PolyCreProjectionDialog::Create(this);
	extendOrShrinkDlg=PolyCreExtendOrShrinkDialog::Create(this);

	// [Modal dialogs]
	numberDlg=FsGuiNumberDialog::Create();


	fdlg=new FsGuiFileDialog;
	mbox=new FsGuiMessageBoxDialog;
	createAirfoilDlg=NULL;
	refBmpDlg=NULL;

	topBar=new FsGuiStatusBar;
	bottomBar=new FsGuiStatusBar;

	topBar->Make(2,800);
	SetTopStatusBar(topBar);
	bottomBar->Make(2,800);
	SetBottomStatusBar(bottomBar);

	YsColor bgCol=topBar->GetBackgroundColor();
	bgCol.SetAi(160);
	topBar->SetBackgroundColor(bgCol);

	bgCol=bottomBar->GetBackgroundColor();
	bgCol.SetAi(160);
	bottomBar->SetBackgroundColor(bgCol);

	sketchUI=PolyCreSketchInterface::Create();

	guiExtension->CreateModelessDialog();
}

GeblGuiEditorBase::~GeblGuiEditorBase()
{
	RemoveDialogAll();
	PerformScheduledDeletion();

	guiExtension->DeleteModelessDialog();

	PolyCreDeleteGuiExtension(guiExtension);

	DeleteMainMenu();

	if(NULL!=crossSectionDlg)
	{
		PolyCreCrossSectionDialog::Delete(crossSectionDlg);
		crossSectionDlg=NULL;
	}
	if(NULL!=numberDlg)
	{
		FsGuiNumberDialog::Delete(numberDlg);
		numberDlg=NULL;
	}
	if(NULL!=faceNameDlg)
	{
		PolyCreSetFaceGroupNameDialog::Delete(faceNameDlg);
		faceNameDlg=NULL;
	}
	if(NULL!=solidOfRevolutionDlg)
	{
		PolyCreSolidOfRevolutionDialog::Delete(solidOfRevolutionDlg);
		solidOfRevolutionDlg=NULL;
	}
	if(NULL!=dnmTreeDlg)
	{
		PolyCreDnmTreeDialog::Delete(dnmTreeDlg);
		dnmTreeDlg=NULL;
	}
	if(NULL!=angleSliderDlg)
	{
		PolyCreAngleSliderDialog::Delete(angleSliderDlg);
		angleSliderDlg=NULL;
	}
	if(NULL!=equilateralDlg)
	{
		PolyCreEquilateralDialog::Delete(equilateralDlg);
		equilateralDlg=NULL;
	}
	if(NULL!=slideShowDlg)
	{
		PolyCreSlideShowDialog::Delete(slideShowDlg);
		slideShowDlg=NULL;
	}
	if(NULL!=parallelogramDlg)
	{
		PolyCreParallelogramDialog::Delete(parallelogramDlg);
		parallelogramDlg=NULL;
	}
	if(NULL!=insertVtxDlg)
	{
		PolyCreInsertVertexDialog::Delete(insertVtxDlg);
		insertVtxDlg=NULL;
	}
	if(NULL!=boolDlg)
	{
		PolyCreBoolDialog::Delete(boolDlg);
		boolDlg=NULL;
	}
	if(NULL!=chopOffDlg)
	{
		PolyCreChopOffDialog::Delete(chopOffDlg);
		chopOffDlg=NULL;
	}
	if(NULL!=extendOrShrinkDlg)
	{
		PolyCreExtendOrShrinkDialog::Delete(extendOrShrinkDlg);
		extendOrShrinkDlg=NULL;
	}
	if(NULL!=viewControlDlg)
	{
		FsGui3DViewControlDialog::Delete(viewControlDlg);
		viewControlDlg=NULL;
	}
	if(NULL!=colorPaletteDlg)
	{
		YsColorPaletteDialog::Delete(colorPaletteDlg);
		colorPaletteDlg=NULL;
	}
	if(NULL!=thickeningDlg)
	{
		thickeningDlg->Delete(thickeningDlg);
		thickeningDlg=NULL;
	}
	if(NULL!=parallelSweepWithPathDlg)
	{
		PolyCreEditParallelSweepWithPathDialog::Delete(parallelSweepWithPathDlg);
		parallelSweepWithPathDlg=NULL;
	}
	if(NULL!=sweepConstEdgeDlg)
	{
		PolyCreEditSweepConstEdgeDialog::Delete(sweepConstEdgeDlg);
		sweepConstEdgeDlg=NULL;
	}
	if(NULL!=thickeningInfo)
	{
		YsShellExt_ThickeningInfo::Delete(thickeningInfo);
		thickeningInfo=NULL;
	}
	if(NULL!=fdlg)
	{
		delete fdlg;
		fdlg=NULL;
	}
	if(NULL!=mbox)
	{
		delete mbox;
		mbox=NULL;
	}

	if(NULL!=createAirfoilDlg)
	{
		YsCreateAirfoilDialog::Delete(createAirfoilDlg);
		createAirfoilDlg=NULL;
	}
	if(NULL!=refBmpStore)
	{
		PolyCreReferenceBitmapStore::Delete(refBmpStore);
		refBmpStore=NULL;
	}
	if(NULL!=refBmpDlg)
	{
		PolyCreRefBmpDialog::Delete(refBmpDlg);
		refBmpDlg=NULL;
	}
	if(NULL!=mirrorImageDlg)
	{
		PolyCreEditCreateMirrorImageDialog::Delete(mirrorImageDlg);
		mirrorImageDlg=NULL;
	}
	if(NULL!=createPrimitiveDlg)
	{
		PolyCreCreatePrimitiveDialog::Delete(createPrimitiveDlg);
		createPrimitiveDlg=NULL;
	}
	if(NULL!=projectionDlg)
	{
		PolyCreProjectionDialog::Delete(projectionDlg);
		projectionDlg=NULL;
	}

	if(NULL!=topBar)
	{
		delete topBar;
		topBar=NULL;
	}
	if(NULL!=bottomBar)
	{
		delete bottomBar;
		bottomBar=NULL;
	}

	PolyCreFileName::Delete(fileAndDirName);
	PolyCreConfig::Delete(config);

	PolyCreSketchInterface::Delete(sketchUI);
}

FsGuiPopUpMenuItem *GeblGuiEditorBase::AddMenuItem(FsGuiPopUpMenu *parent,int fsKey,const wchar_t label[],void (THISCLASS::*funcPtr)(FsGuiPopUpMenuItem *))
{
	auto menuPtr=parent->AddTextItem(menuIdSource,fsKey,label);
	menuConn.Register(menuIdSource,this,funcPtr);
	menuIdSource++;
	return menuPtr;
}

FsGuiPopUpMenuItem *GeblGuiEditorBase::AddMenuItem(FsGuiPopUpMenu *parent,int fsKey,const char label[],void (THISCLASS::*funcPtr)(FsGuiPopUpMenuItem *))
{
	auto menuPtr=parent->AddTextItem(menuIdSource,fsKey,label);
	menuConn.Register(menuIdSource,this,funcPtr);
	menuIdSource++;
	return menuPtr;
}

void GeblGuiEditorBase::RegisterCloseModalCallBack(void (THISCLASS::*funcPtr)(FsGuiDialog *,int))
{
	closeModalCallBack.Register(this,funcPtr);
}

void GeblGuiEditorBase::AddViewControlDialog(void)
{
	viewControlDlg=guiExtension->CreateCustomViewControlDialog(drawEnv);
	if(NULL==viewControlDlg)
	{
		auto stdViewControlDlg=FsGui3DViewControlDialog::Create();
		stdViewControlDlg->Make(this,&drawEnv);
		viewControlDlg=stdViewControlDlg;
	}
	this->AddDialog(viewControlDlg);
}

void GeblGuiEditorBase::DeleteMainMenu(void)
{
	delete mainMenu;
}

void GeblGuiEditorBase::MessageDialog(const wchar_t title[],const wchar_t msg[],YSBOOL okBtnBottom)
{
	mbox->Make(title,msg,FSGUI_COMMON_OK,NULL,0,0,okBtnBottom);
	mbox->SetCloseModalCallBack(NULL);
	AttachModalDialog(mbox);
}

void GeblGuiEditorBase::MessageDialog(const wchar_t title[],const wchar_t msg[],const wchar_t copyBtn[])
{
	mbox->Make(title,msg,FSGUI_COMMON_OK,NULL,copyBtn,0,0);
	mbox->SetCloseModalCallBack(NULL);
	AttachModalDialog(mbox);
}

void GeblGuiEditorBase::YesNoDialog(
	    const wchar_t title[],const wchar_t msg[],const wchar_t yes[],const wchar_t no[],
	    void (*closeModalCallBackFunc)(FsGuiControlBase *,FsGuiDialog *,int))
{
	mbox->Make(title,msg,yes,no,1,0);
	mbox->SetCloseModalCallBack(closeModalCallBackFunc);
	AttachModalDialog(mbox);
}

void GeblGuiEditorBase::OnInterval(void)
{
	std::lock_guard <std::mutex> guard(guiLock);

	guiExtension->OnInterval();

	{
		int key;
		while(FSKEY_NULL!=(key=FsInkey()))
		{
			if(YSOK!=drawEnv.KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT)))
			{
				if(YSOK!=threeDInterface.KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT)))
				{
					this->KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT));
				}
			}
		}
	}

	{
		int charCode;
		while(0!=(charCode=FsInkeyChar()))
		{
			this->CharIn(charCode);
		}
	}

	{
		int lb,mb,rb,mx,my;
		while(FSMOUSEEVENT_NONE!=FsGetMouseEvent(lb,mb,rb,mx,my))
		{
			if(YSOK!=drawEnv.SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
			{
				if(YSOK!=this->SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
				{
					if(YSOK!=threeDInterface.SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
					{
					}
				}
			}
		}
	}

	{
		auto nTouch=FsGetNumCurrentTouch();
		auto touch=FsGetCurrentTouch();
		if(YSOK!=drawEnv.SetTouchState(nTouch,touch))
		{
			if(YSOK!=this->SetTouchState(nTouch,touch))
			{
				if(YSOK!=threeDInterface.SetTouchState(nTouch,touch))
				{
				}
			}
		}
	}

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);
	this->SetWindowSize(winWid,winHei,/*autoArrangeDialog=*/YSTRUE);

	drawEnv.SetWindowSize(winWid,winHei);
	drawEnv.SetViewportByTwoCorner(0,0,winWid,winHei);


	if(YSTRUE==NeedRedraw() ||
	   YSTRUE==drawEnv.NeedRedraw() ||
	   YSTRUE==threeDInterface.NeedRedraw() /* ||
	   0!=FsCheckWindowExposure() */)
	{
		FsPushOnPaintEvent(); // Draw();
	}
}

void GeblGuiEditorBase::OnSelectMenuItem(FsGuiPopUpMenuItem *menuItem)
{
	if(NULL!=menuItem)
	{
		YsString sys;
		YsUnicodeToSystemEncoding(sys,menuItem->GetString());
		printf("Selected: %s\n",sys.Txt());
	}
	if(NULL!=menuItem)
	{
		menuConn.Invoke(menuItem);
		guiExtension->OnSelectMenuItem(menuItem);
	}
}

YSRESULT GeblGuiEditorBase::OnKeyDown(int fskey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	switch(fskey)
	{
	case FSKEY_SPACE:
		if(nullptr!=spaceKeyCallBack)
		{
			printf("Reacting to Space Key\n");
			spaceKeyCallBack();
			return YSOK;
		}
		if(NULL!=SpaceKeyCallBack)
		{
			printf("Reacting to Space key\n");
			return (*SpaceKeyCallBack)(*this);
		}
		break;
	case FSKEY_BS:
		if(NULL!=BackSpaceKeyCallBack)
		{
			printf("Reacting to BackSpace key\n");
			return (*BackSpaceKeyCallBack)(*this);
		}
		break;
	}

	for(YSSIZE_T idx=0; idx<shortCut.GetN(); ++idx)
	{
		if(shortCut[idx].fskey==fskey && shortCut[idx].shift==shift && shortCut[idx].ctrl==ctrl && YSTRUE!=alt)
		{
			printf("Reacting to %s key Shift=%d Ctrl=%d\n",FsKeyCodeToString(fskey),shift,ctrl);
			(*shortCut[idx].callBackFunc)(this,NULL,NULL);
			return YSOK;
		}
	}

	return YSERR;
}

YSRESULT GeblGuiEditorBase::OnLButtonDown(YSBOOL,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	if(NULL!=LButtonBoxCallBack)
	{
		lBtnDown=YSTRUE;
		lBtnDownMx=mx;
		lBtnDownMy=my;
	}
	else if(NULL!=LButtonCallBack)
	{
		return LButtonCallBack(*this,YSTRUE,mb,rb,mx,my);
	}
	else if(nullptr!=lButtonDownCallBack)
	{
		return lButtonDownCallBack(YSTRUE,mb,rb,mx,my);
	}
	return YSERR;
}

YSRESULT GeblGuiEditorBase::OnLButtonUp(YSBOOL,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	const int dx=YsAbs(mx-lBtnDownMx);
	const int dy=YsAbs(my-lBtnDownMy);

	lBtnDown=YSFALSE;

	if(NULL!=LButtonBoxCallBack && (16<dx || 16<dy))
	{
		SetNeedRedraw(YSTRUE);
		const int xMin=YsSmaller(mx,lBtnDownMx);
		const int yMin=YsSmaller(my,lBtnDownMy);
		const int xMax=YsGreater(mx,lBtnDownMx);
		const int yMax=YsGreater(my,lBtnDownMy);
		return LButtonBoxCallBack(*this,xMin,yMin,xMax,yMax);
	}
	else if(NULL!=LButtonCallBack)
	{
		return LButtonCallBack(*this,YSFALSE,mb,rb,mx,my);
	}
	else if(nullptr!=lButtonUpCallBack)
	{
		return lButtonUpCallBack(YSFALSE,mb,rb,mx,my);
	}
	return YSERR;
}

YSRESULT GeblGuiEditorBase::OnRButtonDown(YSBOOL,YSBOOL,YSBOOL,int,int)
{
	return YSERR;
}

YSRESULT GeblGuiEditorBase::OnRButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	if(NULL!=RButtonCallBack)
	{
		return RButtonCallBack(*this,lb,mb,rb,mx,my);
	}
	return YSERR;
}

YSRESULT GeblGuiEditorBase::OnMouseMove(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my,int,int)
{
	YSRESULT res=YSOK;

	if(NULL!=MouseMoveCallBack)
	{
		res=MouseMoveCallBack(*this,lb,mb,rb,mx,my);
	}
	if(nullptr!=mouseMoveCallBack)
	{
		res=mouseMoveCallBack(lb,mb,rb,mx,my);
	}

	if(YSTRUE==lBtnDown)
	{
		moveMx=mx;
		moveMy=my;
		SetNeedRedraw(YSTRUE);
		return res;
	}
	return YSERR;
}

/* virtual */ YSRESULT GeblGuiEditorBase::OnTouchStateChange(const TouchState &touchState)
{
	// Emulating Left Button
	if(1==touchState.newTouch.size() &&
	   0==touchState.movement.size() &&
	   0==touchState.endedTouch.size())
	{
		return OnLButtonDown(YSTRUE,YSFALSE,YSFALSE,touchState.newTouch[0].xi(),touchState.newTouch[0].yi());
	}
	else if(0==touchState.newTouch.size() &&
	   0==touchState.movement.size() &&
	   1==touchState.endedTouch.size())
	{
		return OnLButtonUp(YSFALSE,YSFALSE,YSFALSE,touchState.endedTouch[0].xi(),touchState.endedTouch[0].yi());
	}
	else if(0==touchState.newTouch.size() &&
	   1==touchState.movement.size() &&
	   0==touchState.endedTouch.size())
	{
		return OnMouseMove(YSTRUE,YSFALSE,YSFALSE,
		    touchState.movement[0].to.xi(),touchState.movement[0].to.yi(),
		    touchState.movement[0].from.xi(),touchState.movement[0].from.yi());
	}
	return YSERR;
}

YSRESULT GeblGuiEditorBase::OnSaveView(const class FsGui3DViewControlDialog *)
{
	YsString dataDir;
	dataDir.EncodeUTF8 <wchar_t> (GetDataDir());

	YsString def;
	def.MakeFullPathName(dataDir,"*.yvw");

	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Save Viewpoint");
	fdlg->fileExtensionArray.Append(L".yvw");
	fdlg->defaultFileName.SetUTF8String(def);
	fdlg->BindCloseModalCallBack(&THISCLASS::OnSaveView_FileSelected,this);
	AttachModalDialog(fdlg);

	return YSOK;
}

void GeblGuiEditorBase::OnSaveView_FileSelected(FsGuiDialog *,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue && 0<fdlg->selectedFileArray.GetN())
	{
		YsWString ful(fdlg->selectedFileArray[0]);

		FILE *fp=YsFileIO::Fopen(fdlg->selectedFileArray[0],"r");
		if(NULL!=fp)
		{
			fclose(fp);
			YesNoDialog(FSGUI_DLG_CONFIRMOVERWRITE_TITLE,FSGUI_DLG_CONFIRMOVERWRITE_MESSAGE,FSGUI_COMMON_OK,FSGUI_COMMON_CANCEL,&THISCLASS::OnSaveView_OverwriteChecked,this);
		}
		else
		{
			OnSaveView_OverwriteChecked(fdlg,returnValue);
		}
	}
}

void GeblGuiEditorBase::OnSaveView_OverwriteChecked(FsGuiDialog *,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue && 0<fdlg->selectedFileArray.GetN())
	{
		YsWString ful(fdlg->selectedFileArray[0]);

		FILE *fp=YsFileIO::Fopen(fdlg->selectedFileArray[0],"w");
		if(NULL!=fp)
		{
			YsTextFileOutputStream outStream(fp);

			int wid,hei;
			FsGetWindowSize(wid,hei);
			viewControlDlg->GetViewControl().SaveViewpoint(wid,hei,outStream);

			fclose(fp);
		}
	}
}


YSRESULT GeblGuiEditorBase::OnLoadView(class FsGui3DViewControlDialog *)
{
	YsString dataDir;
	dataDir.EncodeUTF8 <wchar_t> (GetDataDir());

	YsString def;
	def.MakeFullPathName(dataDir,"*.yvw");

	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Open Viewpoint");
	fdlg->fileExtensionArray.Append(L".yvw");
	fdlg->defaultFileName.SetUTF8String(def);
	fdlg->BindCloseModalCallBack(&THISCLASS::OnLoadView_FileSelected,this);
	AttachModalDialog(fdlg);

	return YSOK;
}

void GeblGuiEditorBase::OnLoadView_FileSelected(FsGuiDialog *,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue && 0<fdlg->selectedFileArray.GetN())
	{
		YsWString ful(fdlg->selectedFileArray[0]);

		FILE *fp=YsFileIO::Fopen(fdlg->selectedFileArray[0],"r");
		if(NULL!=fp)
		{
			YsTextFileInputStream inStream(fp);

			int wid,hei;

			viewControlDlg->SaveLastView();
			viewControlDlg->GetViewControl().LoadViewpoint(wid,hei,inStream);

			SetNeedRedraw(YSTRUE);
			fclose(fp);
		}
	}
}

/* virtual */ void GeblGuiEditorBase::OnModalDialogClosed(int dialogIdent,class FsGuiDialog *closedModalDialog,int modalDialogEndCode)
{
	guiExtension->OnModalDialogClosed(dialogIdent,closedModalDialog,modalDialogEndCode);

	auto callbackCopy=closeModalCallBack;  // Caution!  closeModalCallBack may be updated in Invoke.
	closeModalCallBack.Register(NULL,NULL);  // Therefore, clearing call back must be done before Invoke.
	callbackCopy.Invoke(closedModalDialog,modalDialogEndCode);  // And, it should be Invoked from the copy.

	closedModalDialog->UnbindCloseModalCallBack();
}


void GeblGuiEditorBase::MakeInitialEmptyShell(void)
{
	slHd=shlGrp.CreateShell(NULL);
}

GeblGuiFoundation::SlhdAndErrorCode GeblGuiEditorBase::LoadGeneral(const wchar_t fn[],YsShellDnmContainer <YsShellExtEditGui>::Node *parent,YSBOOL recordRecentFiles)
{
	auto result=GeblGuiFoundation::LoadGeneral(fn,parent);

	switch(result.errCode)
	{
	case GEBLERROR_NOERROR:
		if(NULL!=config && YSTRUE==config->autoFixOrientationInDnm)
		{
			YsWString ext,ful(fn);
			ful.GetExtension(ext);
			ext.Capitalize();
			if(0==ext.Strcmp(L".DNM"))
			{
				auto allNode=ShlGrp().GetNodePointerAll();
				for(auto node : allNode)
				{
					YsShellExtEdit_OrientationUtil orientationUtil;
					orientationUtil.FixOrientationBasedOnAssignedNormal(*node);
				}
			}
		}
		if(YSTRUE==config->autoAdjustTolerance)
		{
			YsVec3 bbx[2];
			shlGrp.GetBoundingBox(bbx);
			YsSetToleranceFromDimension((bbx[1]-bbx[0]).GetLength());
			printf("Tolerance adjusted to %.20lf\n",YsTolerance);
		}

		File_Open_AfterOpenFile(result.slHd,fn,recordRecentFiles);
		break;
	case GEBLERROR_FILE_NOT_FOUND:
		MessageDialog(L"Cannot open file.",L"Cannot open file.  The file is inaccessible.");
		break;
	case GEBLERROR_FILE_CORRUPTED:
		MessageDialog(FSGUI_COMMON_ERROR,FSGUI_MENU_FILE_LOADERROR);
		break;
	case GEBLERROR_FILE_NOT_SUPPORTED:
		MessageDialog(L"Error.",L"Unsupported file type.");
		break;
	}
	return result;
}

void GeblGuiEditorBase::SaveGeneral(const YsShellExtEdit &shl,const wchar_t fn[],YSBOOL recordRecentFiles)
{
	YsString fnUtf8;
	fnUtf8.EncodeUTF8 <wchar_t> (fn);

	auto ext=fnUtf8.GetExtension();

	YsWString ful,pth,fil;
	ful.Set(fn);
	ful.SeparatePathFile(pth,fil);

	YsWString savedMsg;
	savedMsg.Append(L"Saved:");
	savedMsg.Append(fil);
	savedMsg.Append(L"\n");
	savedMsg.Append(L"Path: ");
	savedMsg.Append(pth);

	if(0==ext.STRCMP(".SRF"))
	{
		FILE *fp=YsFileIO::Fopen(fn,"w");
		if(NULL!=fp)
		{
			YsTextFileOutputStream outStream(fp);

			YsShellExtWriter writer;
			writer.SaveSrf(outStream,shl.Conv());
			fclose(fp);

			shl.SetFileName(fn);
			shl.Saved();
			lastAccessedFileName.Set(fn);
			if(YSTRUE==recordRecentFiles)
			{
				AddRecentlyUsedFile(fn);
			}

			MessageDialog(FSGUI_DLG_SAVED_TITLE,savedMsg);
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,L"Cannot open the file in write-mode.");
		}
	}
	else if(0==ext.STRCMP(".OBJ"))
	{
		FILE *fp=YsFileIO::Fopen(fn,"w");
		if(NULL!=fp)
		{
			YsTextFileOutputStream outStream(fp);

			YsShellExtObjWriter writer;
			YsShellExtObjWriter::WriteOption option;
			writer.WriteObj(outStream,(const YsShellExt &)shl,option);
			fclose(fp);

			shl.SetFileName(fn);
			shl.Saved();
			lastAccessedFileName.Set(fn);
			if(YSTRUE==recordRecentFiles)
			{
				AddRecentlyUsedFile(fn);
			}

			MessageDialog(FSGUI_DLG_SAVED_TITLE,savedMsg);
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,L"Cannot open the file in write-mode.");
		}
	}
	else if(0==ext.STRCMP(".STL"))
	{
		/* for(auto plHd : shl.AllPolygon())
		{
			if(3!=shl.GetNumVertexOfPolygon(plHd))
			{
				MessageDialog(FSGUI_COMMON_ERROR,
				              L"The model includes at least one non-triangular polygon.\n"
				              L"STL can only save triangles.\n"
				              L"Please use Global->Triangulate All to make all triangles.");
				return;
			}
		} */

		if(YSTRUE==config->SaveBinarySTL())
		{
			FILE *fp=YsFileIO::Fopen(fn,"wb");
			if(NULL!=fp)
			{
				int nIgnored;
				shl.SaveBinStl(nIgnored,fp,"STL generated by PolygonCrest editor.");
				fclose(fp);

				if(YSTRUE==recordRecentFiles)
				{
					AddRecentlyUsedFile(fn);
				}
				shl.SetFileName(fn);
				MessageDialog(FSGUI_DLG_SAVED_TITLE,savedMsg);
			}
			else
			{
				MessageDialog(FSGUI_COMMON_ERROR,L"Cannot open the file in write-mode.");
			}
		}
		else
		{
			FILE *fp=YsFileIO::Fopen(fn,"w");
			if(NULL!=fp)
			{
				int nIgnored;
				shl.SaveStl(nIgnored,fp,"STL generated by PolygonCrest editor.");
				fclose(fp);

				if(YSTRUE==recordRecentFiles)
				{
					AddRecentlyUsedFile(fn);
				}
				shl.SetFileName(fn);
				MessageDialog(FSGUI_DLG_SAVED_TITLE,savedMsg);
			}
			else
			{
				MessageDialog(FSGUI_COMMON_ERROR,L"Cannot open the file in write-mode.");
			}
		}
	}
	else if(0==ext.STRCMP(".OFF"))
	{
		FILE *fp=YsFileIO::Fopen(fn,"w");
		if(NULL!=fp)
		{
			YsTextFileOutputStream outStream(fp);

			YsShellExtOffWriter writer;
			writer.WriteOff(outStream,(const YsShellExt &)shl);
			fclose(fp);

			shl.SetFileName(fn);
			shl.Saved();
			lastAccessedFileName.Set(fn);
			if(YSTRUE==recordRecentFiles)
			{
				AddRecentlyUsedFile(fn);
			}

			MessageDialog(FSGUI_DLG_SAVED_TITLE,savedMsg);
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,L"Cannot open the file in write-mode.");
		}
	}
	else if(0==ext.STRCMP(".T3D"))
	{
		YsFileIO::File fp(fn,"wb");
		if(NULL!=fp)
		{
			YsShellExtFMTOWNSWriter::WriteOption opt;
			YsShellExtFMTOWNSWriter writer;
			if(YSOK!=writer.WriteFMTOWNST3D(fp,shl.Conv(),opt))
			{
				MessageDialog(FSGUI_COMMON_ERROR,L"Write error.");
			}
			else
			{
				MessageDialog(FSGUI_DLG_SAVED_TITLE,savedMsg);
			}
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,L"Cannot open the file in write-mode.");
		}
	}
	else if(0==ext.STRCMP(".DNM"))
	{
		FILE *fp=YsFileIO::Fopen(fn,"w");
		if(NULL!=fp)
		{
			YsTextFileOutputStream outStream(fp);
			shlGrp.SaveDnm(outStream);
			fclose(fp);

			lastAccessedFileName.Set(fn);
			if(YSTRUE==recordRecentFiles)
			{
				AddRecentlyUsedFile(fn);
			}

			MessageDialog(FSGUI_DLG_SAVED_TITLE,savedMsg);
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,L"Cannot open the file in write-mode.");
		}
	}
	else
	{
		MessageDialog(FSGUI_COMMON_ERROR,
		              L"The model cannot be saved in this file type.\n"
		              L"Please use SaveAs and specify a different file extension.");
	}
}

YsWString GeblGuiEditorBase::GetDataDir(void) const
{
	if(0<lastAccessedFileName.Strlen())
	{
		YsWString pth,fil;
		lastAccessedFileName.SeparatePathFile(pth,fil);
		return pth;
	}
	else if(NULL!=config && 0<config->defDataDir.Strlen())
	{
		return config->defDataDir;
	}
	else
	{
		return YsWString(L".");
	}
}

void GeblGuiEditorBase::AddShortCut(int fskey,YSBOOL shift,YSBOOL ctrl,void (*callBackFunc)(void *,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *))
{
	for(YSSIZE_T idx=0; idx<shortCut.GetN(); ++idx)
	{
		if(shortCut[idx].fskey==fskey && shortCut[idx].shift==shift && shortCut[idx].ctrl==ctrl)
		{
			YsPrintf("Internal Error!\n");
			YsPrintf("  Shortcut already defined!\n");
			exit(1);
		}
	}

	shortCut.Increment();
	shortCut.GetEnd().callBackFunc=callBackFunc;
	shortCut.GetEnd().fskey=fskey;
	shortCut.GetEnd().shift=shift;
	shortCut.GetEnd().ctrl=ctrl;
}

void GeblGuiEditorBase::RefreshRecentlyUsedFileList(void)
{
	if(nullptr!=fileRecent)
	{
		YsWString recentFn=fileAndDirName->GetRecentFileListFileName();
		FILE *fp=YsFileIO::Fopen(recentFn,"r");
		if(NULL!=fp)
		{
			YsTextFileInputStream inStream(fp);
			recent.Open(inStream);
			fclose(fp);

			recent.PopulateMenu(*fileRecent,16,File_Recent,this);
		}
	}
}

void GeblGuiEditorBase::AddRecentlyUsedFile(const wchar_t wfn[])
{
	if(nullptr!=fileRecent)
	{
		recent.AddFile(wfn);
		recent.PopulateMenu(*fileRecent,16,File_Recent,this);

		YsWString recentFn=fileAndDirName->GetRecentFileListFileName();

		fileAndDirName->MakeDirectoryForFile(recentFn);

		FILE *fp=YsFileIO::Fopen(recentFn,"w");
		if(NULL!=fp)
		{
			YsTextFileOutputStream outStream(fp);
			recent.Save(outStream,16);
			fclose(fp);
		}
	}
}

void GeblGuiEditorBase::SaveViewPreference(void)
{
	auto fn=fileAndDirName->GetViewConfigFileName();
	YsFileIO::File fp(fn,"w");
	if(nullptr!=fp)
	{
		auto outStream=fp.OutStream();
		propIO.Save(outStream);
	}
}
void GeblGuiEditorBase::LoadViewPreference(void)
{
	auto fn=fileAndDirName->GetViewConfigFileName();
	YsFileIO::File fp(fn,"r");
	if(nullptr!=fp)
	{
		auto inStream=fp.InStream();
		propIO.Load(inStream);
	}
}

// Picking Aware of Dnm-Editing Mode
YsShellVertexHandle GeblGuiEditorBase::PickedVtHd(int mx,int my,YSSIZE_T nExclude,const YsShellVertexHandle excludeVtHd[]) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::PickedVtHd(mx,my,nExclude,excludeVtHd,modelTfm);
}
YsShellVertexHandle GeblGuiEditorBase::PickedVtHdAmongSelection(int mx,int my) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::PickedVtHdAmongSelection(mx,my,modelTfm);
}
YsArray <YsPair <YsShellVertexHandle,double> > GeblGuiEditorBase::BoxedVtHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellVertexHandle excludeVtHd[]) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::BoxedVtHd(x0,y0,x1,y1,nExclude,excludeVtHd,modelTfm);
}
YsArray <YsPair <YsShellVertexHandle,double> > GeblGuiEditorBase::BoxedVtHdAmongSelection(int x0,int y0,int x1,int y1) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::BoxedVtHdAmongSelection(x0,y0,x1,y1,modelTfm);
}
YsShellPolygonHandle GeblGuiEditorBase::PickedPlHd(int mx,int my,YSSIZE_T nExclude,const YsShellPolygonHandle excludePlHd[]) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::PickedPlHd(mx,my,nExclude,excludePlHd,modelTfm);
}
YsShellPolygonHandle GeblGuiEditorBase::PickedPlHdAmongSelection(int mx,int my) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::PickedPlHdAmongSelection(mx,my,modelTfm);
}
YsArray <YsPair <YsShellPolygonHandle,double> > GeblGuiEditorBase::BoxedPlHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellPolygonHandle excludePlHd[]) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::BoxedPlHd(x0,y0,x1,y1,nExclude,excludePlHd,modelTfm);
}
YsArray <YsPair <YsShellPolygonHandle,double> > GeblGuiEditorBase::BoxedPlHdAmongSelection(int x0,int y0,int x1,int y1) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::BoxedPlHdAmongSelection(x0,y0,x1,y1,modelTfm);
}

YsArray <YsPair <YsShellPolygonHandle,double> > GeblGuiEditorBase::EnclosedPlHd(const class PolyCreSketchInterface &sketchUI) const
{
	YsArray <YsVec2> winCoord;
	for(auto c : sketchUI.GetStroke())
	{
		winCoord.push_back(c.winCoord);
	}
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::EnclosedPlHd(winCoord,modelTfm);
}

YsShellExt::ConstEdgeHandle GeblGuiEditorBase::PickedCeHd(int mx,int my,YSSIZE_T nExclude,const YsShellExt::ConstEdgeHandle excludeCeHd[]) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::PickedCeHd(mx,my,nExclude,excludeCeHd,modelTfm);
}
YsShellExt::ConstEdgeHandle GeblGuiEditorBase::PickedCeHdAmongSelection(int mx,int my) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::PickedCeHdAmongSelection(mx,my,modelTfm);
}
YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > GeblGuiEditorBase::BoxedCeHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellExt::ConstEdgeHandle excludeCeHd[]) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::BoxedCeHd(x0,y0,x1,y1,nExclude,excludeCeHd,modelTfm);
}
YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > GeblGuiEditorBase::BoxedCeHdAmongSelection(int x0,int y0,int x1,int y1) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::BoxedCeHdAmongSelection(x0,y0,x1,y1,modelTfm);
}
YsShellExt::FaceGroupHandle GeblGuiEditorBase::PickedFgHd(int mx,int my,int nExclude,const YsShellExt::FaceGroupHandle excludeFgHd[]) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::PickedFgHd(mx,my,nExclude,excludeFgHd,modelTfm);
}
YsShellExt::FaceGroupHandle GeblGuiEditorBase::PickedFgHdAmongSelection(int mx,int my) const
{
	YsMatrix4x4 modelTfm;
	if(YSTRUE==dnmEditMode && nullptr!=slHd)
	{
		modelTfm=dnmState.GetNodeToRootTransformation(slHd);
	}
	return GeblGuiFoundation::PickedFgHdAmongSelection(mx,my,modelTfm);
}
const YsShellDnmContainer <YsShellExtEditGui>::Node *GeblGuiEditorBase::PickedSlHd(int mx,int my,YSSIZE_T nExclude,const YsShellDnmContainer <YsShellExtEditGui>::Node *excludeSlHd[]) const
{
	if(YSTRUE==dnmEditMode)
	{
		return GeblGuiFoundation::PickedSlHd(mx,my,nExclude,excludeSlHd,&dnmState);
	}
	else
	{
		return GeblGuiFoundation::PickedSlHd(mx,my,nExclude,excludeSlHd,nullptr);
	}
}
YsShellDnmContainer <YsShellExtEditGui>::Node *GeblGuiEditorBase::PickedSlHd(int mx,int my,YSSIZE_T nExclude,const YsShellDnmContainer <YsShellExtEditGui>::Node *excludeSlHd[])
{
	if(YSTRUE==dnmEditMode)
	{
		return GeblGuiFoundation::PickedSlHd(mx,my,nExclude,excludeSlHd,&dnmState);
	}
	else
	{
		return GeblGuiFoundation::PickedSlHd(mx,my,nExclude,excludeSlHd,nullptr);
	}
}
// Picking <<

////////////////////////////////////////////////////////////



int FsGui3dAppSleepIntervalInMS(void)
{
	return appSleepInterval;
}

class FsGui3DAppCommandParameterInfo
{
public:
	YSBOOL forceEnglishMode;
	YSBOOL peacefullyExit;

	FsGui3DAppCommandParameterInfo();
	YSRESULT RecognizeCommandParameter(YsArray <char *> &args);
};

FsGui3DAppCommandParameterInfo::FsGui3DAppCommandParameterInfo()
{
	forceEnglishMode=YSFALSE;
	peacefullyExit=YSFALSE;
}

YSRESULT FsGui3DAppCommandParameterInfo::RecognizeCommandParameter(YsArray <char *> &args)
{
	YSSIZE_T idx=0;
	while(idx<args.GetN())
	{
		if(0==strcmp(args[idx],"-en"))
		{
			forceEnglishMode=YSTRUE;
			args.Delete(idx);
		}
		else if(0==strcmp(args[idx],"-h"))
		{
			YsPrintf("Command Parameters:\n");
			YsPrintf("  -h Print help.\n");
			YsPrintf("  -en Force English mode.\n");
			peacefullyExit=YSTRUE;
			args.Delete(idx);
		}
		else
		{
			++idx;
		}
	}
	return YSOK;
}

GeblGuiEditorBase *FsGui3DAppInitialize(int ac,char *av[])
{
	YsArray <char *> args;
	for(int i=0; i<ac; ++i)
	{
		args.Append(av[i]);
	}

	FsGui3DAppCommandParameterInfo cpmi;
	if(YSOK!=cpmi.RecognizeCommandParameter(args))
	{
		printf("Parameter Error!\n");
		exit(1);
	}
	if(YSTRUE==cpmi.peacefullyExit)
	{
		exit(1);
	}

	YsWString progBaseDir;
	if(YSOK==YsSpecialPath::GetProgramBaseDir(progBaseDir))
	{
		YsLocale locale;
		const char *langStr=locale.GetLanguagePart();

		if(YSTRUE==cpmi.forceEnglishMode)
		{
			langStr="en";
		}

		if(NULL!=langStr)
		{
			YsWString langWStr;
			langWStr.SetUTF8String(langStr);

			YsWString langPath;
			langPath.MakeFullPathName(progBaseDir,L"language");

			YsWString langFileName;
			langFileName.MakeFullPathName(langPath,langWStr);

			langFileName.ReplaceExtension(L".uitxt");

			printf("%ls\n",langFileName.Txt());

			FILE *fp=YsFileIO::Fopen(langFileName,"r");
			if(NULL!=fp)
			{
				fsGuiTextResource.LoadFile(fp);
				fclose(fp);
			}
		}
	}



	auto appMainCanvas=new GeblGuiEditorBase;
	YsGLSLRenderer::CreateSharedRenderer();

	appMainCanvas->drawEnv.SetTargetBoundingBox(-10.0*YsXYZ(),10.0*YsXYZ());
	appMainCanvas->drawEnv.SetViewTarget(YsOrigin());
	appMainCanvas->drawEnv.SetViewDistance(15.0);

	appMainCanvas->MakeInitialEmptyShell();

	appMainCanvas->colorPaletteDlg->MakeDialog(appMainCanvas);
	appMainCanvas->colorPaletteDlg->SetIsPermanent(YSTRUE);
	appMainCanvas->AddDialog(appMainCanvas->colorPaletteDlg);

	appMainCanvas->fileAndDirName->GetOptionFileName();

	appMainCanvas->Option_ReapplyConfig();

	if(YSOPENGL==YsCoordSysModel)
	{
		appMainCanvas->drawEnv.SetViewDirection(-YsXYZ());
	}
	else
	{
		appMainCanvas->drawEnv.SetViewDirection(YsVec3(1.0,-1.0,-1.0));
	}



	if(2<=args.GetN())
	{
		YsWString wfn;
		wfn.SetUTF8String(args[1]);

		YsWString pth,fil;
		wfn.SeparatePathFile(pth,fil);

		if(0==pth.Strlen())
		{
			wfn.MakeFullPathName(L".",fil);
		}
		wfn=YsFileIO::GetRealPath(wfn);

	#ifndef __APPLE__
		appMainCanvas->LoadGeneral(wfn,NULL,YSTRUE);
	#else
		// Some garbage arguments are given from the Finder.
		// It still can take parameters when it is launched from the terminal or a script.
		// It needs to distinguish if the argument is a supported file, or one of the garbage parameters.
		if(YSTRUE==appMainCanvas->IsSupportedFileType(wfn) &&
		   YSTRUE==YsFileIO::CheckFileExist(wfn))
		{
			appMainCanvas->LoadGeneral(wfn,NULL,YSTRUE);
		}
	#endif
	}


	if(NULL!=appMainCanvas->guiExtension)
	{
		appMainCanvas->guiExtension->FinalSetUp();
	}

	return appMainCanvas;
}

