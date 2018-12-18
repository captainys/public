/* ////////////////////////////////////////////////////////////

File Name: edit_sweepmenu.cpp
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
#include "edit_sweepdialog.h"
#include "../miscdlg.h"
#include <ysgl.h>

#include <ysshellextutil.h>
#include <ysshellext_sweeputil.h>
#include <ysshellext_geomcalc.h>
#include <ysshellextedit_orientationutil.h>

#include "../textresource.h"


////////////////////////////////////////////////////////////

void PolyCreEditParallelSweepDialog::Make(class GeblGuiEditorBase *canvas)
{
	this->canvas=canvas;

	Initialize();

	okBtn=AddTextButton(0,FSKEY_SPACE,FSGUI_PUSHBUTTON,L"Sweep",YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	scaleTxt=AddTextBox(0,FSKEY_NULL,FSGUI_DLG_COMMON_SCALINGFACTOR,L"1.0",5,YSTRUE);
	scaleTxt->SetTextType(FSGUI_REALNUMBER);

	numLayerTxt=AddTextBox(MakeIdent("numdiv"),FSKEY_NULL,FSGUI_DLG_COMMON_DIVCOUNT,L"1",5,YSTRUE);
	numLayerTxt->SetTextType(FSGUI_INTEGER);

	twistAngleTxt=AddTextBox(MakeIdent("twist"),FSKEY_NULL,FSGUI_COMMON_ANGLE,L"0.0",5,YSTRUE);
	twistAngleTxt->SetTextType(FSGUI_REALNUMBER);

	twistCenterBtn[0]=AddTextButton(MakeIdent("center_center"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_CENTER,YSTRUE);
	twistCenterBtn[1]=AddTextButton(MakeIdent("center_bbxcenter"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_BBXCENTER,YSFALSE);
	SetRadioButtonGroup(2,twistCenterBtn);
	twistCenterBtn[1]->SetCheck(YSTRUE);

	triOrQuadBtn[0]=AddTextButton(MakeIdent("tri"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_TRIANGLE,YSTRUE);
	triOrQuadBtn[1]=AddTextButton(MakeIdent("quad"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_QUADRILATERAL,YSFALSE);
	triOrQuadBtn[2]=AddTextButton(MakeIdent("auto_tri_quad"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_AUTOMATIC,YSFALSE);
	SetRadioButtonGroup(3,triOrQuadBtn);
	triOrQuadBtn[2]->SetCheck(YSTRUE);

	deleteSourcePolygonBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_SWEEP_DELSRCPLG,YSTRUE);

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();

}

double PolyCreEditParallelSweepDialog::GetScalingFactor(void) const
{
	return scaleTxt->GetRealNumber();
}

void PolyCreEditParallelSweepDialog::SetDeleteSourcePolygon(YSBOOL sw)
{
	deleteSourcePolygonBtn->SetCheck(sw);
}

YSBOOL PolyCreEditParallelSweepDialog::GetDeleteSourcePolygon(void) const
{
	return deleteSourcePolygonBtn->GetCheck();
}

YSBOOL PolyCreEditParallelSweepDialog::UseQuad(void) const
{
	if(YSTRUE==triOrQuadBtn[1]->GetCheck() ||
	   (YSTRUE==triOrQuadBtn[2]->GetCheck() && fabs(twistAngleTxt->GetRealNumber())<YsTolerance))
	{
		return YSTRUE;
	}
	return YSFALSE;
}
YSBOOL PolyCreEditParallelSweepDialog::UseTri(void) const
{
	if(YSTRUE==triOrQuadBtn[0]->GetCheck() ||
	   (YSTRUE==triOrQuadBtn[2]->GetCheck() && fabs(twistAngleTxt->GetRealNumber())>=YsTolerance))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

void PolyCreEditParallelSweepDialog::RemakeSweepInfo(void)
{
	if(YSTRUE==twistCenterBtn[0]->GetCheck())
	{
		sweepOpt.twistCenter=sweepInfo.GetCenter(canvas->Slhd()->Conv());
	}
	else if(YSTRUE==twistCenterBtn[1]->GetCheck())
	{
		sweepOpt.twistCenter=sweepInfo.GetBoundingBoxCenter(canvas->Slhd()->Conv());
	}

	int nDiv=numLayerTxt->GetInteger();
	sweepOpt.pathPnt.clear();
	sweepOpt.scaling.clear();
	sweepOpt.twistAngle.clear();
	sweepOpt.pathPnt.push_back(sweepInfo.GetCenter(canvas->Slhd()->Conv()));
	sweepOpt.scaling.push_back(1.0);
	if(YsTolerance<fabs(twistAngleTxt->GetRealNumber()))
	{
		sweepOpt.twistAngle.push_back(0.0);
	}
	if(2<nDiv)
	{
		auto p0=sweepInfo.GetCenter(canvas->Slhd()->Conv());
		auto p1=canvas->threeDInterface.point_pos;
		for(int i=1; i<nDiv; ++i)
		{
			double t=(double)i/(double)(nDiv-1);
			sweepOpt.pathPnt.push_back(p0*(1.0-t)+p1*t);
			sweepOpt.scaling.push_back(1.0*(1.0-t)+scaleTxt->GetRealNumber()*t);
			if(YsTolerance<fabs(twistAngleTxt->GetRealNumber()))
			{
				double a=YsDegToRad(twistAngleTxt->GetRealNumber())*t;
				sweepOpt.twistAngle.push_back(a);
			}
		}
	}
	else
	{
		sweepOpt.pathPnt.push_back(canvas->threeDInterface.point_pos);
		sweepOpt.scaling.push_back(scaleTxt->GetRealNumber());
		if(YsTolerance<fabs(twistAngleTxt->GetRealNumber()))
		{
			double a=YsDegToRad(twistAngleTxt->GetRealNumber());
			sweepOpt.twistAngle.push_back(a);
		}
	}

	if(YSOK!=sweepInfo.SetUpParallelSweepWithPath(canvas->Slhd()->Conv(),sweepOpt))
	{
		printf("Failed to create sweep info.\n");
	}
}

void PolyCreEditParallelSweepDialog::RemakeDrawingBuffer(void)
{
	const auto &shl=*(canvas->Slhd());

	lineBuf.CleanUp();
	for(auto &layer : sweepInfo.layerArray)
	{
		for(YSSIZE_T idx=0; idx<=sweepInfo.srcEdVtHd.GetN()-2; idx+=2)
		{
			YSSIZE_T matchingIndex[2];
			if(YSOK==sweepInfo.srcVtKeyToMidPointIndex.FindElement(matchingIndex[0],shl.GetSearchKey(sweepInfo.srcEdVtHd[idx])) &&
			   YSOK==sweepInfo.srcVtKeyToMidPointIndex.FindElement(matchingIndex[1],shl.GetSearchKey(sweepInfo.srcEdVtHd[idx+1])))
			{
				const YsVec3 pos[2]=
				{
					layer.pointArray[matchingIndex[0]].pos,
					layer.pointArray[matchingIndex[1]].pos
				};
				lineBuf.AddVertex(pos[0]);
				lineBuf.AddVertex(pos[1]);
			}
		}
	}
}

void PolyCreEditParallelSweepDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		canvas->Edit_Sweep_ParallelSweep_SpaceKeyCallBack();
	}
	else if(cancelBtn==btn)
	{
		canvas->Edit_ClearUIMode();
	}
	else
	{
		RemakeSweepInfo();
		RemakeDrawingBuffer();
	}
}
void PolyCreEditParallelSweepDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	RemakeSweepInfo();
	RemakeDrawingBuffer();
}

////////////////////////////////////////////////////////////

void GeblGuiEditorBase::Edit_Sweep_ParallelSweep(FsGuiPopUpMenuItem *)
{
	YsArray <YsShellPolygonHandle> selPlHd;
	YsArray <YsShellExt::ConstEdgeHandle> selCeHd;

	if(NULL!=this->slHd)
	{
		this->slHd->GetSelectedConstEdge(selCeHd);
		this->slHd->GetSelectedPolygon(selPlHd);

		if(0<selCeHd.GetN() || 0<selPlHd.GetN())
		{
			YSBOOL dirSw[3]={YSTRUE,YSTRUE,YSTRUE};
			YsVec3 dir[3]={YsXVec(),YsYVec(),YsZVec()};

			YsVec3 nomSum=YsShell_CalculatePolygonAverageNormal(this->slHd->Conv(),selPlHd);
			if(YSOK==nomSum.Normalize())
			{
				dir[0]=nomSum;
				dir[1]=nomSum.GetArbitraryPerpendicularVector();
				dir[2]=dir[0]^dir[1];
			}

			this->Edit_ClearUIMode();

			auto parallelSweepDlg=FsGuiDialog::CreateSelfDestructiveDialog<PolyCreEditParallelSweepDialog>();
			auto &sweepInfo=parallelSweepDlg->sweepInfo;
			auto &sweepOpt=parallelSweepDlg->sweepOpt;

			sweepInfo.MakeInfo(this->slHd->Conv(),selPlHd,selCeHd);
			sweepInfo.CacheNonDuplicateVtHdArray();

			const YsVec3 cen=sweepInfo.GetCenter(this->slHd->Conv());
			this->threeDInterface.BeginInputPoint2(cen,dirSw,dir);
			this->threeDInterface.SetInputPoint2UseCustomOrientation(YSTRUE);
			this->threeDInterface.BindCallBack(&THISCLASS::Edit_Sweep_ParallelSweep_PointMoved,this);

			this->UIDrawCallBack3D=Edit_Sweep_ParallelSweep_DrawCallBack3D;
			this->SpaceKeyCallBack=Edit_Sweep_ParallelSweep_SpaceKeyCallBack;

			this->undoRedoRequireToClearUIMode=YSTRUE;
			this->deletionRequireToClearUIMode=YSTRUE;

			sweepOpt.twistAxis=nomSum;
			sweepOpt.twistCenter=cen;
			sweepOpt.pathPnt.push_back(cen);
			sweepOpt.pathPnt.push_back(cen);
			if(YSOK!=sweepInfo.SetUpParallelSweepWithPath(slHd->Conv(),sweepOpt))
			{
				printf("Failed to create sweep info.\n");
			}

			parallelSweepDlg->Make(this);
			int minUse,maxUse;
			sweepInfo.GetEdgeUsageMinMax(minUse,maxUse,this->slHd->Conv());
			parallelSweepDlg->SetDeleteSourcePolygon((1!=maxUse) ? YSTRUE : YSFALSE);
			parallelSweepDlg->RemakeDrawingBuffer();

			this->AddDialog(parallelSweepDlg);
			this->ArrangeDialog();
		}
		else
		{
			this->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTONEPLGORCONSTEDGE);
		}
	}
}

void GeblGuiEditorBase::Edit_Sweep_ParallelSweep_PointMoved(FsGui3DInterface *itfc)
{
	auto parallelSweepDlg=FindTypedModelessDialog<PolyCreEditParallelSweepDialog>();
	if(nullptr!=parallelSweepDlg)
	{
		parallelSweepDlg->RemakeSweepInfo();
		parallelSweepDlg->RemakeDrawingBuffer();
	}
}

/*static*/ void GeblGuiEditorBase::Edit_Sweep_ParallelSweep_DrawCallBack3D(GeblGuiEditorBase &canvas)
{
	canvas.Edit_Sweep_ParallelSweep_DrawCallBack3D();
}

