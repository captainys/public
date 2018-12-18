/* ////////////////////////////////////////////////////////////

File Name: edit_offsetmenu.cpp
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
#include "edit_offsetdialog.h"
#include "../miscdlg.h"
#include <ysgl.h>

#include <ysshellextutil.h>
#include <ysshellext_thickeningutil.h>
#include <ysshellextedit_orientationutil.h>

#include "../textresource.h"


PolyCreEditOffsetThickeningDialog::PolyCreEditOffsetThickeningDialog(class GeblGuiEditorBase *canvas)
{
	this->canvas=canvas;
}

PolyCreEditOffsetThickeningDialog::~PolyCreEditOffsetThickeningDialog()
{
}

void PolyCreEditOffsetThickeningDialog::Make(void)
{
	Initialize();

	okBtn=AddTextButton(0,FSKEY_SPACE,FSGUI_PUSHBUTTON,L"Apply",YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	plusOffset=AddHorizontalSlider(0,FSKEY_NULL,50,0.0,1.0,YSTRUE);
	minusOffset=AddHorizontalSlider(0,FSKEY_NULL,50,0.0,1.0,YSTRUE);

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();
}

/* static */ PolyCreEditOffsetThickeningDialog *PolyCreEditOffsetThickeningDialog::Create(class GeblGuiEditorBase *canvas)
{
	return new PolyCreEditOffsetThickeningDialog(canvas);
}

/* static */ void PolyCreEditOffsetThickeningDialog::Delete(PolyCreEditOffsetThickeningDialog *ptr)
{
	delete ptr;
}

const double PolyCreEditOffsetThickeningDialog::GetPlusThickness(void) const
{
	return plusOffset->GetScaledValue();
}

const double PolyCreEditOffsetThickeningDialog::GetMinusThickness(void) const
{
	return minusOffset->GetScaledValue();
}

/* virtual */ void PolyCreEditOffsetThickeningDialog::OnButtonClick(FsGuiButton *)
{
}

/* virtual */ void PolyCreEditOffsetThickeningDialog::OnSliderPositionChange(FsGuiSlider *,const double &,const double &)
{



	const double plusThickness=GetPlusThickness();
	const double minusThickness=GetMinusThickness();

	auto &shl=*(canvas->Slhd());

	auto &info=*(canvas->thickeningInfo);
	info.MakeInfo(shl.Conv(),selPlHd);
	info.CalculateOppositeVertexPosition(shl.Conv(),plusThickness);
	info.CalculateDisplacedPosition(shl.Conv(),minusThickness);

	canvas->SetNeedRedraw(YSTRUE);
}



////////////////////////////////////////////////////////////

/* static */ void GeblGuiEditorBase::Edit_Offset_Thickening(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;

	if(NULL!=canvas.slHd)
	{
		auto &shl=*(canvas.slHd);

		YsArray <YsShellPolygonHandle> selPlHd;
		YsArray <YsShellExt::FaceGroupHandle> selFgHd;

		shl.GetSelectedFaceGroup(selFgHd);
		shl.GetSelectedPolygon(selPlHd);

		for(auto fgHd : selFgHd)
		{
			auto fgPlHd=shl.GetFaceGroup(fgHd);
			selPlHd.Append(fgPlHd);
		}

		YsRemoveDuplicateInUnorderedArray(selPlHd);

		if(0<selPlHd.GetN())
		{
			canvas.Edit_ClearUIMode();

			canvas.thickeningInfo->MakeInfo(shl.Conv(),selPlHd);

			canvas.UIDrawCallBack3D=Edit_Offset_Thickening_DrawCallBack3D;
			canvas.SpaceKeyCallBack=Edit_Offset_Thickening_SpaceKeyCallBack;

			canvas.undoRedoRequireToClearUIMode=YSTRUE;
			canvas.deletionRequireToClearUIMode=YSTRUE;

			canvas.thickeningDlg->Make();
			canvas.thickeningDlg->selPlHd=selPlHd;

			canvas.AddDialog(canvas.thickeningDlg);
			canvas.ArrangeDialog();
		}
		else
		{
			canvas.MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTONEPLGORCONSTEDGE);
		}
	}
}

