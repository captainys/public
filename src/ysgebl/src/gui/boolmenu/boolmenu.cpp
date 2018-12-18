/* ////////////////////////////////////////////////////////////

File Name: boolmenu.cpp
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

#include "../ysgebl_gui_editor_base.h"
#include "../miscdlg.h"
#include <ysgl.h>
#include <ysglbuffer.h>

#include <ysshellext_diagnoseutil.h>
#include <ysshellextedit_boolutil.h>
#include <ysshellextedit_orientationutil.h>
#include <ysshellextedit_localop.h>

#include "booldialog.h"
#include "../textresource.h"

#include <thread>

////////////////////////////////////////////////////////////

PolyCreBoolDialog::PolyCreBoolDialog()
{
	canvas=NULL;
	cancelBtn=NULL;
}

PolyCreBoolDialog::~PolyCreBoolDialog()
{
}

/*static*/ PolyCreBoolDialog *PolyCreBoolDialog::Create(class GeblGuiEditorBase *canvas)
{
	PolyCreBoolDialog *dlg=new PolyCreBoolDialog;
	dlg->canvas=canvas;
	return dlg;
}

/*static*/ void PolyCreBoolDialog::Delete(PolyCreBoolDialog *ptr)
{
	delete ptr;
}

YSRESULT PolyCreBoolDialog::Make(void)
{
	if(NULL==cancelBtn)
	{
		FsGuiDialog::Initialize();

		AddStaticText(0,FSKEY_NULL,FSGUI_DLG_BOOL_CLICKONSHELL,YSTRUE);

		diagnoseBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_BOOL_DIAGNOSE,YSTRUE);
		recalcNormalBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_BOOL_RECALCNORMAL,YSTRUE);
		triangulateNonPlanarBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_BOOL_TRIANGULATENONPLANAR,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSTRUE);

		diagnoseBtn->SetCheck(YSTRUE);
		recalcNormalBtn->SetCheck(YSTRUE);
		triangulateNonPlanarBtn->SetCheck(YSTRUE);

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(0.3);
		Fit();
	}

	return YSOK;
}

void PolyCreBoolDialog::RemakeDrawingBuffer(void)
{
	// As required
}

void PolyCreBoolDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==cancelBtn)
	{
		canvas->Edit_ClearUIMode();
	}
}

void PolyCreBoolDialog::OnDropListSelChange(FsGuiDropList *,int)
{
}

void PolyCreBoolDialog::OnTextBoxChange(FsGuiTextBox *)
{
}

void PolyCreBoolDialog::OnSliderPositionChange(FsGuiSlider *,const double &,const double &)
{
}

////////////////////////////////////////////////////////////

/*static*/ void GeblGuiEditorBase::BoolUnion(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	YsShellExtEdit *shl=canvas.slHd;
	if(NULL!=shl)
	{
		canvas.Edit_ClearUIMode();

		canvas.boolDlg->Make();
		canvas.AddDialog(canvas.boolDlg);
		canvas.ArrangeDialog();

		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;

		canvas.boolOpType=YSBOOLOR;
		canvas.LButtonCallBack=GeblGuiEditorBase::Bool_LButtonCallBack;
	}
}

/*static*/ void GeblGuiEditorBase::BoolSubtraction(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	YsShellExtEdit *shl=canvas.slHd;
	if(NULL!=shl)
	{
		canvas.Edit_ClearUIMode();

		canvas.boolDlg->Make();
		canvas.AddDialog(canvas.boolDlg);
		canvas.ArrangeDialog();

		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;

		canvas.boolOpType=YSBOOLMINUS;
		canvas.LButtonCallBack=GeblGuiEditorBase::Bool_LButtonCallBack;
	}
}

/*static*/ void GeblGuiEditorBase::BoolIntersection(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	YsShellExtEdit *shl=canvas.slHd;
	if(NULL!=shl)
	{
		canvas.Edit_ClearUIMode();

		canvas.boolDlg->Make();
		canvas.AddDialog(canvas.boolDlg);
		canvas.ArrangeDialog();

		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;

		canvas.boolOpType=YSBOOLAND;
		canvas.LButtonCallBack=GeblGuiEditorBase::Bool_LButtonCallBack;
	}
}

/*static*/ void GeblGuiEditorBase::BoolMerge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	YsShellExtEdit *shl=canvas.slHd;
	if(NULL!=shl)
	{
		canvas.Edit_ClearUIMode();

		canvas.boolDlg->Make();
		canvas.AddDialog(canvas.boolDlg);
		canvas.ArrangeDialog();

		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;

		canvas.boolOpType=YSBOOLBLEND;
		canvas.LButtonCallBack=GeblGuiEditorBase::Bool_LButtonCallBack;
	}
}

