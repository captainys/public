/* ////////////////////////////////////////////////////////////

File Name: editmenu.cpp
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

#include <ysgl.h>
#include <ysglbuffer.h>

#include <ysshellextutil.h>
#include <ysshellext_trackingutil.h>
#include <ysshellextedit_topologyutil.h>
#include <ysshellextedit_localop.h>
#include <ysshellext_offsetutil.h>
#include <ysshellextedit_sewingutil.h>
#include <ysshellext_imprintingutil.h>

#include "../ysgebl_gui_editor_base.h"
#include "../miscdlg.h"

#include "edit_create_airfoil.h"
#include "edit_sweepdialog.h"
#include "edit_mirrorimagedialog.h"
#include "edit_hemmingdialog.h"
#include "edit_rounddialog.h"
#include "edit_createprimitivedialog.h"
#include "edit_projdialog.h"
#include "edit_intersectiondialog.h"
#include "edit_insertdialog.h"
#include "edit_solidofrevolutiondialog.h"
#include "edit_offsetdialog.h"

#include "../util/utildlg.h"

#include "../refbmp/refbmpdialog.h"

#include "../dnm/dnmtreedialog.h"

#include "../textresource.h"

#include "../viewmenu/viewdialog.h"

#include "../ysgebl_gui_extension_base.h"


void GeblGuiEditorBase::Edit_ESCKey(FsGuiPopUpMenuItem *)
{
	if(YSTRUE==nextESCOnlyCancelDnmEditSubMode)
	{
		nextESCOnlyCancelDnmEditSubMode=YSFALSE;
		Edit_ClearUIModeExceptDNMEditMode();
	}
	else
	{
		Edit_ClearUIMode();
	}
}

void GeblGuiEditorBase::Edit_ClearUIMode(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode();
}

static void NullDraw(void)
{
}

void GeblGuiEditorBase::Edit_ClearUIMode(void)
{
	dnmEditMode=YSFALSE;
	Edit_ClearUIModeExceptDNMEditMode();
	guiExtension->OnClearUIMode();
}

void GeblGuiEditorBase::Edit_ClearUIModeExceptDNMEditMode(void)
{
	if(NULL!=ModeCleanUpCallBack)
	{
		ModeCleanUpCallBack(*this);
	}
	if(nullptr!=modeCleanUpCallBack)
	{
		modeCleanUpCallBack();
	}

	ModeCleanUpCallBack=NULL;
	SpaceKeyCallBack=NULL;
	BackSpaceKeyCallBack=NULL;
	LButtonCallBack=NULL;
	RButtonCallBack=NULL;
	LButtonBoxCallBack=NULL;
	MouseMoveCallBack=NULL;
	threeDInterface.ClearCallBack();

	ClearLButtonDownCallBack();
	ClearLButtonUpCallBack();
	ClearMButtonDownCallBack();
	ClearMButtonUpCallBack();
	ClearRButtonDownCallBack();
	ClearRButtonUpCallBack();
	ClearMouseMoveCallBack();
	ClearTouchStateChangeCallBack();
	draw3dCallBack=nullptr;
	draw2dCallBack=nullptr;
	spaceKeyCallBack=nullptr;
	modeCleanUpCallBack=nullptr;

	lButtonDownCallBack=nullptr;
	lButtonUpCallBack=nullptr;
	mouseMoveCallBack=nullptr;

	LButtonCallBack=NULL;
	UIBeforeDrawCallBack=NULL;
	UIDrawCallBack2D=NULL;
	UIDrawCallBack3D=NULL;
	UILineVtxBuf.CleanUp();

	threeDInterface.Initialize();

	if(nullptr!=slHd)
	{
		slHd->GetTemporaryModification().CleanUp();
	}

	modeMessage=L"";

	undoRedoRequireToClearUIMode=YSTRUE;
	deletionRequireToClearUIMode=YSTRUE;
	RemoveAllNonPermanentDialog();
	SetNeedRedraw(YSTRUE);
}

void GeblGuiEditorBase::Edit_Undo(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd && YSOK==slHd->Undo())
	{
		if(YSTRUE==undoRedoRequireToClearUIMode)
		{
			Edit_ClearUIMode();
		}

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_Redo(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd && YSOK==slHd->Redo())
	{
		if(YSTRUE==undoRedoRequireToClearUIMode)
		{
			Edit_ClearUIMode();
		}

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_Copy(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		auto selVtHd=shl.GetSelectedVertex();
		auto selPlHd=shl.GetSelectedPolygon();
		auto selFgHd=shl.GetSelectedFaceGroup();
		auto selCeHd=shl.GetSelectedConstEdge();
		// auto selVlHd=canvas.shl.GetSelectedVolume();

		cutBuffer.CleanUp();
		cutBuffer.SetShell((const YsShellExt &)shl);
		cutBuffer.AddVertex(selVtHd);
		cutBuffer.AddPolygon(selPlHd);
		cutBuffer.AddConstEdge(selCeHd);
		cutBuffer.AddFaceGroup(selFgHd);
		// canvas.cutBuffer.AddVolume(selVlHd);
	}
}

void GeblGuiEditorBase::Edit_DeleteSelection(FsGuiPopUpMenuItem *)
{
	Edit_DeleteSelection();
}

void GeblGuiEditorBase::Edit_DeleteSelection(void)
{
	if(NULL!=slHd)
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);

		YSBOOL worked=YSFALSE;
		YSBOOL remain=YSFALSE;

		YsArray <YsShellExt::FaceGroupHandle> selFgHd;
		slHd->GetSelectedFaceGroup(selFgHd);
		for(auto fgHd : selFgHd)
		{
			if(YSOK!=slHd->DeleteFaceGroup(fgHd))
			{
				remain=YSTRUE;
			}
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
			worked=YSTRUE;
		}

		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		slHd->GetSelectedConstEdge(selCeHd);
		for(auto ceHd : selCeHd)
		{
			if(YSOK!=slHd->DeleteConstEdge(ceHd))
			{
				remain=YSTRUE;
			}
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_CONSTEDGE;
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
			worked=YSTRUE;
		}

		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);
		if(0<selPlHd.GetN())
		{
			slHd->DeleteFaceGroupPolygonMulti(selPlHd);

			if(YSOK!=slHd->DeleteMultiPolygon(selPlHd))
			{
				remain=YSTRUE;
			}
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_POLYGON;
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
			worked=YSTRUE;
		}


		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);
		if(0<selVtHd.GetN())
		{
			if(YSOK!=slHd->DeleteMultiVertex(selVtHd))
			{
				remain=YSTRUE;
			}
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_VERTEX;
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
			worked=YSTRUE;
		}

		if(YSTRUE==remain)
		{
			MessageDialog(L"Warning",L"Some entities could not be deleted because they are used.");
		}

		if(YSTRUE==worked)
		{
			if(deletionRequireToClearUIMode)
			{
				Edit_ClearUIMode();
			}
			SetNeedRedraw(YSTRUE);
		}
	}
}

void GeblGuiEditorBase::Edit_ForceDeleteSelection(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);

		// Remove face groups from using volumes

		// Delete face group polygons.
		auto selPlHd=slHd->GetSelectedPolygon();
		if(0<selPlHd)
		{
			if(YSOK==slHd->DeleteFaceGroupPolygonMulti(selPlHd))
			{
				needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
			}
		}

		// Delete polygons and const edges using selected vertices
		auto selVtHd=slHd->GetSelectedVertex();
		for(auto vtHd : selVtHd)
		{
			auto vtPlHd=slHd->FindPolygonFromVertex(vtHd);
			if(0<vtPlHd.GetN())
			{
				slHd->DeleteFaceGroupPolygonMulti(vtPlHd);
				slHd->DeleteMultiPolygon(vtPlHd);
				needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_POLYGON;
				needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
			}

			auto vtCeHd=slHd->FindConstEdgeFromVertex(vtHd);
			if(0<vtCeHd.GetN())
			{
				slHd->DeleteMultiConstEdge(vtCeHd);
				needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_CONSTEDGE;
				needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
			}
		}

		Edit_DeleteSelection(nullptr);  // Then delete everything selected.
	}
printf("%s %d\n",__FUNCTION__,__LINE__);
}

////////////////////////////////////////////////////////////

PolyCreInsertVertexDialog::PolyCreInsertVertexDialog()
{
	canvas=NULL;
	cancelBtn=NULL;
}

PolyCreInsertVertexDialog::~PolyCreInsertVertexDialog()
{
}

/*static*/ PolyCreInsertVertexDialog *PolyCreInsertVertexDialog::Create(class GeblGuiEditorBase *canvas)
{
	PolyCreInsertVertexDialog *dlg=new PolyCreInsertVertexDialog;
	dlg->canvas=canvas;
	return dlg;
}

/*static*/ void PolyCreInsertVertexDialog::Delete(PolyCreInsertVertexDialog *ptr)
{
	delete ptr;
}

YSRESULT PolyCreInsertVertexDialog::Make(void)
{
	if(NULL==cancelBtn)
	{
		FsGuiDialog::Initialize();
		SetIdent("insertVertex");

		insertBtn=AddTextButton(MkId("insert"),FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_INSERT,YSFALSE);
		selectWhenInsertedBtn=AddTextButton(MkId("autoSelect"),FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_INSERTVERTEX_ADDVERTEXTOSELECTION,YSFALSE);
		changePointSchemeBtn=AddTextButton(MkId("changeScheme"),FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_INSERTVERTEX_CHANGESCHEME,YSFALSE);
		cancelBtn=AddTextButton(MkId("cancel"),FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(0.3);
		Fit();
	}

	return YSOK;
}

void PolyCreInsertVertexDialog::RemakeDrawingBuffer(void)
{
	// As required
}

void PolyCreInsertVertexDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==changePointSchemeBtn)
	{
		if(FsGui3DInterface::IFTYPE_POINT==canvas->threeDInterface.GetInterfaceType())
		{
			canvas->threeDInterface.ChangeInputPoint1to2(NULL,NULL);
			canvas->SetNeedRedraw(YSTRUE);
		}
		else
		{
			canvas->threeDInterface.ChangeInputPoint2to1();
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
	if(btn==insertBtn)
	{
		canvas->Edit_InsertVertex_SpaceKeyCallBack(*canvas);
	}
	if(btn==cancelBtn)
	{
		canvas->Edit_ClearUIMode();
	}
}

void PolyCreInsertVertexDialog::OnDropListSelChange(FsGuiDropList *,int)
{
}

void PolyCreInsertVertexDialog::OnTextBoxChange(FsGuiTextBox *)
{
}

void PolyCreInsertVertexDialog::OnSliderPositionChange(FsGuiSlider *,const double &,const double &)
{
}

void GeblGuiEditorBase::Edit_InsertVertex(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		Edit_ClearUIMode();

		insertVtxDlg->Make();
		AddDialog(insertVtxDlg);
		ArrangeDialog();

		const YsVec3 focus=drawEnv.GetViewTarget();
		threeDInterface.BeginInputPoint1(focus);
		SetNeedRedraw(YSTRUE);

		SpaceKeyCallBack=Edit_InsertVertex_SpaceKeyCallBack;
		UIDrawCallBack2D=NULL;
		UIDrawCallBack3D=NULL;

		undoRedoRequireToClearUIMode=YSFALSE;
		deletionRequireToClearUIMode=YSFALSE;
	}
}

