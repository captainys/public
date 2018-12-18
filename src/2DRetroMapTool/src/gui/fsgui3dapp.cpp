/* ////////////////////////////////////////////////////////////

File Name: fsgui3dapp.cpp
Copyright (c) 2015 Soji Yamakawa.  All rights reserved.
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

#include <ysclass.h>
#include <ysport.h>
#include <yscompilerwarning.h>
#include <ysgl.h>

#include "fsgui3dapp.h"
#include "textresource.h"


static FsGui3DMainCanvas *appMainCanvas;

////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::DrawingToolDialog::Make(FsGui3DMainCanvas *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	noneBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_DRAWING_NONE,YSFALSE);
	lineBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_DRAWING_LINE,YSFALSE);
	lineStripBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_DRAWING_LINESTRIP,YSFALSE);

	{
		const int nInGrp=3;
		FsGuiButton *radioBtnGrp[nInGrp]={noneBtn,lineBtn,lineStripBtn};
		SetRadioButtonGroup(nInGrp,radioBtnGrp);
	}

	multipleBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_DRAWING_MULTIPLEELEM,YSFALSE);

	lineColor=AddCompactColorPalette(0,FSKEY_NULL,"",255,0,0,YSFALSE);
	fillColor=AddCompactColorPalette(0,FSKEY_NULL,"",255,255,255,YSFALSE);
	fillBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_DRAWING_FILL,YSFALSE);
	fillBtn->SetCheck(YSFALSE);
	frontArrowBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"<-",YSFALSE);
	backArrowBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"->",YSFALSE);

	arrowSizeTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,"ArrowSize",4,YSFALSE);
	arrowSizeTxt->SetTextType(FSGUI_REALNUMBER);
	arrowSizeTxt->SetRealNumber(canvasPtr->world.GetDefaultArrowSize(),1);
	makeDefaultArrowSizeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,L"Make Default Size",YSFALSE);

	autoAnchorBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_DRAWING_AUTOANCHOR,YSFALSE);
	autoAnchorBtn->SetCheck(YSTRUE);

	closeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CLOSE,YSFALSE);
	Fit();
	SetArrangeType(FSDIALOG_ARRANGE_BOTTOM_LEFT);
}
void FsGui3DMainCanvas::DrawingToolDialog::TurnOffDrawingMode(void)
{
	if(canvasPtr->mode==DRAWING_LINE || canvasPtr->mode==DRAWING_LINE_STRIP)
	{
		canvasPtr->Edit_ClearUIMode(nullptr);
	}
}
/* virtual */ void FsGui3DMainCanvas::DrawingToolDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==noneBtn)
	{
		TurnOffDrawingMode();
	}
	else if(btn==lineBtn)
	{
		canvasPtr->MarkUp_InsertLine(nullptr);
		btn->SetCheck(YSTRUE);  // Edit_ClearUIMode un-checks the button.  Need re-check.
	}
	else if(btn==lineStripBtn)
	{
		canvasPtr->MarkUp_InsertLineStrip(nullptr);
		btn->SetCheck(YSTRUE);  // Edit_ClearUIMode un-checks the button.  Need re-check.
	}
	else if(btn==closeBtn)
	{
		canvasPtr->RemoveDialog(this);
		canvasPtr->Edit_ClearUIMode(nullptr);
	}
	else if(btn==makeDefaultArrowSizeBtn)
	{
		auto sz=arrowSizeTxt->GetRealNumber();
		canvasPtr->world.SetDefaultArrowSize(sz);
	}
}

////////////////////////////////////////////////////////////

class FsGui3DMainCanvas::ControlDialog : public FsGuiDialog
{
public:
	FsGui3DMainCanvas *canvasPtr;
	FsGuiDropList *fieldDrp;
	FsGuiStatic *numBmpTxt;
	FsGuiButton *up,*down,*left,*right;
	FsGuiButton *addFile,*addNewScrnshot,*addCommentBtn;
	FsGuiButton *showDiffBtn;
	FsGuiNumberBox *diffThrNbx;
	FsGuiButton *adjustBtn;
	void Make(FsGui3DMainCanvas *canvasPtr);
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnDropListSelChange(FsGuiDropList *drp,int prevSel);
	virtual void OnNumberBoxChange(FsGuiNumberBox *nbx,int prevNum);
};

void FsGui3DMainCanvas::ControlDialog::Make(FsGui3DMainCanvas *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	FsGuiDialog::Initialize();

	fieldDrp=AddEmptyDropList(0,FSKEY_NULL,"",32,24,24,YSTRUE);
	up=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_CONTROL_UP,YSFALSE);
	addFile=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_CONTROL_ADD_IMAGE_FILE,YSFALSE);
	addNewScrnshot=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_CONTROL_ADD_NEW_SCRNSHOT,YSFALSE);
	addCommentBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_CONTROL_ADD_COMMENT,YSFALSE);

	numBmpTxt=AddStaticText(0,FSKEY_NULL,L"",YSTRUE);
	left=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_CONTROL_LEFT,YSFALSE);
	down=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_CONTROL_DOWN,YSFALSE);
	right=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_CONTROL_RIGHT,YSFALSE);
	showDiffBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_CONTROL_SHOWDIFF,YSFALSE);
	showDiffBtn->SetCheck(YSTRUE);
	diffThrNbx=AddNumberBox(0,FSKEY_NULL,FSGUI_DLG_CONTROL_DIFFTHR,8,32,1,255,1,YSFALSE);
	adjustBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_CONTROL_ADJUSTMENT,YSFALSE);

	YsArray <YsArray <FsGuiDialogItem *> > matrix;
	matrix.Increment();
	matrix.Last().Add(fieldDrp); 	matrix.Last().Add(nullptr); matrix.Last().Add(up); 		matrix.Last().Add(nullptr); 
	matrix.Last().Add(addFile);		matrix.Last().Add(addNewScrnshot);	matrix.Last().Add(addCommentBtn);

	matrix.Increment();
	matrix.Last().Add(numBmpTxt); 	matrix.Last().Add(left); 	matrix.Last().Add(down); 	matrix.Last().Add(right); 
	matrix.Last().Add(showDiffBtn); 	matrix.Last().Add(diffThrNbx);			matrix.Last().Add(adjustBtn);
	AlignLeftMiddle(matrix);

	Fit();
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
}
/* virtual */ void FsGui3DMainCanvas::ControlDialog::OnButtonClick(FsGuiButton *btn)
{
	if(up==btn)
	{
		canvasPtr->OnUpArrowKey(nullptr);
	}
	if(left==btn)
	{
		canvasPtr->OnLeftArrowKey(nullptr);
	}
	if(right==btn)
	{
		canvasPtr->OnRightArrowKey(nullptr);
	}
	if(down==btn)
	{
		canvasPtr->OnDownArrowKey(nullptr);
	}
	if(addFile==btn)
	{
		canvasPtr->File_InsertImage(nullptr);
	}
	if(addNewScrnshot==btn)
	{
		canvasPtr->MapPiece_InsertNewScreenshot(nullptr);
	}
	if(addCommentBtn==btn)
	{
		canvasPtr->Edit_Insert_Comment(nullptr);
	}
	if(adjustBtn==btn)
	{
		canvasPtr->MapPiece_MinimizeError(nullptr);
	}
}
/* virtual */ void FsGui3DMainCanvas::ControlDialog::OnDropListSelChange(FsGuiDropList *drp,int prevSel)
{
	if(drp==fieldDrp)
	{
		auto key=fieldDrp->GetIntAttrib(fieldDrp->GetSelection());
		auto newSelFdHd=canvasPtr->world.FindField(key);
		if(nullptr!=newSelFdHd && newSelFdHd!=canvasPtr->GetCurrentField())
		{
			canvasPtr->SetCurrentField(newSelFdHd);
		}
	}
}
/* virtual */ void FsGui3DMainCanvas::ControlDialog::OnNumberBoxChange(FsGuiNumberBox *nbx,int prevNum)
{
	if(nbx==diffThrNbx)
	{
		canvasPtr->world.SetDiffThreshold(nbx->GetNumber());
		canvasPtr->UpdateDiff();
	}
}