void GeblGuiEditorBase::Edit_Sweep_ParallelSweep_DrawCallBack3D(void)
{
	auto parallelSweepDlg=FindTypedModelessDialog<PolyCreEditParallelSweepDialog>();
	if(nullptr!=parallelSweepDlg)
	{
		struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
		YsGLSLUse3DRenderer(renderer);

		const GLfloat edgeCol[4]={0,1.0f,0,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)parallelSweepDlg->lineBuf.GetNumVertex(),parallelSweepDlg->lineBuf);

		YsGLSLEndUse3DRenderer(renderer);
	}
}

/*static*/ YSRESULT GeblGuiEditorBase::Edit_Sweep_ParallelSweep_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	return canvas.Edit_Sweep_ParallelSweep_SpaceKeyCallBack();
}

YSRESULT GeblGuiEditorBase::Edit_Sweep_ParallelSweep_SpaceKeyCallBack(void)
{
	auto parallelSweepDlg=FindTypedModelessDialog<PolyCreEditParallelSweepDialog>();
	if(NULL!=slHd && nullptr!=parallelSweepDlg)
	{
		auto selPlHd=slHd->GetSelectedPolygon();

		YsShellExtEdit::StopIncUndo incUndo(*Slhd());

		auto &sweepInfo=parallelSweepDlg->sweepInfo;
		auto &shl=*Slhd();
		for(auto &layer : sweepInfo.layerArray)
		{
			for(auto &point : layer.pointArray)
			{
				point.vtHd=shl.AddVertex(point.pos);
			}
		}

		auto quadArray=sweepInfo.MakeSideFaceAndFirstToLastVertexMapping(shl.Conv());
		for(auto &quad : quadArray)
		{
			if(YSTRUE==parallelSweepDlg->UseQuad())
			{
				auto plHd=shl.AddPolygon(4,quad.quadVtHd);
				shl.SetPolygonColor(plHd,colorPaletteDlg->GetColor());
			}
			else
			{
				double d[2]=
				{
					shl.GetEdgeLength(quad.quadVtHd[0],quad.quadVtHd[2]),
					shl.GetEdgeLength(quad.quadVtHd[1],quad.quadVtHd[3])
				};
				YsShell::VertexHandle triVtHd[2][3];
				if(d[0]<d[1])
				{
					triVtHd[0][0]=quad.quadVtHd[0];
					triVtHd[0][1]=quad.quadVtHd[1];
					triVtHd[0][2]=quad.quadVtHd[2];
					triVtHd[1][0]=quad.quadVtHd[0];
					triVtHd[1][1]=quad.quadVtHd[2];
					triVtHd[1][2]=quad.quadVtHd[3];
				}
				else
				{
					triVtHd[0][0]=quad.quadVtHd[3];
					triVtHd[0][1]=quad.quadVtHd[0];
					triVtHd[0][2]=quad.quadVtHd[1];
					triVtHd[1][0]=quad.quadVtHd[1];
					triVtHd[1][1]=quad.quadVtHd[2];
					triVtHd[1][2]=quad.quadVtHd[3];
				}
				YsShell::PolygonHandle plHd[2]=
				{
					shl.AddPolygon(3,triVtHd[0]),
					shl.AddPolygon(3,triVtHd[1]),
				};
				shl.SetPolygonColor(plHd[0],colorPaletteDlg->GetColor());
				shl.SetPolygonColor(plHd[1],colorPaletteDlg->GetColor());
			}
		}

		for(auto plHd : selPlHd)
		{
			YsArray <YsShellVertexHandle,4> plVtHd;
			shl.GetPolygon(plVtHd,plHd);

			if(YSOK==sweepInfo.MapVertexArray(plVtHd,shl.Conv()))
			{
				if(YSTRUE==parallelSweepDlg->GetDeleteSourcePolygon())
				{
					shl.SetPolygonVertex(plHd,plVtHd);
				}
				else
				{
					auto newPlHd=shl.AddPolygon(plVtHd);
					auto attrib=shl.GetPolygonAttrib(plHd);
					auto nom=shl.GetNormal(plHd);
					auto col=shl.GetColor(plHd);

					shl.SetPolygonAttrib(newPlHd,*attrib);
					shl.SetPolygonNormal(newPlHd,nom);
					shl.SetPolygonColor(newPlHd,col);
				}
			}
		}

		slHd->SelectPolygon(0,NULL);
		slHd->SelectConstEdge(0,NULL);

		needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON|
								NEED_REMAKE_DRAWING_SELECTED_POLYGON|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE);
		SetNeedRedraw(YSTRUE);

		Edit_ClearUIMode();

		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

PolyCreEditParallelSweepWithPathDialog::PolyCreEditParallelSweepWithPathDialog()
{
	okBtn=NULL;
	cancelBtn=NULL;
	deleteSourcePolygonBtn=NULL;
}

PolyCreEditParallelSweepWithPathDialog::~PolyCreEditParallelSweepWithPathDialog()
{
}

/*static*/ PolyCreEditParallelSweepWithPathDialog *PolyCreEditParallelSweepWithPathDialog::Create(GeblGuiEditorBase *canvas)
{
	PolyCreEditParallelSweepWithPathDialog *dlg=new PolyCreEditParallelSweepWithPathDialog;
	dlg->canvas=canvas;
	return dlg;
}

/*static*/ void PolyCreEditParallelSweepWithPathDialog::Delete(PolyCreEditParallelSweepWithPathDialog *ptr)
{
	delete ptr;
}

YSRESULT PolyCreEditParallelSweepWithPathDialog::Make(void)
{
	if(NULL!=canvas->Slhd())
	{
		auto &shl=*(canvas->Slhd());
		auto pathCeHd=shl.GetSelectedConstEdge();
		auto srcPlHd=shl.GetSelectedPolygon();

		auto selFgHd=shl.GetSelectedFaceGroup();
		for(auto fgHd : selFgHd)
		{
			YsArray <YsShellPolygonHandle> fgPlHd;
			shl.GetFaceGroup(fgPlHd,fgHd);
			srcPlHd.Append(fgPlHd);
		}

		if(1>pathCeHd.GetN() || 2<pathCeHd.GetN() || 0==srcPlHd.GetN())
		{
			canvas->MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_SWEEP_NEEDPATHANDSRC);
			return YSERR;
		}

		srcPlHdCache=srcPlHd;

		YSBOOL isLoop;
		shl.GetConstEdge(pathVtHdCache,isLoop,pathCeHd[0]);

		scaling.CleanUp();
		if(2==pathCeHd.GetN())
		{
			YsArray <YsShellVertexHandle> guideVtHdArray;
			YSBOOL isLoop;
			shl.GetConstEdge(guideVtHdArray,isLoop,pathCeHd[1]);

			YsVec3 sweepDir=YsShell_CalculatePolygonAverageNormal(shl.Conv(),srcPlHdCache);
			scaling=YsShellExt_SweepInfoMultiStep::CalculateScalingForParallelSweepWithPathAndGuideLine(
				shl.Conv(),sweepDir,pathVtHdCache,guideVtHdArray);
		}

		if(1==pathCeHd.GetN() && 2<=pathVtHdCache.size())
		{
			// Maybe an arc connecting two vertices of the source polygons.
			// In which case, the path const-edge must be divided into two, and one taken as a path, the other half as a guideline.
			// Also must avoid crunching at the center.
			YsShellVertexStore srcVtHd(shl.Conv());
			for(auto plHd : srcPlHd)
			{
				srcVtHd.Add(shl.GetPolygonVertex(plHd));
			}
			if(YSTRUE==srcVtHd.IsIncluded(pathVtHdCache.front()) && YSTRUE==srcVtHd.IsIncluded(pathVtHdCache.back()))
			{
				int n=(int)pathVtHdCache.size()/2;
				YsArray <YsShell::VertexHandle> guideVtHdArray=pathVtHdCache.Subset(pathVtHdCache.size()-n);
				guideVtHdArray.Invert();
				pathVtHdCache.resize(n);

				YsVec3 sweepDir=YsShell_CalculatePolygonAverageNormal(shl.Conv(),srcPlHdCache);
				scaling=YsShellExt_SweepInfoMultiStep::CalculateScalingForParallelSweepWithPathAndGuideLine(
					shl.Conv(),sweepDir,pathVtHdCache,guideVtHdArray);
			}
		}

		if(0==scaling.GetN())
		{
			scaling.Set(pathVtHdCache.GetN(),NULL);
			for(auto &s : scaling)
			{
				s=1.0;
			}
		}


		FsGuiDialog::Initialize();

		okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);
		deleteSourcePolygonBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_SWEEP_DELSRCPLG,YSTRUE);

		scaleTxt.CleanUp();
		for(YSSIZE_T idx=1; idx<20 && idx<pathVtHdCache.GetN(); ++idx)
		{
			scaleTxt.Append(AddTextBox(0,FSKEY_NULL,FSGUI_DLG_COMMON_SCALINGFACTOR,L"1.0",5,YSTRUE));
			scaleTxt.Last()->SetRealNumber(scaling[idx],2);
			scaleTxt.Last()->SetTextType(FSGUI_REALNUMBER);
		}

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(0.3);
		Fit();

		SetUpParallelSweepWithPath();
		RemakeDrawingBuffer();

		return YSOK;
	}
	return YSERR;
}