/*static*/ void GeblGuiEditorBase::BoolSeparate(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	YsShellExtEdit *shl=canvas.slHd;
	if(NULL!=shl)
	{
		canvas.Edit_ClearUIMode();

		canvas.boolDlg->Make();
		canvas.AddDialog(canvas.boolDlg);
		canvas.ArrangeDialog();

		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;

		canvas.boolOpType=YSBOOLSEPARATE;
		canvas.LButtonCallBack=GeblGuiEditorBase::Bool_LButtonCallBack;
	}
}

/* static */ void GeblGuiEditorBase::BoolOpenSurfaceMinusSolid(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	YsShellExtEdit *shl=canvas.slHd;
	if(NULL!=shl)
	{
		canvas.Edit_ClearUIMode();

		canvas.boolDlg->Make();
		canvas.AddDialog(canvas.boolDlg);
		canvas.ArrangeDialog();

		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;

		canvas.boolOpType=YSBOOL_OPENMINUSSOLID;
		canvas.LButtonCallBack=GeblGuiEditorBase::Bool_LButtonCallBack;
	}
}

/* static */ void GeblGuiEditorBase::BoolOpenSurfacePlusSolid(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	YsShellExtEdit *shl=canvas.slHd;
	if(NULL!=shl)
	{
		canvas.Edit_ClearUIMode();

		canvas.boolDlg->Make();
		canvas.AddDialog(canvas.boolDlg);
		canvas.ArrangeDialog();

		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;

		canvas.boolOpType=YSBOOL_OPENPLUSSOLID;
		canvas.LButtonCallBack=GeblGuiEditorBase::Bool_LButtonCallBack;
	}
}

/* static */ void GeblGuiEditorBase::BoolOpenSurfaceAndSolid(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	YsShellExtEdit *shl=canvas.slHd;
	if(NULL!=shl)
	{
		canvas.Edit_ClearUIMode();

		canvas.boolDlg->Make();
		canvas.AddDialog(canvas.boolDlg);
		canvas.ArrangeDialog();

		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;

		canvas.boolOpType=YSBOOL_OPENTIMESSOLID;
		canvas.LButtonCallBack=GeblGuiEditorBase::Bool_LButtonCallBack;
	}
}

/*static*/ YSRESULT GeblGuiEditorBase::Bool_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL,YSBOOL,int mx,int my)
{
	if(YSTRUE!=lb)
	{
		YsShellDnmContainer <YsShellExtEditGui>::Node *slHdB=canvas.PickedSlHd(mx,my,0,NULL);
		if(NULL!=slHdB && slHdB!=canvas.slHd)
		{
			canvas.boolDlg->slHdB=slHdB;
			if(YSTRUE!=canvas.boolDlg->diagnoseBtn->GetCheck())
			{
				return canvas.Bool_Apply(canvas.boolOpType,*canvas.slHd,*slHdB);
			}
			else
			{
				canvas.Bool_DiagnoseDialog();
				return YSOK;
			}
		}
	}
	return YSERR;
}

static void DiagnoseCallBack(YsShellExt_DiagnoseUtil *util)
{
	util->CheckSelfIntersection();
	util->CheckNonManifoldEdge();
}

void GeblGuiEditorBase::Bool_DiagnoseDialog(void)
{
	YsShellExt_DiagnoseUtil diagA,diagB;
	diagA.SetShell(slHd->Conv());
	diagB.SetShell(boolDlg->slHdB->Conv());

	diagA.SetNumThread(2);
	diagB.SetNumThread(2);

	diagA.debug_threadId=0;
	diagB.debug_threadId=1;

	std::thread t1(DiagnoseCallBack,&diagA);
	std::thread t2(DiagnoseCallBack,&diagB);
	t1.join();
	t2.join();

	YSBOOL defect=YSFALSE;
	YsWString msg;
	msg.Set(FSGUI_DLG_BOOL_POTENTIALPROBLEMS);
	msg.Append(L"\n");
	if(0<diagA.nonManifoldEdgeArray.GetN() || 0<diagA.selfIntersectionArray.GetN())
	{
		msg.Append(FSGUI_DLG_BOOL_CURRENTSHELL);
		msg.Append(L"\n");
		defect=YSTRUE;
	}
	if(0<diagA.nonManifoldEdgeArray.GetN())
	{
		msg.Append(FSGUI_DLG_BOOL_NONMANIFOLD);
		msg.Append(L"\n");
		defect=YSTRUE;
	}
	if(0<diagA.selfIntersectionArray.GetN())
	{
		msg.Append(FSGUI_DLG_BOOL_SELFINTERSECTION);
		msg.Append(L"\n");
		defect=YSTRUE;
	}

	if(0<diagB.nonManifoldEdgeArray.GetN() || 0<diagB.selfIntersectionArray.GetN())
	{
		msg.Append(FSGUI_DLG_BOOL_PICKEDSHELL);
		msg.Append(L"\n");
		defect=YSTRUE;
	}
	if(0<diagB.nonManifoldEdgeArray.GetN())
	{
		msg.Append(FSGUI_DLG_BOOL_NONMANIFOLD);
		msg.Append(L"\n");
		defect=YSTRUE;
	}
	if(0<diagB.selfIntersectionArray.GetN())
	{
		msg.Append(FSGUI_DLG_BOOL_SELFINTERSECTION);
		msg.Append(L"\n");
		defect=YSTRUE;
	}

	if(YSTRUE==defect)
	{
		msg.Append(L"\n");
		msg.Append(FSGUI_DLG_BOOL_DEFECTEXPLANATION);
		YesNoDialog(FSGUI_DLG_BOOL_DEFECTFOUND,msg,FSGUI_COMMON_YES,FSGUI_COMMON_NO,Bool_DiagnoseDialogCallBack);
	}
	else
	{
		Bool_Apply(boolOpType,*slHd,*(boolDlg->slHdB));
	}
}