////////////////////////////////////////////////////////////

FsGui3DMainCanvas::FsGui3DMainCanvas() : threeDInterface(this),viewControlDlg(NULL)
{
	appMustTerminate=YSFALSE;

	mode=NORMAL;
	defAction=DEFAULTACTION_STANDBY;
	insertMarkUp_Status=INSERTMARKUP_STANDBY;

	currentFieldKey=YSNULLHASHKEY;

	controlDlg=nullptr;
	commentDlg=nullptr;
	drawingDlg=nullptr;

	view_showPositionReferenceMarker=nullptr;
	view_restoreViewWhenSwitchingField=nullptr;

	showDiff=YSFALSE;
}

FsGui3DMainCanvas::~FsGui3DMainCanvas()
{
	// The following two lines ensure that all self-destructive dialogs are cleaned. 2015/03/18
	RemoveDialogAll();
	PerformScheduledDeletion();

	DeleteMainMenu();
	if(NULL!=viewControlDlg)
	{
		FsGui3DViewControlDialog::Delete(viewControlDlg);
	}

	position_reference_marker=position_reference_marker.Origin();
}

void FsGui3DMainCanvas::Initialize(int argc,char *argv[])
{
	YsDisregardVariable(argc);
	YsDisregardVariable(argv);


	const YsWString optFile(fileAndDirName.GetOptionFileName());
	if(YSOK==config.Load(optFile))
	{
		YsFileIO::File tstCrashWhileCache(fileAndDirName.GetCachingIndicatorFile(),"rb");
		if(nullptr!=tstCrashWhileCache)
		{
			tstCrashWhileCache.Close();
			Config_Config(nullptr);
			auto configDlg=GetActiveModalDialog();
			if(nullptr!=configDlg)
			{
				auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
				msgDlg->Make(FSGUI_COMMON_ERROR,FSGUI_ERROR_CRASH_WHILE_CACHE,FSGUI_COMMON_CLOSE,nullptr);
				configDlg->AttachModalDialog(msgDlg);
			}
		}
		else
		{
			config.UpdateScreenshotCache();
		}
	}
	else
	{
		// Use default paths.
		config.UpdateScreenshotCache();
	}


	YsGLSLCreateSharedRenderer();

	YsVec3 minmax[2]={YsVec3::Origin(),YsVec3(1024.0,1024.0,0.0)};
	drawEnv.SetTargetBoundingBox(minmax[0],minmax[1]);
	drawEnv.SetViewTarget((minmax[0]+minmax[1])/2.0);
	drawEnv.SetViewDistance(750.0);

	YsWString progBaseDir;
	if(YSOK==YsSpecialPath::GetProgramBaseDir(progBaseDir))
	{
		YsLocale locale;
		const char *langStr=locale.GetLanguagePart();

		// if(YSTRUE==cpmi.forceEnglishMode)
		// {
		// 	langStr="en";
		// }

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

	MakeMainMenu();
	AddViewControlDialog();
	drawEnv.AttachCanvas(this);

	drawEnv.SetStraightAttitude(YsAtt3(0,0,0));
	drawEnv.SetF1ViewAttitude(YsAtt3(0,0,0));
	drawEnv.SetF4ViewAttitude(YsAtt3(YsPi,0,0));
	drawEnv.StraightenAttitude();
	drawEnv.SetOrientationLock(YSTRUE);
	drawEnv.SetPerspective(YSFALSE);

	controlDlg=FsGuiDialog::CreateSelfDestructiveDialog<ControlDialog>();
	controlDlg->SetIsPermanent(YSTRUE);
	controlDlg->Make(this);
	AddDialog(controlDlg);
	ArrangeDialog();

	Edit_ClearUIMode(nullptr);

	if(2==argc)
	{
		YsWString wStr;
		wStr.SetUTF8String(argv[1]);
		wStr=YsFileIO::GetRealPath(wStr);
		File_Open_Open(wStr);
	}
}

void FsGui3DMainCanvas::AddViewControlDialog(void)
{
	viewControlDlg=FsGui3DViewControlDialog::Create();
	viewControlDlg->Make(this,&drawEnv);
	this->AddDialog(viewControlDlg);
}

void FsGui3DMainCanvas::DeleteMainMenu(void)
{
	delete mainMenu;
}

YSBOOL FsGui3DMainCanvas::ShowDiff(void) const
{
	if(nullptr!=controlDlg)
	{
		return controlDlg->showDiffBtn->GetCheck();
	}
	return YSFALSE;
}
int FsGui3DMainCanvas::GetDiffThreshold(void) const
{
	if(nullptr!=controlDlg)
	{
		return controlDlg->diffThrNbx->GetNumber();
	}
	return 32;
}
void FsGui3DMainCanvas::UpdateDiff(void)
{
	auto fdHd=GetCurrentField();
	if(YSTRUE==ShowDiff() && nullptr!=fdHd)
	{
		RetroMap_World::MapPieceStore mpStore,exclusion;

		auto fieldPtr=world.GetField(fdHd);
		auto selMpHd=fieldPtr->SelectedMapPiece();
		exclusion.Add(selMpHd);

		// At this time, limit to first a few.
		const int maxDiffCount=4;
		if(4<selMpHd.GetN())
		{
			selMpHd.Resize(4);
		}

		for(auto mpHd : selMpHd)
		{
			mpStore.Add(mpHd);
		}
		UpdateDiff(mpStore,exclusion);
	}
}

void FsGui3DMainCanvas::UpdateDiff(
    const RetroMap_World::MapPieceStore &mpStore,
    const RetroMap_World::MapPieceStore &exclusion)
{
	auto fdHd=GetCurrentField();
	if(YSTRUE==ShowDiff() && nullptr!=fdHd)
	{
		auto diff=world.MakeDiff(fdHd,mpStore,exclusion,GetDiffThreshold(),YsRed());
		double errorPerPixel=0.0;
		if(0<diff.numSample)
		{
			errorPerPixel=(double)diff.totalError/(double)diff.numSample;
		}
		printf("NumSample %d TotalError %d ErrorPerPixel %lf NumErrorPixel %d\n",(int)diff.numSample,(int)diff.totalError,errorPerPixel,(int)diff.numErrorPixel);
	}
}

RetroMap_World::FieldHandle FsGui3DMainCanvas::GetCurrentFieldCreateIfNotExist(void)
{
	auto fdHd=world.FindField(currentFieldKey);
	if(nullptr==fdHd)
	{
		fdHd=world.CreateField();
		currentFieldKey=world.GetSearchKey(fdHd);
		UpdateFieldSelector();
	}
	return fdHd;
}

RetroMap_World::FieldHandle FsGui3DMainCanvas::GetCurrentField(void) const
{
	return world.FindField(currentFieldKey);
}

void FsGui3DMainCanvas::DisableDefaultAction(void)
{
	ClearLButtonDownCallBack();
	ClearMouseMoveCallBack();
	ClearLButtonUpCallBack();
	ClearTouchStateChangeCallBack();

	ClearKeyCallBack(FSKEY_UP,YSFALSE,YSFALSE,YSFALSE);
	ClearKeyCallBack(FSKEY_DOWN,YSFALSE,YSFALSE,YSFALSE);
	ClearKeyCallBack(FSKEY_LEFT,YSFALSE,YSFALSE,YSFALSE);
	ClearKeyCallBack(FSKEY_RIGHT,YSFALSE,YSFALSE,YSFALSE);

	ClearKeyCallBack(FSKEY_UP,YSTRUE,YSFALSE,YSFALSE);
	ClearKeyCallBack(FSKEY_DOWN,YSTRUE,YSFALSE,YSFALSE);
	ClearKeyCallBack(FSKEY_LEFT,YSTRUE,YSFALSE,YSFALSE);
	ClearKeyCallBack(FSKEY_RIGHT,YSTRUE,YSFALSE,YSFALSE);

	ClearKeyCallBack(FSKEY_UP,YSTRUE,YSTRUE,YSFALSE);
	ClearKeyCallBack(FSKEY_DOWN,YSTRUE,YSTRUE,YSFALSE);
	ClearKeyCallBack(FSKEY_LEFT,YSTRUE,YSTRUE,YSFALSE);
	ClearKeyCallBack(FSKEY_RIGHT,YSTRUE,YSTRUE,YSFALSE);

	ClearKeyCallBack(FSKEY_UP,YSFALSE,YSTRUE,YSFALSE);
	ClearKeyCallBack(FSKEY_DOWN,YSFALSE,YSTRUE,YSFALSE);

	controlDlg->up->Disable();
	controlDlg->left->Disable();
	controlDlg->right->Disable();
	controlDlg->down->Disable();
}
void FsGui3DMainCanvas::EnableDefaultAction(void)
{
	BindLButtonDownCallBack(&THISCLASS::OnLButtonDown,this);
	BindMouseMoveCallBack(&THISCLASS::OnMouseMove,this);
	BindLButtonUpCallBack(&THISCLASS::OnLButtonUp,this);
	BindTouchStateChangeCallBack(&THISCLASS::OnTouchStateChange,this);

	ClearKeyCallBack(FSKEY_SPACE,YSFALSE,YSFALSE,YSFALSE);
	ClearKeyCallBack(FSKEY_BS,YSFALSE,YSFALSE,YSFALSE);
	ClearKeyCallBack(FSKEY_SPACE,YSFALSE,YSFALSE,YSTRUE);
	ClearKeyCallBack(FSKEY_BS,YSFALSE,YSFALSE,YSTRUE);

	BindKeyCallBack(FSKEY_UP,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::OnUpArrowKey,this);
	BindKeyCallBack(FSKEY_DOWN,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::OnDownArrowKey,this);
	BindKeyCallBack(FSKEY_LEFT,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::OnLeftArrowKey,this);
	BindKeyCallBack(FSKEY_RIGHT,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::OnRightArrowKey,this);

	BindKeyCallBack(FSKEY_UP,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::OnUpArrowKeyShift,this);
	BindKeyCallBack(FSKEY_DOWN,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::OnDownArrowKeyShift,this);
	BindKeyCallBack(FSKEY_LEFT,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::OnLeftArrowKeyShift,this);
	BindKeyCallBack(FSKEY_RIGHT,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::OnRightArrowKeyShift,this);

	BindKeyCallBack(FSKEY_UP,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::OnUpArrowKeyCtrl,this);
	BindKeyCallBack(FSKEY_DOWN,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::OnDownArrowKeyCtrl,this);
	BindKeyCallBack(FSKEY_LEFT,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::OnLeftArrowKeyCtrl,this);
	BindKeyCallBack(FSKEY_RIGHT,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::OnRightArrowKeyCtrl,this);

	BindKeyCallBack(FSKEY_PAGEUP,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::OnPageUpKey,this);
	BindKeyCallBack(FSKEY_PAGEDOWN,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::OnPageDownKey,this);

	controlDlg->up->Enable();
	controlDlg->left->Enable();
	controlDlg->right->Enable();
	controlDlg->down->Enable();
}

YsVec2i FsGui3DMainCanvas::GetPointToInsert(void) const
{
	auto o=drawEnv.GetViewPosition(),v=drawEnv.GetViewDirection();
	YsPlane pln;
	pln.Set(YsOrigin(),YsZVec());

	YsVec3 crs;
	if(YSOK==pln.GetIntersection(crs,o,v))
	{
		YsVec2i pos;
		pos.Set((int)crs.x(),(int)crs.y());
		return pos;
	}
	return YsVec2i::Origin();
}

void FsGui3DMainCanvas::SetCurrentField(RetroMap_World::FieldHandle fdHd)
{
	auto prevFdHd=GetCurrentField();
	if(prevFdHd!=fdHd)
	{
		if(nullptr!=view_restoreViewWhenSwitchingField && YSTRUE==view_restoreViewWhenSwitchingField->GetCheck())
		{
			auto fieldPtr=world.GetField(GetCurrentField());
			if(nullptr!=fieldPtr)
			{
				int wid,hei;
				FsGetWindowSize(wid,hei);
				fieldPtr->savedView=drawEnv.SaveViewpoint(wid,hei);
			}
		}

		currentFieldKey=world.GetSearchKey(fdHd);

		if(nullptr!=view_restoreViewWhenSwitchingField && YSTRUE==view_restoreViewWhenSwitchingField->GetCheck())
		{
			auto fieldPtr=world.GetField(fdHd);
			if(0<fieldPtr->savedView.GetN())
			{
				int w,h;
				drawEnv.LoadViewpoint(w,h,fieldPtr->savedView);
			}
		}
		UpdateFieldSelector();
		UpdateNumBmp();
	}
}

void FsGui3DMainCanvas::UpdateFieldSelector(void)
{
	if(nullptr!=controlDlg)
	{
		controlDlg->fieldDrp->ClearChoice();
		for(auto fdHd : world.AllField())
		{
			auto select=(fdHd==GetCurrentField() ? YSTRUE : YSFALSE);
			auto key=world.GetSearchKey(fdHd);
			auto fdName=world.GetFieldName(fdHd);
			auto id=controlDlg->fieldDrp->AddString(fdName,select);
			controlDlg->fieldDrp->SetIntAttrib(id,(int)world.GetSearchKey(fdHd));
		}
	}
}

void FsGui3DMainCanvas::UpdateNumBmp(void)
{
	if(nullptr!=controlDlg)
	{
		auto fieldPtr=world.GetField(GetCurrentField());
		if(nullptr!=fieldPtr)
		{
			YsString str;
			str.Printf("%d bitmaps",(int)fieldPtr->GetNumMapPiece());
			controlDlg->numBmpTxt->SetText(str);
		}
		else
		{
			controlDlg->numBmpTxt->SetText(L"");
		}
	}
}

void FsGui3DMainCanvas::UpdateDiffThresholdFromWorld(void)
{
	if(nullptr!=controlDlg)
	{
		controlDlg->diffThrNbx->SetNumber(world.GetDiffThreshold());
	}
}

void FsGui3DMainCanvas::OpenDrawingDialog(void)
{
	if(nullptr==drawingDlg)
	{
		drawingDlg=new DrawingToolDialog;
		drawingDlg->Make(this);
	}
	drawingDlg->SetIsPermanent(YSTRUE);
	AddDialog(drawingDlg);
	ArrangeDialog();
}

void FsGui3DMainCanvas::TurnOffDrawingMode(void)
{
	if(nullptr!=drawingDlg)
	{
		drawingDlg->lineBtn->SetCheck(YSFALSE);
		drawingDlg->lineStripBtn->SetCheck(YSFALSE);
	}
}

void FsGui3DMainCanvas::SnapToUnit(YsVec3 &pos) const
{
	auto unit=world.GetUnit();
	if(1<unit.x())
	{
		double dx=unit.xd();
		pos.SetX(floor((pos.x()+dx*0.5)/dx)*dx);
	}
	if(1<unit.y())
	{
		double dy=unit.yd();
		pos.SetY(floor((pos.y()+dy*0.5)/dy)*dy);
	}
}

void FsGui3DMainCanvas::SnapToUnit(YsVec2i &pos) const
{
	YsVec3 pos3;
	pos3.Set(pos.xd(),pos.yd(),0.0);
	SnapToUnit(pos3);
	pos.Set((int)pos3.x(),(int)pos3.y());
}

void FsGui3DMainCanvas::OnInterval(void)
{
	FsGuiCanvas::Interval();

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

	if(YSTRUE==drawEnv.NeedRedraw() ||
	   YSTRUE==threeDInterface.NeedRedraw() ||
	   0!=FsCheckWindowExposure())
	{
		SetNeedRedraw(YSTRUE);
	}
}

RetroMap_World::MapPieceHandle FsGui3DMainCanvas::PickedMpHd(YsVec2i pos) const
{
	YsVec3 o,v;
	drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

	YsPlane pln(YsOrigin(),YsZVec());
	YsVec3 crs;
	if(YSOK==pln.GetIntersection(crs,o,v))
	{
		RetroMap_World::MapPieceHandle pickedMpHd=nullptr;
		auto fieldPtr=world.GetField(GetCurrentField());
		if(nullptr!=fieldPtr)
		{
			for(auto mpHd : fieldPtr->AllMapPiece())
			{
				auto mapPiece=fieldPtr->GetMapPiece(mpHd);
				auto bbx=mapPiece->GetBoundingBox();
				if(YSTRUE==bbx.IsInside(crs))
				{
					pickedMpHd=mpHd;
				}
			}
		}
		return pickedMpHd;
	}

	return nullptr;
}

RetroMap_World::MarkUpHandle FsGui3DMainCanvas::PickedMuHd(YsVec2i pos) const
{
	YsVec3 o,v;
	drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

	YsPlane pln(YsOrigin(),YsZVec());
	YsVec3 crs;
	RetroMap_World::MarkUpHandle pickedMuHd=nullptr;
	if(YSOK==pln.GetIntersection(crs,o,v))
	{
		auto fieldPtr=world.GetField(GetCurrentField());
		if(nullptr!=fieldPtr)
		{
			for(auto muHd : fieldPtr->AllMarkUp())
			{
				auto markUp=fieldPtr->GetMarkUp(muHd);
				if(markUp->MARKUP_STRING==markUp->GetMarkUpType())
				{
					auto bbx=markUp->GetBoundingBox();
					if(YSTRUE==bbx.IsInside(crs))
					{
					 	pickedMuHd=muHd;
					}
				}
				else if(markUp->MARKUP_POINT_ARRAY==markUp->GetMarkUpType())
				{
					auto plg=markUp->GetPointArray();
					for(auto idx : plg.AllIndex())
					{
						if(YSTRUE!=markUp->IsLoop() && idx==plg.GetN()-1)
						{
							break;
						}
						const YsVec3 line[2]=
						{
							YsVec3(plg[idx].x(),plg[idx].y(),0),
							YsVec3(plg.GetCyclic(idx+1).x(),plg.GetCyclic(idx+1).y(),0)
						};
						double z;
						if(YSTRUE==drawEnv.IsLinePicked(z,pos.x(),pos.y(),GetPickableDistance(),line[0],line[1]))
						{
							pickedMuHd=muHd;
						}
					}
				}
			}
		}
	}
	return pickedMuHd;
}

RetroMap_World::MarkUpPointIndex FsGui3DMainCanvas::PickedMarkUpPoint(YsVec2i pos) const
{
	RetroMap_World::MarkUpPointIndex pickedMupi;

	YsVec3 o,v;
	drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

	YsPlane pln(YsOrigin(),YsZVec());
	YsVec3 crs;
	if(YSOK==pln.GetIntersection(crs,o,v))
	{
		auto fieldPtr=world.GetField(GetCurrentField());
		if(nullptr!=fieldPtr)
		{
			for(auto muHd : fieldPtr->AllMarkUp())
			{
				auto markUp=fieldPtr->GetMarkUp(muHd);
				if(markUp->MARKUP_POINT_ARRAY==markUp->GetMarkUpType())
				{
					auto plg=markUp->GetPointArray();
					for(auto idx : plg.AllIndex())
					{
						YsVec3 point(plg[idx].x(),plg[idx].y(),0);
						double z;
						if(YSTRUE==drawEnv.IsPointPicked(z,pos.x(),pos.y(),GetPickableDistance(),point))
						{
							pickedMupi.muHd=muHd;
							pickedMupi.idx=idx;
						}
					}
				}
			}
		}
	}

	return pickedMupi;
}

RetroMap_World::MarkUpPointIndex FsGui3DMainCanvas::PickedMarkUpPointWithinSelection(YsVec2i pos,const YsConstArrayMask <RetroMap_World::MarkUpHandle> &muHdFilter) const
{
	RetroMap_World::MarkUpPointIndex pickedMupi;

	YsVec3 o,v;
	drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

	YsPlane pln(YsOrigin(),YsZVec());
	YsVec3 crs;
	if(YSOK==pln.GetIntersection(crs,o,v))
	{
		auto fieldPtr=world.GetField(GetCurrentField());
		if(nullptr!=fieldPtr)
		{
			for(auto muHd : muHdFilter)
			{
				auto markUp=fieldPtr->GetMarkUp(muHd);
				if(markUp->MARKUP_POINT_ARRAY==markUp->GetMarkUpType())
				{
					auto plg=markUp->GetPointArray();
					for(auto idx : plg.AllIndex())
					{
						YsVec3 point(plg[idx].x(),plg[idx].y(),0);
						double z;
						if(YSTRUE==drawEnv.IsPointPicked(z,pos.x(),pos.y(),GetPickableDistance(),point))
						{
							pickedMupi.muHd=muHd;
							pickedMupi.idx=idx;
						}
					}
				}
			}
		}
	}

	return pickedMupi;
}

int FsGui3DMainCanvas::GetPickableDistance(void) const
{
	return 8;
}

YsArray <RetroMap_World::MapPieceHandle> FsGui3DMainCanvas::BoxedMpHd(YsVec2 min,YsVec2 max) const
{
	YsRect2 rect(min,max);
	YsArray <RetroMap_World::MapPieceHandle> boxedMpHd;
	auto fieldPtr=world.GetField(GetCurrentField());
	if(nullptr!=fieldPtr)
	{
		for(auto mpHd : fieldPtr->AllMapPiece())
		{
			auto mapPiece=fieldPtr->GetMapPiece(mpHd);
			auto bbx=mapPiece->GetBoundingBox();
			if(YSTRUE==rect.IsInside(bbx[0].xy()) &&
			   YSTRUE==rect.IsInside(bbx[1].xy()))
			{
				boxedMpHd.Add(mpHd);
			}
		}
	}
	return boxedMpHd;
}
YsArray <RetroMap_World::MarkUpHandle> FsGui3DMainCanvas::BoxedMuHd(YsVec2 min,YsVec2 max) const
{
	YsRect2 rect(min,max);
	YsArray <RetroMap_World::MarkUpHandle> boxedMuHd;
	auto fieldPtr=world.GetField(GetCurrentField());
	if(nullptr!=fieldPtr)
	{
		for(auto muHd : fieldPtr->AllMarkUp())
		{
			auto markUp=fieldPtr->GetMarkUp(muHd);
			auto bbx=markUp->GetBoundingBox();
			if(YSTRUE==rect.IsInside(bbx[0].xy()) &&
			   YSTRUE==rect.IsInside(bbx[1].xy()))
			{
				boxedMuHd.Add(muHd);
			}
		}
	}
	return boxedMuHd;
}

YsArray <RetroMap_World::MarkUpPointIndex> FsGui3DMainCanvas::BoxedMarkUpPointIndex(YsVec2 min,YsVec2 max) const
{
	YsRect2 rect(min,max);
	YsArray <RetroMap_World::MarkUpPointIndex> boxedMupi;
	auto fieldPtr=world.GetField(GetCurrentField());

	if(nullptr!=fieldPtr)
	{
		for(auto muHd : fieldPtr->AllMarkUp())
		{
			auto markUp=fieldPtr->GetMarkUp(muHd);
			if(markUp->MARKUP_POINT_ARRAY==markUp->GetMarkUpType())
			{
				auto plg=markUp->GetPointArray();
				for(auto idx : plg.AllIndex())
				{
					YsVec2 pos(plg[idx].x(),plg[idx].y());
					if(YSTRUE==rect.IsInside(pos))
					{
						RetroMap_World::MarkUpPointIndex mupi;
						mupi.muHd=muHd;
						mupi.idx=idx;
						boxedMupi.push_back(mupi);
					}
				}
			}
		}
	}

	return boxedMupi;
}

YSBOOL FsGui3DMainCanvas::IsOnSelectedMapPiece(YsVec2i pos) const
{
	auto selMpHd=world.SelectedMapPiece(GetCurrentField());
	auto fieldPtr=world.GetField(GetCurrentField());
	if(nullptr!=fieldPtr && 0<selMpHd.GetN())
	{
		YsVec3 o,v;
		drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

		YsPlane pln(YsOrigin(),YsZVec());
		YsVec3 crs;
		if(YSOK==pln.GetIntersection(crs,o,v))
		{
			for(auto mpHd : selMpHd)
			{
				auto mapPiece=fieldPtr->GetMapPiece(mpHd);
				auto bbx=mapPiece->GetBoundingBox();
				if(YSTRUE==bbx.IsInside(crs))
				{
					return YSTRUE;
				}
			}
		}
	}
	return YSFALSE;
}

////////////////////////////////////////////////////////////

YSRESULT FsGui3DMainCanvas::OnLButtonDown(FsGuiMouseButtonSet,YsVec2i pos)
{
	return PointerIn(pos);
}
YSRESULT FsGui3DMainCanvas::OnMouseMove(FsGuiMouseButtonSet,YsVec2i pos,YsVec2i prevPos)
{
	return PointerMove(pos);
}

YsVec2i FsGui3DMainCanvas::MouseMove_Displacement(double x,double y) const
{
	double dx=x-defAction_DragStart.x();
	double dy=y-defAction_DragStart.y();
	YsVec2i disp2i((int)dx,(int)dy);

	auto selMpHd=world.SelectedMapPiece(GetCurrentField());
	if(0<selMpHd.GetN())
	{
		auto mapPiecePtr=world.GetMapPiece(GetCurrentField(),selMpHd[0]);
		auto shape=mapPiecePtr->GetShape();
		auto pos0=shape.pos;
		shape.pos+=disp2i;
		shape.ApplyUnit(world.GetUnit());
		disp2i=shape.pos-pos0;
	}

	return disp2i;
}

YSRESULT FsGui3DMainCanvas::OnLButtonUp(FsGuiMouseButtonSet,YsVec2i pos)
{
	return PointerUp(pos);
}
YSRESULT FsGui3DMainCanvas::OnTouchStateChange(const FsGuiObject::TouchState &state)
{
	if(0==state.movement.size() &&
	   0==state.endedTouch.size() &&
	   1==state.newTouch.size())
	{
		YsVec2i pos((int)state.newTouch[0].x(),(int)state.newTouch[0].y());
		PointerIn(pos);
	}
	if(1==state.movement.size() &&
	   0==state.endedTouch.size() &&
	   0==state.newTouch.size())
	{
		if(state.movement[0].to!=state.movement[0].from)
		{
			YsVec2i pos((int)state.movement[0].to.x(),(int)state.movement[0].to.y());
			PointerMove(pos);
		}
	}
	if(0==state.movement.size() &&
	   1==state.endedTouch.size() &&
	   0==state.newTouch.size())
	{
		YsVec2i pos((int)state.endedTouch[0].x(),(int)state.endedTouch[0].y());
		PointerUp(pos);
	}

	if(2<=state.movement.size()+state.endedTouch.size()+state.newTouch.size() ||
	   0==state.movement.size()+state.endedTouch.size()+state.newTouch.size())
	{
		defAction=DEFAULTACTION_STANDBY;
	}

	return YSOK;
}

YSRESULT FsGui3DMainCanvas::PointerIn(YsVec2i pos)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	if(DEFAULTACTION_STANDBY==defAction)
	{
		YsVec3 o,v;
		drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());
		YsPlane pln(YsOrigin(),YsZVec());


		auto mupi=PickedMarkUpPointWithinSelection(pos,world.SelectedMarkUp(GetCurrentField()));
		if(nullptr!=mupi.muHd)
		{
			YsVec3 crs;
			if(YSOK==pln.GetIntersection(crs,o,v))
			{
				defAction_MovingPointIndex=mupi;
				defAction_DragStart=crs;
				defAction=DEFAULTACTION_MOVING_MARKUPPOINT;
				return YSOK;
			}
		}


		auto mpHd=PickedMpHd(pos);
		auto muHd=PickedMuHd(pos);
		if(nullptr!=muHd)
		{
			mpHd=nullptr;
		}
		if(YSTRUE==world.IsSelected(GetCurrentField(),mpHd) ||
		   YSTRUE==world.IsSelected(GetCurrentField(),muHd))
		{
			YsVec3 crs;
			if(YSOK==pln.GetIntersection(crs,o,v))
			{
				defAction_DragStart=crs;
				defAction=DEFAULTACTION_MOVING;
			}
		}
		else
		{
			YsVec3 crs;
			if(YSOK==pln.GetIntersection(crs,o,v))
			{
				defAction_DragStart=crs;
				defAction=DEFAULTACTION_CLICKED;
			}
		}
	}
	else if(DEFAULTACTION_MARKUPPOINTSELECTION==defAction)
	{
		YsVec3 o,v;
		drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());
		YsPlane pln(YsOrigin(),YsZVec());

		YsVec3 crs;
		if(YSOK==pln.GetIntersection(crs,o,v))
		{
			defAction_DragStart=crs;
			defAction=DEFAULTACTION_MARKUPPOINTSELECTION_CLICKED;
		}
	}
	return YSOK;
}
YSRESULT FsGui3DMainCanvas::PointerMove(YsVec2i pos)
{
	if(DEFAULTACTION_CLICKED==defAction)
	{
		defAction=DEFAULTACTION_BOXSELECTING;
		defAction_CurrentDrag=defAction_DragStart;
		// Let it fall back go if(DEFAULTACTION_BOXSELECTING==defAction)
	}

	if(DEFAULTACTION_MARKUPPOINTSELECTION_CLICKED==defAction)
	{
		defAction=DEFAULTACTION_MARKUPPOINTSELECTION_BOXSELECTING;
		defAction_CurrentDrag=defAction_DragStart;
	}

	if(DEFAULTACTION_BOXSELECTING==defAction)
	{
		YsVec3 o,v;
		drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

		YsPlane pln(YsOrigin(),YsZVec());
		YsVec3 crs;
		if(YSOK==pln.GetIntersection(crs,o,v))
		{
			defAction_CurrentDrag=crs;

			YsRect3 rect(defAction_DragStart,defAction_CurrentDrag);

			auto fdHd=GetCurrentField();
			if(nullptr!=fdHd)
			{
				if(YSOK==world.ResetVbo(fdHd))
				{
					SetNeedRedraw(YSTRUE);
				}

				for(auto mpHd : BoxedMpHd(rect[0].xy(),rect[1].xy()))
				{
					auto mapPiecePtr=world.GetMapPiece(fdHd,mpHd);
					auto shape=mapPiecePtr->GetShape();
					world.ReadyVbo(GetCurrentField(),mpHd,shape,0.5);
				}
				for(auto muHd : BoxedMuHd(rect[0].xy(),rect[1].xy()))
				{
					auto markUpPtr=world.GetMarkUp(fdHd,muHd);
					auto pos=markUpPtr->GetPosition();
					YsColor hlCol;
					hlCol.SetIntRGBA(255,255,255,128);
					world.ReadyVbo(GetCurrentField(),muHd,pos,hlCol);
				}
			}

			SetNeedRedraw(YSTRUE);
		}
	}
	else if(DEFAULTACTION_STANDBY==defAction)
	{
		auto fdHd=GetCurrentField();
		auto mpHd=PickedMpHd(pos);
		auto muHd=PickedMuHd(pos);
		if(nullptr!=muHd)
		{
			mpHd=nullptr;
		}

		if(YSOK==world.ResetVbo(fdHd))
		{
			SetNeedRedraw(YSTRUE);
		}

		auto mapPiecePtr=world.GetMapPiece(fdHd,mpHd);
		if(nullptr!=mapPiecePtr)
		{
			auto shape=mapPiecePtr->GetShape();
			world.ReadyVbo(GetCurrentField(),mpHd,shape,0.5);
			SetNeedRedraw(YSTRUE);
		}
		auto markUpPtr=world.GetMarkUp(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			auto pos=markUpPtr->GetPosition();
			YsColor hlCol;
			hlCol.SetIntRGBA(0,0,255,128);
			markUpPtr->SetMarkerSize(GetPickableDistance());
			world.ReadyVbo(GetCurrentField(),muHd,pos,hlCol);
			SetNeedRedraw(YSTRUE);
		}
	}
	else if(DEFAULTACTION_MOVING==defAction)
	{
		YsVec3 o,v;
		drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

		YsPlane pln(YsOrigin(),YsZVec());
		YsVec3 crs;
		if(YSOK==pln.GetIntersection(crs,o,v))
		{
			auto disp2i=MouseMove_Displacement(crs.x(),crs.y());
			for(auto mpHd : world.SelectedMapPiece(GetCurrentField()))
			{
				auto mapPiecePtr=world.GetMapPiece(GetCurrentField(),mpHd);
				auto shape=mapPiecePtr->GetShape();
				shape.pos+=disp2i;
				world.ReadyVbo(GetCurrentField(),mpHd,shape,0.5);
			}
			for(auto muHd : world.SelectedMarkUp(GetCurrentField()))
			{
				auto markUpPtr=world.GetMarkUp(GetCurrentField(),muHd);
				YsColor hlCol;
				hlCol.SetIntRGBA(255,255,255,128);
				world.ReadyVboWithDisplacement(GetCurrentField(),muHd,disp2i,hlCol);
			}
			showDiff=YSTRUE;
			UpdateDiff();
			SetNeedRedraw(YSTRUE);
		}
	}
	else if(DEFAULTACTION_MOVING_MARKUPPOINT==defAction)
	{
		YsVec3 o,v;
		drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

		YsPlane pln(YsOrigin(),YsZVec());
		YsVec3 crs;
		if(YSOK==pln.GetIntersection(crs,o,v))
		{
			auto disp2i=MouseMove_Displacement(crs.x(),crs.y());
			auto markUpPtr=world.GetMarkUp(GetCurrentField(),defAction_MovingPointIndex.muHd);
			YsColor hlCol;
			hlCol.SetIntRGBA(255,255,255,128);

			auto pointCoord=markUpPtr->GetPointArray();
			if(YSTRUE==pointCoord.IsInRange(defAction_MovingPointIndex.idx))
			{
				pointCoord[defAction_MovingPointIndex.idx]+=disp2i;
				if(YSTRUE==markUp_Snap->GetCheck())
				{
					MarkUp_SnapToHalfUnit(pointCoord[defAction_MovingPointIndex.idx]);
				}
				if(YSTRUE==FsGetKeyState(FSKEY_ALT))
				{
					AxisAlignMarkUpPoint(pointCoord,defAction_MovingPointIndex.idx);
				}

				world.ReadyVboWithModifiedPointArray(GetCurrentField(),defAction_MovingPointIndex.muHd,pointCoord,hlCol);
			}

			SetNeedRedraw(YSTRUE);
		}
	}
	else if(DEFAULTACTION_MARKUPPOINTSELECTION_BOXSELECTING==defAction)
	{
		YsVec3 o,v;
		drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

		YsPlane pln(YsOrigin(),YsZVec());
		YsVec3 crs;
		if(YSOK==pln.GetIntersection(crs,o,v))
		{
			defAction_CurrentDrag=crs;
			SetNeedRedraw(YSTRUE);
		}
	}
	return YSOK;
}
YSRESULT FsGui3DMainCanvas::PointerUp(YsVec2i pos)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	if(nullptr==GetCurrentField())
	{
		return YSERR;
	}

	DEFAULTACTION nextDefaultAction=DEFAULTACTION_STANDBY;
	RetroMap_World::UndoGuard undoGuard(world);

	if(DEFAULTACTION_MOVING==defAction)
	{
		auto fieldPtr=world.GetField(GetCurrentField());

		YsVec3 o,v;
		drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

		YsPlane pln(YsOrigin(),YsZVec());
		YsVec3 crs;
		if(YSOK==pln.GetIntersection(crs,o,v))
		{
			auto disp2i=MouseMove_Displacement(crs.x(),crs.y());
			for(auto mpHd : world.SelectedMapPiece(GetCurrentField()))
			{
				auto mapPiecePtr=world.GetMapPiece(GetCurrentField(),mpHd);
				auto shape=mapPiecePtr->GetShape();
				shape.pos+=disp2i;
				if(shape.pos!=mapPiecePtr->GetPosition())
				{
					world.SetPosition(GetCurrentField(),mpHd,shape.pos);
					world.ReapplyAnchor(GetCurrentField(),mpHd);
					world.ReadyVbo(GetCurrentField(),mpHd);
					world.ReadyVboOfAnchoredMarkUp(GetCurrentField(),mpHd);
				}
			}
			for(auto muHd : world.SelectedMarkUp(GetCurrentField()))
			{
				auto markUpPtr=world.GetMarkUp(GetCurrentField(),muHd);
				world.MoveMarkUp(GetCurrentField(),muHd,disp2i);

				// This is for the mark ups that depends on this mark up.
				world.ReapplyAnchor(GetCurrentField(),muHd);

				// If this mark up is anchored, it may have moved out of the map part/mark up.
				Reanchor(GetCurrentField(),muHd);

				world.ReadyVbo(GetCurrentField(),muHd);
				world.ReadyVboOfAnchoredMarkUp(GetCurrentField(),muHd);
			}
			SetNeedRedraw(YSTRUE);
		}
	}
	else if(DEFAULTACTION_CLICKED==defAction)
	{
		auto mpHd=PickedMpHd(pos);
		auto muHd=PickedMuHd(pos);
		if(nullptr!=mpHd)
		{
			if(YSTRUE==FsGetKeyState(FSKEY_CTRL))
			{
				world.AddSelectedMapPiece(GetCurrentField(),mpHd);
			}
			else
			{
				world.UnselectAllMarkUp(GetCurrentField());
				world.SelectMapPiece(GetCurrentField(),mpHd);
			}
			SetNeedRedraw(YSTRUE);
		}
		if(nullptr!=muHd)
		{
			if(YSTRUE==FsGetKeyState(FSKEY_CTRL))
			{
				world.AddSelectedMarkUp(GetCurrentField(),muHd);
			}
			else
			{
				world.UnselectAllMapPiece(GetCurrentField());
				world.SelectMarkUp(GetCurrentField(),muHd);
			}
			SetNeedRedraw(YSTRUE);
		}
	}
	else if(DEFAULTACTION_BOXSELECTING==defAction)
	{
		auto fdHd=GetCurrentField();
		if(nullptr!=fdHd)
		{
			if(YSTRUE!=FsGetKeyState(FSKEY_CTRL))
			{
				world.UnselectAllMapPiece(fdHd);
				world.UnselectAllMarkUp(fdHd);
			}

			YsRect3 rect(defAction_DragStart,defAction_CurrentDrag);

			world.AddSelectedMapPiece(fdHd,BoxedMpHd(rect[0].xy(),rect[1].xy()));
			world.AddSelectedMarkUp(fdHd,BoxedMuHd(rect[0].xy(),rect[1].xy()));

		}
		SetNeedRedraw(YSTRUE);
	}
	else if(DEFAULTACTION_MOVING_MARKUPPOINT==defAction)
	{
		YsVec3 o,v;
		drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());

		YsPlane pln(YsOrigin(),YsZVec());
		YsVec3 crs;
		if(YSOK==pln.GetIntersection(crs,o,v))
		{
			auto disp2i=MouseMove_Displacement(crs.x(),crs.y());
			auto markUpPtr=world.GetMarkUp(GetCurrentField(),defAction_MovingPointIndex.muHd);

			auto pointCoord=markUpPtr->GetPointArray();
			if(YSTRUE==pointCoord.IsInRange(defAction_MovingPointIndex.idx))
			{
				pointCoord[defAction_MovingPointIndex.idx]+=disp2i;
				if(YSTRUE==markUp_Snap->GetCheck())
				{
					MarkUp_SnapToHalfUnit(pointCoord[defAction_MovingPointIndex.idx]);
				}
				if(YSTRUE==FsGetKeyState(FSKEY_ALT))
				{
					AxisAlignMarkUpPoint(pointCoord,defAction_MovingPointIndex.idx);
				}
				world.SetMarkUpPointArray(GetCurrentField(),defAction_MovingPointIndex.muHd,pointCoord);
			}

			// Anchored point must not be re-anchored to a new map part or un-anchored if it is not on anything.
			Reanchor(GetCurrentField(),defAction_MovingPointIndex.muHd,defAction_MovingPointIndex.idx);

			SetNeedRedraw(YSTRUE);
		}
	}
	else if(DEFAULTACTION_MARKUPPOINTSELECTION_CLICKED==defAction)
	{
		auto mupi=PickedMarkUpPoint(pos);
		if(nullptr!=mupi.muHd && nullptr!=markUpPointSelectionCallback)
		{
			YsArray <RetroMap_World::MarkUpPointIndex> dat;
			dat.push_back(mupi);
			markUpPointSelectionCallback(dat);
		}
		nextDefaultAction=DEFAULTACTION_MARKUPPOINTSELECTION;
	}
	else if(DEFAULTACTION_MARKUPPOINTSELECTION_BOXSELECTING==defAction)
	{
		YsRect3 rect(defAction_DragStart,defAction_CurrentDrag);
		auto mupi=BoxedMarkUpPointIndex(rect.Min().xy(),rect.Max().xy());
		if(0<mupi.size() && nullptr!=markUpPointSelectionCallback)
		{
			markUpPointSelectionCallback(mupi);
		}
		nextDefaultAction=DEFAULTACTION_MARKUPPOINTSELECTION;
		SetNeedRedraw(YSTRUE);
	}
	defAction=nextDefaultAction;
	return YSOK;
}

