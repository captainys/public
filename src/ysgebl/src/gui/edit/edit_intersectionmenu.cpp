/* ////////////////////////////////////////////////////////////

File Name: edit_intersectionmenu.cpp
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
#include <ysgl.h>
#include <ysshellextutil.h>
#include <ysshellextedit_localop.h>
#include <ysshellextmisc.h>

#include "edit_intersectiondialog.h"

#include "../textresource.h"

template <const int N>
static YSRESULT GetVertexLoopForCutting(YsArray <YsShellVertexHandle,N> &vtHdLoop,GeblGuiEditorBase *canvas,const YsShellExtEdit &shl)
{
	YsArray <YsShellVertexHandle> selVtHd;
	YsArray <YsShellPolygonHandle> selPlHd;
	YsArray <YsShellExt::ConstEdgeHandle> selCeHd;

	shl.GetSelectedVertex(selVtHd);
	shl.GetSelectedPolygon(selPlHd);
	shl.GetSelectedConstEdge(selCeHd);

	if(3==selVtHd.GetN() && 0==selPlHd.GetN() && 0==selCeHd.GetN())
	{
		vtHdLoop=selVtHd;
		return YSOK;
	}
	else if(0==selVtHd.GetN() && 1==selPlHd.GetN() && 0==selCeHd.GetN())
	{
		shl.GetVertexListOfPolygon(vtHdLoop,selPlHd[0]);
		if(3>vtHdLoop.GetN())
		{
			canvas->MessageDialog(FSGUI_COMMON_ERROR,
			    L"Selected polygon consists of less than 3 vertices.");
			return YSERR;
		}
		return YSOK;
	}
	else if(0==selVtHd.GetN() && 0==selPlHd.GetN() && 1==selCeHd.GetN())
	{
		YSBOOL isLoop;
		shl.GetConstEdge(vtHdLoop,isLoop,selCeHd[0]);
		if(3>vtHdLoop.GetN())
		{
			canvas->MessageDialog(FSGUI_COMMON_ERROR,
			    L"Selected constraint edge consists of less than 3 vertices.");
			return YSERR;
		}
		return YSOK;
	}
	else
	{
		canvas->MessageDialog(FSGUI_COMMON_ERROR,
		    L"A cutting plane must be selected by one of:\n"
		    L"(1) Three selected vertices,\n"
		    L"(2) One selected polygon, or\n"
		    L"(3) One selected constraint edge");
		return YSERR;
	}
}

void GeblGuiEditorBase::Edit_Intersection_CutByPlane(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase *canvas=(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas->slHd)
	{
		YsArray <YsShellVertexHandle,4> cuttingLoopVtHd;
		if(YSOK==GetVertexLoopForCutting(cuttingLoopVtHd,canvas,*(canvas->slHd)))
		{
			YsShellExtEdit_CutByPlane cutByPlane;
			cutByPlane.CutByPolygon(*(canvas->slHd),cuttingLoopVtHd);

			canvas->needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_VERTEX);
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
}

////////////////////////////////////////////////////////////

PolyCreChopOffDialog::PolyCreChopOffDialog()
{
	canvas=NULL;
	okBtn=NULL;
	cancelBtn=NULL;
}

PolyCreChopOffDialog::~PolyCreChopOffDialog()
{
}

/*static*/ PolyCreChopOffDialog *PolyCreChopOffDialog::Create(class GeblGuiEditorBase *canvas)
{
	PolyCreChopOffDialog *dlg=new PolyCreChopOffDialog;
	dlg->canvas=canvas;
	return dlg;
}

/*static*/ void PolyCreChopOffDialog::Delete(PolyCreChopOffDialog *ptr)
{
	delete ptr;
}