YSBOOL PolyCreEditParallelSweepWithPathDialog::GetDeleteSourcePolygon(void) const
{
	return deleteSourcePolygonBtn->GetCheck();
}

void PolyCreEditParallelSweepWithPathDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		canvas->SpaceKeyCallBack(*canvas);
	}
	else if(cancelBtn==btn)
	{
		canvas->Edit_ClearUIMode();
	}
}

void PolyCreEditParallelSweepWithPathDialog::SetUpParallelSweepWithPath(void)
{
	if(0<pathVtHdCache.GetN())
	{
		scaling.Set(pathVtHdCache.GetN(),NULL);
		scaling[0]=1.0;
		for(auto index : scaling.AllIndex())
		{
			if(0<index && index<=scaleTxt.GetN())
			{
				scaling[index]=scaleTxt[index-1]->GetRealNumber();
			}
			else
			{
				scaling[index]=scaleTxt.Last()->GetRealNumber();
			}
		}

		YsArray <YsShellExt::ConstEdgeHandle> empty;
		sweepUtil.CleanUp();
		sweepUtil.MakeInfo(*(const YsShellExt *)(canvas->Slhd()),srcPlHdCache,empty);
		sweepUtil.SetUpParallelSweepWithPath(*(const YsShellExt *)(canvas->Slhd()),pathVtHdCache,scaling);
	}
}

