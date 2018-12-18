/* ////////////////////////////////////////////////////////////

File Name: edit_roundmenu.cpp
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

#include <ysshellextedit_roundutil.h>
#include <ysshellextedit_offsetutil.h>
#include <ysshellextedit_topologyutil.h>
#include <ysshellextedit_localop.h>
#include <ysshellext_trackingutil.h>
#include "edit_rounddialog.h"

#include "../textresource.h"

PolyCreRoundDialog::PolyCreRoundDialog()
{
	okBtn=NULL;
	cancelBtn=NULL;
	slider=NULL;
	subdivTxt=NULL;
}

PolyCreRoundDialog::~PolyCreRoundDialog()
{
}

/*static*/ PolyCreRoundDialog *PolyCreRoundDialog::Create(class GeblGuiEditorBase *canvas)
{
	PolyCreRoundDialog *newDlg=new PolyCreRoundDialog;
	newDlg->canvas=canvas;
	return newDlg;
}

/*static*/ void PolyCreRoundDialog::Delete(PolyCreRoundDialog *ptr)
{
	delete ptr;
}


YSRESULT PolyCreRoundDialog::Make(const YsShell *shl)
{
	this->shl=shl;

	if(NULL==okBtn)
	{
		Initialize();

		okBtn=AddTextButton(0,FSKEY_SPACE,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

		slider=AddHorizontalSlider(0,FSKEY_NULL,50,0.0,1.0,YSTRUE);

		subdivTxt=AddTextBox(0,FSKEY_NULL,FSGUI_DLG_COMMON_DIVCOUNT,L"2",5,YSTRUE);
		radiusTxt=AddTextBox(0,FSKEY_NULL,L"Radius",L"0.4",5,YSTRUE);
	}

	slider->SetPosition(0.4);

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();

	canvas->ArrangeDialog();

	return YSOK;
}

void PolyCreRoundDialog::UpdateDrawingBuffer(const double r)
{
	const int nDiv=YsBound(subdivTxt->GetInteger(),1,256);
	if(YSTRUE==mode3D)
	{
		roundUtil3d.CalculateRoundingAll(*shl,r);
		roundUtil3d.SubdivideByNumberOfSubdivision(nDiv);
	}
	else
	{
		roundUtil2d.CalculateRoundingAll(*shl,r,nDiv);
	}
}

void PolyCreRoundDialog::SetWidgetForMaxRadius(void)
{
	const double radius=maxRadius*slider->GetPosition();
	radiusTxt->SetRealNumber(radius,2);
}

void PolyCreRoundDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		if(YSOK==canvas->Edit_Round_SpaceKeyCallBack())
		{
			canvas->Edit_ClearUIMode();
		}
	}
	else if(btn==cancelBtn)
	{
		canvas->Edit_ClearUIMode();
	}
}

/* virtual */ void PolyCreRoundDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	if(txt==radiusTxt)
	{
		const double radius=txt->GetRealNumber();
		const double t=radius/maxRadius;
		slider->SetPosition(t);
		UpdateDrawingBuffer(radius);
	}
}

void PolyCreRoundDialog::OnSliderPositionChange(FsGuiSlider *slider,const double &,const double &)
{
	const double radius=maxRadius*slider->GetPosition();

	radiusTxt->SetRealNumber(radius,2);
	UpdateDrawingBuffer(radius);
}


////////////////////////////////////////////////////////////

/*static*/ void GeblGuiEditorBase::Edit_Round_PolygonOrConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase *canvas=(GeblGuiEditorBase *)appPtr;
	canvas->Edit_Round_PolygonOrConstEdge(); // Test Implementation
}

void GeblGuiEditorBase::Edit_Round_PolygonOrConstEdge(void)
{
	if(NULL!=slHd)
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);

		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);

		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);

		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		slHd->GetSelectedConstEdge(selCeHd);

		roundDlg->roundUtil2d.CleanUp();

		for(auto plHd : selPlHd)
		{
			roundDlg->roundUtil2d.SetUpRoundPolygon(*(const YsShell *)slHd,plHd,NULL);
		}

		for(auto ceHd : selCeHd)
		{
			roundDlg->roundUtil2d.SetUpRoundConstEdge(*(const YsShellExt *)slHd,ceHd,NULL);
		}

		roundDlg->Make((const YsShell *)slHd);

		const double maxRadius=roundDlg->roundUtil2d.CalculateMaximumRadius(*(const YsShell *)slHd);
		if(YsTolerance<maxRadius)
		{
			Edit_ClearUIMode();

			roundDlg->mode3D=YSFALSE;
			roundDlg->maxRadius=maxRadius;
			roundDlg->SetWidgetForMaxRadius();

			const double radius=maxRadius*roundDlg->slider->GetPosition();
			roundDlg->roundUtil2d.CalculateRoundingAll(*(const YsShell *)slHd,radius,4);

			AddDialog(roundDlg);
			ArrangeDialog();

			undoRedoRequireToClearUIMode=YSTRUE;
			deletionRequireToClearUIMode=YSTRUE;

			UIDrawCallBack3D=GeblGuiEditorBase::Edit_Round_DrawCallBack2d;
			SpaceKeyCallBack=GeblGuiEditorBase::Edit_Round_SpaceKeyCallBack;

			return;
		}
	}
}