/*static*/ void GeblGuiEditorBase::Bool_DiagnoseDialogCallBack(FsGuiControlBase *appPtr,FsGuiDialog *,int returnValue)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(YSOK==(YSRESULT)returnValue)
	{
		canvas.Bool_Apply(canvas.boolOpType,*canvas.slHd,*(canvas.boolDlg->slHdB));
	}
	else
	{
		canvas.Edit_ClearUIMode();
	}
}

static void FixOrientationHook(void *ptr)
{
	YsShellExtEdit *shl=(YsShellExtEdit *)ptr;
	YsShellExtEdit_OrientationUtil::FixOrientationOfClosedSolid(*shl);
}

static void TriangulationHook(void *ptr)
{
	YsShellExtEdit *shl=(YsShellExtEdit *)ptr;
	YsShellExt_DiagnoseUtil util;
	util.SetNumThread(2);
	util.SetShell(shl->Conv());
	util.CheckNonPlanarPolygon();
	for(auto &nonPlanarPolygon : util.nonPlanarPolygonArray)
	{
		YsShellExtEdit_TriangulatePolygon(*shl,nonPlanarPolygon.plHd);
	}
}

YSRESULT GeblGuiEditorBase::Bool_Apply(YSBOOLEANOPERATION boolOpType,YsShellExtEdit &slHdA,YsShellExtEdit &slHdB)
{
	YsShellExtEdit_BooleanUtil boolUtil;

	YsShellExtEdit::StopIncUndo incUndoA(&slHdA);
	YsShellExtEdit::StopIncUndo incUndoB(&slHdB);

	if(YSTRUE==boolDlg->triangulateNonPlanarBtn->GetCheck())
	{
		std::thread t1(TriangulationHook,&slHdA);
		std::thread t2(TriangulationHook,&slHdB);
		t1.join();
		t2.join();
	}

	if(YSTRUE==boolDlg->recalcNormalBtn->GetCheck())
	{
		std::thread t1(FixOrientationHook,&slHdA);
		std::thread t2(FixOrientationHook,&slHdB);
		t1.join();
		t2.join();
	}

	boolUtil.SetShellAndOpType((YsShellExtEdit &)slHdA,(YsShellExtEdit &)slHdB,boolOpType);
	boolUtil.Run();

	boolUtil.MakeVertexMapping(0);  // Also tentatively mark vertices from the other shell as 'used'.
	boolUtil.CacheUnusedPolygon(0);
	boolUtil.TransferRemainingPartOfConstEdge(0);
	boolUtil.ReconstructConstEdge(0);
	boolUtil.CombineConstEdgeOfSameOrigin(0);
	boolUtil.ConstrainBetweenInputShell(0);
	boolUtil.DeleteTemporaryVertex(0);
	boolUtil.MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(0);
	boolUtil.AddPolygonFromTheOtherShell(0);

	if(YSBOOLSEPARATE==boolOpType)
	{
		boolUtil.MakeVertexMapping(1);  // Also tentatively mark vertices from the other shell as 'used'.
		boolUtil.CacheUnusedPolygon(1);
		boolUtil.TransferRemainingPartOfConstEdge(1);
		boolUtil.ReconstructConstEdge(1);
		boolUtil.CombineConstEdgeOfSameOrigin(1);
		boolUtil.ConstrainBetweenInputShell(1);
		boolUtil.DeleteTemporaryVertex(1);
		boolUtil.MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(1);
		boolUtil.AddPolygonFromTheOtherShell(1);
	}

	boolUtil.ModifyShell(0);
	boolUtil.ApplyConstEdgeChange(0);
	boolUtil.DeleteCachedUnusedPolygon(0);
	boolUtil.DeleteUsedVertexTurnedUnused(0);

	if(YSBOOLSEPARATE==boolOpType)
	{
		boolUtil.ModifyShell(1);
		boolUtil.ApplyConstEdgeChange(1);
		boolUtil.DeleteCachedUnusedPolygon(1);
		boolUtil.DeleteUsedVertexTurnedUnused(1);
	}

	needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	SetNeedRedraw(YSTRUE);

	Edit_ClearUIMode();

	return YSERR;
}