void PolyCreEditParallelSweepWithPathDialog::RemakeDrawingBuffer(void)
{
	const auto &shl=*(canvas->Slhd());

	lineBuf.CleanUp();
	for(auto &layer : sweepUtil.layerArray)
	{
		for(YSSIZE_T idx=0; idx<=sweepUtil.srcEdVtHd.GetN()-2; idx+=2)
		{
			YSSIZE_T matchingIndex[2];
			if(YSOK==sweepUtil.srcVtKeyToMidPointIndex.FindElement(matchingIndex[0],shl.GetSearchKey(sweepUtil.srcEdVtHd[idx])) &&
			   YSOK==sweepUtil.srcVtKeyToMidPointIndex.FindElement(matchingIndex[1],shl.GetSearchKey(sweepUtil.srcEdVtHd[idx+1])))
			{
				const YsVec3 pos[2]=
				{
					layer.pointArray[matchingIndex[0]].pos,
					layer.pointArray[matchingIndex[1]].pos
				};
				lineBuf.AddVertex(pos[0]);
				lineBuf.AddVertex(pos[1]);
			}
		}
	}
}

void PolyCreEditParallelSweepWithPathDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	if(YsTolerance<txt->GetRealNumber())
	{
		SetUpParallelSweepWithPath();
		RemakeDrawingBuffer();
		canvas->SetNeedRedraw(YSTRUE);
	}
}

/*static*/ void GeblGuiEditorBase::Edit_Sweep_ParallelSweepWithPath(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(YSOK==canvas.parallelSweepWithPathDlg->Make())
	{
		canvas.Edit_ClearUIMode();

		canvas.AddDialog(canvas.parallelSweepWithPathDlg);
		canvas.ArrangeDialog();

		canvas.UIDrawCallBack3D=Edit_Sweep_ParallelSweepWithPath_DrawCallBack3D;
		canvas.SpaceKeyCallBack=Edit_Sweep_ParallelSweepWithPath_SpaceKeyCallBack;

		canvas.undoRedoRequireToClearUIMode=YSTRUE;
		canvas.deletionRequireToClearUIMode=YSTRUE;
	}
}

/*static*/ void GeblGuiEditorBase::Edit_Sweep_ParallelSweepWithPath_DrawCallBack3D(GeblGuiEditorBase &canvas)
{
	canvas.Edit_Sweep_ParallelSweepWithPath_DrawCallBack3D();
}

void GeblGuiEditorBase::Edit_Sweep_ParallelSweepWithPath_DrawCallBack3D(void)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
	YsGLSLUse3DRenderer(renderer);

	const GLfloat edgeCol[4]={0,1.0f,0,1.0f};
	YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)parallelSweepWithPathDlg->lineBuf.GetNumVertex(),parallelSweepWithPathDlg->lineBuf);

	YsGLSLEndUse3DRenderer(renderer);
}

/*static*/ YSRESULT GeblGuiEditorBase::Edit_Sweep_ParallelSweepWithPath_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	return canvas.Edit_Sweep_ParallelSweepWithPath_SpaceKeyCallBack();
}

YSRESULT GeblGuiEditorBase::Edit_Sweep_ParallelSweepWithPath_SpaceKeyCallBack(void)
{
	if(NULL!=Slhd())
	{
		YsShellExtEdit::StopIncUndo incUndo(*Slhd());

		auto &sweepUtil=parallelSweepWithPathDlg->sweepUtil;
		auto &shl=*Slhd();
		for(auto &layer : sweepUtil.layerArray)
		{
			for(auto &point : layer.pointArray)
			{
				point.vtHd=shl.AddVertex(point.pos);
			}
		}

		auto quadArray=sweepUtil.MakeSideFaceAndFirstToLastVertexMapping(shl.Conv());
		for(auto &quad : quadArray)
		{
			auto plHd=shl.AddPolygon(4,quad.quadVtHd);
			shl.SetPolygonColor(plHd,colorPaletteDlg->GetColor());
		}

		for(auto plHd : parallelSweepWithPathDlg->srcPlHdCache)
		{
			YsArray <YsShellVertexHandle,4> plVtHd;
			shl.GetPolygon(plVtHd,plHd);

			if(YSOK==sweepUtil.MapVertexArray(plVtHd,shl.Conv()))
			{
				if(YSTRUE==parallelSweepWithPathDlg->deleteSourcePolygonBtn->GetCheck())
				{
					shl.SetPolygonVertex(plHd,plVtHd);
				}
				else
				{
					auto newPlHd=shl.AddPolygon(plVtHd);
					auto attrib=shl.GetPolygonAttrib(plHd);
					auto nom=shl.GetNormal(plHd);
					auto col=shl.GetColor(plHd);

					shl.SetPolygonAttrib(newPlHd,*attrib);
					shl.SetPolygonNormal(newPlHd,nom);
					shl.SetPolygonColor(newPlHd,col);
				}
			}
		}

		slHd->SelectPolygon(0,NULL);
		slHd->SelectConstEdge(0,NULL);

		needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON|
		                          NEED_REMAKE_DRAWING_SELECTED_POLYGON|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE);
		SetNeedRedraw(YSTRUE);

		Edit_ClearUIMode();

		return YSOK;
	}

	return YSERR;
}

////////////////////////////////////////////////////////////