YSRESULT PolyCreChopOffDialog::Make(void)
{
	if(NULL==okBtn)
	{
		FsGuiDialog::Initialize();
		okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

		const wchar_t *cutPlaneChoice[]=
		{
			FSGUI_DLG_CHOPOFF_POLYGONORCONSTEDGE,
			FSGUI_DLG_CHOPOFF_3VERTEX,
			FSGUI_DLG_CHOPOFF_XYPLANE,
			FSGUI_DLG_CHOPOFF_XZPLANE,
			FSGUI_DLG_CHOPOFF_YZPLANE,
			FSGUI_DLG_CHOPOFF_XYPLANEFIRSTVERTEX,
			FSGUI_DLG_CHOPOFF_XZPLANEFIRSTVERTEX,
			FSGUI_DLG_CHOPOFF_YZPLANEFIRSTVERTEX
		};
		cutPlnDrp=AddDropList(0,FSKEY_NULL,"Plane",8,cutPlaneChoice,12,40,40,YSTRUE);
		cutPlnDrp->Select(0);

		const wchar_t *whichSideChoice[]=
		{
			FSGUI_DLG_CHOPOFF_DROPSELVERTEXSIDE,
			FSGUI_DLG_CHOPOFF_LEAVESELVERTEXSIDE,
			FSGUI_DLG_CHOPOFF_DONTDROP
		};
		whichSideDrp=AddDropList(0,FSKEY_NULL,"Which side to chop off",3,whichSideChoice,3,40,40,YSTRUE);
		whichSideDrp->Select(0);

		copyToCutBuffer=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_CHOPOFF_COPYTOBUFFER,YSTRUE);

		const wchar_t *wholeModelOrLocal[]=
		{
			FSGUI_DLG_CHOPOFF_WHOLEMODEL,
			FSGUI_DLG_CHOPOFF_ONLYINTERSECTINGPART
		};
		wholeModelOrLocalDrp=AddDropList(0,FSKEY_NULL,"Whole model or local",2,wholeModelOrLocal,3,40,40,YSTRUE);
		wholeModelOrLocalDrp->Select(0);

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(0.3);
		Fit();
	}

	SetUpIntersectionUtil(YSFALSE);
	RemakeDrawingBuffer();

	return YSOK;
}

YSRESULT PolyCreChopOffDialog::GetCuttingPlane(YsPlane &pln,YSBOOL showError) const
{
	switch(cutPlnDrp->GetSelection())
	{
	case 0: //FSGUI_DLG_CHOPOFF_POLYGONORCONSTEDGE,
	case 1: //FSGUI_DLG_CHOPOFF_3VERTEX,
		{
			YsArray <YsVec3> plVtPos;
			if(YSOK==GetCuttingPolygon(plVtPos,showError))
			{
				YsVec3 nom;
				if(YSOK==YsGetAverageNormalVector(nom,plVtPos))
				{
					pln.Set(YsGetCenter(plVtPos),nom);
					return YSOK;
				}
				else if(YSTRUE==showError)
				{
					canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTCALCULATENORMAL);
					return YSERR;
				}
			}
		}
		break;
	case 2: //FSGUI_DLG_CHOPOFF_XYPLANE,
		pln.Set(YsOrigin(),YsZVec());
		return YSOK;
	case 3: //FSGUI_DLG_CHOPOFF_XZPLANE,
		pln.Set(YsOrigin(),YsYVec());
		return YSOK;
	case 4: //FSGUI_DLG_CHOPOFF_YZPLANE,
		pln.Set(YsOrigin(),YsZVec());
		return YSOK;
	case 5: //FSGUI_DLG_CHOPOFF_XYPLANEFIRSTVERTEX,
		if(NULL!=canvas->Slhd())
		{
			auto selVtHd=canvas->Slhd()->GetSelectedVertex();
			if(0<selVtHd.GetN())
			{
				auto vtPos=canvas->Slhd()->GetVertexPosition(selVtHd[0]);
				pln.Set(vtPos,YsZVec());
				return YSOK;
			}
		}
		break;
	case 6: //FSGUI_DLG_CHOPOFF_XZPLANEFIRSTVERTEX,
		if(NULL!=canvas->Slhd())
		{
			auto selVtHd=canvas->Slhd()->GetSelectedVertex();
			if(0<selVtHd.GetN())
			{
				auto vtPos=canvas->Slhd()->GetVertexPosition(selVtHd[0]);
				pln.Set(vtPos,YsYVec());
				return YSOK;
			}
		}
		break;
	case 7: //FSGUI_DLG_CHOPOFF_YZPLANEFIRSTVERTEX
		if(NULL!=canvas->Slhd())
		{
			auto selVtHd=canvas->Slhd()->GetSelectedVertex();
			if(0<selVtHd.GetN())
			{
				auto vtPos=canvas->Slhd()->GetVertexPosition(selVtHd[0]);
				pln.Set(vtPos,YsXVec());
				return YSOK;
			}
		}
		break;
	}
	pln.Set(YsOrigin(),YsXVec());
	return YSERR;
}