void FsGui3DMainCanvas::AxisAlignMarkUpPoint(YsArrayMask <YsVec2i> pointCoord,int idx) const
{
	if(2<=pointCoord.size() && idx<pointCoord.size())
	{
		auto d=pointCoord[0];
		if(0<defAction_MovingPointIndex.idx)
		{
			d=pointCoord[defAction_MovingPointIndex.idx]-pointCoord[defAction_MovingPointIndex.idx-1];
		}
		else
		{
			d=pointCoord[defAction_MovingPointIndex.idx+1]-pointCoord[defAction_MovingPointIndex.idx];
		}

		auto dx=YsAbs(d.x());
		auto dy=YsAbs(d.y());
		if(dy<dx/2)
		{
			d.SetY(0);
		}
		else if(dx<dy/2)
		{
			d.SetX(0);
		}
		else if(dx>dy)
		{
			d.SetY(dx*d.y()/dy);
		}
		else
		{
			d.SetX(dy*d.x()/dx);
		}

		if(0<defAction_MovingPointIndex.idx)
		{
			pointCoord[defAction_MovingPointIndex.idx]=d+pointCoord[defAction_MovingPointIndex.idx-1];
		}
		else
		{
			pointCoord[defAction_MovingPointIndex.idx]=pointCoord[defAction_MovingPointIndex.idx+1]-d;
		}
	}
}