/*static*/ void GeblGuiEditorBase::Edit_Round_PolygonOrConstEdgeSelectedCorner(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase *canvas=(GeblGuiEditorBase *)appPtr;
	canvas->Edit_Round_PolygonOrConstEdgeSelectedCorner(); // Test Implementation
}

void GeblGuiEditorBase::Edit_Round_PolygonOrConstEdgeSelectedCorner(void)
{
	if(NULL!=slHd)
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);

		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);

		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);

		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		slHd->GetSelectedConstEdge(selCeHd);

		YsShellVertexStore vtHdToRound(*(const YsShell *)slHd);
		for(auto vtHd : selVtHd)
		{
			vtHdToRound.AddVertex(vtHd);
		}

		roundDlg->roundUtil2d.CleanUp();

		for(auto plHd : selPlHd)
		{
			roundDlg->roundUtil2d.SetUpRoundPolygon(*(const YsShell *)slHd,plHd,&vtHdToRound);
		}

		for(auto ceHd : selCeHd)
		{
			roundDlg->roundUtil2d.SetUpRoundConstEdge(*(const YsShellExt *)slHd,ceHd,&vtHdToRound);
		}

		roundDlg->Make((const YsShell *)slHd);

		const double maxRadius=roundDlg->roundUtil2d.CalculateMaximumRadius(*(const YsShell *)slHd);
		if(YsTolerance<maxRadius)
		{
			Edit_ClearUIMode();

			roundDlg->mode3D=YSFALSE;
			roundDlg->maxRadius=maxRadius;
			roundDlg->SetWidgetForMaxRadius();

			const double radius=maxRadius*roundDlg->slider->GetPosition();
			roundDlg->roundUtil2d.CalculateRoundingAll(*(const YsShell *)slHd,radius,4);

			AddDialog(roundDlg);
			ArrangeDialog();

			undoRedoRequireToClearUIMode=YSTRUE;
			deletionRequireToClearUIMode=YSTRUE;

			UIDrawCallBack3D=GeblGuiEditorBase::Edit_Round_DrawCallBack2d;
			SpaceKeyCallBack=GeblGuiEditorBase::Edit_Round_SpaceKeyCallBack;

			return;
		}
	}
}

/*static*/ void GeblGuiEditorBase::Edit_Round_DrawCallBack2d(GeblGuiEditorBase &canvas)
{
	canvas.Edit_Round_DrawCallBack2d();
}

void GeblGuiEditorBase::Edit_Round_DrawCallBack2d(void)
{
	if(NULL!=slHd)
	{
		YsGLVertexBuffer vtxBuf;
		for(auto corner : roundDlg->roundUtil2d.cornerArray)
		{
			for(int side=0; side<2; ++side)
			{
				for(YSSIZE_T idx=0; idx<corner.subDiv[side].GetN()-1; ++idx)
				{
					vtxBuf.AddVertex(corner.subDiv[side][idx].pos);
					vtxBuf.AddVertex(corner.subDiv[side][idx+1].pos);
				}
				vtxBuf.AddVertex(corner.subDiv[side].Last().pos);
				vtxBuf.AddVertex(corner.roundedCornerPos);
			}
		}

		struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
		YsGLSLUse3DRenderer(renderer);

		const GLfloat edgeCol[4]={0,0,0,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)vtxBuf.GetNumVertex(),vtxBuf);

		YsGLSLEndUse3DRenderer(renderer);
	}}