/* static */ void GeblGuiEditorBase::Edit_Sweep_AlongSweepPath(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	printf("%s %d\n",__FUNCTION__,__LINE__);
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.Slhd())
	{
		auto &shl=*(canvas.Slhd());
		auto pathCeHd=shl.GetSelectedConstEdge();
		auto srcPlHd=shl.GetSelectedPolygon();

		auto selFgHd=shl.GetSelectedFaceGroup();
		for(auto fgHd : selFgHd)
		{
			YsArray <YsShellPolygonHandle> fgPlHd;
			shl.GetFaceGroup(fgPlHd,fgHd);
			srcPlHd.Append(fgPlHd);
		}

		if(1>pathCeHd.GetN() || 2<pathCeHd.GetN() || 0==srcPlHd.GetN())
		{
			canvas.MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_SWEEP_NEEDPATHANDSRC);
			return;
		}

		auto srcPlHdCache=srcPlHd;

		YsArray <YsShellVertexHandle> pathVtHdCache;
		YSBOOL isLoop;
		shl.GetConstEdge(pathVtHdCache,isLoop,pathCeHd[0]);

		YsArray <double> scaling(pathVtHdCache.GetN(),NULL);
		for(auto &s : scaling)
		{
			s=1.0;
		}


		YsShellExtEdit::StopIncUndo incUndo(shl);
		YsShellExt_SweepInfoMultiStep sweepUtil;

		YsArray <YsShellExt::ConstEdgeHandle> empty;
		sweepUtil.CleanUp();
		sweepUtil.MakeInfo(shl.Conv(),srcPlHdCache,empty);
		if(YSOK==sweepUtil.SetUpNonParallelSweepWithPath(shl.Conv(),pathVtHdCache,isLoop,scaling,YsShellExt_SweepInfoMultiStep::ORICON_AVERAGE_ANGLE,YsShellExt_SweepInfoMultiStep::ORICON_AVERAGE_ANGLE))
		{
			for(auto &layer : sweepUtil.layerArray)
			{
				for(auto &point : layer.pointArray)
				{
					if(NULL==point.vtHd)
					{
						point.vtHd=shl.AddVertex(point.pos);
					}
				}
			}
		}



		auto quadArray=sweepUtil.MakeSideFaceAndFirstToLastVertexMapping(shl.Conv());
		for(auto &quad : quadArray)
		{
			auto plHd=shl.AddPolygon(4,quad.quadVtHd);
			shl.SetPolygonColor(plHd,canvas.colorPaletteDlg->GetColor());
		}

		if(YSTRUE==isLoop)
		{
			for(auto plHd : srcPlHdCache)
			{
				shl.DeletePolygon(plHd);
			}
		}
		else
		{
			const YSBOOL deleteSrcPolygon=YSFALSE;
			for(auto plHd : srcPlHdCache)
			{
				YsArray <YsShellVertexHandle,4> plVtHd;
				shl.GetPolygon(plVtHd,plHd);

				if(YSOK==sweepUtil.MapVertexArray(plVtHd,shl.Conv()))
				{
					if(YSTRUE==deleteSrcPolygon)
					{
						shl.SetPolygonVertex(plHd,plVtHd);
					}
					else
					{
						auto newPlHd=shl.AddPolygon(plVtHd);
						auto attrib=shl.GetPolygonAttrib(plHd);
						auto nom=shl.GetNormal(plHd);
						auto col=shl.GetColor(plHd);

						shl.SetPolygonAttrib(newPlHd,*attrib);
						shl.SetPolygonNormal(newPlHd,nom);
						shl.SetPolygonColor(newPlHd,col);
					}
				}
			}
		}



		shl.SelectPolygon(0,NULL);
		shl.SelectConstEdge(0,NULL);

		canvas.needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON|
		                          NEED_REMAKE_DRAWING_SELECTED_POLYGON|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE);
		canvas.SetNeedRedraw(YSTRUE);

		canvas.Edit_ClearUIMode();
	}
}

////////////////////////////////////////////////////////////

PolyCreEditSweepConstEdgeDialog::PolyCreEditSweepConstEdgeDialog()
{
	okBtn=NULL;
	cancelBtn=NULL;
	invertBtn=NULL;
	canvas=NULL;
}

PolyCreEditSweepConstEdgeDialog::~PolyCreEditSweepConstEdgeDialog()
{
}

/*static*/ PolyCreEditSweepConstEdgeDialog *PolyCreEditSweepConstEdgeDialog::Create(GeblGuiEditorBase *canvas)
{
	PolyCreEditSweepConstEdgeDialog *dlg=new PolyCreEditSweepConstEdgeDialog;
	dlg->canvas=canvas;
	return dlg;
}

/*static*/ void PolyCreEditSweepConstEdgeDialog::Delete(PolyCreEditSweepConstEdgeDialog *ptr)
{
	delete ptr;
}

void PolyCreEditSweepConstEdgeDialog::CleanUp(void)
{
	ceHdArray.CleanUp();
	multiPatch.CleanUp();
}

void PolyCreEditSweepConstEdgeDialog::Make(void)
{
	if(NULL==okBtn)
	{
		FsGuiDialog::Initialize();

		deleteNonManifoldSrcPolygon=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SWEEPCONSTEDGE_DELETE_NONMANIFOLD,YSTRUE);
		deleteMidPolygon=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SWEEPCONSTEDGE_DELETE_MID,YSFALSE);
		doNotDeletePolygon=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SWEEPCONSTEDGE_DO_NOT_DELETE,YSFALSE);
		{
			FsGuiButton *radioButtonGrp[3]=
			{
				deleteNonManifoldSrcPolygon,deleteMidPolygon,doNotDeletePolygon
			};
			SetRadioButtonGroup(3,radioButtonGrp);
		}
		useQuadBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_SWEEPCONSTEDGE_USEQUAD,YSTRUE);

		okBtn=AddTextButton(0,FSKEY_SPACE,FSGUI_PUSHBUTTON,FSGUI_DLG_SWEEPCONSTEDGE_SWEEP,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);
		invertBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_DLG_SWEEPCONSTEDGE_INVERT,YSFALSE);
	}

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();
}

YSRESULT PolyCreEditSweepConstEdgeDialog::SetUp(void)
{
	if(NULL!=canvas->slHd)
	{
		auto selCeHd=canvas->slHd->GetSelectedConstEdge();
		auto selPlHd=canvas->slHd->GetSelectedPolygon();
		if(2<=selCeHd.GetN())
		{
			ceHdArray=selCeHd;
			srcPlHdArray.CleanUp();

			deleteNonManifoldSrcPolygon->Disable();
			deleteMidPolygon->Disable();
			doNotDeletePolygon->Disable();

			multiPatch.SetUpFromConstEdge(canvas->slHd->Conv(),ceHdArray);

			return YSOK;
		}
		else if(2<=selPlHd.GetN())
		{
			ceHdArray.CleanUp();
			srcPlHdArray=selPlHd;

			deleteNonManifoldSrcPolygon->Enable();
			deleteMidPolygon->Enable();
			doNotDeletePolygon->Enable();

			deleteNonManifoldSrcPolygon->SetCheck(YSTRUE);
			deleteMidPolygon->SetCheck(YSFALSE);
			doNotDeletePolygon->SetCheck(YSFALSE);

			multiPatch.SetUpFromPolygon(canvas->slHd->Conv(),selPlHd);
			return YSOK;
		}
	}
	return YSERR;
}

void PolyCreEditSweepConstEdgeDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		canvas->Edit_Sweep_BetweenConstEdge_SpaceKeyCallBack(*canvas);
	}
	else if(btn==cancelBtn)
	{
		canvas->Edit_ClearUIMode();
	}
	else if(btn==invertBtn)
	{
		multiPatch.Invert();
		RemakeDrawingBuffer();
		canvas->SetNeedRedraw(YSTRUE);
	}
}

