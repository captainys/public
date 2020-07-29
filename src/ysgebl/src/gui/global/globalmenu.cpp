/* ////////////////////////////////////////////////////////////

File Name: globalmenu.cpp
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
#include "../textresource.h"

#include <ysshellextedit.h>

#include <ysshellextedit_localop.h>
#include <ysshellext_orientationutil.h>
#include <ysshellextedit_orientationutil.h>
#include <ysshellext_mappingutil.h>
#include <ysshellext_stitchingutil.h>
#include <ysshellextedit_topologyutil.h>
#include <ysshellext_trackingutil.h>


YsArray <YsShellDnmContainer <YsShellExtEditGui>::Node *> GeblGuiEditorBase::Global_GetTarget(void)
{
	if(YSTRUE==FsGuiPopUpMenuItem::GetCheck(globalTargetCurrentShellOnly))
	{
		YsArray <YsShellDnmContainer <YsShellExtEditGui>::Node *> r;
		if(NULL!=slHd)
		{
			r.Append(slHd);
		}
		return r;
	}
	else
	{
		return shlGrp.GetNodePointerAll();
	}
}


void GeblGuiEditorBase::Global_Target_Current(FsGuiPopUpMenuItem *)
{
	globalTargetCurrentShellOnly->SetCheck(YSTRUE);
	globalTargetAllShell->SetCheck(YSFALSE);
}
void GeblGuiEditorBase::Global_Target_All(FsGuiPopUpMenuItem *)
{
	globalTargetCurrentShellOnly->SetCheck(YSFALSE);
	globalTargetAllShell->SetCheck(YSTRUE);
}


void GeblGuiEditorBase::Global_DeleteUnusedVertexAll(FsGuiPopUpMenuItem *)
{
	auto target=Global_GetTarget();

	for(auto slHd : target)
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);
		/* for(auto vtHd=slHd->Vertex().begin(); slHd->Vertex().end()!=vtHd; ++vtHd)
		{
			if(YSTRUE!=slHd->IsVertexUsed(*vtHd))
			{
				slHd->DeleteVertex(*vtHd);
			}
		} */
		for(auto vtHd : slHd->AllVertex())
		{
			if(YSTRUE!=slHd->IsVertexUsed(vtHd))
			{
				slHd->DeleteVertex(vtHd);
			}
		}
	}
	needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_VERTEX;
	if(1<target.GetN())
	{
		needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_VERTEX;
	}
	SetNeedRedraw(YSTRUE);
}

/*static*/ void GeblGuiEditorBase::Global_InvertPolygonAll(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto canvas=(GeblGuiEditorBase *)appPtr;
	auto target=canvas->Global_GetTarget();

	for(auto slHd : target)
	{
		YsShellExtEdit::StopIncUndo stopIncUndo(slHd);

		YsArray <YsShellPolygonHandle> plHdArray;
		YsArray <YsVec3> newNomArray;
		for(auto plHd : slHd->AllPolygon())
		{
			YsArray <YsShellVertexHandle,4> plVtHd;
			slHd->GetPolygon(plVtHd,plHd);
			plVtHd.Invert();
			slHd->SetPolygonVertex(plHd,plVtHd);

			YsVec3 nom;
			slHd->GetNormal(nom,plHd);
			if(YsOrigin()!=nom)
			{
				nom=-nom;
				plHdArray.Append(plHd);
				newNomArray.Append(nom);
			}
		}

		slHd->SetPolygonNormalMulti(plHdArray,newNomArray);
	}
	canvas->needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	if(1<target.GetN())
	{
		canvas->needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	}
	canvas->SetNeedRedraw(YSTRUE);
}