YSRESULT GeblGuiEditorBase::Edit_InsertVertex_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	const YsVec3 pos=canvas.threeDInterface.point_pos;
	if(NULL!=canvas.slHd)
	{
		auto vtHd=canvas.slHd->AddVertex(pos);
		if(YSTRUE==canvas.insertVtxDlg->selectWhenInsertedBtn->GetCheck())
		{
			canvas.slHd->AddSelectedVertex(1,&vtHd);
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
		}
		canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_VERTEX;
		canvas.SetNeedRedraw(YSTRUE);
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

void GeblGuiEditorBase::Edit_InsertVertexAtTwoLineIntersection(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto selVtHd=slHd->GetSelectedVertex();
		if(4==selVtHd.size())
		{
			auto &shl=*slHd;
			YsVec3 p[4]=
			{
				shl.GetVertexPosition(selVtHd[0]),
				shl.GetVertexPosition(selVtHd[1]),
				shl.GetVertexPosition(selVtHd[2]),
				shl.GetVertexPosition(selVtHd[3]),
			};
			YsVec3 n1,n2;
			if(YSOK==YsGetNearestPointOfTwoLine(n1,n2,p[0],p[1],p[2],p[3]))
			{
				shl.AddVertex((n1+n2)/2.0);
				needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_VERTEX;
				SetNeedRedraw(YSTRUE);
			}
			else
			{
				MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTCALCULATENEARESTPOINT);
			}
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECT4VERTEX);
		}
	}
}

////////////////////////////////////////////////////////////

void GeblGuiEditorBase::Edit_InsertPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);

		if(3<=selVtHd.GetN())
		{
			YsShellExtEdit::StopIncUndo incUndo(slHd);

			YsShellPolygonHandle plHd=slHd->AddPolygon(selVtHd.GetN(),selVtHd);
			slHd->SelectVertex(0,NULL);
			slHd->SetPolygonColor(plHd,colorPaletteDlg->GetColor());

			needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_SELECTED_VERTEX|NEED_REMAKE_DRAWING_POLYGON;
			SetNeedRedraw(YSTRUE);
		}
	}
}

void GeblGuiEditorBase::Edit_InsertConstEdge(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);
		slHd->AddConstEdge(selVtHd,YSFALSE);
		slHd->SelectVertex(0,NULL);

		needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_SELECTED_VERTEX|NEED_REMAKE_DRAWING_CONSTEDGE;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_InsertFaceGroup(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		auto selPlHd=shl.GetSelectedPolygon();
		if(0<selPlHd.GetN())
		{
			shl.AddFaceGroup(selPlHd);
			shl.SelectPolygon(0,NULL);

			needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
			SetNeedRedraw(YSTRUE);
		}
	}
}

void GeblGuiEditorBase::Edit_SmoothShadeSelectedVertex(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsShellExtEdit::StopIncUndo undoGuard(*slHd);

		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);

		for(YSSIZE_T idx=0; idx<selVtHd.GetN(); ++idx)
		{
			slHd->SetVertexRoundFlag(selVtHd[idx],YSTRUE);
		}

		needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_FlatShadeSelectedVertex(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		YsShellExtEdit::StopIncUndo undoGuard(*slHd);

		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);

		for(YSSIZE_T idx=0; idx<selVtHd.GetN(); ++idx)
		{
			slHd->SetVertexRoundFlag(selVtHd[idx],YSFALSE);
		}

		needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_ShadeSelectedPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsShellExtEdit::StopIncUndo undoGuard(*slHd);

		auto selPlHd=slHd->GetSelectedPolygon();
		for(auto plHd : selPlHd)
		{
			slHd->SetPolygonNoShadingFlag(plHd,YSFALSE);
		}

		needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}
void GeblGuiEditorBase::Edit_SelfLightSelectedPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsShellExtEdit::StopIncUndo undoGuard(*slHd);

		auto selPlHd=slHd->GetSelectedPolygon();
		for(auto plHd : selPlHd)
		{
			slHd->SetPolygonNoShadingFlag(plHd,YSTRUE);
		}

		needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_SetRenderAsLight(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		YsShellExtEdit::StopIncUndo undoGuard(*slHd);
		for(auto plHd : slHd->GetSelectedPolygon())
		{
			slHd->SetPolygonAsLightFlag(plHd,YSTRUE);
		}
		needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}
void GeblGuiEditorBase::Edit_DontRenderAsLight(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		YsShellExtEdit::StopIncUndo undoGuard(*slHd);
		for(auto plHd : slHd->GetSelectedPolygon())
		{
			slHd->SetPolygonAsLightFlag(plHd,YSFALSE);
		}
		needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_ZeroNormalSelectedPolygon(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		YsShellExtEdit::StopIncUndo undoGuard(*slHd);
		for(auto plHd : slHd->GetSelectedPolygon())
		{
			slHd->SetPolygonNormal(plHd,YsVec3::Origin());
		}
		needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}


void GeblGuiEditorBase::Edit_Paint_ByPickingPolygon(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		Edit_ClearUIMode();
		undoRedoRequireToClearUIMode=YSFALSE;
		deletionRequireToClearUIMode=YSFALSE;
		BindLButtonUpCallBack(&THISCLASS::Select_Paint_ByPickingPolygon_LButtonCallBack,this);
		SetNeedRedraw(YSTRUE);
	}
}

YSRESULT GeblGuiEditorBase::Select_Paint_ByPickingPolygon_LButtonCallBack(FsGuiMouseButtonSet btn,YsVec2i pos)
{
	if(YSTRUE!=btn.lb && NULL!=slHd)
	{
		YsShellPolygonHandle pickedPlHd=PickedPlHd(pos.x(),pos.y(),0,NULL);
		if(NULL!=pickedPlHd)
		{
			YsColor col;
			slHd->GetColorOfPolygon(col,pickedPlHd);

			const YsColor selCol=colorPaletteDlg->GetColor();
			if(col!=selCol)
			{
				slHd->SetPolygonColor(pickedPlHd,selCol);
				needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_POLYGON;
				SetNeedRedraw(YSTRUE);
			}
		}
	}
	return YSOK;
}

void GeblGuiEditorBase::Edit_Paint_PickUpColor(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		Edit_ClearUIMode();
		undoRedoRequireToClearUIMode=YSFALSE;
		deletionRequireToClearUIMode=YSFALSE;
		BindLButtonUpCallBack(&THISCLASS::Select_Paint_PickUpColor_LButtonCallBack,this);
		SetNeedRedraw(YSTRUE);
	}
}

YSRESULT GeblGuiEditorBase::Select_Paint_PickUpColor_LButtonCallBack(FsGuiMouseButtonSet btn,YsVec2i pos)
{
	if(YSTRUE!=btn.lb && NULL!=slHd)
	{
		YsShellPolygonHandle pickedPlHd=PickedPlHd(pos.x(),pos.y(),0,NULL);
		if(NULL!=pickedPlHd)
		{
			auto col=slHd->GetColor(pickedPlHd);
			colorPaletteDlg->SetColor(col);
			SetNeedRedraw(YSTRUE);
		}
	}
	return YSOK;
}


/*static*/ void GeblGuiEditorBase::Edit_Paint_ByPickingFaceGroup(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase *canvas=(GeblGuiEditorBase *)appPtr;
	canvas->MessageDialog(L"",L"Sorry, not implemented yet.");
}


void GeblGuiEditorBase::Edit_Paint_SelectedPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);

		if(0==selPlHd.GetN())
		{
			MessageDialog(L"Error",L"No polygon is selected.");
		}
		else
		{
			YSBOOL change=YSFALSE;

			const YsColor selCol=colorPaletteDlg->GetColor();
			for(YSSIZE_T plIdx=0; plIdx<selPlHd.GetN(); ++plIdx)
			{
				YsColor col;
				slHd->GetColorOfPolygon(col,selPlHd[plIdx]);

				if(col!=selCol)
				{
					slHd->SetPolygonColor(selPlHd[plIdx],selCol);
					change=YSTRUE;
				}
			}

			if(YSTRUE==change)
			{
				needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_POLYGON;
				SetNeedRedraw(YSTRUE);
			}
		}
	}
}


void GeblGuiEditorBase::Edit_Paint_All(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);

		const YsColor selCol=colorPaletteDlg->GetColor();
		for(YsShellPolygonHandle plHd=NULL; YSOK==slHd->MoveToNextPolygon(plHd); )
		{
			slHd->SetPolygonColor(plHd,selCol);
		}
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}


void GeblGuiEditorBase::Edit_Paint_FloodFill_SameColor(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		Edit_ClearUIMode();
		undoRedoRequireToClearUIMode=YSFALSE;
		deletionRequireToClearUIMode=YSFALSE;
		BindLButtonUpCallBack(&THISCLASS::Edit_Paint_FloodFill_SameColor_LButtonCallBack,this);
		SetNeedRedraw(YSTRUE);
	}
}

YSRESULT GeblGuiEditorBase::Edit_Paint_FloodFill_SameColor_LButtonCallBack(FsGuiMouseButtonSet btn,YsVec2i pos)
{
	if(YSTRUE!=btn.lb && NULL!=slHd)
	{
		YsShellPolygonHandle pickedPlHd=PickedPlHd(pos.x(),pos.y(),0,NULL);

		if(NULL!=pickedPlHd)
		{
			YsShellExtPolygonFloodFill fill;
			fill.FloodFillSameColor(slHd->Conv(),pickedPlHd);

			const YsColor selCol=colorPaletteDlg->GetColor();

			YsShellExtEdit::StopIncUndo incUndo(slHd);

			YSSIZE_T nPl;
			const YsShellPolygonHandle *plHdArray;
			fill.Get(nPl,plHdArray);

			for(YSSIZE_T idx=0; idx<nPl; ++idx)
			{
				slHd->SetPolygonColor(plHdArray[idx],selCol);
			}

			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_POLYGON;
			SetNeedRedraw(YSTRUE);

			return YSOK;
		}
	}
	return YSERR;
}


