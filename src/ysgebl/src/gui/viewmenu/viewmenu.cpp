/* ////////////////////////////////////////////////////////////

File Name: viewmenu.cpp
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

#include <ysclass.h>
#include <ysport.h>
#include <ysgl.h>
#include <ysshellext_geomcalc.h>
#include <ysshellext_unfoldingutil.h>

#include <fsguifiledialog.h>

#include "../textresource.h"
#include "../ysgebl_gui_editor_base.h"
#include "../miscdlg.h"
#include "../refbmp/refbmp.h"
#include "../refbmp/refbmpdialog.h"



void GeblGuiEditorBase::View_RefreshBuffer(FsGuiPopUpMenuItem *)
{
	needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
	needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_ALL;

	auto allNode=shlGrp.GetNodePointerAll();
	for(auto shl : allNode)
	{
		shl->SetNeedRemakeHighlightVertexBuffer(YSTRUE);
	}

	SetNeedRedraw(YSTRUE);
}

void GeblGuiEditorBase::View_ResetViewRange(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		viewControlDlg->SaveLastView();

		auto &shl=*Slhd();
		YsVec3 bbx[2];
		shl.GetBoundingBox(bbx);
		drawEnv.SetViewTarget((bbx[0]+bbx[1])/2.0);
		drawEnv.SetTargetBoundingBox(bbx);
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::View_ResetViewOption(FsGuiPopUpMenuItem *)
{
	SetDefaultViewOption();
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}

void GeblGuiEditorBase::View_DrawAxis(FsGuiPopUpMenuItem *)
{
	YsFlip(drawAxis);
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}

void GeblGuiEditorBase::View_OpenRefBmpDialog(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	canvas.Edit_ClearUIMode();

	if(NULL==canvas.refBmpDlg)
	{
		canvas.refBmpDlg=PolyCreRefBmpDialog::Create();
	}
	canvas.refBmpDlg->Make(&canvas,canvas.refBmpStore);
	canvas.AddDialog(canvas.refBmpDlg);

	canvas.UIDrawCallBack3D=View_OpenRefBmpDialog_UIDrawCallBack3D;

	canvas.threeDInterface.SetCallBack(PolyCreRefBmpDialog::ThreeDInterfaceCallBack,canvas.refBmpDlg);

	canvas.ArrangeDialog();
	canvas.SetNeedRedraw(YSTRUE);
}

/*static*/ void GeblGuiEditorBase::View_OpenRefBmpDialog_UIDrawCallBack3D(GeblGuiEditorBase &canvas)
{
	const int sel=canvas.refBmpDlg->GetSelectedBitmapIndex();
	canvas.refBmpStore->DrawHighlight(sel);
}



void GeblGuiEditorBase::View_DrawVertex(FsGuiPopUpMenuItem *)
{
	YsFlip(drawVertex);
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}

void GeblGuiEditorBase::View_DrawConstEdge(FsGuiPopUpMenuItem *)
{
	YsFlip(drawConstEdge);
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}

void GeblGuiEditorBase::View_DrawPolygonFill(FsGuiPopUpMenuItem *)
{
	YsFlip(drawPolygon);
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}


void GeblGuiEditorBase::View_DrawWireframe(FsGuiPopUpMenuItem *)
{
	YsFlip(drawPolygonEdge);
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}

void GeblGuiEditorBase::View_DrawBackFaceInDifferentColor(FsGuiPopUpMenuItem *)
{
	YsFlip(drawBackFaceInDifferentColor);
	SetMenuCheck();
	needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_POLYGON;
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}

void GeblGuiEditorBase::View_DrawOtherShellWireframe(FsGuiPopUpMenuItem *)
{
	drawOtherShellWireframe=YSTRUE;
	drawOtherShellFill=YSFALSE;
	needRemakeDrawingBuffer_for_OtherShell|=NEED_REMAKE_DRAWING_POLYGON;
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}
void GeblGuiEditorBase::View_DrawOtherShellFill(FsGuiPopUpMenuItem *)
{
	drawOtherShellWireframe=YSFALSE;
	drawOtherShellFill=YSTRUE;
	needRemakeDrawingBuffer_for_OtherShell|=NEED_REMAKE_DRAWING_POLYGON;
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}
void GeblGuiEditorBase::View_DrawOtherShellNone(FsGuiPopUpMenuItem *)
{
	drawOtherShellWireframe=YSFALSE;
	drawOtherShellFill=YSFALSE;
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}

void GeblGuiEditorBase::View_DrawShrunkPolygon(FsGuiPopUpMenuItem *)
{
	YsFlip(drawShrunkPolygon);
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
	SaveViewPreference();
}