/*static*/ void GeblGuiEditorBase::Global_SetZeroNormalAll(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto canvas=(GeblGuiEditorBase *)appPtr;
	auto target=canvas->Global_GetTarget();

	for(auto slHd : target)
	{
		YsArray <YsShellPolygonHandle> plHdArray;
		YsArray <YsVec3> newNomArray;
		for(auto plHd : slHd->AllPolygon())
		{
			plHdArray.Append(plHd);
			newNomArray.Append(YsOrigin());
		}

		slHd->SetPolygonNormalMulti(plHdArray,newNomArray);
	}
	canvas->needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	if(1<target.GetN())
	{
		canvas->needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	}
	canvas->SetNeedRedraw(YSTRUE);
}

/*static*/ void GeblGuiEditorBase::Global_FixOrientationAndNormalClosedVolume(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto canvas=(GeblGuiEditorBase *)appPtr;
	auto target=canvas->Global_GetTarget();

	for(auto slHd : target)
	{
		YsShellExtEdit::StopIncUndo stopIncUndo(slHd);
		YsShellExtEdit_OrientationUtil::FixOrientationOfClosedSolid(slHd->Conv());
	}
	canvas->needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	if(1<target.GetN())
	{
		canvas->needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	}
	canvas->SetNeedRedraw(YSTRUE);
}

void GeblGuiEditorBase::Global_FixOrientationBasedOnAssignedNormal(FsGuiPopUpMenuItem *)
{
	auto target=Global_GetTarget();

	for(auto slHd : target)
	{
		YsShellExtEdit_OrientationUtil orientationUtil;
		orientationUtil.FixOrientationBasedOnAssignedNormal(*slHd);
	}
	needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	if(1<target.GetN())
	{
		needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	}
	SetNeedRedraw(YSTRUE);
}

void GeblGuiEditorBase::Global_FixNormalBasedOnOrientation(FsGuiPopUpMenuItem *)
{
	auto target=Global_GetTarget();

	for(auto slHd : target)
	{
		YsShellExtEdit_OrientationUtil orientationUtil;
		orientationUtil.RecalculateNormalFromCurrentOrientation(*slHd);
	}
	needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	if(1<target.GetN())
	{
		needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	}
	SetNeedRedraw(YSTRUE);
}

/*static*/ void GeblGuiEditorBase::Global_TriangulateAll(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	YSRESULT res=YSOK;

	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;

	auto target=canvas.Global_GetTarget();

	for(auto slHd : target)
	{
		auto &shl=*slHd;
		auto selPlHd=shl.GetSelectedPolygon();

		YsShellExtEdit::StopIncUndo incUndo(slHd);

		for(auto plHd : shl.AllPolygon())
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
	}

	canvas.needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	if(1<target.GetN())
	{
		canvas.needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	}
	canvas.SetNeedRedraw(YSTRUE);

	if(YSOK!=res)
	{
		canvas.MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_TRIANGULATIONERROR);
	}
}

/* static */ void GeblGuiEditorBase::Global_DeleteRedundantVertex(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	canvas.numberDlg->Make(FSGUI_COMMON_DISTTHR,FSGUI_COMMON_OK,FSGUI_COMMON_CANCEL);
	canvas.numberDlg->SetCloseModalCallBack(Global_DeleteRedundantVertex_ToleranceEntered);
	canvas.numberDlg->SetRealNumber(YsTolerance,20);
	canvas.AttachModalDialog(canvas.numberDlg);
}

/* static */ void GeblGuiEditorBase::Global_DeleteRedundantVertex_ToleranceEntered(FsGuiControlBase *appPtr,FsGuiDialog *,int returnValue)
{
	if(YSOK!=(YSRESULT)returnValue)
	{
		return;
	}

	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	const double tol=canvas.numberDlg->GetRealNumber();

	canvas.Global_DeleteRedundantVertex(tol,NULL);
}



/* static */ void GeblGuiEditorBase::Global_DeleteRedundantVertexNonManifoldEdgeOnly(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	canvas.numberDlg->Make(FSGUI_COMMON_DISTTHR,FSGUI_COMMON_OK,FSGUI_COMMON_CANCEL);
	canvas.numberDlg->SetCloseModalCallBack(Global_DeleteRedundantVertexNonManifoldEdgeOnly_ToleranceEntered);
	canvas.numberDlg->SetRealNumber(YsTolerance);
	canvas.AttachModalDialog(canvas.numberDlg);
}