YSRESULT GeblGuiEditorBase::Edit_Round2d(void)
{
	const int nDiv=roundDlg->subdivTxt->GetInteger();
	const double radius=roundDlg->slider->GetPosition()*roundDlg->maxRadius;

	if(YSOK!=roundDlg->roundUtil2d.CalculateRoundingAll(*(const YsShell *)slHd,radius,nDiv))
	{
		return YSERR;
	}

	YsShellExtEdit::StopIncUndo stopIncUndo(slHd);

	for(auto plKey : roundDlg->roundUtil2d.targetPlKeyArray)
	{
		const YsShellPolygonHandle plHd=slHd->FindPolygon(plKey);
		if(NULL==plHd)
		{
			continue;
		}

		YsArray <YsShellVertexHandle> plVtHd;
		slHd->GetPolygon(plVtHd,plHd);

		YsArray <YsShellExt_RoundUtil::VertexPositionPair> rounded=roundDlg->roundUtil2d.MakeRoundedVertexSequence(*(const YsShell *)slHd,plVtHd,YSTRUE);

		plVtHd.CleanUp();
		for(YSSIZE_T idx=0; idx<rounded.GetN(); ++idx)
		{
			if(NULL!=rounded[idx].vtHd)
			{
				plVtHd.Append(rounded[idx].vtHd);
				slHd->SetVertexPosition(rounded[idx].vtHd,rounded[idx].pos);
			}
			else
			{
				YsShellVertexHandle vtHd=slHd->AddVertex(rounded[idx].pos);
				plVtHd.Append(vtHd);
			}
		}

		slHd->SetPolygonVertex(plHd,plVtHd.GetN(),plVtHd);
	}
	for(auto ceKey : roundDlg->roundUtil2d.targetCeKeyArray)
	{
		const YsShellExt::ConstEdgeHandle ceHd=slHd->FindConstEdge(ceKey);
		if(NULL==ceHd)
		{
			continue;
		}

		YsArray <YsShellVertexHandle> ceVtHd;
		YSBOOL isLoop;
		slHd->GetConstEdge(ceVtHd,isLoop,ceHd);

		YsArray <YsShellExt_RoundUtil::VertexPositionPair> rounded=roundDlg->roundUtil2d.MakeRoundedVertexSequence(*(const YsShell *)slHd,ceVtHd,isLoop);

		ceVtHd.CleanUp();
		for(YSSIZE_T idx=0; idx<rounded.GetN(); ++idx)
		{
			if(NULL!=rounded[idx].vtHd)
			{
				ceVtHd.Append(rounded[idx].vtHd);
				slHd->SetVertexPosition(rounded[idx].vtHd,rounded[idx].pos);
			}
			else
			{
				YsShellVertexHandle vtHd=slHd->AddVertex(rounded[idx].pos);
				ceVtHd.Append(vtHd);
			}
		}

		slHd->ModifyConstEdge(ceHd,ceVtHd,isLoop);
	}

	needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_CONSTEDGE|
	                          NEED_REMAKE_DRAWING_SELECTED_VERTEX|
	                          NEED_REMAKE_DRAWING_SELECTED_POLYGON|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE);
	SetNeedRedraw(YSTRUE);

	return YSOK;
}

////////////////////////////////////////////////////////////

/*static*/ void GeblGuiEditorBase::Edit_Round_AroundVertex(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase *canvas=(GeblGuiEditorBase *)appPtr;

	if(NULL!=canvas->slHd)
	{
		YsArray <YsShellVertexHandle> selVtHd;
		canvas->slHd->GetSelectedVertex(selVtHd);

		canvas->roundDlg->Make((const YsShell *)canvas->slHd);
		if(YSOK==canvas->roundDlg->roundUtil3d.SetUpRoundVertex(*(const YsShell *)canvas->slHd,selVtHd))
		{
			const double maxRadius=canvas->roundDlg->roundUtil3d.CalculateMaximumRadius(*(const YsShell *)canvas->slHd);

			if(YsTolerance<maxRadius)
			{
				canvas->Edit_ClearUIMode();

				canvas->roundDlg->mode3D=YSTRUE;
				canvas->roundDlg->maxRadius=maxRadius;
				canvas->roundDlg->SetWidgetForMaxRadius();

				const double radius=maxRadius*canvas->roundDlg->slider->GetPosition();
				canvas->roundDlg->roundUtil3d.CalculateRoundingAll(*(const YsShell *)canvas->slHd,radius);
				canvas->roundDlg->roundUtil3d.SubdivideByNumberOfSubdivision(4);

				canvas->AddDialog(canvas->roundDlg);
				canvas->ArrangeDialog();

				canvas->undoRedoRequireToClearUIMode=YSTRUE;
				canvas->deletionRequireToClearUIMode=YSTRUE;

				canvas->UIDrawCallBack3D=GeblGuiEditorBase::Edit_Round_DrawCallBack3d;
				canvas->SpaceKeyCallBack=GeblGuiEditorBase::Edit_Round_SpaceKeyCallBack;

				return;
			}
		}
	}

	canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTROUNDVTX);
}