void GeblGuiEditorBase::Edit_Paint_FloodFill_BoundedByConstEdge(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		Edit_ClearUIMode();
		undoRedoRequireToClearUIMode=YSFALSE;
		deletionRequireToClearUIMode=YSFALSE;
		BindLButtonUpCallBack(&THISCLASS::Edit_Paint_FloodFill_BoundedByConstEdge_LButtonCallBack,this);
		SetNeedRedraw(YSTRUE);
	}
}

YSRESULT GeblGuiEditorBase::Edit_Paint_FloodFill_BoundedByConstEdge_LButtonCallBack(FsGuiMouseButtonSet btn,YsVec2i pos)
{
	if(YSTRUE!=btn.lb && NULL!=slHd)
	{
		YsShellPolygonHandle pickedPlHd=PickedPlHd(pos.x(),pos.y(),0,NULL);

		if(NULL!=pickedPlHd)
		{
			YsShellExtPolygonFloodFill fill;
			fill.FloodFillBoundedByConstEdge(slHd->Conv(),pickedPlHd);

			const YsColor selCol=colorPaletteDlg->GetColor();

			YsShellExtEdit::StopIncUndo incUndo(slHd);

			YSSIZE_T nPl;
			const YsShellPolygonHandle *plHdArray;
			fill.Get(nPl,plHdArray);

			for(YSSIZE_T idx=0; idx<nPl; ++idx)
			{
				slHd->SetPolygonColor(plHdArray[idx],selCol);
			}

			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_POLYGON;
			SetNeedRedraw(YSTRUE);

			return YSOK;
		}
	}
	return YSERR;
}



void GeblGuiEditorBase::Edit_Move_SelectDirectionFromNormal(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		auto selPlHd=shl.GetSelectedPolygon();

		if(1==selPlHd.GetN())
		{
			auto nom=shl.GetNormal(selPlHd[0]);
			if(YSOK==nom.Normalize())
			{
				preSelectedMovingDirection=nom;
				MessageDialog(L"",L"Direction Selected.");
			}
			else
			{
			}
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECT1POLYGON);
			return;
		}
	}
}

void GeblGuiEditorBase::Edit_Move_SelectVectorFromTwoVertex(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		auto selVtHd=shl.GetSelectedVertex();

		if(2==selVtHd.GetN())
		{
			preSelectedMovingDirection=shl.GetVertexPosition(selVtHd[1])-shl.GetVertexPosition(selVtHd[0]);
			MessageDialog(L"",L"Vector Selected.");
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECT2VERTEX);
			return;
		}
	}
}

void GeblGuiEditorBase::Edit_Move_SelectionInPreSelectedDirection(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		const YsVec3 dir[3]=
		{
			YsUnitVector(preSelectedMovingDirection),YsXVec(),YsYVec()
		};
		const YSBOOL dirSwitch[3]=
		{
			YSTRUE,YSFALSE,YSFALSE
		};
		Edit_StartMoveSelection(dirSwitch,dir);
	}
}

void GeblGuiEditorBase::Edit_Move_SelectionInPreSelectedDirection_SpecificDistance(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiInputNumberDialog>();
		dlg->Make(0.0,6,L"Enter Distance",L"Enter Distance",L"Distance",FSGUI_COMMON_OK,FSGUI_COMMON_CANCEL,(int)YSOK,(int)YSERR);
		dlg->BindCloseModalCallBack(&THISCLASS::Edit_Move_SelectionInPreSelectedDirection_SpecificDistance_CloseModalCallBack,this);
		AttachModalDialog(dlg);
	}
}
void GeblGuiEditorBase::Edit_Move_SelectionInPreSelectedDirection_SpecificDistance_CloseModalCallBack(FsGuiDialog *dlg,int returnCode)
{
	if((int)YSOK==returnCode && nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		auto numDlg=dynamic_cast <FsGuiInputNumberDialog *>(dlg);
		if(nullptr!=numDlg)
		{
			auto dist=numDlg->GetNumber();
			YsVec3 offset=YsUnitVector(preSelectedMovingDirection)*dist;


			YsArray <YsShellVertexHandle> allSelVtHd;
			YsShellExtEdit_GetAllVertexInSelection(allSelVtHd,shl);

			YsArray <YsVec3> newPosArray;
			newPosArray.Set(allSelVtHd.GetN(),NULL);
			for(YSSIZE_T idx=0; idx<allSelVtHd.GetN(); ++idx)
			{
				shl.GetVertexPosition(newPosArray[idx],allSelVtHd[idx]);
				newPosArray[idx]+=offset;
			}

			shl.SetMultiVertexPosition(allSelVtHd,newPosArray);

			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			SetNeedRedraw(YSTRUE);
		}
	}
}

void GeblGuiEditorBase::Edit_Move_SelectionInPreSelectedVector(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		YsArray <YsShellVertexHandle> allSelVtHd;
		YsShellExtEdit_GetAllVertexInSelection(allSelVtHd,shl);

		YsArray <YsVec3> newPosArray;
		newPosArray.Set(allSelVtHd.GetN(),NULL);
		for(YSSIZE_T idx=0; idx<allSelVtHd.GetN(); ++idx)
		{
			shl.GetVertexPosition(newPosArray[idx],allSelVtHd[idx]);
			newPosArray[idx]+=preSelectedMovingDirection;
		}

		shl.SetMultiVertexPosition(allSelVtHd,newPosArray);

		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_MoveVertexToThreePlaneIntersection(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		auto selVtHd=shl.GetSelectedVertex();
		auto selPlHd=shl.GetSelectedPolygon();

		if(1==selVtHd.size() && 3==selPlHd.size())
		{
			YsPlane pln[3];
			for(YSSIZE_T idx=0; idx<3; ++idx)
			{
				auto plVtPos=shl.GetPolygonVertexPosition(selPlHd[idx]);
				YsVec3 o,n;
				if(YSOK!=YsFindLeastSquareFittingPlane(o,n,plVtPos))
				{
					MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_LEASTSQUAREPLANE);
					return;
				}
				pln[idx].Set(o,n);
			}

			YsVec3 pos;
			if(YSOK==YsFindLeastSquarePointFromPlane(pos,pln))
			{
				shl.SetVertexPosition(selVtHd[0],pos);
				needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
				SetNeedRedraw(YSTRUE);
			}
			else
			{
				MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTCALCULATEINTERSECTION);
				return;
			}
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECT_1VTX3PLG);
		}
	}
}

void GeblGuiEditorBase::Edit_MoveSelection(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		Edit_StartMoveSelection(nullptr,nullptr);
	}
}

void GeblGuiEditorBase::Edit_StartMoveSelection(const YSBOOL dirSwitch[3],const YsVec3 dir[3])
{

	Edit_ClearUIMode();
	undoRedoRequireToClearUIMode=YSFALSE;
	deletionRequireToClearUIMode=YSTRUE;

	UIDrawCallBack3D=Edit_MoveSelection_DrawCallBack3D;
	SpaceKeyCallBack=Edit_MoveSelection_SpaceKeyCallBack;

	YsArray <YsShellVertexHandle> allSelVtHd;
	YsShellExtEdit_GetAllVertexInSelection(allSelVtHd,*slHd);

	YsVec3 cen;
	slHd->GetCenterOfVertexGroup(cen,allSelVtHd.GetN(),allSelVtHd);

	if(nullptr!=dirSwitch)
	{
		threeDInterface.SetInputPoint2UseCustomOrientation(YSTRUE);
	}
	else
	{
		threeDInterface.SetInputPoint2UseCustomOrientation(YSFALSE);
	}

	threeDInterface.BeginInputPoint2(cen,dirSwitch,dir);
	threeDInterface.BindCallBack(&THISCLASS::Edit_MoveSelection_CursorMoved,this);

	SetNeedRedraw(YSTRUE);
}

/*static*/ void GeblGuiEditorBase::Edit_MoveSelection_DrawCallBack3D(GeblGuiEditorBase &canvas)
{
	if(NULL!=canvas.slHd)
	{
		struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
		YsGLSLUse3DRenderer(renderer);

		const GLfloat col[4]={0.0f,1.0f,0.0f,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,col);

		YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)canvas.UILineVtxBuf.GetNumVertex(),canvas.UILineVtxBuf);

		YsGLSLEndUse3DRenderer(renderer);
	}
}

void GeblGuiEditorBase::Edit_MoveSelection_CursorMoved(FsGui3DInterface *)
{
	if(NULL!=slHd)
	{
		UILineVtxBuf.CleanUp();

		const YsVec3 offset=threeDInterface.point_pos-threeDInterface.point_pos_org;

		YsArray <YsShellVertexHandle> allSelVtHd;
		YsShellExtEdit_GetAllVertexInSelection(allSelVtHd,*slHd);

		YsHashTable <YsVec3> vtxMap;

		YsGLVertexBuffer &buf=UILineVtxBuf;
		for(YSSIZE_T idx=0; idx<allSelVtHd.GetN(); ++idx)
		{
			YsVec3 vtPos[2];
			slHd->GetVertexPosition(vtPos[0],allSelVtHd[idx]);
			vtPos[1]=vtPos[0]+offset;
			buf.AddVertex(vtPos[0]);
			buf.AddVertex(vtPos[1]);

			vtxMap.AddElement(slHd->GetSearchKey(allSelVtHd[idx]),vtPos[1]);
		}

		for(YSSIZE_T idx=0; idx<allSelVtHd.GetN(); ++idx)
		{
			YsArray <YsShellVertexHandle,16> connVtHd;
			slHd->GetConnectedVertex(connVtHd,allSelVtHd[idx]);

			YsVec3 vtPos[2];
			slHd->GetVertexPosition(vtPos[0],allSelVtHd[idx]);
			vtPos[0]+=offset;
			for(YSSIZE_T neiIdx=0; neiIdx<connVtHd.GetN(); ++neiIdx)
			{
				if(YSOK!=vtxMap.FindElement(vtPos[1],slHd->GetSearchKey(connVtHd[neiIdx])))
				{
					slHd->GetVertexPosition(vtPos[1],connVtHd[neiIdx]);
				}
				buf.AddVertex(vtPos[0]);
				buf.AddVertex(vtPos[1]);
			}
		}
	}
}