/* static */ void GeblGuiEditorBase::Global_DeleteRedundantVertexNonManifoldEdgeOnly_ToleranceEntered(FsGuiControlBase *appPtr,FsGuiDialog *,int returnValue)
{
	if(YSOK!=(YSRESULT)returnValue)
	{
		return;
	}

	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	auto shlPtr=canvas.Slhd();
	if(NULL!=shlPtr)
	{
		auto &shl=*shlPtr;

		const double tol=canvas.numberDlg->GetRealNumber();

		YsShellVertexStore limitedVtx(shl.Conv());
		for(auto vtHd : shl.AllVertex())
		{
			auto connVtHd=shl.GetConnectedVertex(vtHd);
			for(auto vtHd2 : connVtHd)
			{
				if(2>shl.GetNumPolygonUsingEdge(vtHd,vtHd2))
				{
					limitedVtx.AddVertex(vtHd);
					break;
				}
			}
		}

		canvas.Global_DeleteRedundantVertex(tol,&limitedVtx);
	}
}



void GeblGuiEditorBase::Global_DeleteRedundantVertex(const double tol,const YsShellVertexStore *limitedVtx)
{
	auto target=Global_GetTarget();

	for(auto slHd : target)
	{
		auto &shl=*slHd;

		YsShellExtEdit::StopIncUndo incUndo(slHd);

		YsShellExt_MergeVertex(shl,tol,limitedVtx);

		YsShellExtEdit_TopologyUtil topoUtil;
		topoUtil.SplitAllDegeneratePolygon(shl);
	}

	needRemakeDrawingBuffer=(unsigned int)
	    NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON|
	    NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
	if(1<target.GetN())
	{
		needRemakeDrawingBuffer_for_OtherShell=(unsigned int)
		    NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON|
		    NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
	}
	SetNeedRedraw(YSTRUE);
}

////////////////////////////////////////////////////////////

class GeblGuiEditorBase::Global_ScalingDialog : public FsGuiDialog
{
public:
	GeblGuiEditorBase *canvasPtr;
	FsGuiTextBox *xTxt,*yTxt,*zTxt;
	FsGuiButton *okBtn,*cancelBtn;

	void Make(GeblGuiEditorBase *ptr);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void GeblGuiEditorBase::Global_ScalingDialog::Make(GeblGuiEditorBase *ptr)
{
	canvasPtr=ptr;

	xTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"X:",8,YSTRUE);
	xTxt->SetRealNumber(1.0,6);

	yTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"Y:",8,YSFALSE);
	yTxt->SetRealNumber(1.0,6);

	zTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"Z:",8,YSFALSE);
	zTxt->SetRealNumber(1.0,6);

	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	Fit();
}

/* virtual */ void GeblGuiEditorBase::Global_ScalingDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		canvasPtr->Global_Scaling_DialogCallBack(
		    xTxt->GetRealNumber(),
		    yTxt->GetRealNumber(),
		    zTxt->GetRealNumber());
		CloseModalDialog(YSOK);
	}
	else if(btn==cancelBtn)
	{
		CloseModalDialog(YSERR);
	}
}

void GeblGuiEditorBase::Global_Scaling(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <Global_ScalingDialog>();

	dlg->Make(this);
	dlg->BindCloseModalCallBack(&THISCLASS::Dnm_SetDnmName_CloseModalCallBack,this);
	AttachModalDialog(dlg);
	ArrangeDialog();
}

