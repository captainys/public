/* ////////////////////////////////////////////////////////////

File Name: utilmenu.cpp
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

#include <ysshellextmisc.h>
#include <ysshellext_geomcalc.h>
#include <ysshellext_trackingutil.h>
#include <ysshellextedit_localop.h>
#include <ysshellextedit_constedgeutil.h>
#include <ysshellext_constedgeutil.h>
#include <ysshellextedit_facegrouputil.h>

#include "utildlg.h"


#include "../textresource.h"


/* static */ void GeblGuiEditorBase::UtilMenu_ClearConstEdgeAll(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL==canvas.Slhd())
	{
		return;
	}

	YsShellExtEdit &shl=*(canvas.Slhd());
	YsShellExtEdit::StopIncUndo incUndo(shl);

	for(auto ceHd : shl.AllConstEdge())
	{
		shl.DeleteConstEdge(ceHd);
	}

	canvas.needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE);
	canvas.SetNeedRedraw(YSTRUE);
	if(YSTRUE==canvas.deletionRequireToClearUIMode)
	{
		canvas.Edit_ClearUIMode();
	}
}



class GeblGuiEditorBase::SetConstEdgeNameDialog : public FsGuiDialog
{
private:
	GeblGuiEditorBase *canvasPtr;
	FsGuiTextBox *labelTxt;
	FsGuiButton *okBtn,*cancelBtn;

public:
	void Make(GeblGuiEditorBase *canvasPtr,const char defName[]);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void GeblGuiEditorBase::SetConstEdgeNameDialog::Make(GeblGuiEditorBase *canvasPtr,const char defName[])
{
	this->canvasPtr=canvasPtr;

	labelTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::VERTICAL,L"Const Edge Name",L"",16,YSFALSE);
	okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	labelTxt->SetText(defName);

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();
}

/* virtual */ void GeblGuiEditorBase::SetConstEdgeNameDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		YsString txt;
		labelTxt->GetText(txt);
		auto &shl=*(canvasPtr->Slhd());
		auto selCeHd=shl.GetSelectedConstEdge();

		for(auto ceHd :selCeHd)
		{
			shl.SetConstEdgeLabel(ceHd,txt);
		}

		CloseModalDialog(YSOK);
	}
	else if(btn==cancelBtn)
	{
		CloseModalDialog(YSERR);
	}
}

void GeblGuiEditorBase::UtilMenu_SetConstEdgeName(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		auto selCeHd=shl.GetSelectedConstEdge();
		if(0==selCeHd)
		{
			MessageDialog(FSGUI_ERROR_NEEDSELECTCONSTEDGE,FSGUI_COMMON_OK);
			return;
		}

		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<SetConstEdgeNameDialog>();
		dlg->Make(this,shl.GetConstEdgeLabel(selCeHd[0]));

		AttachModalDialog(dlg);
		ArrangeDialog();
	}
}



/* static */ void GeblGuiEditorBase::UtilMenu_ConstrainAllEdges(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL==canvas.Slhd())
	{
		return;
	}

	YsShellExtEdit &shl=*(canvas.Slhd());
	YsShellExtEdit::StopIncUndo incUndo(shl);

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		YsShellVertexHandle edVtHd[2];
		shl.GetEdge(edVtHd,edHd);

		if(0==shl.GetNumConstEdgeUsingEdgePiece(edVtHd))
		{
			shl.AddConstEdge(2,edVtHd,YSFALSE);
		}
	}

	canvas.needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE);
	canvas.SetNeedRedraw(YSTRUE);
	if(YSTRUE==canvas.deletionRequireToClearUIMode)
	{
		canvas.Edit_ClearUIMode();
	}
}



////////////////////////////////////////////////////////////



PolyCreAngleSliderDialog::PolyCreAngleSliderDialog()
{
	canvas=NULL;
	okBtn=NULL;
	cancelBtn=NULL;
}

PolyCreAngleSliderDialog::~PolyCreAngleSliderDialog()
{
}

/*static*/ PolyCreAngleSliderDialog *PolyCreAngleSliderDialog::Create(class GeblGuiEditorBase *canvas)
{
	PolyCreAngleSliderDialog *dlg=new PolyCreAngleSliderDialog;
	dlg->canvas=canvas;
	return dlg;
}

/*static*/ void PolyCreAngleSliderDialog::Delete(PolyCreAngleSliderDialog *ptr)
{
	delete ptr;
}