void GeblGuiEditorBase::View_FocusOn_SelectionCenterOfGravity(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		YsVec3 sum=YsOrigin();
		YSSIZE_T nSample=0;

		YsArray <YsShellVertexHandle> selVtHd;
		canvas.slHd->GetSelectedVertex(selVtHd);
		for(YSSIZE_T idx=0; idx<selVtHd.GetN(); ++idx)
		{
			YsVec3 vtPos;
			canvas.slHd->GetVertexPosition(vtPos,selVtHd[idx]);
			sum+=vtPos;
			++nSample;
		}

		YsArray <YsShellPolygonHandle> selPlHd;
		canvas.slHd->GetSelectedPolygon(selPlHd);
		for(YSSIZE_T idx=0; idx<selPlHd.GetN(); ++idx)
		{
			YsArray <YsVec3,4> plVtPos;
			canvas.slHd->GetVertexListOfPolygon(plVtPos,selPlHd[idx]);
			for(YSSIZE_T vtIdx=0; vtIdx<plVtPos.GetN(); ++vtIdx)
			{
				sum+=plVtPos[vtIdx];
				++nSample;
			}
		}

		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		canvas.slHd->GetSelectedConstEdge(selCeHd);
		for(YSSIZE_T idx=0; idx<selCeHd.GetN(); ++idx)
		{
			YSBOOL isLoop;
			YsArray <YsShellVertexHandle,16> ceVtHd;
			canvas.slHd->GetConstEdge(ceVtHd,isLoop,selCeHd[idx]);
			for(YSSIZE_T vtIdx=0; vtIdx<ceVtHd.GetN(); ++vtIdx)
			{
				YsVec3 vtPos;
				canvas.slHd->GetVertexPosition(vtPos,ceVtHd[vtIdx]);
				sum+=vtPos;
				++nSample;
			}
		}

		YsArray <YsShellExt::FaceGroupHandle> selFgHd;
		canvas.slHd->GetSelectedFaceGroup(selFgHd);
		for(YSSIZE_T idx=0; idx<selFgHd.GetN(); ++idx)
		{
			YsArray <YsShellPolygonHandle,16> fgPlHd;
			canvas.slHd->GetFaceGroup(fgPlHd,selFgHd[idx]);
			for(YSSIZE_T plIdx=0; plIdx<fgPlHd.GetN(); ++plIdx)
			{
				YsArray <YsVec3,4> plVtPos;
				canvas.slHd->GetVertexListOfPolygon(plVtPos,fgPlHd[plIdx]);
				for(YSSIZE_T vtIdx=0; vtIdx<plVtPos.GetN(); ++vtIdx)
				{
					sum+=plVtPos[vtIdx];
					++nSample;
				}
			}
		}

		if(0<nSample)
		{
			const YsVec3 focusOn=sum/(double)nSample;
			canvas.drawEnv.SetViewTarget(focusOn);
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}


void GeblGuiEditorBase::View_FocusOn_SelectionBoundingBoxCenter(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		YsBoundingBoxMaker3 mkBbx;
		YSSIZE_T nSample=0;

		YsArray <YsShellVertexHandle> selVtHd;
		canvas.slHd->GetSelectedVertex(selVtHd);
		for(YSSIZE_T idx=0; idx<selVtHd.GetN(); ++idx)
		{
			YsVec3 vtPos;
			canvas.slHd->GetVertexPosition(vtPos,selVtHd[idx]);
			mkBbx.Add(vtPos);
			++nSample;
		}

		YsArray <YsShellPolygonHandle> selPlHd;
		canvas.slHd->GetSelectedPolygon(selPlHd);
		for(YSSIZE_T idx=0; idx<selPlHd.GetN(); ++idx)
		{
			YsArray <YsVec3,4> plVtPos;
			canvas.slHd->GetVertexListOfPolygon(plVtPos,selPlHd[idx]);
			for(YSSIZE_T vtIdx=0; vtIdx<plVtPos.GetN(); ++vtIdx)
			{
				mkBbx.Add(plVtPos[vtIdx]);
				++nSample;
			}
		}

		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		canvas.slHd->GetSelectedConstEdge(selCeHd);
		for(YSSIZE_T idx=0; idx<selCeHd.GetN(); ++idx)
		{
			YSBOOL isLoop;
			YsArray <YsShellVertexHandle,16> ceVtHd;
			canvas.slHd->GetConstEdge(ceVtHd,isLoop,selCeHd[idx]);
			for(YSSIZE_T vtIdx=0; vtIdx<ceVtHd.GetN(); ++vtIdx)
			{
				YsVec3 vtPos;
				canvas.slHd->GetVertexPosition(vtPos,ceVtHd[vtIdx]);
				mkBbx.Add(vtPos);
				++nSample;
			}
		}

		YsArray <YsShellExt::FaceGroupHandle> selFgHd;
		canvas.slHd->GetSelectedFaceGroup(selFgHd);
		for(YSSIZE_T idx=0; idx<selFgHd.GetN(); ++idx)
		{
			YsArray <YsShellPolygonHandle,16> fgPlHd;
			canvas.slHd->GetFaceGroup(fgPlHd,selFgHd[idx]);
			for(YSSIZE_T plIdx=0; plIdx<fgPlHd.GetN(); ++plIdx)
			{
				YsArray <YsVec3,4> plVtPos;
				canvas.slHd->GetVertexListOfPolygon(plVtPos,fgPlHd[plIdx]);
				for(YSSIZE_T vtIdx=0; vtIdx<plVtPos.GetN(); ++vtIdx)
				{
					mkBbx.Add(plVtPos[vtIdx]);
					++nSample;
				}
			}
		}

		if(0<nSample)
		{
			YsVec3 bbxCen;
			mkBbx.GetCenter(bbxCen);
			canvas.drawEnv.SetViewTarget(bbxCen);
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}


void GeblGuiEditorBase::View_FocusOn_Vertex(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		canvas.Edit_ClearUIMode();
		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;
		canvas.LButtonCallBack=View_FocusOn_Vertex_LButtonCallBack;
		canvas.SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::View_FocusOn_Vertex_RKey(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		int lb,mb,rb;
		int mx,my;
		FsGetMouseEvent(lb,mb,rb,mx,my);
		canvas.View_FocusOn_Vertex_Click(mx,my);
	}
}

YSRESULT GeblGuiEditorBase::View_FocusOn_Vertex_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL,YSBOOL,YSBOOL,int mx,int my)
{
	return canvas.View_FocusOn_Vertex_Click(mx,my);
}

YSRESULT GeblGuiEditorBase::View_FocusOn_Vertex_Click(int mx,int my)
{
	if(NULL!=slHd)
	{
		YsShellVertexHandle vtHd=PickedVtHd(mx,my,0,NULL);
		if(NULL!=vtHd)
		{
			YsVec3 vtPos;
			slHd->GetVertexPosition(vtPos,vtHd);
			drawEnv.SetViewTarget(vtPos);
			SetNeedRedraw(YSTRUE);
			return YSOK;
		}
	}
	return YSERR;
}



void GeblGuiEditorBase::View_FocusOn_Polygon(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		canvas.Edit_ClearUIMode();
		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;
		canvas.LButtonCallBack=View_FocusOn_Polygon_LButtonCallBack;
		canvas.SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::View_FocusOn_Polygon_ShiftRKey(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		int lb,mb,rb;
		int mx,my;
		FsGetMouseEvent(lb,mb,rb,mx,my);
		canvas.View_FocusOn_Polygon_Click(mx,my,YSFALSE);
	}
}

YSRESULT GeblGuiEditorBase::View_FocusOn_Polygon_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL,YSBOOL,YSBOOL,int mx,int my)
{
	return canvas.View_FocusOn_Polygon_Click(mx,my,YSFALSE);
}