/*static*/ YSRESULT GeblGuiEditorBase::Edit_MoveSelection_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	if(NULL!=canvas.slHd)
	{
		const YsVec3 offset=canvas.threeDInterface.point_pos-canvas.threeDInterface.point_pos_org;

		YsArray <YsShellVertexHandle> allSelVtHd;
		YsShellExtEdit_GetAllVertexInSelection(allSelVtHd,*(canvas.slHd));

		YsArray <YsVec3> newPosArray;
		newPosArray.Set(allSelVtHd.GetN(),NULL);
		for(YSSIZE_T idx=0; idx<allSelVtHd.GetN(); ++idx)
		{
			canvas.slHd->GetVertexPosition(newPosArray[idx],allSelVtHd[idx]);
			newPosArray[idx]+=offset;
		}

		canvas.slHd->SetMultiVertexPosition(allSelVtHd,newPosArray);

		canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
		canvas.SetNeedRedraw(YSTRUE);

		canvas.Edit_ClearUIMode();
	}
	return YSOK;
}


void GeblGuiEditorBase::Edit_RotateSelection(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		Edit_ClearUIMode();
		undoRedoRequireToClearUIMode=YSFALSE;
		deletionRequireToClearUIMode=YSTRUE;

		UIDrawCallBack3D=Edit_RotateSelection_DrawCallBack3D;
		SpaceKeyCallBack=Edit_RotateSelection_SpaceKeyCallBack;

		YsArray <YsShellVertexHandle> allSelVtHd;
		YsShellExtEdit_GetAllVertexInSelection(allSelVtHd,*slHd);

		YsVec3 bbx[2];
		slHd->GetBoundingBox(bbx,allSelVtHd);

		const double bbxDgn=(bbx[1]-bbx[0]).GetLength();

		YsVec3 cen;
		slHd->GetCenterOfVertexGroup(cen,allSelVtHd.GetN(),allSelVtHd);

		const YSBOOL allowChangeAxis=YSTRUE;
		const YSBOOL allowMoveCenter=YSTRUE;
		const YSBOOL allowMirror=YSTRUE;
		threeDInterface.BeginInputRotation(YsZVec(),0.0,cen,bbxDgn/2.0,allowChangeAxis,allowMoveCenter,allowMirror);

		threeDInterface.BindCallBack(&THISCLASS::Edit_RotateSelection_CursorMoved,this);

		SetNeedRedraw(YSTRUE);
	}
}
/* static */ void GeblGuiEditorBase::Edit_RotateSelection_DrawCallBack3D(GeblGuiEditorBase &canvas)
{
	if(NULL!=canvas.slHd)
	{
		struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
		YsGLSLUse3DRenderer(renderer);

		const GLfloat col[4]={0.0f,1.0f,0.0f,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,col);

		YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)canvas.UILineVtxBuf.GetNumVertex(),canvas.UILineVtxBuf);

		YsGLSLEndUse3DRenderer(renderer);
	}
}
void GeblGuiEditorBase::Edit_RotateSelection_CursorMoved(FsGui3DInterface *)
{
	if(NULL!=slHd)
	{
		UILineVtxBuf.CleanUp();

		YsMatrix4x4 tfm;
		threeDInterface.GetRotationMatrix(tfm);

		YsArray <YsShellVertexHandle> allSelVtHd;
		YsShellExtEdit_GetAllVertexInSelection(allSelVtHd,*slHd);

		YsHashTable <YsVec3> vtxMap;

		YsGLVertexBuffer &buf=UILineVtxBuf;
		for(YSSIZE_T idx=0; idx<allSelVtHd.GetN(); ++idx)
		{
			YsVec3 vtPos[2];
			slHd->GetVertexPosition(vtPos[0],allSelVtHd[idx]);
			vtPos[1]=tfm*vtPos[0];
			buf.AddVertex(vtPos[0]);
			buf.AddVertex(vtPos[1]);
			vtxMap.AddElement(slHd->GetSearchKey(allSelVtHd[idx]),vtPos[1]);
		}

		for(YSSIZE_T idx=0; idx<allSelVtHd.GetN(); ++idx)
		{
			YsArray <YsShellVertexHandle,16> connVtHd;
			slHd->GetConnectedVertex(connVtHd,allSelVtHd[idx]);

			YsVec3 vtPos[2];
			slHd->GetVertexPosition(vtPos[0],allSelVtHd[idx]);
			tfm.Mul(vtPos[0],vtPos[0],1.0);
			for(YSSIZE_T neiIdx=0; neiIdx<connVtHd.GetN(); ++neiIdx)
			{
				if(YSOK!=vtxMap.FindElement(vtPos[1],slHd->GetSearchKey(connVtHd[neiIdx])))
				{
					slHd->GetVertexPosition(vtPos[1],connVtHd[neiIdx]);
				}
				buf.AddVertex(vtPos[0]);
				buf.AddVertex(vtPos[1]);
			}
		}
	}
}
/* static */ YSRESULT GeblGuiEditorBase::Edit_RotateSelection_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	if(NULL!=canvas.slHd)
	{
		YsMatrix4x4 tfm;
		canvas.threeDInterface.GetRotationMatrix(tfm);

		YsArray <YsShellVertexHandle> allSelVtHd;
		YsShellExtEdit_GetAllVertexInSelection(allSelVtHd,*(canvas.slHd));

		YsArray <YsVec3> newPosArray;
		newPosArray.Set(allSelVtHd.GetN(),NULL);
		for(YSSIZE_T idx=0; idx<allSelVtHd.GetN(); ++idx)
		{
			canvas.slHd->GetVertexPosition(newPosArray[idx],allSelVtHd[idx]);
			tfm.Mul(newPosArray[idx],newPosArray[idx],1.0);
		}

		canvas.slHd->SetMultiVertexPosition(allSelVtHd,newPosArray);

		canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
		canvas.SetNeedRedraw(YSTRUE);

		canvas.Edit_ClearUIMode();
	}
	return YSOK;
}


void GeblGuiEditorBase::Edit_SwapEdge(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);

		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);

		YsShell_SwapInfo swapInfo;
		if(2==selVtHd.GetN())
		{
			if(YSOK!=swapInfo.MakeInfo(slHd->Conv(),selVtHd[0],selVtHd[1]))
			{
				MessageDialog(L"Error",
				    L"Two selected vertices must be used by two triangles.");
				return;
			}
		}
		else if(2==selPlHd.GetN())
		{
			if(YSOK!=swapInfo.MakeInfo(slHd->Conv(),selPlHd[0],selPlHd[1]))
			{
				MessageDialog(L"Error",
				    L"Two selected polygons must be two neighboring triangles.");
				return;
			}
		}
		else
		{
			MessageDialog(L"Error",
			    L"Two vertices or two neighboring triangles must be selected.");
			return;
		}

		YsShellExtEdit::StopIncUndo incUndo(slHd);
		slHd->SetPolygonVertex(swapInfo.triPlHd[0],3,swapInfo.newTriVtHd[0]);
		slHd->SetPolygonVertex(swapInfo.triPlHd[1],3,swapInfo.newTriVtHd[1]);

		slHd->SelectVertex(0,NULL);
		slHd->SelectPolygon(0,NULL);

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_VERTEX|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_MergePolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);

		YsShellPolygonStore selPlHash(slHd->Conv());
		YsArray <unsigned int> selPlKey;
		for(YSSIZE_T idx=0; idx<selPlHd.GetN(); ++idx)
		{
			selPlKey.Append(slHd->GetSearchKey(selPlHd[idx]));
			selPlHash.AddPolygon(selPlHd[idx]);
		}

		YsShellExtEdit::StopIncUndo incUndo(slHd);
		for(YSSIZE_T idx=0; idx<selPlKey.GetN(); ++idx)
		{
		RETRY:
			YsShellPolygonHandle seedPlHd=slHd->FindPolygon(selPlKey[idx]);
			if(NULL!=seedPlHd)
			{
				const int nPlVt=slHd->GetNumVertexOfPolygon(seedPlHd);
				for(int edIdx=0; edIdx<nPlVt; ++edIdx)
				{
					YsShellPolygonHandle neiPlHd=slHd->GetNeighborPolygon(seedPlHd,edIdx);
					if(NULL!=neiPlHd && YSTRUE==selPlHash.IsIncluded(neiPlHd))
					{
						YsShell_MergeInfo mergeInfo;
						if(YSOK==mergeInfo.MakeInfo(slHd->Conv(),seedPlHd,neiPlHd))
						{
							slHd->SetPolygonVertex(seedPlHd,mergeInfo.newPlVtHd);
							slHd->DeleteFaceGroupPolygon(neiPlHd);
							slHd->DeletePolygon(neiPlHd);
							goto RETRY;
						}
					}
				}
			}
		}

		slHd->SelectPolygon(0,NULL);

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_VERTEX|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_SplitPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellVertexHandle,2> selVtHd;
		slHd->GetSelectedVertex(selVtHd);

		if(2==selVtHd.GetN())
		{
			YsArray <YsShellPolygonHandle> vtPlHd;
			slHd->FindPolygonFromVertex(vtPlHd,selVtHd[0]);

			for(YSSIZE_T plIdx=vtPlHd.GetN()-1; 0<=plIdx; --plIdx)
			{
				YsArray <YsShellVertexHandle,4> plVtHd;
				slHd->GetPolygon(plVtHd,vtPlHd[plIdx]);
				if(YSTRUE!=plVtHd.IsIncluded(selVtHd[1]))
				{
					vtPlHd.DeleteBySwapping(plIdx);
				}
			}

			if(1==vtPlHd.GetN())
			{
				YsShellExtEdit_TopologyUtil topoUtil;
				if(YSOK==topoUtil.SplitPolygon(*slHd,vtPlHd[0],selVtHd[0],selVtHd[1]))
				{
					slHd->SelectVertex(0,NULL);

					needRemakeDrawingBuffer=(unsigned int)
					    NEED_REMAKE_DRAWING_POLYGON|
					    NEED_REMAKE_DRAWING_SELECTED_VERTEX|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
					SetNeedRedraw(YSTRUE);
				}
			}
			else
			{
				// Cannot find a unique polygon from two vertices.
			}
		}
	}
}

void GeblGuiEditorBase::Edit_VertexCollapseMidPoint(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);
		if(2==selVtHd.GetN())
		{
			YsVec3 cen;
			slHd->GetCenterOfEdge(cen,selVtHd[0],selVtHd[1]);
			Edit_VertexCollapse(selVtHd[0],selVtHd[1],cen);
		}
		else
		{
			MessageDialog(L"Error",L"Two vertices must be selected.");
		}
	}
}