YSRESULT PolyCreAngleSliderDialog::Make(void)
{
	if(NULL==okBtn)
	{
		FsGuiDialog::Initialize();

		slider=AddHorizontalSlider(0,FSKEY_NULL,40,0.0,180.0,YSTRUE);
		slider->SetPositionByScaledValue(60.0);
		angleTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_COMMON_ANGLE,L"60",3,YSFALSE);

		okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(0.3);
		Fit();

		RemakeDrawingBuffer();
	}

	return YSOK;
}

void PolyCreAngleSliderDialog::RemakeDrawingBuffer(void)
{
	lineBuf.CleanUp();

	YsShellExtEdit &shl=*(canvas->Slhd());

	const double angleThr=angleTxt->GetRealNumber();

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		YsShellVertexHandle edVtHd[2];
		shl.GetEdge(edVtHd,edHd);

		const double angleDeg=YsRadToDeg(YsShellExt_CalculateDihedralAngle(shl.Conv(),edVtHd));
		if(angleThr<=angleDeg)
		{
			lineBuf.AddVertex(shl.GetVertexPosition(edVtHd[0]));
			lineBuf.AddVertex(shl.GetVertexPosition(edVtHd[1]));
		}
	}
}

void PolyCreAngleSliderDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		(*canvas->SpaceKeyCallBack)(*canvas);
		canvas->Edit_ClearUIMode();
	}
	else if(btn==cancelBtn)
	{
		canvas->Edit_ClearUIMode();
	}
}

void PolyCreAngleSliderDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	if(txt==angleTxt)
	{
		const double degree=txt->GetRealNumber();
		slider->SetPositionByScaledValue(degree);
		RemakeDrawingBuffer();
		canvas->SetNeedRedraw(YSTRUE);
	}
}

void PolyCreAngleSliderDialog::OnSliderPositionChange(FsGuiSlider *slider,const double &,const double &)
{
	if(slider==this->slider)
	{
		const double degree=slider->GetScaledValue();
		YsString str;
		str.Printf("%.1lf",degree);
		angleTxt->SetText(str);
		RemakeDrawingBuffer();
		canvas->SetNeedRedraw(YSTRUE);
	}
}



/*static*/ void GeblGuiEditorBase::UtilMenu_AddConstEdgeByDihedralAngle(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL==canvas.Slhd())
	{
		return;
	}

	
	{
		if(YSOK==canvas.angleSliderDlg->Make())
		{
			canvas.Edit_ClearUIMode();

			canvas.AddDialog(canvas.angleSliderDlg);
			canvas.ArrangeDialog();

			canvas.UIDrawCallBack3D=UtilMenu_AddConstEdgeByDihedralAngle_DrawCallBack3D;
			canvas.SpaceKeyCallBack=UtilMenu_AddConstEdgeByDihedralAngle_SpaceKeyCallBack;

			canvas.undoRedoRequireToClearUIMode=YSFALSE;
			canvas.deletionRequireToClearUIMode=YSFALSE;

			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}

/*static*/ void GeblGuiEditorBase::UtilMenu_AddConstEdgeByDihedralAngle_DrawCallBack3D(GeblGuiEditorBase &canvas)
{
	if(NULL==canvas.Slhd())
	{
		return;
	}

	auto renderer=YsGLSLSharedFlat3DRenderer();
	YsGLSLUse3DRenderer(renderer);

	auto &lineBuf=canvas.angleSliderDlg->lineBuf;

	glLineWidth(3);

	const GLfloat edgeCol[4]={1,0.3f,0.3f,1};
	YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)lineBuf.GetNumVertex(),lineBuf);

	glLineWidth(1);

	YsGLSLEndUse3DRenderer(renderer);
}

/*static*/ YSRESULT GeblGuiEditorBase::UtilMenu_AddConstEdgeByDihedralAngle_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	return canvas.UtilMenu_AddConstEdgeByDihedralAngle_SpaceKeyCallBack();
}

YSRESULT GeblGuiEditorBase::UtilMenu_AddConstEdgeByDihedralAngle_SpaceKeyCallBack(void)
{
	if(NULL==Slhd())
	{
		return YSERR;
	}
	YsShellExtEdit &shl=*Slhd();



	YsShellExtEdit::StopIncUndo incUndo(shl);

	const double angleThr=YsDegToRad(angleSliderDlg->angleTxt->GetRealNumber());

	YsShellExtEdit_AddConstEdgeByHighDihedralAngleThreshold(shl,angleThr);

	needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_CONSTEDGE;
	SetNeedRedraw(YSTRUE);

	Edit_ClearUIMode();

	return YSOK;
}