void PolyCreEditSweepConstEdgeDialog::RemakeDrawingBuffer(void)
{
	lineBuf.CleanUp();
	triBuf.CleanUp();

	for(auto &patch : multiPatch.GetPatchSequence())
	{
		const auto &shl=patch.shl;
		YsShellEdgeEnumHandle edHd=NULL;
		while(YSOK==shl.MoveToNextEdge(edHd))
		{
			YsShellVertexHandle edVtHd[2];
			shl.GetEdge(edVtHd,edHd);

			lineBuf.AddVertex(shl.GetVertexPosition(edVtHd[0]));
			lineBuf.AddVertex(shl.GetVertexPosition(edVtHd[1]));
		}

		for(auto plHd : shl.AllPolygon())
		{
			YsArray <YsVec3,4> plVtPos;
			shl.GetPolygon(plVtPos,plHd);

			YsBoundingBoxMaker3 mkBbx(plVtPos);
			double l=mkBbx.GetDiagonalLength();

			const YsVec3 cen=shl.GetCenter(plHd);
			const YsVec3 nom=shl.GetNormal(plHd);
			const YsVec3 arrowHead=cen+nom*l/5.0;
			lineBuf.AddVertex(cen);
			lineBuf.AddVertex(arrowHead);

			triBuf.AddArrowHeadTriangle(cen,arrowHead,0.1);
		}
	}
}

void GeblGuiEditorBase::Edit_Sweep_BetweenConstEdge(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode();

	auto selCeHd=slHd->GetSelectedConstEdge();
	auto selPlHd=slHd->GetSelectedPolygon();
	if(2<=selCeHd.GetN() || 2<=selPlHd.GetN())
	{
		sweepConstEdgeDlg->Make();
		if(YSOK==sweepConstEdgeDlg->SetUp())
		{
			sweepConstEdgeDlg->RemakeDrawingBuffer();

			AddDialog(sweepConstEdgeDlg);
			ArrangeDialog();

			UIDrawCallBack3D=Edit_Sweep_BetweenConstEdge_DrawCallBack3D;
			SpaceKeyCallBack=Edit_Sweep_BetweenConstEdge_SpaceKeyCallBack;
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,L"Cannot prepare sweeping calculation.");
		}
	}
}

/*static*/ void GeblGuiEditorBase::Edit_Sweep_BetweenConstEdge_DrawCallBack3D(GeblGuiEditorBase &canvas)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
	YsGLSLUse3DRenderer(renderer);

	const GLfloat edgeCol[4]={0,1.0f,0,1.0f};
	YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)canvas.sweepConstEdgeDlg->lineBuf.GetNumVertex(),canvas.sweepConstEdgeDlg->lineBuf);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_TRIANGLES,(int)canvas.sweepConstEdgeDlg->triBuf.GetNumVertex(),canvas.sweepConstEdgeDlg->triBuf);

	YsGLSLEndUse3DRenderer(renderer);
}

/*static*/ YSRESULT GeblGuiEditorBase::Edit_Sweep_BetweenConstEdge_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	return canvas.Edit_Sweep_BetweenConstEdge_Sweep();
}

YSRESULT GeblGuiEditorBase::Edit_Sweep_BetweenConstEdge_Sweep(void)
{
	if(NULL!=slHd)
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);

		const YsColor col=colorPaletteDlg->GetColor();

		if(YSTRUE==sweepConstEdgeDlg->useQuadBtn->GetCheck())
		{
			sweepConstEdgeDlg->multiPatch.MergeTriFormQuad();
		}
		sweepConstEdgeDlg->multiPatch.CreateMissingVertexInOriginalShell(*slHd);
		sweepConstEdgeDlg->multiPatch.TransferPatchToOriginalShell(*slHd,col);

		if(0<sweepConstEdgeDlg->srcPlHdArray.GetN())
		{
			if(YSTRUE==sweepConstEdgeDlg->deleteNonManifoldSrcPolygon->GetCheck())
			{
				for(auto plHd : sweepConstEdgeDlg->srcPlHdArray)
				{
					int minPlgCount=100;
					auto plVtHd=slHd->GetPolygonVertex(plHd);
					for(int i=0; i<plVtHd.GetN(); ++i)
					{
						YsMakeSmaller<int>(minPlgCount,slHd->GetNumPolygonUsingEdge(plVtHd[i],plVtHd.GetCyclic(i+1)));
					}
					if(2<minPlgCount)
					{
						slHd->ForceDeletePolygon(plHd);
					}
				}
			}
			else if(YSTRUE==sweepConstEdgeDlg->deleteMidPolygon->GetCheck())
			{
				for(int i=1; i<sweepConstEdgeDlg->srcPlHdArray.GetN()-1; ++i)
				{
					slHd->ForceDeletePolygon(sweepConstEdgeDlg->srcPlHdArray[i]);
				}
			}
		}

		slHd->SelectConstEdge(0,NULL);
		slHd->SelectPolygon(0,NULL);
	}

	needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
	SetNeedRedraw(YSTRUE);

	Edit_ClearUIMode();
	return YSOK;
}

////////////////////////////////////////////////////////////

class GeblGuiEditorBase::Edit_Sweep_RotationalWithPathDialog : public FsGuiDialog
{
public:
	GeblGuiEditorBase *canvasPtr;
	FsGuiButton *deleteOrDontDeleteSrcPlg[2];
	FsGuiButton *invertBtn;
	FsGuiButton *useQuadBtn;
	FsGuiButton *okBtn,*cancelBtn;

	YsShellExt_MultiPatchSequence multiPatch;
	YsArray <YsShell::PolygonHandle> srcPlHd;
	YsArray <YsShellExt::ConstEdgeHandle> srcCeHd;

	YsGLVertexBuffer lineBuf;
	YsGLVertexBuffer triBuf;