void FsGui3DMainCanvas::OnUpArrowKey(FsGuiPopUpMenuItem *)
{
	RetroMap_World::UndoGuard undoGuard(world);

	auto unit=world.GetUnit();
	const YsVec2i moveDir(0,unit.y());
	for(auto mpHd : world.SelectedMapPiece(GetCurrentField()))
	{
		auto mapPiecePtr=world.GetMapPiece(GetCurrentField(),mpHd);
		auto shape=mapPiecePtr->GetShape();
		shape.pos+=moveDir;
		world.SetPosition(GetCurrentField(),mpHd,shape.pos);
		world.ReadyVbo(GetCurrentField(),mpHd,shape,0.5);
		world.ReapplyAnchor(GetCurrentField(),mpHd);
		world.ReadyVboOfAnchoredMarkUp(GetCurrentField(),mpHd);
	}
	for(auto muHd : world.SelectedMarkUp(GetCurrentField()))
	{
		auto markUpPtr=world.GetMarkUp(GetCurrentField(),muHd);
		world.MoveMarkUp(GetCurrentField(),muHd,moveDir);
		world.ReadyVbo(GetCurrentField(),muHd);
		world.ReapplyAnchor(GetCurrentField(),muHd);
		world.ReadyVboOfAnchoredMarkUp(GetCurrentField(),muHd);
	}
	showDiff=YSTRUE;
	UpdateDiff();
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::OnDownArrowKey(FsGuiPopUpMenuItem *)
{
	RetroMap_World::UndoGuard undoGuard(world);

	auto unit=world.GetUnit();
	const YsVec2i moveDir(0,-unit.y());
	for(auto mpHd : world.SelectedMapPiece(GetCurrentField()))
	{
		auto mapPiecePtr=world.GetMapPiece(GetCurrentField(),mpHd);
		auto shape=mapPiecePtr->GetShape();
		shape.pos+=moveDir;
		world.SetPosition(GetCurrentField(),mpHd,shape.pos);
		world.ReadyVbo(GetCurrentField(),mpHd,shape,0.5);
		world.ReapplyAnchor(GetCurrentField(),mpHd);
		world.ReadyVboOfAnchoredMarkUp(GetCurrentField(),mpHd);
	}
	for(auto muHd : world.SelectedMarkUp(GetCurrentField()))
	{
		auto markUpPtr=world.GetMarkUp(GetCurrentField(),muHd);
		world.MoveMarkUp(GetCurrentField(),muHd,moveDir);
		world.ReadyVbo(GetCurrentField(),muHd);
		world.ReapplyAnchor(GetCurrentField(),muHd);
		world.ReadyVboOfAnchoredMarkUp(GetCurrentField(),muHd);
	}
	showDiff=YSTRUE;
	UpdateDiff();
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::OnLeftArrowKey(FsGuiPopUpMenuItem *)
{
	RetroMap_World::UndoGuard undoGuard(world);

	auto unit=world.GetUnit();
	const YsVec2i moveDir(-unit.x(),0);
	for(auto mpHd : world.SelectedMapPiece(GetCurrentField()))
	{
		auto mapPiecePtr=world.GetMapPiece(GetCurrentField(),mpHd);
		auto shape=mapPiecePtr->GetShape();
		shape.pos+=moveDir;
		world.SetPosition(GetCurrentField(),mpHd,shape.pos);
		world.ReadyVbo(GetCurrentField(),mpHd,shape,0.5);
		world.ReapplyAnchor(GetCurrentField(),mpHd);
		world.ReadyVboOfAnchoredMarkUp(GetCurrentField(),mpHd);
	}
	for(auto muHd : world.SelectedMarkUp(GetCurrentField()))
	{
		auto markUpPtr=world.GetMarkUp(GetCurrentField(),muHd);
		world.MoveMarkUp(GetCurrentField(),muHd,moveDir);
		world.ReadyVbo(GetCurrentField(),muHd);
		world.ReapplyAnchor(GetCurrentField(),muHd);
		world.ReadyVboOfAnchoredMarkUp(GetCurrentField(),muHd);
	}
	showDiff=YSTRUE;
	UpdateDiff();
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::OnRightArrowKey(FsGuiPopUpMenuItem *)
{
	RetroMap_World::UndoGuard undoGuard(world);

	auto unit=world.GetUnit();
	const YsVec2i moveDir(unit.x(),0);
	for(auto mpHd : world.SelectedMapPiece(GetCurrentField()))
	{
		auto mapPiecePtr=world.GetMapPiece(GetCurrentField(),mpHd);
		auto shape=mapPiecePtr->GetShape();
		shape.pos+=moveDir;
		world.SetPosition(GetCurrentField(),mpHd,shape.pos);
		world.ReadyVbo(GetCurrentField(),mpHd,shape,0.5);
		world.ReapplyAnchor(GetCurrentField(),mpHd);
		world.ReadyVboOfAnchoredMarkUp(GetCurrentField(),mpHd);
	}
	for(auto muHd : world.SelectedMarkUp(GetCurrentField()))
	{
		auto markUpPtr=world.GetMarkUp(GetCurrentField(),muHd);
		world.MoveMarkUp(GetCurrentField(),muHd,moveDir);
		world.ReadyVbo(GetCurrentField(),muHd);
		world.ReapplyAnchor(GetCurrentField(),muHd);
		world.ReadyVboOfAnchoredMarkUp(GetCurrentField(),muHd);
	}
	showDiff=YSTRUE;
	UpdateDiff();
	SetNeedRedraw(YSTRUE);
}

void FsGui3DMainCanvas::OnUpArrowKeyShift(FsGuiPopUpMenuItem *)
{
	auto uni=world.GetUnit();
	auto t=drawEnv.GetViewTarget();
	t.AddY(uni.y());
	drawEnv.SetViewTarget(t);
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::OnDownArrowKeyShift(FsGuiPopUpMenuItem *)
{
	auto uni=world.GetUnit();
	auto t=drawEnv.GetViewTarget();
	t.SubY(uni.y());
	drawEnv.SetViewTarget(t);
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::OnLeftArrowKeyShift(FsGuiPopUpMenuItem *)
{
	auto uni=world.GetUnit();
	auto t=drawEnv.GetViewTarget();
	t.SubX(uni.x());
	drawEnv.SetViewTarget(t);
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::OnRightArrowKeyShift(FsGuiPopUpMenuItem *)
{
	auto uni=world.GetUnit();
	auto t=drawEnv.GetViewTarget();
	t.AddX(uni.x());
	drawEnv.SetViewTarget(t);
	SetNeedRedraw(YSTRUE);
}

void FsGui3DMainCanvas::OnUpArrowKeyCtrl(FsGuiPopUpMenuItem *)
{
	auto uni=world.GetDefaultTrimRect();
	auto t=drawEnv.GetViewTarget();
	t.AddY(uni.GetSize().y());
	drawEnv.SetViewTarget(t);
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::OnDownArrowKeyCtrl(FsGuiPopUpMenuItem *)
{
	auto uni=world.GetDefaultTrimRect();
	auto t=drawEnv.GetViewTarget();
	t.SubY(uni.GetSize().y());
	drawEnv.SetViewTarget(t);
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::OnLeftArrowKeyCtrl(FsGuiPopUpMenuItem *)
{
	auto uni=world.GetDefaultTrimRect();
	auto t=drawEnv.GetViewTarget();
	t.SubX(uni.GetSize().x());
	drawEnv.SetViewTarget(t);
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::OnRightArrowKeyCtrl(FsGuiPopUpMenuItem *)
{
	auto uni=world.GetDefaultTrimRect();
	auto t=drawEnv.GetViewTarget();
	t.AddX(uni.GetSize().x());
	drawEnv.SetViewTarget(t);
	SetNeedRedraw(YSTRUE);
}

void FsGui3DMainCanvas::OnPageUpKey(FsGuiPopUpMenuItem *)
{
	drawEnv.SetZoom(drawEnv.GetZoom()*1.1);
	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::OnPageDownKey(FsGuiPopUpMenuItem *)
{
	drawEnv.SetZoom(drawEnv.GetZoom()/1.1);
	SetNeedRedraw(YSTRUE);
}

////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::File_Exit(FsGuiPopUpMenuItem *)
{
	if(YSTRUE==world.IsModified())
	{
		auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
		msgDlg->Make(FSGUI_DLG_CONFIRM_QUIT_TITLE,FSGUI_DLG_CONFIRM_QUIT_MESSAGE,FSGUI_COMMON_YES,FSGUI_COMMON_NO);
		msgDlg->BindCloseModalCallBack(&THISCLASS::File_Exit_ConfirmExitCallBack,this);
		AttachModalDialog(msgDlg);
	}
	else
	{
		File_Exit_ReallyExit();
	}
}

void FsGui3DMainCanvas::File_Exit_ConfirmExitCallBack(FsGuiDialog *,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue)
	{
		File_Exit_ReallyExit();
	}
}

void FsGui3DMainCanvas::File_Exit_ReallyExit(void)
{
	this->appMustTerminate=YSTRUE;
}

////////////////////////////////////////////////////////////