/* static */ void GeblGuiEditorBase::UtilMenu_AddConstEdgeAlongNonManifoldEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;

	if(NULL==canvas.Slhd())
	{
		return;
	}
	YsShellExtEdit &shl=*canvas.Slhd();

	YsShellExtEdit_AddConstEdgeAlongNonManifoldEdge(shl);

	canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_CONSTEDGE;
	canvas.SetNeedRedraw(YSTRUE);

	canvas.Edit_ClearUIMode();
}


/* static */ void GeblGuiEditorBase::UtilMenu_AddConstEdgeOnFaceGroupBoundary(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;

	if(NULL==canvas.Slhd())
	{
		return;
	}
	YsShellExtEdit &shl=*canvas.Slhd();

	YsShellExtEdit_AddConstEdgeAlongFaceGroupBoundary(shl);

	canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_CONSTEDGE;
	canvas.SetNeedRedraw(YSTRUE);

	canvas.Edit_ClearUIMode();
}



/* static */ void GeblGuiEditorBase::UtilMenu_ClearFaceGroupAll(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL==canvas.Slhd())
	{
		return;
	}

	YsShellExtEdit &shl=*(canvas.Slhd());
	YsShellExtEdit::StopIncUndo incUndo(shl);

	for(auto fgHd : shl.AllFaceGroup())
	{
		shl.DeleteFaceGroup(fgHd);
	}

	canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
	canvas.SetNeedRedraw(YSTRUE);
	if(YSTRUE==canvas.deletionRequireToClearUIMode)
	{
		canvas.Edit_ClearUIMode();
	}
}



/* static */ void GeblGuiEditorBase::UtilMenu_MakeFaceGroupFromConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL==canvas.Slhd())
	{
		return;
	}

	YsShellExtEdit &shl=*(canvas.Slhd());
	YsShellExtEdit_MakeFaceGroupByConstEdge(shl);

	canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
	canvas.SetNeedRedraw(YSTRUE);
	canvas.Edit_ClearUIMode();
}


void GeblGuiEditorBase::UtilMenu_PaintBasedOnFaceGroup(FsGuiPopUpMenuItem *)
{
	if(NULL==Slhd())
	{
		return;
	}

	YsShellExtEdit &shl=*Slhd();
	YsShellExtEdit_PaintFaceGroupDifferentColor(shl);
	needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_POLYGON;
	SetNeedRedraw(YSTRUE);
}

void GeblGuiEditorBase::UtilMenu_ExpandCurrentFaceGroupToUnassignedPolygon(FsGuiPopUpMenuItem *)
{
	if(NULL==Slhd())
	{
		return;
	}

	YsShellExtEdit &shl=*Slhd();

	YsShellExtEdit::StopIncUndo undoGuard(shl);

printf("%s %d\n",__FUNCTION__,__LINE__);
	YsArray <YsShell::PolygonHandle> todo;
	for(auto plHd : shl.AllPolygon())
	{
		if(nullptr==shl.FindFaceGroupFromPolygon(plHd))
		{
			auto nEdge=shl.GetPolygonNumVertex(plHd);
			for(decltype(nEdge) edIdx=0; edIdx<nEdge; ++edIdx)
			{
				if(YSTRUE!=shl.IsEdgePieceConstrained(plHd,edIdx))
				{
					auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
					if(nullptr!=neiPlHd)
					{
						auto neiFgHd=shl.FindFaceGroupFromPolygon(neiPlHd);
						if(nullptr!=neiFgHd)
						{
							shl.AddFaceGroupPolygon(neiFgHd,1,&plHd);
							todo.Add(plHd);
							break;
						}
					}
				}
			}
		}
	}
printf("%s %d\n",__FUNCTION__,__LINE__);

	while(0<todo.GetN())
	{
		auto plHd=todo.Last();
		todo.DeleteLast();

		auto fgHd=shl.FindFaceGroupFromPolygon(plHd);

		auto nEdge=shl.GetPolygonNumVertex(plHd);
		for(decltype(nEdge) edIdx=0; edIdx<nEdge; ++edIdx)
		{
			if(YSTRUE!=shl.IsEdgePieceConstrained(plHd,edIdx))
			{
				auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
				if(nullptr!=neiPlHd && nullptr==shl.FindFaceGroupFromPolygon(neiPlHd))
				{
					shl.AddFaceGroupPolygon(fgHd,1,&neiPlHd);
					todo.Add(neiPlHd);
				}
			}
		}
	}
printf("%s %d\n",__FUNCTION__,__LINE__);

	needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
	SetNeedRedraw(YSTRUE);
}