/*static*/ void GeblGuiEditorBase::Edit_Round_AroundSelectedPolygon(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase *canvas=(GeblGuiEditorBase *)appPtr;

	YsArray <YsShellPolygonHandle> selPlHd;
	canvas->slHd->GetSelectedPolygon(selPlHd);

	canvas->roundDlg->Make((const YsShell *)canvas->slHd);
	if(YSOK==canvas->roundDlg->roundUtil3d.SetUpForAroundPolygonGroup(*(const YsShellExt *)canvas->slHd,selPlHd.GetN(),selPlHd))
	{
		const double maxRadius=canvas->roundDlg->roundUtil3d.CalculateMaximumRadius(*(const YsShell *)canvas->slHd);

		if(YsTolerance<maxRadius)
		{
			canvas->Edit_ClearUIMode();

			canvas->roundDlg->mode3D=YSTRUE;
			canvas->roundDlg->maxRadius=maxRadius;
			canvas->roundDlg->SetWidgetForMaxRadius();

			const double radius=maxRadius*canvas->roundDlg->slider->GetPosition();
			canvas->roundDlg->roundUtil3d.CalculateRoundingAll(*(const YsShell *)canvas->slHd,radius);
			canvas->roundDlg->roundUtil3d.SubdivideByNumberOfSubdivision(4);

			canvas->AddDialog(canvas->roundDlg);
			canvas->ArrangeDialog();

			canvas->undoRedoRequireToClearUIMode=YSTRUE;
			canvas->deletionRequireToClearUIMode=YSTRUE;

			canvas->UIDrawCallBack3D=GeblGuiEditorBase::Edit_Round_DrawCallBack3d;
			canvas->SpaceKeyCallBack=GeblGuiEditorBase::Edit_Round_SpaceKeyCallBack;

			return;
		}
	}

	canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTROUNDPLGBOUNDARY);
}

/*static*/ void GeblGuiEditorBase::Edit_Round_OpenVertexSequence(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase *canvas=(GeblGuiEditorBase *)appPtr;

	auto selVtHd=canvas->slHd->GetSelectedVertex();
	auto selCeHd=canvas->slHd->GetSelectedConstEdge();

	if(0<selVtHd.GetN() && 0<selCeHd.GetN())
	{
		canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTVTXORCONSTEDGE);
		return;
	}

	if(0<selCeHd.GetN())
	{
		selVtHd=YsShellExt_TrackingUtil::MakeVertexSequenceFromMultiConstEdge(canvas->slHd->Conv(),selCeHd);
		if(0==selVtHd.GetN())
		{
			canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTMAKECONSTEDGECHAIN);
			return;
		}
	}

	canvas->roundDlg->Make((const YsShell *)canvas->slHd);
	canvas->roundDlg->roundUtil3d.SetAlwaysUseOffset(YSTRUE); // 2014/10/16
	if(YSOK==canvas->roundDlg->roundUtil3d.SetUpForVertexSequence(canvas->slHd->Conv(),selVtHd))
	{
		const double maxRadius=canvas->roundDlg->roundUtil3d.CalculateMaximumRadius(canvas->slHd->Conv());

		printf("maxRadius %lf\n",maxRadius);

		if(YsTolerance<maxRadius)
		{
			canvas->Edit_ClearUIMode();

			canvas->roundDlg->mode3D=YSTRUE;
			canvas->roundDlg->maxRadius=maxRadius;
			canvas->roundDlg->SetWidgetForMaxRadius();

			const double radius=maxRadius*canvas->roundDlg->slider->GetPosition();
			canvas->roundDlg->roundUtil3d.CalculateRoundingAll(canvas->slHd->Conv(),radius);
			canvas->roundDlg->roundUtil3d.SubdivideByNumberOfSubdivision(4);

			canvas->AddDialog(canvas->roundDlg);
			canvas->ArrangeDialog();

			canvas->undoRedoRequireToClearUIMode=YSTRUE;
			canvas->deletionRequireToClearUIMode=YSTRUE;

			canvas->UIDrawCallBack3D=GeblGuiEditorBase::Edit_Round_DrawCallBack3d;
			canvas->SpaceKeyCallBack=GeblGuiEditorBase::Edit_Round_SpaceKeyCallBack;
		}
		else
		{
			canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTROUNDPLGBOUNDARY);
		}
	}
}

/*static*/ void GeblGuiEditorBase::Edit_Round_DrawCallBack3d(GeblGuiEditorBase &canvas)
{
	canvas.Edit_Round_DrawCallBack3d();
}