void GeblGuiEditorBase::Edit_VertexCollapseAtoB(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);
		if(2==selVtHd.GetN())
		{
			YsVec3 to;
			slHd->GetVertexPosition(to,selVtHd[1]);
			Edit_VertexCollapse(selVtHd[0],selVtHd[1],to);
		}
		else
		{
			MessageDialog(L"Error",L"Two vertices must be selected.");
		}
	}
}

void GeblGuiEditorBase::Edit_VertexCollapse(YsShellVertexHandle vtHdToDel,YsShellVertexHandle vtHdToSurvive,const YsVec3 &newPos)
{
	if(NULL!=slHd)
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);

		if(YSOK==YsShellExtEdit_CollapseEdge(*(YsShellExtEdit *)slHd,vtHdToDel,vtHdToSurvive))
		{
			slHd->SetVertexPosition(vtHdToSurvive,newPos);
			slHd->SelectVertex(0,NULL);

			slHd->SetVertexPosition(vtHdToSurvive,newPos);

			slHd->SelectVertex(0,NULL);

			needRemakeDrawingBuffer=(unsigned int)
			    NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_VERTEX|
			    NEED_REMAKE_DRAWING_SELECTED_VERTEX|NEED_REMAKE_DRAWING_SELECTED_POLYGON|
			    NEED_REMAKE_DRAWING_CONSTEDGE;
			SetNeedRedraw(YSTRUE);
		}
	}
}

////////////////////////////////////////////////////////////

PolyCreHemmingDialog::PolyCreHemmingDialog()
{
}

PolyCreHemmingDialog::~PolyCreHemmingDialog()
{
}

/*static*/ PolyCreHemmingDialog *PolyCreHemmingDialog::Create(class GeblGuiEditorBase *canvas)
{
	PolyCreHemmingDialog *newDlg=new PolyCreHemmingDialog;
	newDlg->canvas=canvas;
	return newDlg;
}

/*static*/ void PolyCreHemmingDialog::Delete(PolyCreHemmingDialog *ptr)
{
	delete ptr;
}

YSRESULT PolyCreHemmingDialog::Make(void)
{
	Initialize();

	okBtn=AddTextButton(0,FSKEY_SPACE,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	slider=AddHorizontalSlider(0,FSKEY_NULL,50,0.0,1.0,YSTRUE);
	slider->SetPosition(0.4);

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();

	return YSOK;
}

/*virtual*/ void PolyCreHemmingDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		canvas->Edit_Hemming_AroundSelectedPolygon();
		canvas->Edit_ClearUIMode();
	}
	else if(btn==cancelBtn)
	{
		canvas->Edit_ClearUIMode();
	}
}

/*virtual*/ void PolyCreHemmingDialog::OnSliderPositionChange(FsGuiSlider *,const double &,const double &)
{
}

void GeblGuiEditorBase::Edit_Hemming_AroundSelectedPolygon(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode();

	if(nullptr!=slHd)
	{
		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);

		hemmingDlg->offsetUtil.CleanUp();
		if(YSOK==hemmingDlg->offsetUtil.SetUpForEdgeConnectedPolygonGroup(slHd->Conv(),selPlHd))
		{
			if(YSOK==hemmingDlg->Make())
			{
				AddDialog(hemmingDlg);
				ArrangeDialog();
				UIDrawCallBack3D=Edit_Hemming_AroundSelectedPolygon_UIDrawCallBack3D;
			}
		}
	}
}

void GeblGuiEditorBase::Edit_Hemming_AroundOutsideOfSelectedPolygon(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode();

	if(nullptr!=slHd)
	{
		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);

		hemmingDlg->offsetUtil.CleanUp();
		if(YSOK==hemmingDlg->offsetUtil.SetUpForOutsideOfEdgeConnectedPolygonGroup(slHd->Conv(),selPlHd))
		{
			if(YSOK==hemmingDlg->Make())
			{
				AddDialog(hemmingDlg);
				ArrangeDialog();
				UIDrawCallBack3D=Edit_Hemming_AroundSelectedPolygon_UIDrawCallBack3D;
			}
		}
	}
}

/*static*/ void GeblGuiEditorBase::Edit_Hemming_AroundSelectedPolygon_UIDrawCallBack3D(GeblGuiEditorBase &canvas)
{
	if(NULL!=canvas.slHd)
	{
		const double t=canvas.hemmingDlg->slider->GetPosition();

		double dist=YsInfinity;
		for(auto newVtx : canvas.hemmingDlg->offsetUtil.newVtxArray)
		{
			const double d=newVtx.maxDist*t;
			dist=YsSmaller(dist,d);
		}

		for(auto &newVtx : canvas.hemmingDlg->offsetUtil.newVtxArray)
		{
			const YsVec3 newVtxPos=newVtx.fromPos+newVtx.dir*dist;
			newVtx.toPos=newVtxPos;
		}

		YsGLVertexBuffer vtxBuf;

		YsArray <YsShellPolygonHandle> selPlHdArray=canvas.hemmingDlg->offsetUtil.srcPlHdArray;

		for(auto plHd : selPlHdArray)
		{
			YsArray <YsVec3,4> plVtPos;
			YsArray <YsShellVertexHandle,4> plVtHd;

			canvas.slHd->GetPolygon(plVtPos,plHd);
			canvas.slHd->GetPolygon(plVtHd,plHd);

			for(YSSIZE_T vtIdx=0; vtIdx<plVtPos.GetN(); ++vtIdx)
			{
				YSSIZE_T newVtxIdx;
				if(YSOK==canvas.hemmingDlg->offsetUtil.vtKeyToNewVtxIndex.FindElement(newVtxIdx,canvas.slHd->GetSearchKey(plVtHd[vtIdx])))
				{
					plVtPos[vtIdx]=canvas.hemmingDlg->offsetUtil.newVtxArray[newVtxIdx].toPos;
				}
			}

			for(YSSIZE_T vtIdx=0; vtIdx<plVtPos.GetN(); ++vtIdx)
			{
				vtxBuf.AddVertex(plVtPos[vtIdx]);
				vtxBuf.AddVertex(plVtPos.GetCyclic(vtIdx+1));
			}

			struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
			YsGLSLUse3DRenderer(renderer);

			const GLfloat edgeCol[4]={0,0,0,1.0f};
			YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
			YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)vtxBuf.GetNumVertex(),vtxBuf);

			YsGLSLEndUse3DRenderer(renderer);
		}
	}
}

void GeblGuiEditorBase::Edit_Hemming_AroundSelectedPolygon(void)
{
	if(NULL!=slHd)
	{
		const double t=hemmingDlg->slider->GetPosition();

		hemmingDlg->offsetUtil.ApplyByRatio(*(YsShellExtEdit *)slHd,t);

		needRemakeDrawingBuffer=(unsigned int)
		    NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_VERTEX|
		    NEED_REMAKE_DRAWING_SELECTED_POLYGON|NEED_REMAKE_DRAWING_CONSTEDGE;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_Hemming_OneSideOfVertexSequence(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode();

	if(nullptr!=slHd)
	{
		auto selVtHd=slHd->GetSelectedVertex();
		auto selCeHd=slHd->GetSelectedConstEdge();

		if(0<selVtHd.GetN() && 0<selCeHd.GetN())
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTVTXORCONSTEDGE);
			return;
		}

		if(0<selCeHd.GetN())
		{
			selVtHd=YsShellExt_TrackingUtil::MakeVertexSequenceFromMultiConstEdge(slHd->Conv(),selCeHd);
			if(0==selVtHd.GetN())
			{
				MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTMAKECONSTEDGECHAIN);
				return;
			}
		}


		YsArray <YsArray <YsShellVertexHandle> > contourVtHd(1,NULL);
		YsArray <YsShellPolygonHandle> selPlHd;

		contourVtHd[0]=selVtHd;
		slHd->GetSelectedPolygon(selPlHd);

		hemmingDlg->offsetUtil.CleanUp();
		if(YSOK==hemmingDlg->offsetUtil.SetUpForOneSideOfVertexSequence(slHd->Conv(),contourVtHd,selPlHd))
		{
			if(YSOK==hemmingDlg->Make())
			{
				AddDialog(hemmingDlg);
				ArrangeDialog();
				UIDrawCallBack3D=Edit_Hemming_AroundSelectedPolygon_UIDrawCallBack3D;
			}
		}
	}
}

////////////////////////////////////////////////////////////

PolyCreProjectionDialog::PolyCreProjectionDialog()
{
	canvas=NULL;
	okBtn=NULL;
	cancelBtn=NULL;
	projDirDrp=NULL;
	projTargetDrp=NULL;
}

PolyCreProjectionDialog::~PolyCreProjectionDialog()
{
}

/*static*/ PolyCreProjectionDialog *PolyCreProjectionDialog::Create(GeblGuiEditorBase *canvas)
{
	PolyCreProjectionDialog *dlg=new PolyCreProjectionDialog;
	dlg->canvas=canvas;
	return dlg;
}

/*static*/ void PolyCreProjectionDialog::Delete(PolyCreProjectionDialog *ptr)
{
	delete ptr;
}