YSRESULT PolyCreChopOffDialog::GetCuttingPolygon(YsArray <YsVec3> &plVtPos,YSBOOL showError) const
{
	YsArray <YsShellVertexHandle> plVtHd;
	if(YSOK==GetCuttingPolygon(plVtHd,showError))
	{
		plVtPos.Set(plVtHd.GetN(),NULL);
		for(auto index : plVtHd.AllIndex())
		{
			plVtPos[index]=canvas->Slhd()->GetVertexPosition(plVtHd[index]);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT PolyCreChopOffDialog::GetCuttingPolygon(YsArray <YsShellVertexHandle> &plVtHd,YSBOOL showError) const
{
	plVtHd.CleanUp();
	switch(cutPlnDrp->GetSelection())
	{
	case 0: //FSGUI_DLG_CHOPOFF_POLYGONORCONSTEDGE,
		if(NULL!=canvas->Slhd())
		{
			auto selPlHd=canvas->Slhd()->GetSelectedPolygon();
			auto selCeHd=canvas->Slhd()->GetSelectedConstEdge();

			if(1==selPlHd.GetN() && 0==selCeHd.GetN())
			{
				canvas->Slhd()->GetPolygon(plVtHd,selPlHd[0]);
			}
			else if(0==selPlHd.GetN() && 1==selCeHd.GetN())
			{
				YSBOOL isLoop;
				canvas->Slhd()->GetConstEdge(plVtHd,isLoop,selCeHd[0]);
			}
			else if(YSTRUE==showError)
			{
				canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTONEPLGORCONSTEDGE);
				return YSERR;
			}

			return YSOK;
		}
		break;
	case 1: //FSGUI_DLG_CHOPOFF_3VERTEX,
		if(NULL!=canvas->Slhd())
		{
			auto selVtHd=canvas->Slhd()->GetSelectedVertex();
			if(3>selVtHd.GetN())
			{
				if(YSTRUE==showError)
				{
					canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTATLEAST3VERTEX);
				}
				return YSERR;
			}

			plVtHd=selVtHd;
			return YSOK;
		}
		break;
	}
	return YSERR;
}

YsShellVertexHandle PolyCreChopOffDialog::GetSideVertex(YSBOOL showError) const
{
	if(NULL!=canvas->Slhd())
	{
		auto selVtHd=canvas->Slhd()->GetSelectedVertex();
		switch(cutPlnDrp->GetSelection())
		{
		default:
			if(0<selVtHd.GetN())
			{
				return selVtHd[0];
			}
			if(YSTRUE==showError)
			{
				canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTATLEAST1VERTEX);
			}
			break;
		case 1: //FSGUI_DLG_CHOPOFF_3VERTEX,
			if(4<=selVtHd.GetN())
			{
				return selVtHd[3];
			}
			if(YSTRUE==showError)
			{
				canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTATLEAST4VERTEX);
			}
			break;
		}
	}
	return NULL;
}