////////////////////////////////////////////////////////////

PolyCreSetFaceGroupNameDialog::PolyCreSetFaceGroupNameDialog()
{
	labelTxt=NULL;
	mbox=new FsGuiMessageBoxDialog;
}
PolyCreSetFaceGroupNameDialog::~PolyCreSetFaceGroupNameDialog()
{
	if(NULL!=mbox)
	{
		delete mbox;
		mbox=NULL;
	}
}
/* static */ PolyCreSetFaceGroupNameDialog *PolyCreSetFaceGroupNameDialog::Create(class GeblGuiEditorBase *canvas)
{
	auto dlg=new PolyCreSetFaceGroupNameDialog;
	dlg->canvas=canvas;
	return dlg;
}
/* static */ void PolyCreSetFaceGroupNameDialog::Delete(PolyCreSetFaceGroupNameDialog *dlg)
{
	delete dlg;
}
void PolyCreSetFaceGroupNameDialog::Make(void)
{
	if(NULL==labelTxt)
	{
		labelTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::VERTICAL,FSGUI_DLG_FACEGROUPONAME_NAME,L"",16,YSFALSE);
		okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(0.3);
		Fit();
	}
	auto slHd=canvas->Slhd();
	if(NULL!=slHd)
	{
		auto selFgHd=slHd->GetSelectedFaceGroup();
		if(0<selFgHd.GetN())
		{
			auto label=slHd->GetFaceGroupLabel(selFgHd[0]);
			labelTxt->SetText(label);
		}
	}
}

/* virtual */ void PolyCreSetFaceGroupNameDialog::OnTextBoxSpecialKey(FsGuiTextBox *,int fskey)
{
	if(FSKEY_ENTER==fskey && YSOK==SetLabel())
	{
		CloseModalDialog(YSOK);
	}
}

/* virtual */ void PolyCreSetFaceGroupNameDialog::OnButtonClick(FsGuiButton *btn)
{
	if(cancelBtn==btn)
	{
		CloseModalDialog(YSERR);
	}
	else if(okBtn==btn && YSOK==SetLabel())
	{
		CloseModalDialog(YSOK);
	}
}

YSRESULT PolyCreSetFaceGroupNameDialog::SetLabel(void)
{
	auto slHd=canvas->Slhd();
	if(NULL!=slHd)
	{
		YsString label;
		labelTxt->GetText(label);

		auto selFgHd=slHd->GetSelectedFaceGroup();
		for(auto fgHd : selFgHd)
		{
			slHd->SetFaceGroupLabel(fgHd,label);
		}
		return YSOK;
	}
	return YSERR;
}

/* static */ void GeblGuiEditorBase::UtilMenu_FaceGroup_SetName(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	auto slHd=canvas.Slhd();
	if(NULL!=slHd && 0<slHd->GetSelectedFaceGroup().GetN())
	{
		canvas.faceNameDlg->Make();
		canvas.AttachModalDialog(canvas.faceNameDlg);
		canvas.ArrangeDialog();
	}
	else
	{
		// At least one face group must be selected.
	}
}

/* static */ void GeblGuiEditorBase::UtilMenu_OpenCloseConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	auto slHd=canvas.Slhd();
	if(NULL!=slHd && 0<slHd->GetSelectedConstEdge().GetN())
	{
		auto &shl=*slHd;
		auto selCeHd=shl.GetSelectedConstEdge();

		YsShellExtEdit::StopIncUndo incUndo(shl);

		for(auto ceHd : selCeHd)
		{
			auto isLoop=shl.IsConstEdgeLoop(ceHd);
			YsFlip(isLoop);
			shl.SetConstEdgeIsLoop(ceHd,isLoop);
		}

		canvas.needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE);
		canvas.SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::UtilMenu_ReconsiderConstEdge(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		YsShellExtEdit_ReconsiderConstEdge(*Slhd());
		needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE);
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::UtilMenu_AddConstEdgeAlongColorBoundary(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		YsShellExtEdit &shl=*Slhd();

		YsShellExtEdit::StopIncUndo incUndo(shl);

		YsShellExt_AddConstEdgeAlongColorBoundary(shl);

		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_CONSTEDGE;
		SetNeedRedraw(YSTRUE);
	}
}