void PolyCreProjectionDialog::Make(void)
{
	if(NULL==okBtn)
	{
		Initialize();

		okBtn=AddTextButton(0,FSKEY_SPACE,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

		AddStaticText(0,FSKEY_NULL,L"Direction",YSTRUE);
		const char * const dirStr[]=
		{
			"Shortest to the projection plane",
			"X-axis",
			"Y-axis",
			"Z-axis",
			"First two vertices",
			"Last two vertices"
		};
		projDirDrp=AddDropList(0,FSKEY_NULL,"Direction",6,dirStr,6,32,32,YSFALSE);

		AddStaticText(0,FSKEY_NULL,L"Target Plane",YSTRUE);
		const char * const targetStr[]=
		{
			"Selected Polygon or Const Edge",
			"First three vertices",
			"Last three vertices",
			"XY-plane",
			"XZ-plane",
			"YZ-plane",
			"Parallel to XY passing through the first vertex",
			"Parallel to XZ passing through the first vertex",
			"Parallel to YZ passing through the first vertex",
			"Parallel to XY passing through the last vertex",
			"Parallel to XZ passing through the last vertex",
			"Parallel to YZ passing through the last vertex"
		};
		projTargetDrp=AddDropList(0,FSKEY_NULL,"Plane",12,targetStr,12,40,40,YSFALSE);
	}

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();

	RemakeVertexProjectionArray();
}

YSRESULT PolyCreProjectionDialog::RemakeVertexProjectionArray(void)
{
	vertexPositionPair.CleanUp();
	if(nullptr!=canvas->slHd)
	{
		YsPlane pln;
		if(YSOK!=CalculateProjectionPlane(pln))
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			return YSERR;
		}
		YsVec3 dir;
		if(YSOK!=CalculateProjectionDirection(dir,pln))
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			return YSERR;
		}

		if(fabs(dir*pln.GetNormal())<YsTolerance)
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
printf("%s\n",dir.Txt());
printf("%s\n",pln.GetNormal().Txt());
			return YSERR;
		}

		YsArray <YsShellVertexHandle> selVtHd=GetVertexToBeMoved();

		for(auto vtHd : selVtHd)
		{
			YsVec3 itsc;
			if(YSOK!=pln.GetIntersectionHighPrecision(itsc,canvas->slHd->GetVertexPosition(vtHd),dir))
			{
				vertexPositionPair.CleanUp();
printf("%s %d\n",__FUNCTION__,__LINE__);
				return YSERR;
			}
			vertexPositionPair.Increment();
			vertexPositionPair.Last().a=vtHd;
			vertexPositionPair.Last().b=itsc;
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT PolyCreProjectionDialog::RemakeDrawingBuffer(void)
{
	lineBuf.CleanUp();
	triBuf.CleanUp();

	for(auto pair : vertexPositionPair)
	{
		const YsVec3 vtPos=canvas->slHd->GetVertexPosition(pair.a);
		lineBuf.AddVertex(vtPos);
		lineBuf.AddVertex(pair.b);

		triBuf.AddArrowHeadTriangle(vtPos,pair.b,0.1);
	}

	return YSOK;
}

YSRESULT PolyCreProjectionDialog::CalculateProjectionPlane(YsPlane &pln) const
{
	if(NULL==canvas->slHd)
	{
		return YSERR;
	}

	switch(projTargetDrp->GetSelection())
	{
	default:
		return YSERR;
	case 0: //		"Selected Polygon or Const Edge",
		{
			YsArray <YsShellPolygonHandle> selPlHd;
			canvas->slHd->GetSelectedPolygon(selPlHd);

			YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
			canvas->slHd->GetSelectedConstEdge(selCeHd);

			if(1==selPlHd.GetN() && 0==selCeHd.GetN())
			{
				YsArray <YsVec3> plVtPos;
				canvas->slHd->GetPolygon(plVtPos,selPlHd[0]);
				return pln.MakeBestFitPlane(plVtPos.GetN(),plVtPos);
			}
			else if(0==selPlHd.GetN() && 1==selCeHd.GetN())
			{
				YsArray <YsShellVertexHandle> ceVtHd;
				YSBOOL isLoop;
				canvas->slHd->GetConstEdge(ceVtHd,isLoop,selCeHd[0]);

				YsArray <YsVec3> ceVtPos;
				for(auto vtHd : ceVtHd)
				{
					ceVtPos.Append(canvas->slHd->GetVertexPosition(vtHd));
				}
				return pln.MakeBestFitPlane(ceVtPos.GetN(),ceVtPos);
			}
			else
			{
				return YSERR;
			}
		}
		break;
	case 1: //		"First three vertices",
		{
			YsArray <YsShellVertexHandle> selVtHd;
			canvas->slHd->GetSelectedVertex(selVtHd);
			if(3<=selVtHd.GetN())
			{
				const YsVec3 vtPos[3]=
				{
					canvas->slHd->GetVertexPosition(selVtHd[0]),
					canvas->slHd->GetVertexPosition(selVtHd[1]),
					canvas->slHd->GetVertexPosition(selVtHd[2])
				};
				return pln.MakePlaneFromTriangle(vtPos[0],vtPos[1],vtPos[2]);
			}
		}
		break;
	case 2: //		"Last three vertices",
		{
			YsArray <YsShellVertexHandle> selVtHd;
			canvas->slHd->GetSelectedVertex(selVtHd);
			if(3<=selVtHd.GetN())
			{
				const YsVec3 vtPos[3]=
				{
					canvas->slHd->GetVertexPosition(selVtHd.GetCyclic(-1)),
					canvas->slHd->GetVertexPosition(selVtHd.GetCyclic(-2)),
					canvas->slHd->GetVertexPosition(selVtHd.GetCyclic(-3))
				};
				return pln.MakePlaneFromTriangle(vtPos[0],vtPos[1],vtPos[2]);
			}
		}
		break;
	case 3: //		"XY-plane",
		pln.Set(YsOrigin(),YsZVec());
		return YSOK;
	case 4: //		"XZ-plane",
		pln.Set(YsOrigin(),YsYVec());
		return YSOK;
	case 5: //		"YZ-plane",
		pln.Set(YsOrigin(),YsXVec());
		return YSOK;
	case 6: //		"Parallel to XY passing through the first vertex",
		{
			YsArray <YsShellVertexHandle> selVtHd;
			canvas->slHd->GetSelectedVertex(selVtHd);
			if(0<selVtHd.GetN())
			{
				const YsVec3 vtPos=canvas->slHd->GetVertexPosition(selVtHd[0]);
				pln.Set(vtPos,YsZVec());
				return YSOK;
			}
		}
		break;
	case 7: //		"Parallel to XZ passing through the first vertex",
		{
			YsArray <YsShellVertexHandle> selVtHd;
			canvas->slHd->GetSelectedVertex(selVtHd);
			if(0<selVtHd.GetN())
			{
				const YsVec3 vtPos=canvas->slHd->GetVertexPosition(selVtHd[0]);
				pln.Set(vtPos,YsYVec());
				return YSOK;
			}
		}
		break;
	case 8: //		"Parallel to YZ passing through the first vertex",
		{
			YsArray <YsShellVertexHandle> selVtHd;
			canvas->slHd->GetSelectedVertex(selVtHd);
			if(0<selVtHd.GetN())
			{
				const YsVec3 vtPos=canvas->slHd->GetVertexPosition(selVtHd[0]);
				pln.Set(vtPos,YsXVec());
				return YSOK;
			}
		}
		break;
	case 9: //		"Parallel to XY passing through the last vertex",
		{
			YsArray <YsShellVertexHandle> selVtHd;
			canvas->slHd->GetSelectedVertex(selVtHd);
			if(0<selVtHd.GetN())
			{
				const YsVec3 vtPos=canvas->slHd->GetVertexPosition(selVtHd.Last());
				pln.Set(vtPos,YsZVec());
				return YSOK;
			}
		}
		break;
	case 10: //		"Parallel to XZ passing through the last vertex",
		{
			YsArray <YsShellVertexHandle> selVtHd;
			canvas->slHd->GetSelectedVertex(selVtHd);
			if(0<selVtHd.GetN())
			{
				const YsVec3 vtPos=canvas->slHd->GetVertexPosition(selVtHd.Last());
				pln.Set(vtPos,YsYVec());
				return YSOK;
			}
		}
		break;
	case 11: //		"Parallel to YZ passing through the last vertex"
		{
			YsArray <YsShellVertexHandle> selVtHd;
			canvas->slHd->GetSelectedVertex(selVtHd);
			if(0<selVtHd.GetN())
			{
				const YsVec3 vtPos=canvas->slHd->GetVertexPosition(selVtHd.Last());
				pln.Set(vtPos,YsXVec());
				return YSOK;
			}
		}
		break;
	}
	return YSERR;
}

YSRESULT PolyCreProjectionDialog::CalculateProjectionDirection(YsVec3 &dir,const YsPlane &pln) const
{
	switch(projDirDrp->GetSelection())
	{
	case 0: //"Shortest to the projection plane",
		dir=pln.GetNormal();
		return YSOK;
	case 1: //"X-axis",
		dir=YsXVec();
		return YSOK;
	case 2: //"Y-axis",
		dir=YsYVec();
		return YSOK;
	case 3: //"Z-axis",
		dir=YsZVec();
		return YSOK;
	case 4: //"First two vertices"
		{
			YsArray <YsShellVertexHandle> selVtHd;
			canvas->slHd->GetSelectedVertex(selVtHd);
			if(2<=selVtHd.GetN())
			{
				const YsVec3 vtPos[2]=
				{
					canvas->slHd->GetVertexPosition(selVtHd[0]),
					canvas->slHd->GetVertexPosition(selVtHd[1])
				};
				dir=vtPos[1]-vtPos[0];
				return dir.Normalize();
			}
		}
		break;
	case 5: //"Last two vertices",
		{
			YsArray <YsShellVertexHandle> selVtHd;
			canvas->slHd->GetSelectedVertex(selVtHd);
			if(2<=selVtHd.GetN())
			{
				const YsVec3 vtPos[2]=
				{
					canvas->slHd->GetVertexPosition(selVtHd.GetCyclic(-1)),
					canvas->slHd->GetVertexPosition(selVtHd.GetCyclic(-2))
				};
				dir=vtPos[1]-vtPos[0];
				return dir.Normalize();
			}
		}
		break;
	default:
		break;
	}
	return YSERR;
}

YsArray <YsShellVertexHandle> PolyCreProjectionDialog::GetVertexToBeMoved(void) const
{
	auto selVtHd=canvas->slHd->GetSelectedVertex();

	switch(projTargetDrp->GetSelection())
	{
	default:
		break;
	case 1: //		"First three vertices",
		for(int i=0; i<3 && i<selVtHd.GetN(); ++i)
		{
			selVtHd[i]=NULL;
		}
		break;
	case 2: //		"Last three vertices",
		for(auto i=selVtHd.GetN()-1; 0<=i && selVtHd.GetN()-3<=i; --i)
		{
			selVtHd[i]=NULL;
		}
		break;
	case 6: //		"Parallel to XY passing through the first vertex",
	case 7: //		"Parallel to XZ passing through the first vertex",
	case 8: //		"Parallel to YZ passing through the first vertex",
		if(0<selVtHd.GetN())
		{
			selVtHd[0]=NULL;
		}
	case 9: //		"Parallel to XY passing through the last vertex",
	case 10: //		"Parallel to XZ passing through the last vertex",
	case 11: //		"Parallel to YZ passing through the last vertex"
		if(0<selVtHd.GetN())
		{
			selVtHd.GetCyclic(-1)=NULL;
		}
		break;
	}

	switch(projDirDrp->GetSelection())
	{
	case 0: //"Shortest to the projection plane",
	case 1: //"X-axis",
	case 2: //"Y-axis",
	case 3: //"Z-axis",
		break;
	case 4: //"First two vertices"
		if(0<selVtHd.GetN())
		{
			selVtHd[0]=NULL;
		}
		break;
	case 5: //"Last two vertices",
		if(0<selVtHd.GetN())
		{
			selVtHd.Last()=NULL;
		}
		break;
	default:
		break;
	}

	YsArray <YsShellVertexHandle> targetVtHd;
	for(auto vtHd : selVtHd)
	{
		if(NULL!=vtHd)
		{
			targetVtHd.Append(vtHd);
		}
	}

	return targetVtHd;
}

void PolyCreProjectionDialog::OnDropListSelChange(FsGuiDropList *,int)
{
	RemakeVertexProjectionArray();
	RemakeDrawingBuffer();
}

void PolyCreProjectionDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		if(YSOK==canvas->Edit_Projection_Project())
		{
			canvas->Edit_ClearUIMode();
		}
	}
	else
	{
		canvas->Edit_ClearUIMode();
	}
}