	void Make(GeblGuiEditorBase *canvasPtr);
	void RemakeDrawingBuffer(void);
	void SetUp(const YsShellExt &shl,const YsConstArrayMask <YsShell::VertexHandle> &seg0,const YsConstArrayMask <YsShell::VertexHandle> &seg1,const YsConstArrayMask <YsShell::VertexHandle> &path);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void GeblGuiEditorBase::Edit_Sweep_RotationalWithPathDialog::Make(GeblGuiEditorBase *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	Initialize();

	deleteOrDontDeleteSrcPlg[0]=AddTextButton(MakeIdent("deletesrcplg"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SWEEPCONSTEDGE_DELETE_SOURCE,YSTRUE);
	deleteOrDontDeleteSrcPlg[1]=AddTextButton(MakeIdent("dontdeleteplg"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SWEEPCONSTEDGE_DO_NOT_DELETE,YSFALSE);
	SetRadioButtonGroup(2,deleteOrDontDeleteSrcPlg);
	deleteOrDontDeleteSrcPlg[1]->SetCheck(YSTRUE);

	useQuadBtn=AddTextButton(MakeIdent("usequad"),FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_SWEEPCONSTEDGE_USEQUAD,YSTRUE);

	invertBtn=AddTextButton(MakeIdent("invert"),FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_SWEEPCONSTEDGE_INVERT,YSTRUE);
	okBtn=AddTextButton(MakeIdent("ok"),FSKEY_SPACE,FSGUI_PUSHBUTTON,L"Sweep",YSTRUE);
	cancelBtn=AddTextButton(MakeIdent("cancel"),FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	Fit();
}

void GeblGuiEditorBase::Edit_Sweep_RotationalWithPathDialog::RemakeDrawingBuffer(void)
{
	lineBuf.CleanUp();
	triBuf.CleanUp();

	for(auto &patch : multiPatch.GetPatchSequence())
	{
		const auto &shl=patch.shl;
		YsShellEdgeEnumHandle edHd=NULL;
		while(YSOK==shl.MoveToNextEdge(edHd))
		{
			YsShellVertexHandle edVtHd[2];
			shl.GetEdge(edVtHd,edHd);

			lineBuf.AddVertex(shl.GetVertexPosition(edVtHd[0]));
			lineBuf.AddVertex(shl.GetVertexPosition(edVtHd[1]));
		}

		for(auto plHd : shl.AllPolygon())
		{
			YsArray <YsVec3,4> plVtPos;
			shl.GetPolygon(plVtPos,plHd);

			YsBoundingBoxMaker3 mkBbx(plVtPos);
			double l=mkBbx.GetDiagonalLength();

			const YsVec3 cen=shl.GetCenter(plHd);
			const YsVec3 nom=shl.GetNormal(plHd);
			const YsVec3 arrowHead=cen+nom*l/5.0;
			lineBuf.AddVertex(cen);
			lineBuf.AddVertex(arrowHead);

			triBuf.AddArrowHeadTriangle(cen,arrowHead,0.1);
		}
	}
}

void GeblGuiEditorBase::Edit_Sweep_RotationalWithPathDialog::SetUp(const YsShellExt &shl,const YsConstArrayMask <YsShell::VertexHandle> &seg0,const YsConstArrayMask <YsShell::VertexHandle> &seg1,const YsConstArrayMask <YsShell::VertexHandle> &path)
{
	multiPatch.SetUpFromFirstLastCrossSectionAndPathBetween(shl,seg0,seg1,path);
}

/* virtual */ void GeblGuiEditorBase::Edit_Sweep_RotationalWithPathDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		canvasPtr->Edit_Sweep_RotationalWithPath_Sweep();
	}
	else if(btn==cancelBtn)
	{
		canvasPtr->Edit_ClearUIMode();
	}
	else if(btn==invertBtn)
	{
		multiPatch.Invert();
		RemakeDrawingBuffer();
		canvasPtr->SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Edit_Sweep_RotationalWithPath(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto &shl=*slHd;
		auto selPlHd=shl.GetSelectedPolygon();
		auto selCeHd=shl.GetSelectedConstEdge();
		if(3!=selPlHd.size()+selCeHd.size() || selCeHd.empty())
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_ATLEAST_2PLGORCE_PLUS_1CE_NEEDED);
			return;
		}

		YsArray <YsShell::PolygonHandle> srcPlHd;
		YsArray <YsShellExt::ConstEdgeHandle> srcCeHd;
		YSBOOL sectionIsLoop[2]={YSFALSE,YSFALSE};
		YsArray <YsShell::VertexHandle> section[2],path;
		for(int i=0; i<2; ++i)
		{
			if(0<selPlHd.size())
			{
				sectionIsLoop[i]=YSTRUE;
				section[i]=shl.GetPolygonVertex(selPlHd[0]);
				srcPlHd.push_back(selPlHd[0]);
				selPlHd.Delete(0);
			}
			else if(0<selCeHd.size())
			{
				shl.GetConstEdge(section[i],sectionIsLoop[i],selCeHd[0]);
				srcCeHd.push_back(selCeHd[0]);
				selCeHd.Delete(0);
			}
		}
		if(selCeHd.empty())
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_ATLEAST_2PLGORCE_PLUS_1CE_NEEDED);
			return;
		}
		path=shl.GetConstEdgeVertex(selCeHd[0]);

		YsArray <YsShell::VertexHandle> seg[2];
		if(YSOK!=YsExtractNonSharedPart<YsShell::VertexHandle>(seg[0],seg[1],section[0],sectionIsLoop[0],section[1],sectionIsLoop[1]))
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_ROTATIONALSWEEP_CANNOTEXTRACTSHAREDPARTS);
			return;
		}

		Edit_ClearUIMode();
		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<Edit_Sweep_RotationalWithPathDialog>();
		dlg->Make(this);
		dlg->SetUp(shl.Conv(),seg[0],seg[1],path);
		dlg->srcPlHd=srcPlHd;
		dlg->srcCeHd=srcCeHd;

		draw3dCallBack=std::bind(&THISCLASS::Edit_Sweep_RotationalWithPath_Draw3DCallBack,this);
		spaceKeyCallBack=std::bind(&THISCLASS::Edit_Sweep_RotationalWithPath_Sweep,this);

		AddDialog(dlg);
		ArrangeDialog();
	}
}
void GeblGuiEditorBase::Edit_Sweep_RotationalWithPath_Draw3DCallBack(void)
{
	auto dlg=FindTypedModelessDialog<Edit_Sweep_RotationalWithPathDialog>();
	if(nullptr!=dlg)
	{
		dlg->RemakeDrawingBuffer();

		struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
		YsGLSLUse3DRenderer(renderer);

		const GLfloat edgeCol[4]={0,1.0f,0,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)dlg->lineBuf.GetNumVertex(),dlg->lineBuf);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_TRIANGLES,(int)dlg->triBuf.GetNumVertex(),dlg->triBuf);

		YsGLSLEndUse3DRenderer(renderer);
	}
}
void GeblGuiEditorBase::Edit_Sweep_RotationalWithPath_Sweep(void)
{
	auto dlg=FindTypedModelessDialog<Edit_Sweep_RotationalWithPathDialog>();
	if(nullptr!=dlg && nullptr!=slHd)
	{
		YsShellExtEdit::StopIncUndo incUndo(slHd);

		const YsColor col=colorPaletteDlg->GetColor();

		if(YSTRUE==dlg->useQuadBtn->GetCheck())
		{
			dlg->multiPatch.MergeTriFormQuad();
		}
		dlg->multiPatch.CreateMissingVertexInOriginalShell(*slHd);
		dlg->multiPatch.TransferPatchToOriginalShell(*slHd,col);

		slHd->SelectConstEdge(0,NULL);
		slHd->SelectPolygon(0,NULL);

		if(YSTRUE==dlg->deleteOrDontDeleteSrcPlg[0]->GetCheck())
		{
			slHd->DeleteMultiPolygon(dlg->srcPlHd);
		}
	}

	needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_POLYGON;
	SetNeedRedraw(YSTRUE);


	Edit_ClearUIMode();
}

////////////////////////////////////////////////////////////

PolyCreExtendOrShrinkDialog::PolyCreExtendOrShrinkDialog()
{
	canvas=NULL;
	okBtn=NULL;
	cancelBtn=NULL;
}

PolyCreExtendOrShrinkDialog::~PolyCreExtendOrShrinkDialog()
{
}

/*static*/ PolyCreExtendOrShrinkDialog *PolyCreExtendOrShrinkDialog::Create(GeblGuiEditorBase *canvas)
{
	PolyCreExtendOrShrinkDialog *dlg=new PolyCreExtendOrShrinkDialog;
	dlg->canvas=canvas;
	return dlg;
}

/*static*/ void PolyCreExtendOrShrinkDialog::Delete(PolyCreExtendOrShrinkDialog *ptr)
{
	delete ptr;
}