void GeblGuiEditorBase::Global_Scaling_DialogCallBack(double sx,double sy,double sz)
{
	auto target=Global_GetTarget();
	for(auto slHd : target)
	{
		auto &shl=*slHd;
		YsArray <YsShell::VertexHandle> allVtHd;
		YsArray <YsVec3> newPos;
		for(auto vtHd : shl.AllVertex())
		{
			auto pos=shl.GetVertexPosition(vtHd);
			pos.MulX(sx);
			pos.MulY(sy);
			pos.MulZ(sz);
			newPos.Add(pos);
			allVtHd.Add(vtHd);
		}
		shl.SetMultiVertexPosition(allVtHd,newPos);
	}

	needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	if(1<target.GetN())
	{
		needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	}
	SetNeedRedraw(YSTRUE);
}


void GeblGuiEditorBase::Global_Scale_InDirection(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		auto selVtHd=shl.GetSelectedVertex();
		if(2==selVtHd.GetN())
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiInputNumberDialog>();
			dlg->Make(1.0,2,
			    FSGUI_DLG_SCALE_IN_DIRECTION_TITLE,
			    L"",
			    FSGUI_DLG_SCALE_IN_DIRECTION_FACTOR,
			    FSGUI_COMMON_OK,FSGUI_COMMON_CANCEL);
			dlg->BindCloseModalCallBack(&THISCLASS::Global_Scale_InDirection_DialogCallBack,this);
			AttachModalDialog(dlg);
			ArrangeDialog();
			SetNeedRedraw(YSTRUE);
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECT2VERTEX);
		}
	}
}

void GeblGuiEditorBase::Global_Scale_InDirection_DialogCallBack(FsGuiDialog *dlg,int returnCode)
{
	auto numDlg=dynamic_cast <FsGuiInputNumberDialog *>(dlg);
	if((int)YSOK==returnCode && nullptr!=numDlg && nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		auto selVtHd=shl.GetSelectedVertex();
		if(2==selVtHd.GetN())
		{
			auto s=numDlg->GetNumber();

			const auto o=shl.GetVertexPosition(selVtHd[0]);
			const auto m=shl.GetVertexPosition(selVtHd[1]);
			const auto v=YsUnitVector(m-o);

			auto target=Global_GetTarget();
			for(auto slHd : target)
			{
				auto &shl=*slHd;
				YsArray <YsShell::VertexHandle> allVtHd;
				YsArray <YsVec3> newPos;
				for(auto vtHd : shl.AllVertex())
				{
					auto pos=shl.GetVertexPosition(vtHd);

					auto L=(pos-o)*v;
					auto f=pos-v*L;

					pos=f+v*L*s;

					newPos.Add(pos);
					allVtHd.Add(vtHd);
				}
				shl.SetMultiVertexPosition(allVtHd,newPos);
			}

			needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
			if(1<target.GetN())
			{
				needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_ALL;
			}
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECT2VERTEX);
		}
		SetNeedRedraw(YSTRUE);
	}
}

////////////////////////////////////////////////////////////

class GeblGuiEditorBase::Global_MoveDialog : public FsGuiDialog
{
public:
	FsGuiButton *okBtn,*cancelBtn;
	GeblGuiEditorBase *editor;
	void Make(GeblGuiEditorBase *);
	virtual void OnButtonClick(FsGuiButton *btn);
};
void GeblGuiEditorBase::Global_MoveDialog::Make(GeblGuiEditorBase *editor)
{
	this->editor=editor;
	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);
	Fit();
}
/* virtual */ void GeblGuiEditorBase::Global_MoveDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		auto d=editor->threeDInterface.point_pos-editor->threeDInterface.point_pos_org;
		editor->Global_Move_DialogCallBack(d);
		editor->Edit_ClearUIMode();
	}
	else if(btn==cancelBtn)
	{
		editor->Edit_ClearUIMode();
	}
}