/*static*/ void GeblGuiEditorBase::Edit_Projection(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;

	canvas.Edit_ClearUIMode();

	canvas.undoRedoRequireToClearUIMode=YSTRUE;
	canvas.deletionRequireToClearUIMode=YSTRUE;

	canvas.UIDrawCallBack3D=GeblGuiEditorBase::Edit_Projection_DrawCallBack3d;;
	canvas.SpaceKeyCallBack=GeblGuiEditorBase::Edit_Projection_SpaceKeyCallBack;

	canvas.projectionDlg->Make();
	canvas.AddDialog(canvas.projectionDlg);
	canvas.ArrangeDialog();
	canvas.ArrangeDialog();
}

/*static*/ void GeblGuiEditorBase::Edit_Projection_DrawCallBack3d(GeblGuiEditorBase &canvas)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
	YsGLSLUse3DRenderer(renderer);

	const GLfloat edgeCol[4]={0,0,0,1.0f};
	YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)canvas.projectionDlg->lineBuf.GetNumVertex(),canvas.projectionDlg->lineBuf);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_TRIANGLES,(int)canvas.projectionDlg->triBuf.GetNumVertex(),canvas.projectionDlg->triBuf);

	YsGLSLEndUse3DRenderer(renderer);
}

/*static*/ YSRESULT GeblGuiEditorBase::Edit_Projection_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	if(YSOK==canvas.Edit_Projection_Project())
	{
		canvas.Edit_ClearUIMode();
		return YSOK;
	}
	return YSERR;
}

YSRESULT GeblGuiEditorBase::Edit_Projection_Project(void)
{
	if(NULL!=slHd && YSOK==projectionDlg->RemakeVertexProjectionArray())
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);
		for(auto pair : projectionDlg->vertexPositionPair)
		{
			slHd->SetVertexPosition(pair.a,pair.b);
		}
		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
		return YSOK;
	}
	else
	{
		MessageDialog(L"Error",L"Cannot calculate projection");
	}
	return YSERR;
}

/*static*/ void GeblGuiEditorBase::Edit_TriangulateSelection(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	YSRESULT res=YSOK;

	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	auto shlPtr=canvas.Slhd();
	if(NULL!=shlPtr)
	{
		auto &shl=*shlPtr;
		auto selPlHd=shl.GetSelectedPolygon();

		YsShellExtEdit::StopIncUndo incUndo(canvas.Slhd());

		for(auto plHd : selPlHd)
		{
			YsArray <YsShellVertexHandle,4> plVtHd;
			shl.GetPolygon(plVtHd,plHd);
			if(3<plVtHd.GetN())
			{
				if(YSOK!=YsShellExtEdit_TriangulatePolygon(shl,plHd))
				{
					res=YSERR;
				}
			}
		}
		shl.SelectPolygon(0,NULL);
	}

	canvas.needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	canvas.SetNeedRedraw(YSTRUE);

	if(YSOK!=res)
	{
		canvas.MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_TRIANGULATIONERROR);
	}
}

/*static*/ void GeblGuiEditorBase::Edit_SplitConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.Slhd())
	{
		auto &shl=*canvas.Slhd();
		auto selVtHd=shl.GetSelectedVertex();
		if(0<selVtHd.GetN())
		{
			YsShellExtEdit::StopIncUndo incUndo(shl);
			for(auto vtHd : selVtHd)
			{
				auto vtCeHd=shl.FindConstEdgeFromVertex(vtHd);
				for(auto ceHd : vtCeHd)
				{
					YsShellExtEdit_SplitConstEdge(shl,ceHd,vtHd);
				}
			}
			shl.SelectConstEdge(0,NULL);

			canvas.needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}

/*static*/ void GeblGuiEditorBase::Edit_InvertConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.Slhd())
	{
		auto &shl=*canvas.Slhd();

		auto selCeHd=shl.GetSelectedConstEdge();

		YsShellExtEdit::StopIncUndo incUndo(shl);

		for(auto ceHd : selCeHd)
		{
			YSBOOL isLoop;
			YsArray <YsShellVertexHandle> ceVtHd;
			shl.GetConstEdge(ceVtHd,isLoop,ceHd);
			ceVtHd.Invert();
			shl.ModifyConstEdge(ceHd,ceVtHd,isLoop);
		}

		shl.SelectConstEdge(0,NULL);

		canvas.needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
		canvas.SetNeedRedraw(YSTRUE);
	}
}

/*static*/ void GeblGuiEditorBase::Edit_MergeConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.Slhd())
	{
		auto &shl=*canvas.Slhd();

		auto selCeHd=shl.GetSelectedConstEdge();
		if(1<selCeHd.GetN())
		{
			YsShellExt::ConstEdgeHandle modifiedCeHd[1]={YsShellExtEdit_MergeConstEdge(shl,selCeHd,YSTRUE)};
			if(NULL!=modifiedCeHd[0])
			{
				shl.SelectConstEdge(1,modifiedCeHd);
				canvas.needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
				canvas.SetNeedRedraw(YSTRUE);
			}
			else
			{
				canvas.MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTMAKECONSTEDGECHAIN);
			}
		}
	}
}

void GeblGuiEditorBase::Edit_SewBetweenTwoVertex(FsGuiPopUpMenuItem *)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	if(NULL!=Slhd())
	{
		auto selVtHd=Slhd()->GetSelectedVertex();

		auto &shl=(*Slhd());

		if(2>selVtHd.GetN())
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTATLEAST1VERTEX);
		}
		else
		{
			YsShellExtEdit::StopIncUndo undo(shl);

			YsShellExtEdit_SewingInfo sewingInfo;

			auto res=sewingInfo.MakeInfo(Slhd()->Conv(),selVtHd);
			printf("%s\n",YsResultToStr(res));
			res=sewingInfo.Apply(Slhd()->Conv());

			needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON|NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;

			SetNeedRedraw(YSTRUE);
		}
	}
}


////////////////////////////////////////////////////////////


class PolyCreSetPolygonAlphaDialog : public FsGuiDialog
{
public:
	GeblGuiEditorBase *canvasPtr;
	FsGuiTextBox *alphaValueTxt;
	FsGuiSlider *slider;
	FsGuiButton *okBtn,*cancelBtn;

	void Make(GeblGuiEditorBase *canvasPtr);
	void SetAlpha(const double alpha);
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnTextBoxChange(FsGuiTextBox *);
	virtual void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);

};

void PolyCreSetPolygonAlphaDialog::Make(GeblGuiEditorBase *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	FsGuiDialog::Initialize();

	alphaValueTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"Alpha:",8,YSTRUE);
	alphaValueTxt->SetRealNumber(1.0,6);

	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,L"OK",YSFALSE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,L"Cancel",YSFALSE);

	AddStaticText(0,FSKEY_NULL,FSGUI_DLG_SETALPHA_TRANSPARENT,YSTRUE);
	slider=AddHorizontalSlider(0,FSKEY_NULL,80,0.0,1.0,YSFALSE);
	slider->SetPosition(1.0);
	AddStaticText(0,FSKEY_NULL,FSGUI_DLG_SETALPHA_SOLID,YSFALSE);

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();
}
void PolyCreSetPolygonAlphaDialog::SetAlpha(const double alpha)
{
	if(nullptr!=canvasPtr->Slhd())
	{
		auto &shl=*(canvasPtr->Slhd());
		auto &mod=shl.GetTemporaryModification();
		auto selPlg=shl.GetSelectedPolygon();

		for(auto plHd : selPlg)
		{
			auto col=canvasPtr->Slhd()->GetColor(plHd);
			col.SetAd(alpha);
			mod.SetColor(plHd,col);
		}

		canvasPtr->SetNeedRedraw(YSTRUE);
	}
}
/* virtual */ void PolyCreSetPolygonAlphaDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		auto &shl=*(canvasPtr->Slhd());
		auto &mod=canvasPtr->Slhd()->GetTemporaryModification();

		mod.MakePermanent(shl);

		canvasPtr->Edit_ClearUIMode();
		canvasPtr->RemoveDialog(this);
	}
	else if(cancelBtn==btn)
	{
		canvasPtr->Edit_ClearUIMode();
		canvasPtr->RemoveDialog(this);
	}
}
/* virtual */ void PolyCreSetPolygonAlphaDialog::OnTextBoxChange(FsGuiTextBox *)
{
	const double alpha=alphaValueTxt->GetRealNumber();
	SetAlpha(alpha);
	slider->SetPosition(alpha);
}
/* virtual */ void PolyCreSetPolygonAlphaDialog::OnSliderPositionChange(FsGuiSlider *slider,const double &,const double &)
{
	const double alpha=slider->GetPosition();
	SetAlpha(alpha);
	alphaValueTxt->SetRealNumber(alpha,6);

}


void GeblGuiEditorBase::Edit_SetPolygonAlpha(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <PolyCreSetPolygonAlphaDialog>();
	dlg->Make(this);
	AddDialog(dlg);
	ArrangeDialog();
}
////////////////////////////////////////////////////////////

void GeblGuiEditorBase::Edit_DivideSmoothShadingRegionOnSelectedConstEdge(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		YsShellExtEdit::StopIncUndo undoGuard(shl);

		auto selCeHd=shl.GetSelectedConstEdge();
		for(auto ceHd : selCeHd)
		{
			shl.SetConstEdgeIsCrease(ceHd,YSTRUE);
		}

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}
void GeblGuiEditorBase::Edit_UndivideSmoothShadingRegionOnSelectedConstEdge(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		YsShellExtEdit::StopIncUndo undoGuard(shl);

		auto selCeHd=shl.GetSelectedConstEdge();
		for(auto ceHd : selCeHd)
		{
			shl.SetConstEdgeIsCrease(ceHd,YSFALSE);
		}

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_InvertSelectedPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		YsShellExtEdit::StopIncUndo undoGuard(shl);

		auto selPlHd=shl.GetSelectedPolygon();
		for(auto plHd : selPlHd)
		{
			auto nom=shl.GetNormal(plHd);
			auto plVtHd=shl.GetPolygonVertex(plHd);

			nom=-nom;
			plVtHd.Invert();

			shl.SetPolygonNormal(plHd,nom);
			shl.SetPolygonVertex(plHd,plVtHd);
		}

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}