void GeblGuiEditorBase::View_FocusOn_PolygonStraight(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		canvas.Edit_ClearUIMode();
		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;
		canvas.LButtonCallBack=View_FocusOn_PolygonStraight_LButtonCallBack;
		canvas.SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::View_FocusOn_Polygon_ShiftCtrlRKey(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		int lb,mb,rb;
		int mx,my;
		FsGetMouseEvent(lb,mb,rb,mx,my);
		canvas.View_FocusOn_Polygon_Click(mx,my,YSTRUE);
	}
}

YSRESULT GeblGuiEditorBase::View_FocusOn_PolygonStraight_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL,YSBOOL,YSBOOL,int mx,int my)
{
	return canvas.View_FocusOn_Polygon_Click(mx,my,YSTRUE);
}

YSRESULT GeblGuiEditorBase::View_FocusOn_Polygon_Click(int mx,int my,YSBOOL alsoChangeOrientation)
{
	if(NULL!=slHd)
	{
		YsShellPolygonHandle plHd=PickedPlHd(mx,my,0,NULL);
		if(NULL!=plHd)
		{
			YsVec3 cen;
			slHd->GetCenterOfPolygon(cen,plHd);
			drawEnv.SetViewTarget(cen);

			if(YSTRUE==alsoChangeOrientation)
			{
				YsVec3 nom;
				slHd->GetNormalOfPolygon(nom,plHd);
				if(YsOrigin()==nom)
				{
					YsArray <YsVec3,4> plVtPos;
					slHd->GetVertexListOfPolygon(plVtPos,plHd);
					YsGetAverageNormalVector(nom,plVtPos.GetN(),plVtPos);
				}

				if(YsOrigin()!=nom)
				{
					YsVec3 curViewDir,newViewDir;
					drawEnv.GetViewDirection(curViewDir);
					if(0.0<=curViewDir*nom)
					{
						newViewDir=nom;
					}
					else
					{
						newViewDir=-nom;
					}
					drawEnv.SetViewDirection(newViewDir);
				}
			}

			SetNeedRedraw(YSTRUE);
			return YSOK;
		}
	}
	return YSERR;
}