YSRESULT PolyCreChopOffDialog::SetUpIntersectionUtil(YSBOOL showError)
{
	if(NULL==canvas->Slhd())
	{
		return YSERR;
	}
	YsShellExtEdit &shl=*canvas->Slhd();

	YsPlane cutPln;
	if(YSOK!=GetCuttingPlane(cutPln,showError))
	{
		return YSERR;
	}

	if(1==wholeModelOrLocalDrp->GetSelection())
	{
		YsArray <YsShellVertexHandle> plVtHd;
		if(YSOK!=GetCuttingPolygon(plVtHd,showError))
		{
			return YSERR;
		}
		if(YSOK!=cutByPlaneUtil.SetCutByPolygon(shl.Conv(),plVtHd))
		{
			return YSERR;
		}
	}
	else
	{
		if(YSOK!=cutByPlaneUtil.SetCutByPlane(shl.Conv(),cutPln))
		{
			return YSERR;
		}
	}

	return YSOK;
}

void PolyCreChopOffDialog::RemakeDrawingBuffer(void)
{
	if(NULL!=canvas->Slhd())
	{
		markerVtxBuffer.CleanUp();
		markerOffsetBuffer.CleanUp();

		YsArray <YsVec3> itscPos=cutByPlaneUtil.GetIntersectingPoint();
		YsArray <YsVec3> onPlnPos=cutByPlaneUtil.GetOnPlaneVertex();

		const GLfloat rad=8.0;
		for(auto pos : itscPos)
		{
			markerVtxBuffer.AddVertex(pos);
			markerOffsetBuffer.AddPixOffset(-rad,-rad);

			markerVtxBuffer.AddVertex(pos);
			markerOffsetBuffer.AddPixOffset(rad*3.0f,-rad);

			markerVtxBuffer.AddVertex(pos);
			markerOffsetBuffer.AddPixOffset(-rad,rad*3.0f);
		}
		for(auto pos : onPlnPos)
		{
			markerVtxBuffer.AddVertex(pos);
			markerOffsetBuffer.AddPixOffset(-rad,-rad);

			markerVtxBuffer.AddVertex(pos);
			markerOffsetBuffer.AddPixOffset(rad*3.0f,-rad);

			markerVtxBuffer.AddVertex(pos);
			markerOffsetBuffer.AddPixOffset(-rad,rad*3.0f);
		}
	}
}

void PolyCreChopOffDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		canvas->SpaceKeyCallBack(*canvas);
	}
	else if(btn==cancelBtn)
	{
		canvas->Edit_ClearUIMode();
	}
}

void PolyCreChopOffDialog::OnDropListSelChange(FsGuiDropList *drp,int prevSel)
{
	if(drp==cutPlnDrp)
	{
		if(0==cutPlnDrp->GetSelection() || 1==cutPlnDrp->GetSelection())
		{
			wholeModelOrLocalDrp->Enable();
		}
		else
		{
			wholeModelOrLocalDrp->Disable();
		}

		if(1==prevSel && 1!=cutPlnDrp->GetSelection())
		{
			whichSideDrp->SetString(0,FSGUI_DLG_CHOPOFF_DROPSELVERTEXSIDE);
			whichSideDrp->SetString(1,FSGUI_DLG_CHOPOFF_LEAVESELVERTEXSIDE);
		}
		else if(1!=prevSel && 1==cutPlnDrp->GetSelection())
		{
			whichSideDrp->SetString(0,FSGUI_DLG_CHOPOFF_DROPLASTVERTEXSIDE);
			whichSideDrp->SetString(1,FSGUI_DLG_CHOPOFF_LEAVELASTVERTEXSIDE);
		}
	}

	SetUpIntersectionUtil(YSFALSE);
	RemakeDrawingBuffer();
}

void PolyCreChopOffDialog::OnTextBoxChange(FsGuiTextBox *)
{
}