void GeblGuiEditorBase::Global_Move(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <Global_MoveDialog>();
		dlg->Make(this);
		draw3dCallBack=std::bind(&THISCLASS::Global_Move_DrawCallBack3d,this);
		spaceKeyCallBack=std::bind(&THISCLASS::Global_Move_SpaceKeyCallBack,this);

		YsVec3 bbx[2];
		slHd->GetBoundingBox(bbx);

		const double bbxDgn=(bbx[1]-bbx[0]).GetLength();

		YsVec3 cen=(bbx[0]+bbx[1])/2.0;

		threeDInterface.BeginInputPoint2(cen);

		AddDialog(dlg);
		ArrangeDialog();
	}
}
void GeblGuiEditorBase::Global_Move_DialogCallBack(const YsVec3 &d)
{
	if(nullptr!=slHd)
	{
		YsShellExtEdit::StopIncUndo undoGuard(*slHd);
		YsArray <YsVec3> allVtPos;
		YsArray <YsShell::VertexHandle> allVtHd;
		for(auto vtHd : slHd->AllVertex())
		{
			allVtPos.push_back(d+slHd->GetVertexPosition(vtHd));
			allVtHd.push_back(vtHd);
		}
		slHd->SetMultiVertexPosition(allVtHd,allVtPos);

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	}
	Edit_ClearUIMode();
}
void GeblGuiEditorBase::Global_Move_DrawCallBack3d(void)
{
	auto d=threeDInterface.point_pos-threeDInterface.point_pos_org;

	if(nullptr!=slHd)
	{
		YsMatrix4x4 tfm;
		threeDInterface.GetRotationMatrix(tfm);

		YsArray <float> vtx;
		for(auto vtHd : slHd->AllVertex())
		{
			auto vtPos=slHd->GetVertexPosition(vtHd)+d;
			vtx.push_back(vtPos.xf());
			vtx.push_back(vtPos.yf());
			vtx.push_back(vtPos.zf());
		}

		struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
		YsGLSLUse3DRenderer(renderer);

#ifdef GL_PROGRAM_POINT_SIZE
        glEnable(GL_PROGRAM_POINT_SIZE);
#endif
		const GLfloat col[4]={1.0f,0.0f,1.0f,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,col);
		YsGLSLSet3DRendererUniformPointSize(renderer,3);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_POINTS,(int)vtx.size()/3,vtx.data());
#ifdef GL_PROGRAM_POINT_SIZE
        glDisable(GL_PROGRAM_POINT_SIZE);
#endif

		YsGLSLEndUse3DRenderer(renderer);
	}
}
void GeblGuiEditorBase::Global_Move_SpaceKeyCallBack(void)
{
	auto d=threeDInterface.point_pos-threeDInterface.point_pos_org;
	Global_Move_DialogCallBack(d);
}

////////////////////////////////////////////////////////////