void GeblGuiEditorBase::View_ApplyViewPortToAllShell(void)
{
	if(NULL!=Slhd())
	{
		const YsVec3 viewPort[2]=
		{
			Slhd()->GetDrawingBuffer().viewPort[0],
			Slhd()->GetDrawingBuffer().viewPort[1]
		};
		YSBOOL enabled=Slhd()->GetDrawingBuffer().viewPortEnabled;

		auto allShlPtr=shlGrp.GetNodePointerAll();
		for(auto shlPtr : allShlPtr)
		{
			shlPtr->GetDrawingBuffer().viewPort[0]=viewPort[0];
			shlPtr->GetDrawingBuffer().viewPort[1]=viewPort[1];
			shlPtr->GetDrawingBuffer().viewPortEnabled=enabled;
		}

		if(YSTRUE==enabled)
		{
			needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_ALL;
			SetNeedRedraw(YSTRUE);
		}
	}
}

/*static*/ void GeblGuiEditorBase::View_UseSameViewPortForAllShell(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		YsFlip(canvas.drawApplyViewPortToAllShell);
		canvas.SetMenuCheck();
		if(YSTRUE==canvas.drawApplyViewPortToAllShell)
		{
			canvas.View_ApplyViewPortToAllShell();
		}
	}
}

void GeblGuiEditorBase::View_EnableViewPort(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		YsFlip(canvas.slHd->GetDrawingBuffer().viewPortEnabled);

		if(YSTRUE==canvas.slHd->GetDrawingBuffer().viewPortEnabled &&
		   YSTRUE==canvas.drawApplyViewPortToAllShell)
		{
			canvas.View_ApplyViewPortToAllShell();
		}

		canvas.SetMenuCheck();
		canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
		canvas.SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::View_ResetViewPort(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		canvas.shlGrp.GetBoundingBox(canvas.slHd->GetDrawingBuffer().viewPort);
		if(YSTRUE==canvas.slHd->GetDrawingBuffer().viewPortEnabled)
		{
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);

			if(YSTRUE==canvas.drawApplyViewPortToAllShell)
			{
				canvas.View_ApplyViewPortToAllShell();
			}
		}
	}
}