////////////////////////////////////////////////////////////


class GeblGuiEditorBase::Edit_ScaleSelectionDialog : public FsGuiDialog
{
private:
	GeblGuiEditorBase *canvasPtr;
	FsGuiTextBox *sx,*sy,*sz;
	FsGuiButton *isotropicScalingBtn;
	FsGuiButton *centerAtOriginBtn;
	FsGuiButton *upBtn,*downBtn;
	FsGuiButton *okBtn,*cancelBtn;

	YsGLVertexBuffer vtxBuf;

public:
	YsArray <YSHASHKEY> vtxKey;

	void Make(GeblGuiEditorBase *canvasPtrIn);
	void RemakeVertexBuffer(void);
	YsVec3 CalculateNewPosition(YsShell::VertexHandle vtHd,const YsVec3 cen,const double sx,const double sy,const double sz) const;
	void Draw3d(void);
	void CursorMoved(FsGui3DInterface *);
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnTextBoxChange(FsGuiTextBox *);
};

void GeblGuiEditorBase::Edit_ScaleSelectionDialog::Make(GeblGuiEditorBase *canvasPtrIn)
{
	this->canvasPtr=canvasPtrIn;

	FsGuiDialog::Initialize();

	sx=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"X:",8,YSTRUE);
	sx->SetRealNumber(1.0,6);

	sy=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"Y:",8,YSFALSE);
	sy->SetRealNumber(1.0,6);

	sz=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"Z:",8,YSFALSE);
	sz->SetRealNumber(1.0,6);

	isotropicScalingBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_SCALING_ISOTROPICSCALING,YSTRUE);
	isotropicScalingBtn->SetCheck(YSTRUE);

	centerAtOriginBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_SCALING_CENTERATORIGIN,YSFALSE);
	upBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"x1.05",YSFALSE);
	downBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"x(1/1.05)",YSFALSE);

	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();
}

void GeblGuiEditorBase::Edit_ScaleSelectionDialog::RemakeVertexBuffer(void)
{
	vtxBuf.CleanUp();
	if(nullptr!=canvasPtr->Slhd())
	{
		auto &shl=*(canvasPtr->Slhd());
		auto cen=canvasPtr->threeDInterface.point_pos;
		auto scaleX=sx->GetRealNumber();
		auto scaleY=sy->GetRealNumber();
		auto scaleZ=sz->GetRealNumber();

		for(auto vtKey : vtxKey)
		{
			auto vtHd=shl.FindVertex(vtKey);
			if(nullptr!=vtHd)
			{
				auto vtPos=CalculateNewPosition(vtHd,cen,scaleX,scaleY,scaleZ);
				vtxBuf.Add(vtPos);
			}
		}
	}
}

YsVec3 GeblGuiEditorBase::Edit_ScaleSelectionDialog::CalculateNewPosition(YsShell::VertexHandle vtHd,const YsVec3 cen,const double scaleX,const double scaleY,const double scaleZ) const
{
	auto &shl=*(canvasPtr->Slhd());
	auto vtPos=shl.GetVertexPosition(vtHd);
	vtPos=vtPos-cen;
	vtPos.MulX(scaleX);
	vtPos.MulY(scaleY);
	vtPos.MulZ(scaleZ);
	vtPos+=cen;
	return vtPos;
}

void GeblGuiEditorBase::Edit_ScaleSelectionDialog::Draw3d(void)
{
	auto renderer=YsGLSLSharedMonoColorMarkerByPointSprite3DRenderer();
	YsGLSLUse3DRenderer(renderer);

	GLfloat color[4]={0,0,1,0.9f};

	YsGLSLSet3DRendererUniformMarkerDimension(renderer,10);
	YsGLSLSet3DRendererUniformColorfv(renderer,color);
	YsGLSLSet3DRendererUniformMarkerType(renderer,YSGLSL_MARKER_TYPE_STAR);

#ifdef GL_POINT_SPRITE
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
#endif
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_POINTS,(int)vtxBuf.GetN(),vtxBuf);
#ifdef GL_POINT_SPRITE
	glDisable(GL_POINT_SPRITE);
	glDisable(GL_PROGRAM_POINT_SIZE);
#endif
	YsGLSLEndUse3DRenderer(renderer);
}

void GeblGuiEditorBase::Edit_ScaleSelectionDialog::CursorMoved(FsGui3DInterface *)
{
	RemakeVertexBuffer();
}

/* virtual */ void GeblGuiEditorBase::Edit_ScaleSelectionDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		if(nullptr!=canvasPtr->Slhd())
		{
			auto &shl=*(canvasPtr->Slhd());
			auto cen=canvasPtr->threeDInterface.point_pos;
			auto scaleX=sx->GetRealNumber();
			auto scaleY=sy->GetRealNumber();
			auto scaleZ=sz->GetRealNumber();

			YsShellExtEdit::StopIncUndo undoGuard(shl);
			YsArray <YsShell::VertexHandle> vtHdArray;
			YsArray <YsVec3> posArray;
			for(auto vtKey : vtxKey)
			{
				auto vtHd=shl.FindVertex(vtKey);
				if(nullptr!=vtHd)
				{
					auto vtPos=CalculateNewPosition(vtHd,cen,scaleX,scaleY,scaleZ);
					vtHdArray.Add(vtHd);
					posArray.Add(vtPos);
				}
			}
			shl.SetMultiVertexPosition(vtHdArray,posArray);
			canvasPtr->needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
			canvasPtr->SetNeedRedraw(YSTRUE);
		}

		canvasPtr->Edit_ClearUIMode();
	}
	else if(cancelBtn==btn)
	{
		canvasPtr->Edit_ClearUIMode();
	}
	else if(centerAtOriginBtn==btn)
	{
		canvasPtr->threeDInterface.point_pos=YsOrigin();
		canvasPtr->SetNeedRedraw(YSTRUE);
		RemakeVertexBuffer();
	}
	else if(upBtn==btn)
	{
		const double x=sx->GetRealNumber()*1.05;
		const double y=sy->GetRealNumber()*1.05;
		const double z=sz->GetRealNumber()*1.05;
		sx->SetRealNumber(x,6);
		sy->SetRealNumber(y,6);
		sz->SetRealNumber(z,6);
		RemakeVertexBuffer();
	}
	else if(downBtn==btn)
	{
		const double x=sx->GetRealNumber()/1.05;
		const double y=sy->GetRealNumber()/1.05;
		const double z=sz->GetRealNumber()/1.05;
		sx->SetRealNumber(x,6);
		sy->SetRealNumber(y,6);
		sz->SetRealNumber(z,6);
		RemakeVertexBuffer();
	}
}

/* virtual */ void GeblGuiEditorBase::Edit_ScaleSelectionDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	auto numStr=txt->GetString();
	if(YSTRUE==isotropicScalingBtn->GetCheck())
	{
		if(txt!=sx)
		{
			sx->SetText(numStr);
		}
		if(txt!=sy)
		{
			sy->SetText(numStr);
		}
		if(txt!=sz)
		{
			sz->SetText(numStr);
		}
	}
	RemakeVertexBuffer();
	canvasPtr->SetNeedRedraw(YSTRUE);
}

void GeblGuiEditorBase::Edit_ScaleSelection(FsGuiPopUpMenuItem *)
{
	YsArray <YsShellVertexHandle> allSelVtHd;
	YsShellExtEdit_GetAllVertexInSelection(allSelVtHd,*slHd);

	if(0>=allSelVtHd.GetN())
	{
		MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTATLEAST1VERTEX);
		return;
	}


	Edit_ClearUIMode();

	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <Edit_ScaleSelectionDialog>();
	dlg->Make(this);

	YsVec3 cen;
	slHd->GetCenterOfVertexGroup(cen,allSelVtHd.GetN(),allSelVtHd);

	threeDInterface.BeginInputPoint1(cen);
	threeDInterface.BindCallBack(&Edit_ScaleSelectionDialog::CursorMoved,dlg);

	dlg->vtxKey.Resize(allSelVtHd.GetN());
	for(auto idx : allSelVtHd.AllIndex())
	{
		dlg->vtxKey[idx]=slHd->GetSearchKey(allSelVtHd[idx]);
	}
	dlg->RemakeVertexBuffer();

	AddDialog(dlg);
	ArrangeDialog();

	draw3dCallBack=std::bind(&Edit_ScaleSelectionDialog::Draw3d,dlg);
}



void GeblGuiEditorBase::Edit_ImprintSelection(FsGuiPopUpMenuItem *)
{
printf("%s %d\n",__FUNCTION__,__LINE__);

	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		auto selPlHd=shl.GetSelectedPolygon();
		auto selCeHd=shl.GetSelectedConstEdge();


		YsShellVertexStore allSelVtHd(shl.Conv());
		for(auto plHd : selPlHd)
		{
			allSelVtHd.Add(shl.GetPolygonVertex(plHd));
		}
		for(auto ceHd : selCeHd)
		{
			for(auto vtHd : shl.GetConstEdgeVertex(ceHd))
			{ 
				allSelVtHd.Add(vtHd);
			}
		}
		YsArray <YsVec3> allVtPos;
		for(auto vtHd : allSelVtHd)
		{
			allVtPos.Add(shl.GetVertexPosition(vtHd));
		}
		YsVec3 dir;
		YsFindLeastSquareFittingPlaneNormal(dir,allVtPos);


		YsShellExtEdit::StopIncUndo undoGuard(shl);

		YsShellExt_ImprintingUtil imprintUtil;
		imprintUtil.CleanUp();
		imprintUtil.SetDestinationShell(shl.Conv());
		imprintUtil.SetSourceShell(shl.Conv());

		imprintUtil.Begin();
		imprintUtil.MakeParallelImprintingData(selPlHd,selCeHd,dir);

		imprintUtil.CreateVertex(shl);
		imprintUtil.CreateVertexOnEdge(shl);

		imprintUtil.MakeDstEdgeMidVertexTable();
		imprintUtil.EmbedEdgeVertex(shl);

		imprintUtil.MakeDstVertexConnection();

		auto dstVtxChain=imprintUtil.MakeDstVertexChain(selPlHd,selCeHd);
for(auto &vtxChain : dstVtxChain)
{
	printf("----\n");
	for(auto vtHd : vtxChain.vtxChain)
	{
		printf("[%d]\n",shl.GetSearchKey(vtHd));
	}
}
		imprintUtil.CutDstPolygon(shl,dstVtxChain);

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}