/* static */ void GeblGuiEditorBase::Edit_Offset_Thickening_DrawCallBack3D(GeblGuiEditorBase &canvas)
{
	YsGLVertexBuffer vtxBuf;

	auto &info=*canvas.thickeningInfo;
	auto &shl=*canvas.Slhd();
	auto srcVtHdArray=info.GetVertexAll();

	for(auto srcVtHd : srcVtHdArray)
	{
		YsVec3 oppVtPos=info.GetOppositeVertexPosition(shl.Conv(),srcVtHd);
		YsVec3 newVtPos=info.GetDisplacedVertexPosition(shl.Conv(),srcVtHd);
		vtxBuf.AddVertex(oppVtPos);
		vtxBuf.AddVertex(newVtPos);
	}

	struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
	YsGLSLUse3DRenderer(renderer);

	const GLfloat edgeCol[4]={0,1.0f,0,1.0f};
	YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)vtxBuf.GetNumVertex(),vtxBuf);

	YsGLSLEndUse3DRenderer(renderer);
}

/* static */ YSRESULT GeblGuiEditorBase::Edit_Offset_Thickening_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	return canvas.Edit_Offset_Thickening_SpaceKeyCallBack();
}

YSRESULT GeblGuiEditorBase::Edit_Offset_Thickening_SpaceKeyCallBack(void)
{
	auto &shl=*slHd;

	auto selPlHd=thickeningDlg->selPlHd;

	const double plusThickness=thickeningDlg->GetPlusThickness();
	const double minusThickness=thickeningDlg->GetMinusThickness();


	auto &info=*(thickeningInfo);

	info.MakeInfo(shl.Conv(),selPlHd);
	info.CalculateOppositeVertexPosition(shl.Conv(),plusThickness);
	info.CalculateDisplacedPosition(shl.Conv(),minusThickness);

	auto allVtHd=info.GetVertexAll();

	YsShellExtEdit::StopIncUndo stopIncUndo(shl);

	for(auto vtHd : allVtHd)
	{
		YsVec3 oppVtPos=info.GetOppositeVertexPosition(shl.Conv(),vtHd);
		auto newVtHd=shl.AddVertex(oppVtPos);
		info.AddMapping(shl.Conv(),vtHd,newVtHd);

		YsVec3 newVtPos=info.GetDisplacedVertexPosition(shl.Conv(),vtHd);
		shl.SetVertexPosition(vtHd,newVtPos);
	}

	YsArray <YsShellPolygonHandle> newPlHdArray;

	for(auto plHd : selPlHd)
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);
		YSRESULT mappingResult=YSOK;
		for(auto &vtHd : plVtHd)
		{
			YsArray <YsShellVertexHandle> mappedVtHd;
			if(YSOK==info.FindMapping(mappedVtHd,shl.Conv(),vtHd) && 1==mappedVtHd.GetN())
			{
				vtHd=mappedVtHd[0];
			}
			else
			{
				mappingResult=YSERR;
				break;
			}
		}
		if(YSOK==mappingResult)
		{
			newPlHdArray.Append(shl.AddPolygon(plVtHd));
		}
	}

	for(YSSIZE_T edIdx=0; edIdx<info.GetNumEdgePiece(); ++edIdx)
	{
		YsShellVertexHandle edVtHd[2];
		YsArray <YsShellVertexHandle> mappedVtHd[2];
		if(YSOK==info.GetEdge(edVtHd,edIdx)&&
		   YSOK==info.FindMapping(mappedVtHd[0],shl.Conv(),edVtHd[0]) &&
		   YSOK==info.FindMapping(mappedVtHd[1],shl.Conv(),edVtHd[1]) &&
		   1<=mappedVtHd[0].GetN() &&
		   1<=mappedVtHd[1].GetN())
		{
			YsShellVertexHandle quad[4]=
			{
				edVtHd[0],
				edVtHd[1],
				mappedVtHd[1][0],
				mappedVtHd[0][0]
			};
			YsShellPolygonHandle newPlHd=shl.AddPolygon(4,quad);
			newPlHdArray.Append(newPlHd);
		}
	}

	YsShellExtEdit_OrientationUtil::FixOrientationOfClosedSolid(shl);
	// Should re-calculate only affected polygons.  Need fix.
	// YsShellExtEdit_OrientationUtil flipper;
	// flipper.RecalculateNormal(*slHd,newPlHdArray);


	needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_CONSTEDGE|
	                          NEED_REMAKE_DRAWING_SELECTED_POLYGON|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE);
	SetNeedRedraw(YSTRUE);

	Edit_ClearUIMode();

	return YSOK;
}