void GeblGuiEditorBase::View_MoveViewPort(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		canvas.Edit_ClearUIMode();
		canvas.undoRedoRequireToClearUIMode=YSFALSE;
		canvas.deletionRequireToClearUIMode=YSFALSE;
		canvas.threeDInterface.BeginInputBox(canvas.slHd->GetDrawingBuffer().viewPort[0],canvas.slHd->GetDrawingBuffer().viewPort[1],YSTRUE,YSFALSE);
		canvas.threeDInterface.SetCallBack(View_MoveViewPort_BoxMoved,appPtr);
		canvas.SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::View_MoveViewPort_BoxMoved(void *appPtr,FsGui3DInterface &itfc)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		itfc.GetBox(canvas.slHd->GetDrawingBuffer().viewPort);

		if(YSTRUE==canvas.drawApplyViewPortToAllShell)
		{
			canvas.View_ApplyViewPortToAllShell();
		}

		if(YSTRUE==canvas.slHd->GetDrawingBuffer().viewPortEnabled)
		{
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}

/*static*/ void GeblGuiEditorBase::View_MoveViewPortToRotationCenter(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		const YsVec3 d=canvas.slHd->GetDrawingBuffer().viewPort[1]-canvas.slHd->GetDrawingBuffer().viewPort[0];

		auto viewTarget=canvas.drawEnv.GetViewTarget();
		canvas.slHd->GetDrawingBuffer().viewPort[0]=viewTarget-d/2.0;
		canvas.slHd->GetDrawingBuffer().viewPort[1]=viewTarget+d/2.0;

		if(YSTRUE==canvas.drawApplyViewPortToAllShell)
		{
			canvas.View_ApplyViewPortToAllShell();
		}

		if(YSTRUE==canvas.slHd->GetDrawingBuffer().viewPortEnabled)
		{
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}

/*static*/ void GeblGuiEditorBase::View_ExpandViewPort(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		const YsVec3 d=(canvas.slHd->GetDrawingBuffer().viewPort[1]-canvas.slHd->GetDrawingBuffer().viewPort[0])*1.1;
		const YsVec3 c=(canvas.slHd->GetDrawingBuffer().viewPort[1]+canvas.slHd->GetDrawingBuffer().viewPort[0])/2.0;

		canvas.slHd->GetDrawingBuffer().viewPort[0]=c-d/2.0;
		canvas.slHd->GetDrawingBuffer().viewPort[1]=c+d/2.0;;

		if(YSTRUE==canvas.drawApplyViewPortToAllShell)
		{
			canvas.View_ApplyViewPortToAllShell();
		}

		if(YSTRUE==canvas.slHd->GetDrawingBuffer().viewPortEnabled)
		{
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}

/*static*/ void GeblGuiEditorBase::View_ShrinkViewPort(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		const YsVec3 d=(canvas.slHd->GetDrawingBuffer().viewPort[1]-canvas.slHd->GetDrawingBuffer().viewPort[0])/1.1;
		const YsVec3 c=(canvas.slHd->GetDrawingBuffer().viewPort[1]+canvas.slHd->GetDrawingBuffer().viewPort[0])/2.0;

		canvas.slHd->GetDrawingBuffer().viewPort[0]=c-d/2.0;
		canvas.slHd->GetDrawingBuffer().viewPort[1]=c+d/2.0;;

		if(YSTRUE==canvas.drawApplyViewPortToAllShell)
		{
			canvas.View_ApplyViewPortToAllShell();
		}

		if(YSTRUE==canvas.slHd->GetDrawingBuffer().viewPortEnabled)
		{
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}

/*static*/ void GeblGuiEditorBase::View_ViewPortFromSelection(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		auto &shl=*canvas.slHd;

		YsShellExtEdit_DuplicateUtil dupUtil;

		auto selVtHd=shl.GetSelectedVertex();
		auto selPlHd=shl.GetSelectedPolygon();
		auto selFgHd=shl.GetSelectedFaceGroup();
		auto selCeHd=shl.GetSelectedConstEdge();
		// auto selVlHd=canvas.shl.GetSelectedVolume();

		dupUtil.CleanUp();
		dupUtil.SetShell(shl.Conv());
		dupUtil.AddVertex(selVtHd);
		dupUtil.AddPolygon(selPlHd);
		dupUtil.AddConstEdge(selCeHd);
		dupUtil.AddFaceGroup(selFgHd);
		// dupUtil.AddVolume(selVlHd);

		YsBoundingBoxMaker3 mkBbx;
		for(auto &vtx : dupUtil.vertexArray)
		{
			mkBbx.Add(vtx.pos);
		}

		YsVec3 bbx[2];
		mkBbx.Get(bbx);

		bbx[0].SubX(YsTolerance);
		bbx[0].SubY(YsTolerance);
		bbx[0].SubZ(YsTolerance);

		bbx[1].AddX(YsTolerance);
		bbx[1].AddY(YsTolerance);
		bbx[1].AddZ(YsTolerance);

		canvas.slHd->GetDrawingBuffer().viewPort[0]=bbx[0];
		canvas.slHd->GetDrawingBuffer().viewPort[1]=bbx[1];

		if(YSTRUE==canvas.drawApplyViewPortToAllShell)
		{
			canvas.View_ApplyViewPortToAllShell();
		}

		if(YSTRUE==canvas.slHd->GetDrawingBuffer().viewPortEnabled)
		{
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}


/* static */ void GeblGuiEditorBase::View_CrossSection_Enable(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		canvas.slHd->GetDrawingBuffer().ToggleCrossSection();
		canvas.SetMenuCheck();
		canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
		canvas.SetNeedRedraw(YSTRUE);
	}
}

/* static */ void GeblGuiEditorBase::View_CrossSection_Reset(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		const YsVec3 o=canvas.drawEnv.GetViewTarget();
		const YsVec3 n=canvas.drawEnv.GetViewDirection();
		const YsPlane pln(o,n);

		canvas.slHd->GetDrawingBuffer().SetCrossSection(pln);
		if(YSTRUE==canvas.slHd->GetDrawingBuffer().CrossSectionEnabled())
		{
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}

/* static */ void GeblGuiEditorBase::View_CrossSection_Invert(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		const YsVec3 o=canvas.drawEnv.GetViewTarget();
		const YsVec3 n=canvas.drawEnv.GetViewDirection();
		const YsPlane pln(o,-n);

		canvas.slHd->GetDrawingBuffer().SetCrossSection(pln);
		if(YSTRUE==canvas.slHd->GetDrawingBuffer().CrossSectionEnabled())
		{
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}



/*static*/ void GeblGuiEditorBase::View_LoadRefBmpConfig(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;

	YsString dataDir;
	dataDir.EncodeUTF8 <wchar_t> (canvas.GetDataDir());

	YsString def;
	def.MakeFullPathName(dataDir,"*.rfb");

	// YsString last(scn.GetLastOpenedFilename()),def;
	// if(last.Strlen()>0)
	// {
	// 	YsString pth,fil;
	// 	last.SeparatePathFile(pth,fil);
	// 	def.MakeFullPathName(pth,"*.fld");
	// }
	// else
	// {
	// 	def.MakeFullPathName(scnDir,"*.fld");
	// }

	canvas.fdlg->Initialize();
	canvas.fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	canvas.fdlg->multiSelect=YSFALSE;
	canvas.fdlg->title.Set(L"Open Reference Bitmap Configuration");
	canvas.fdlg->fileExtensionArray.Append(L".rfb");
	canvas.fdlg->defaultFileName.SetUTF8String(def);
	canvas.fdlg->SetCloseModalCallBack(View_LoadRefBmpConfig_FileSelected);
	canvas.AttachModalDialog(canvas.fdlg);
}

/*static*/ void GeblGuiEditorBase::View_LoadRefBmpConfig_FileSelected(FsGuiControlBase *appPtr,FsGuiDialog *,int returnValue)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;

	if(YSOK==(YSRESULT)returnValue)
	{
		if(NULL!=canvas.refBmpStore)
		{
			canvas.refBmpStore->Load(canvas.fdlg->selectedFileArray[0]);
			canvas.SetNeedRedraw(YSTRUE);
			canvas.fdlg->SetCloseModalCallBack(nullptr);
		}
	}
}



/*static*/ void GeblGuiEditorBase::View_SaveRefBmpConfig(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;

	YsString dataDir;
	dataDir.EncodeUTF8 <wchar_t> (canvas.GetDataDir());

	YsString def;
	def.MakeFullPathName(dataDir,"*.rfb");

	// YsString last(scn.GetLastOpenedFilename()),def;
	// if(last.Strlen()>0)
	// {
	// 	YsString pth,fil;
	// 	last.SeparatePathFile(pth,fil);
	// 	def.MakeFullPathName(pth,"*.fld");
	// }
	// else
	// {
	// 	def.MakeFullPathName(scnDir,"*.fld");
	// }

	canvas.fdlg->Initialize();
	canvas.fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	canvas.fdlg->multiSelect=YSFALSE;
	canvas.fdlg->title.Set(L"Save Reference Bitmap Configuration");
	canvas.fdlg->fileExtensionArray.Append(L".rfb");
	canvas.fdlg->defaultFileName.SetUTF8String(def);
	canvas.fdlg->SetCloseModalCallBack(View_SaveRefBmpConfig_FileSelected);
	canvas.AttachModalDialog(canvas.fdlg);
}

/*static*/ void GeblGuiEditorBase::View_SaveRefBmpConfig_FileSelected(FsGuiControlBase *appPtr,FsGuiDialog *,int returnCode)
{
	if((int)YSOK==returnCode)
	{
		GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;

		FILE *fp=YsFileIO::Fopen(canvas.fdlg->selectedFileArray[0],"rb");
		if(NULL!=fp)
		{
			fclose(fp);
			canvas.YesNoDialog(L"Confirmation",L"File Exists. Overwrite?",L"Yes",L"No",View_SaveRefBmpConfig_PassedOverwriteCheck);
			canvas.SetNeedRedraw(YSTRUE);
		}
		else
		{
			canvas.View_SaveRefBmpConfig_PassedOverwriteCheck(canvas.fdlg->selectedFileArray[0]);
		}
		canvas.fdlg->SetCloseModalCallBack(nullptr);
	}
}

/*static*/ void GeblGuiEditorBase::View_SaveRefBmpConfig_PassedOverwriteCheck(FsGuiControlBase *appPtr,FsGuiDialog *,int returnCode)
{
	if(YSOK==(YSRESULT)returnCode)
	{
		GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
		canvas.View_SaveRefBmpConfig_PassedOverwriteCheck(canvas.fdlg->selectedFileArray[0]);
	}
}

void GeblGuiEditorBase::View_SaveRefBmpConfig_PassedOverwriteCheck(const wchar_t fn[])
{
	if(NULL!=refBmpStore)
	{
		if(YSOK==refBmpStore->Save(fn))
		{
			MessageDialog(L"Saved",L"Saved Reference Bitmap Configuration");
		}
		else
		{
			MessageDialog(L"Error!",L"Failed to save Reference Bitmap Configuration");
		}
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::View_Highlight_ShowHighlight(FsGuiPopUpMenuItem *menuItem)
{
	drawHighlight=YsReverseBool(menuItem->GetCheck());
	menuItem->SetCheck(drawHighlight);
	SetMenuCheck();
	SetNeedRedraw(YSTRUE);
}

/*static*/ void GeblGuiEditorBase::View_HighlightNonManifoldEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	YsFlip(canvas.drawNonManifoldEdge);

	canvas.drawHighlight=YSTRUE;
	canvas.SetMenuCheck();

	canvas.SetMenuCheck();
	canvas.SetNeedRedraw(YSTRUE);
}

////////////////////////////////////////////////////////////

class View_HighlightHighDihedralAngleEdge : public FsGuiDialog
{
private:
	GeblGuiEditorBase *canvasPtr;

public:
	FsGuiTextBox *angleTxt;
	FsGuiButton *highlightBtn;

	void Make(GeblGuiEditorBase &canvas);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void View_HighlightHighDihedralAngleEdge::Make(GeblGuiEditorBase &canvas)
{
	this->canvasPtr=&canvas;

	FsGuiDialog::Initialize();

	angleTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_COMMON_ANGLE,8,YSTRUE);
	highlightBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);

	angleTxt->SetRealNumber(60.0,1);

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();
}

/* virtual */ void View_HighlightHighDihedralAngleEdge::OnButtonClick(FsGuiButton *btn)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	if(btn==highlightBtn)
	{
printf("%s %d\n",__FUNCTION__,__LINE__);
		auto &canvas=*canvasPtr;
printf("%s %d\n",__FUNCTION__,__LINE__);
		if(nullptr!=canvas.Slhd())
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			auto &shl=*canvas.Slhd();
			auto angle=YsDegToRad(angleTxt->GetRealNumber());

printf("%s %d\n",__FUNCTION__,__LINE__);
			YsShellExt_UnfoldingUtil unfoldUtil;
printf("%s %d\n",__FUNCTION__,__LINE__);
			unfoldUtil.MakeInfo(shl.Conv(),angle);

			auto &highlight=shl.GetHighlight();
printf("%s %d\n",__FUNCTION__,__LINE__);
			highlight.CleanUp();
			highlight.highlightedEdge.MakeFromEdgeStore(shl.Conv(),unfoldUtil.GetProblematicEdge());

			canvas.Slhd()->SetNeedRemakeHighlightVertexBuffer(YSTRUE);
		}

		CloseModalDialog((int)YSOK);
	}
}

void GeblGuiEditorBase::View_Highlight_HighDihedralAngleEdgeWithinFaceGroup(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<View_HighlightHighDihedralAngleEdge>();
	dlg->Make(*this);
	AttachModalDialog(dlg);
	ArrangeDialog();
}

////////////////////////////////////////////////////////////

void GeblGuiEditorBase::View_Highlight_InconsistentOrientationAcrossEdge(FsGuiPopUpMenuItem *)
{
	drawHighlight=YSTRUE;
	SetMenuCheck();

	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		YsShellEdgeStore edgeStore(shl.Conv());
		YsShellEdgeEnumHandle edHd=nullptr;
		while(YSOK==shl.MoveToNextEdge(edHd))
		{
			auto edge=shl.GetEdge(edHd);
			auto edPlHd=shl.FindPolygonFromEdgePiece(edge);
			int nForward=0,nBackward=0;
			for(auto plHd : edPlHd)
			{
				auto plVtHd=shl.GetPolygonVertex(plHd);
				for(auto idx : plVtHd.AllIndex())
				{
					if(plVtHd[idx]==edge[0] && plVtHd.GetCyclic(idx+1)==edge[1])
					{
						++nForward;
						break;
					}
					else if(plVtHd[idx]==edge[1] && plVtHd.GetCyclic(idx+1)==edge[0])
					{
						++nBackward;
						break;
					}
				}
			}
			if(2<=nForward || 2<=nBackward)
			{
				edgeStore.Add(edge);
			}
		}

		printf("%d edges.\n",(int)edgeStore.GetN());

		auto &highlight=shl.GetHighlight();
		highlight.CleanUp();
		highlight.highlightedEdge.MakeFromEdgeStore(shl.Conv(),edgeStore);

		shl.SetNeedRemakeHighlightVertexBuffer(YSTRUE);

		SetNeedRedraw(YSTRUE);
	}
}

////////////////////////////////////////////////////////////

/* static */ void GeblGuiEditorBase::View_HighlightSelfIntersectingPolygon(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.Slhd())
	{
		canvas.drawHighlight=YSTRUE;
		canvas.SetMenuCheck();

		auto &highlight=canvas.Slhd()->GetHighlight();
		highlight.CleanUp();
		highlight.highlightedPolygon.MakeSelfIntersectingPolygonCluster(canvas.Slhd()->Conv());
		canvas.Slhd()->SetNeedRemakeHighlightVertexBuffer(YSTRUE);
	}
}

////////////////////////////////////////////////////////////

class View_HighlightHighAspectRatioPolygonDialog : public FsGuiDialog
{
private:
	GeblGuiEditorBase *canvasPtr;
public:
	FsGuiTextBox *aspectThrTxt;
	FsGuiSlider *slider;
	FsGuiButton *triBtn,*quadBtn;
	FsGuiButton *setBtn,*okBtn;

	void Make(GeblGuiEditorBase *canvasPtr);
	double GetAspectThr(void) const;
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);
};