void GeblGuiEditorBase::Edit_Round_DrawCallBack3d(void)
{
	if(NULL!=slHd)
	{
		YsGLVertexBuffer vtxBuf;
		for(auto corner : roundDlg->roundUtil3d.cornerArray)
		{
			if(0<corner.subDiv.GetN())
			{
				for(YSSIZE_T idx=0; idx<corner.subDiv.GetN()-1; ++idx)
				{
					vtxBuf.AddVertex(corner.subDiv[idx].pos);
					vtxBuf.AddVertex(corner.subDiv[idx+1].pos);
				}
				vtxBuf.AddVertex(corner.subDiv.Last().pos);
				vtxBuf.AddVertex(corner.roundedCornerPos);
			}
		}

		struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
		YsGLSLUse3DRenderer(renderer);

		const GLfloat edgeCol[4]={0,0,0,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)vtxBuf.GetNumVertex(),vtxBuf);

		YsGLSLEndUse3DRenderer(renderer);
	}
}

/*static*/ YSRESULT GeblGuiEditorBase::Edit_Round_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	return canvas.Edit_Round_SpaceKeyCallBack();
}

YSRESULT GeblGuiEditorBase::Edit_Round_SpaceKeyCallBack(void)
{
	if(YSTRUE==roundDlg->mode3D)
	{
		return Edit_Round3d();
	}
	else
	{
		return Edit_Round2d();
	}
}


YSRESULT GeblGuiEditorBase::Edit_Round3d(void)
{
	const int nDiv=roundDlg->subdivTxt->GetInteger();
	const double radius=roundDlg->slider->GetPosition()*roundDlg->maxRadius;

	if(0>=nDiv || 256<nDiv)
	{
		MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_DIVCOUNTOUTOF1AND256);
		return YSERR;
	}

	if(NULL!=slHd)
	{
		if(YSOK==YsShellExtEdit_RoundUtil3d::Apply(slHd->Conv(),roundDlg->roundUtil3d,radius,nDiv))
		{
			needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_CONSTEDGE|
			                          NEED_REMAKE_DRAWING_SELECTED_VERTEX|
			                          NEED_REMAKE_DRAWING_SELECTED_POLYGON|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE);
			SetNeedRedraw(YSTRUE);

			return YSOK;
		}
	}
	return YSERR;
}

void GeblGuiEditorBase::Edit_UnroundVertexSequence(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		auto &shl=*slHd;
		auto selVtHd=shl.GetSelectedVertex();
		if(3<=selVtHd.GetN())
		{
			YsArray <YsPlane> constPln;

			// First plane is the best fit plane of the selected vertices.
			{
				YsArray <YsVec3> vtPos;
				for(auto vtHd : selVtHd)
				{
					vtPos.Add(shl.GetVertexPosition(vtHd));
				}
				YsVec3 o,n;
				if(YSOK!=YsFindLeastSquareFittingPlane(o,n,vtPos))
				{
					MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_LEASTSQUAREPLANE);
					return;
				}
				constPln.Add(YsPlane(o,n));
			}

			const YsShell::VertexHandle edVtHd[2][2]=
			{
				{selVtHd[0],selVtHd[1]},
				{selVtHd[selVtHd.GetN()-1],selVtHd[selVtHd.GetN()-2]}
			};
			for(int i=0; i<2; ++i)
			{
				for(auto plHd : shl.FindPolygonFromVertex(edVtHd[i][0]))
				{
					if(YSTRUE!=shl.IsPolygonUsingEdgePiece(plHd,edVtHd[i]))
					{
						YsArray <YsVec3,4> plVtPos;
						shl.GetPolygon(plVtPos,plHd);
						YsVec3 o,n;
						if(YSOK==YsFindLeastSquareFittingPlane(o,n,plVtPos))
						{
							constPln.Add(YsPlane(o,n));
						}
					}
				}
			}

			YsVec3 pos;
			if(YSOK==YsFindLeastSquarePointFromPlane(pos,constPln))
			{
				YsShellExtEdit::StopIncUndo undoGuard(shl);

				for(auto vtHd : selVtHd)
				{
					if(vtHd!=selVtHd[selVtHd.GetN()/2])
					{
						YsShellExtEdit_CollapseEdge(shl.Conv(),vtHd,selVtHd[selVtHd.GetN()/2]);
					}
				}

				shl.SetVertexPosition(selVtHd[selVtHd.GetN()/2],pos);

				selVtHd.CleanUp();
				shl.SelectVertex(selVtHd);

				needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_ALL);
				SetNeedRedraw(YSTRUE);
			}
		}
	}
}