YSRESULT PolyCreExtendOrShrinkDialog::Make(class YsShellExtEdit &shl,const YsShellPolygonStore &targetPlHdStore)
{
	this->shl=&shl;
	this->targetPlHdArray.CleanUp();
	for(auto plHd : targetPlHdStore)
	{
		this->targetPlHdArray.Append(plHd);
	}

	YsShellExt_BoundaryInfo boundaryInfo;
	boundaryInfo.MakeInfo((const YsShell &)shl,targetPlHdArray);
	boundaryInfo.CacheNonDuplicateVtHdArray();
	auto allVtHd=boundaryInfo.GetVertexAll();


	YsArray <YsVec3> allVtPos;
	for(auto vtHd : allVtHd)
	{
		allVtPos.Append(shl.GetVertexPosition(vtHd));
	}
	if(YSOK!=YsFindLeastSquareFittingPlaneNormal(moveDir,allVtPos))
	{
		return YSERR;
	}


	extensionTarget.CleanUp();

	for(auto vtHd : allVtHd)
	{
		YsShellVertexHandle hairVtHd=NULL;
		auto connVtHdArray=shl.GetConnectedVertex(vtHd);
		for(auto connVtHd : connVtHdArray)
		{
			YsArray <YsShellPolygonHandle> edPlHdAll;
			shl.FindPolygonFromEdgePiece(edPlHdAll,vtHd,connVtHd);

			YSBOOL isUsedByTargetPlHd=YSFALSE;
			for(auto edPlHd : edPlHdAll)
			{
				if(YSTRUE==targetPlHdStore.IsIncluded(edPlHd))
				{
					isUsedByTargetPlHd=YSTRUE;
					break;
				}
			}
			if(YSTRUE!=isUsedByTargetPlHd)
			{
				if(NULL!=hairVtHd)
				{
					return YSERR;  // Hair not unique
				}
				else
				{
					hairVtHd=connVtHd;
				}
			}
		}
		if(NULL==hairVtHd)
		{
			return YSERR; // Hiar not found
		}

		auto org=shl.GetVertexPosition(vtHd);
		auto from=shl.GetVertexPosition(hairVtHd);
		auto vec=org-from;
		if(YSOK!=vec.Normalize())
		{
			return YSERR;
		}

		extensionTarget.Increment();
		extensionTarget.Last().toVtHd=vtHd;
		extensionTarget.Last().fromVtHd=hairVtHd;
		extensionTarget.Last().org=org;
		extensionTarget.Last().vec=vec;
		extensionTarget.Last().newPos=org;
	}

	center=boundaryInfo.GetCenter((const YsShellExt &)shl);

	if(NULL==okBtn)
	{
		FsGuiDialog::Initialize();
		okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(0.3);
		Fit();
	}

	RemakeDrawingBuffer();

	return YSOK;
}

void PolyCreExtendOrShrinkDialog::RemakeDrawingBuffer(void)
{
	lineBuf.CleanUp();
	triBuf.CleanUp();

	YsPlane prjPln(center,moveDir);
	for(auto &ext : extensionTarget)
	{
		if(YSOK==prjPln.GetIntersectionHighPrecision(ext.newPos,ext.org,ext.vec))
		{
			lineBuf.AddVertex(ext.org);
			lineBuf.AddVertex(ext.newPos);

			triBuf.AddArrowHeadTriangle(ext.org,ext.newPos,0.1);
		}
	}
}

void PolyCreExtendOrShrinkDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		canvas->SpaceKeyCallBack(*canvas);
	}
	else if(cancelBtn==btn)
	{
		canvas->Edit_ClearUIMode();
	}
}



void GeblGuiEditorBase::Edit_Sweep_ExtendOrShrink(FsGuiPopUpMenuItem *)
{
	if(NULL==Slhd())
	{
		return;
	}

	YsShellExtEdit &shl=*Slhd();
	auto selPlHd=shl.GetSelectedPolygon();
	auto selFgHd=shl.GetSelectedFaceGroup();

	YsShellPolygonStore targetPlHd((const YsShell &)shl);
	targetPlHd.AddPolygon(selPlHd);
	for(auto fgHd : selFgHd)
	{
		YsArray <YsShellPolygonHandle> fgPlHd;
		shl.GetFaceGroup(fgPlHd,fgHd);
		targetPlHd.AddPolygon(fgPlHd);
	}

	if(0<targetPlHd.GetN())
	{
		if(YSOK==extendOrShrinkDlg->Make(shl,targetPlHd))
		{
			Edit_ClearUIMode();

			AddDialog(extendOrShrinkDlg);
			ArrangeDialog();

			UIDrawCallBack3D=Edit_Sweep_ExtendOrShrink_DrawCallBack3D;
			SpaceKeyCallBack=Edit_Sweep_ExtendOrShrink_SpaceKeyCallBack;

			// Set up 3D interface if necessary  Yes, I do need 3D itfc
			threeDInterface.BeginInputPoint2(extendOrShrinkDlg->center);
			threeDInterface.BindCallBack(&THISCLASS::Edit_Sweep_ExtendOrShrink_3DInterfaceCallBack,this);

			undoRedoRequireToClearUIMode=YSTRUE;
			deletionRequireToClearUIMode=YSTRUE;
		}
	}
	else
	{
		// Tell the user something is missing.
	}
}

/*static*/ void GeblGuiEditorBase::Edit_Sweep_ExtendOrShrink_DrawCallBack3D(GeblGuiEditorBase &canvas)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
	YsGLSLUse3DRenderer(renderer);

	const GLfloat edgeCol[4]={0,1.0f,0,1.0f};
	YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,(int)canvas.extendOrShrinkDlg->lineBuf.GetNumVertex(),canvas.extendOrShrinkDlg->lineBuf);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_TRIANGLES,(int)canvas.extendOrShrinkDlg->triBuf.GetNumVertex(),canvas.extendOrShrinkDlg->triBuf);

	YsGLSLEndUse3DRenderer(renderer);
}

void GeblGuiEditorBase::Edit_Sweep_ExtendOrShrink_3DInterfaceCallBack(class FsGui3DInterface *itfc)
{
	extendOrShrinkDlg->center=itfc->point_pos;
	extendOrShrinkDlg->RemakeDrawingBuffer();
	SetNeedRedraw(YSTRUE);
}

/*static*/ YSRESULT GeblGuiEditorBase::Edit_Sweep_ExtendOrShrink_SpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	return canvas.Edit_Sweep_ExtendOrShrink_SpaceKeyCallBack();
}

YSRESULT GeblGuiEditorBase::Edit_Sweep_ExtendOrShrink_SpaceKeyCallBack(void)
{
	if(NULL==Slhd())
	{
		return YSERR;
	}
	YsShellExtEdit &shl=*Slhd();

	YsShellExtEdit::StopIncUndo incUndo(shl);
	for(auto &target : extendOrShrinkDlg->extensionTarget)
	{
		shl.SetVertexPosition(target.toVtHd,target.newPos);
	}

	shl.SelectConstEdge(0,NULL);
	shl.SelectPolygon(0,NULL);

	needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	SetNeedRedraw(YSTRUE);

	Edit_ClearUIMode();

	return YSOK;
}

////////////////////////////////////////////////////////////