void View_HighlightHighAspectRatioPolygonDialog::Make(GeblGuiEditorBase *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	FsGuiDialog::Initialize();

	aspectThrTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"Aspect Ratio:",8,YSTRUE);
	aspectThrTxt->SetRealNumber(10.0,6);

	setBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,L"Set",YSFALSE);
	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,L"Close",YSFALSE);

	triBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Triangles",YSFALSE);
	quadBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Quadrilaterals",YSFALSE);
	triBtn->SetCheck(YSTRUE);
	quadBtn->SetCheck(YSTRUE);

	slider=AddHorizontalSlider(0,FSKEY_NULL,80,1.0,100.0,YSTRUE);

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();
}

double View_HighlightHighAspectRatioPolygonDialog::GetAspectThr(void) const
{
	return aspectThrTxt->GetRealNumber();
}
void GeblGuiEditorBase::View_Highlight_HighAspectRatioTriangleAndQuadrilateral(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <View_HighlightHighAspectRatioPolygonDialog>();
	dlg->Make(this);
	AddDialog(dlg);
	ArrangeDialog();
}

/* virtual */ void View_HighlightHighAspectRatioPolygonDialog::OnButtonClick(FsGuiButton *btn)
{
	if(setBtn==btn)
	{
		canvasPtr->View_Highlight_HighAspectRatioTriangleAndQuadrilateral_RefreshCallBack(this);
	}
	else if(okBtn==btn)
	{
		canvasPtr->View_Highlight_HighAspectRatioTriangleAndQuadrilateral_RefreshCallBack(this);
		canvasPtr->RemoveDialog(this);
	}
}
/* virtual */ void View_HighlightHighAspectRatioPolygonDialog::OnSliderPositionChange(FsGuiSlider *slider,const double &,const double &)
{
	aspectThrTxt->SetRealNumber(slider->GetScaledValue(),1);
	canvasPtr->View_Highlight_HighAspectRatioTriangleAndQuadrilateral_RefreshCallBack(this);
}