void PolyCreChopOffDialog::OnSliderPositionChange(FsGuiSlider *,const double &,const double &)
{
}

/*static*/ void GeblGuiEditorBase::Edit_Intersection_ChopOff(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL==canvas.Slhd())
	{
		return;
	}

	if(YSOK==canvas.chopOffDlg->Make())
	{
		canvas.Edit_ClearUIMode();

		canvas.AddDialog(canvas.chopOffDlg);
		canvas.ArrangeDialog();

		canvas.UIDrawCallBack3D=Edit_Intersection_ChopOff_DrawCallBack3D;
		canvas.SpaceKeyCallBack=Edit_Intersection_ChopOff_SpaceKeyCallBack;

		canvas.undoRedoRequireToClearUIMode=YSTRUE;
		canvas.deletionRequireToClearUIMode=YSTRUE;

		canvas.SetNeedRedraw(YSTRUE);
	}
	else
	{
		// Tell the user something is missing.
	}
}

/*static*/ void GeblGuiEditorBase::Edit_Intersection_ChopOff_DrawCallBack3D(GeblGuiEditorBase &canvas)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLSharedMonoColorMarker3DRenderer();
	YsGLSLUse3DRenderer(renderer);

	auto &vtxBuf=canvas.chopOffDlg->markerVtxBuffer;
	auto &offsetBuf=canvas.chopOffDlg->markerOffsetBuffer;

	const GLfloat selVtxCol[4]={0.0,0.0,1.0,1.0};
	YsGLSLSet3DRendererUniformColorfv(renderer,selVtxCol);
	YsGLSLSet3DRendererUniformMarkerDimension(renderer,8);
	YsGLSLSet3DRendererUniformMarkerType(renderer,YSGLSL_MARKER_TYPE_STAR);
	// YsGLSLSet3DRendererViewportDimensionf(renderer,(GLfloat)viewport[2],(GLfloat)viewport[3]);
	YsGLSLDrawPrimitiveVtxPixelOffsetfv(renderer,GL_TRIANGLES,(int)vtxBuf.GetNumVertex(),vtxBuf,offsetBuf);

	YsGLSLEndUse3DRenderer(renderer);
}

/*static*/ void GeblGuiEditorBase::Edit_Intersection_ChopOff_3DInterfaceCallBack(void *,class FsGui3DInterface &)
{
//	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
//	if(NULL==canvas.Slhd())
//	{
//		return;
//	}
//
//	YsShellExtEdit &shl=*(canvas.Slhd());
}

/*static*/ YSRESULT GeblGuiEditorBase::Edit_Intersection_ChopOff_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	return canvas.Edit_Intersection_ChopOff_SpaceKeyCallBack();
}