class GeblGuiEditorBase::Global_RotateDialog : public FsGuiDialog
{
public:
	FsGuiButton *okBtn,*cancelBtn;
	GeblGuiEditorBase *editor;
	void Make(GeblGuiEditorBase *);
	virtual void OnButtonClick(FsGuiButton *btn);
};
void GeblGuiEditorBase::Global_RotateDialog::Make(GeblGuiEditorBase *editor)
{
	this->editor=editor;
	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);
	Fit();
}
/* virtual */ void GeblGuiEditorBase::Global_RotateDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		YsMatrix4x4 tfm;
		editor->threeDInterface.GetRotationMatrix(tfm);
		editor->Global_Rotate_DialogCallBack(tfm);
		editor->Edit_ClearUIMode();
	}
	else if(btn==cancelBtn)
	{
		editor->Edit_ClearUIMode();
	}
}
void GeblGuiEditorBase::Global_Rotate(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <Global_RotateDialog>();
		dlg->Make(this);
		draw3dCallBack=std::bind(&THISCLASS::Global_Rotate_DrawCallBack3d,this);
		spaceKeyCallBack=std::bind(&THISCLASS::Global_Rotate_SpaceKeyCallBack,this);

		YsVec3 bbx[2];
		slHd->GetBoundingBox(bbx);

		const double bbxDgn=(bbx[1]-bbx[0]).GetLength();

		YsVec3 cen=(bbx[0]+bbx[1])/2.0;

		const YSBOOL allowChangeAxis=YSTRUE;
		const YSBOOL allowMoveCenter=YSTRUE;
		const YSBOOL allowMirror=YSTRUE;
		threeDInterface.BeginInputRotation(YsZVec(),0.0,cen,bbxDgn/2.0,allowChangeAxis,allowMoveCenter,allowMirror);

		AddDialog(dlg);
		ArrangeDialog();
	}
}
void GeblGuiEditorBase::Global_Rotate_DialogCallBack(const YsMatrix4x4 &tfm)
{
	if(nullptr!=slHd)
	{
		YsShellExtEdit::StopIncUndo undoGuard(*slHd);
		YsArray <YsVec3> allVtPos;
		YsArray <YsShell::VertexHandle> allVtHd;
		for(auto vtHd : slHd->AllVertex())
		{
			allVtPos.push_back(tfm*slHd->GetVertexPosition(vtHd));
			allVtHd.push_back(vtHd);
		}
		slHd->SetMultiVertexPosition(allVtHd,allVtPos);

		YsArray <YsShellPolygonHandle> plHdArray;
		YsArray <YsVec3> newNomArray;
		for(auto plHd : slHd->AllPolygon())
		{
			plHdArray.Append(plHd);

			auto nom=slHd->GetNormal(plHd);
			tfm.Mul(nom,nom,0.0);
			newNomArray.Append(nom);
		}
		slHd->SetPolygonNormalMulti(plHdArray,newNomArray);
		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	}
	Edit_ClearUIMode();
}
void GeblGuiEditorBase::Global_Rotate_DrawCallBack3d(void)
{
	if(nullptr!=slHd)
	{
		YsMatrix4x4 tfm;
		threeDInterface.GetRotationMatrix(tfm);

		YsArray <float> vtx;
		for(auto vtHd : slHd->AllVertex())
		{
			auto vtPos=tfm*slHd->GetVertexPosition(vtHd);
			vtx.push_back(vtPos.xf());
			vtx.push_back(vtPos.yf());
			vtx.push_back(vtPos.zf());
		}

		struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
		YsGLSLUse3DRenderer(renderer);

#ifdef GL_PROGRAM_POINT_SIZE
        glEnable(GL_PROGRAM_POINT_SIZE);
#endif
		const GLfloat col[4]={1.0f,0.0f,1.0f,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,col);
		YsGLSLSet3DRendererUniformPointSize(renderer,3);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_POINTS,(int)vtx.size()/3,vtx.data());
#ifdef GL_PROGRAM_POINT_SIZE
        glDisable(GL_PROGRAM_POINT_SIZE);
#endif

		YsGLSLEndUse3DRenderer(renderer);
	}
}
void GeblGuiEditorBase::Global_Rotate_SpaceKeyCallBack(void)
{
	YsMatrix4x4 tfm;
	threeDInterface.GetRotationMatrix(tfm);
	Global_Rotate_DialogCallBack(tfm);
}

void GeblGuiEditorBase::Global_DihedralReducingSwap(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsShellExt_DihedralAngleReducingSwap(*slHd);
		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	}
}

void GeblGuiEditorBase::Global_CloseAllHole(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);
		auto &shl=*Slhd();
		YsShellEdgeEnumHandle edHd=nullptr;
		while(YSOK==shl.MoveToNextEdge(edHd))
		{
			if(1==shl.GetNumPolygonUsingEdge(edHd))
			{
				auto edge=shl.GetEdge(edHd);
				YsShellExt::TrackingAlongSingleUseEdge cond;
				auto loop=YsShellExt_TrackingUtil::TrackEdge(shl.Conv(),edge[0],edge[1],cond);

				if(0<loop.size() && loop.front()==loop.back())
				{
					loop.pop_back();
					shl.AddPolygon(loop);
				}
			}
		}
		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	}
}