void GeblGuiEditorBase::View_Highlight_HighAspectRatioTriangleAndQuadrilateral_RefreshCallBack(FsGuiDialog *dlgPtr)
{
	auto dlg=dynamic_cast <View_HighlightHighAspectRatioPolygonDialog *>(dlgPtr);
	if(NULL!=dlg)
	{
		const double aspectThr=dlg->GetAspectThr();

		if(NULL!=Slhd())
		{
			auto &shl=*Slhd();
			YsShellPolygonStore plgStore(shl.Conv());
			for(auto plHd : shl.AllPolygon())
			{
				if((3==shl.GetPolygonNumVertex(plHd) && YSTRUE==dlg->triBtn->GetCheck()) ||
				   (4==shl.GetPolygonNumVertex(plHd) && YSTRUE==dlg->quadBtn->GetCheck()))
				{
					double length,height;
					YsShellExt_CalculateAspectRatio(length,height,shl.Conv(),plHd);
					if(length>aspectThr*height /*length/height>aspectThr*/)
					{
						plgStore.AddPolygon(plHd);
					}
				}
			}

			drawHighlight=YSTRUE;
			SetMenuCheck();

			auto &highlight=Slhd()->GetHighlight();
			highlight.CleanUp();
			highlight.highlightedPolygon.MakeFromPolygonStore(shl.Conv(),plgStore);
			Slhd()->SetNeedRemakeHighlightVertexBuffer(YSTRUE);
		}
	}
}

////////////////////////////////////////////////////////////

/*static*/ void GeblGuiEditorBase::View_ShowAllRefBmp(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	for(auto &refBmp : canvas.refBmpStore->refBmp)
	{
		refBmp.show=YSTRUE;
	}
	canvas.SetNeedRedraw(YSTRUE);
}

/*static*/ void GeblGuiEditorBase::View_HideAllRefBmp(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	for(auto &refBmp : canvas.refBmpStore->refBmp)
	{
		refBmp.show=YSFALSE;
	}
	canvas.SetNeedRedraw(YSTRUE);
}


void GeblGuiEditorBase::View_DrawBackgroundGradation(FsGuiPopUpMenuItem *itm)
{
	itm->InvertCheck();
	SetNeedRedraw(YSTRUE);
}

void GeblGuiEditorBase::View_DrawDnmConnection(FsGuiPopUpMenuItem *itm)
{
	itm->InvertCheck();
	drawDnmConnection=itm->GetCheck();
	SaveViewPreference();
	SetNeedRedraw(YSTRUE);
}