YSRESULT GeblGuiEditorBase::Edit_Intersection_ChopOff_SpaceKeyCallBack(void)
{
	const YSBOOL showError=YSTRUE;
	if(YSOK==chopOffDlg->SetUpIntersectionUtil(showError))
	{
		YsShellExtEdit::StopIncUndo stopIncUndo(*Slhd());

		auto &cutByPlaneUtil=chopOffDlg->cutByPlaneUtil;

		YsPlane cutPln=cutByPlaneUtil.GetCuttingPlane();

		// Check separatability

		for(auto &edgeItsc : cutByPlaneUtil.AllEdgeIntersection())
		{
			edgeItsc.newVtHd=Slhd()->AddVertex(edgeItsc.pos);
			YsShellExtEdit_InsertVertexOnEdge(*Slhd(),edgeItsc.edVtHd,edgeItsc.newVtHd);
		}

		cutByPlaneUtil.MakeSplitInfo();

		for(auto &plgSplit : cutByPlaneUtil.polygonSplitArray)
		{
			YsShellExtEdit_ApplyPolygonSplit(*Slhd(),plgSplit);
		}
		for(auto &ceSplit :  cutByPlaneUtil.constEdgeSplitArray)
		{
			YsShellExtEdit_ApplyConstEdgeSplit(*Slhd(),ceSplit);
		}

		const int whichSideToDropOption=chopOffDlg->whichSideDrp->GetSelection();
		int whichSideToDrop=0;
		if(0==whichSideToDropOption || 1==whichSideToDropOption)
		{
			const YsShellVertexHandle sideVtHd=chopOffDlg->GetSideVertex(showError);
			if(NULL==sideVtHd)
			{
				return YSERR;
			}

			auto sideVtPos=Slhd()->GetVertexPosition(sideVtHd);
			whichSideToDrop=cutPln.GetSideOfPlane(sideVtPos);
			if(0==whichSideToDrop)
			{
				MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTIDENTIFYWHICHSIDETODROP);
				return YSERR;
			}
			if(1==whichSideToDropOption)
			{
				whichSideToDrop*=-1;
			}

			YsShellPolygonStore plgToDrop;
			YsShellExt::ConstEdgeStore ceToDrop;

printf("%s %d\n",__FUNCTION__,__LINE__);
			cutByPlaneUtil.MakeToDrop(plgToDrop,ceToDrop,whichSideToDrop);
printf("%s %d\n",__FUNCTION__,__LINE__);

			cutByPlaneUtil.MakeFaceGroupSplitInfo(plgToDrop);
printf("%s %d\n",__FUNCTION__,__LINE__);
			for(auto &fgSplit : cutByPlaneUtil.faceGroupSplitArray)
			{
				YsShellExtEdit_ApplyFaceGroupSplit(*Slhd(),fgSplit);
			}
printf("%s %d\n",__FUNCTION__,__LINE__);

			if(YSTRUE==chopOffDlg->copyToCutBuffer->GetCheck())
			{
				cutBuffer.CleanUp();
				cutBuffer.SetShell(*(const YsShellExt *)Slhd());
				cutBuffer.AddPolygon(plgToDrop);
				cutBuffer.AddConstEdge(ceToDrop);
				YsShellExt::FaceGroupStore fgStore(*(const YsShellExt *)Slhd());
				for(auto plHd : plgToDrop)
				{
					auto fgHd=Slhd()->FindFaceGroupFromPolygon(plHd);
					if(NULL!=fgHd)
					{
						fgStore.AddFaceGroup(fgHd);
					}
				}
				cutBuffer.AddFaceGroup(fgStore);
			}

			YsShellVertexStore allVtx(*(const YsShell *)Slhd());
			for(auto plHd : plgToDrop)
			{
				allVtx.AddVertex(Slhd()->GetPolygonVertex(plHd));
			}
			for(auto plHd : plgToDrop)
			{
				Slhd()->ForceDeletePolygon(plHd);
			}
			for(auto ceHd : ceToDrop)
			{
				Slhd()->DeleteConstEdge(ceHd);
			}
			for(auto vtHd : allVtx)
			{
				auto attrib=Slhd()->GetVertexAttrib(vtHd);
				if(0==attrib->refCount)
				{
					Slhd()->DeleteVertex(vtHd);
				}
			}
		}
		else
		{
			YsShellPolygonStore plgToDrop(*(const YsShell *)Slhd());
			YsShellExt::ConstEdgeStore ceToDrop(*(const YsShellExt *)Slhd());
			cutByPlaneUtil.MakeToDrop(plgToDrop,ceToDrop,1);  // Either way.
			cutByPlaneUtil.MakeFaceGroupSplitInfo(plgToDrop);
			for(auto &fgSplit : cutByPlaneUtil.faceGroupSplitArray)
			{
				YsShellExtEdit_ApplyFaceGroupSplit(*Slhd(),fgSplit);
			}
		}


		Slhd()->SelectVertex(0,NULL);
		Slhd()->SelectConstEdge(0,NULL);

		Edit_ClearUIMode();
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);

		return YSOK;
	}
	return YSERR;
}

