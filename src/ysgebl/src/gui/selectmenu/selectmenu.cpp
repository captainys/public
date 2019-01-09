/* ////////////////////////////////////////////////////////////

File Name: selectmenu.cpp
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
#include <ysshellext_trackingutil.h>
#include <ysshellextmisc.h>
#include <ysshellext_geomcalc.h>
#include <ysshellext_polygontanglerepair.h>
#include <ysshellext_astar.h>

#include "../ysgebl_gui_editor_base.h"
#include "../sketch/sketch.h"
#include "../textresource.h"

void GeblGuiEditorBase::Select_All(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		slHd->SelectVertex(slHd->AllVertex().Array());
		slHd->SelectPolygon(slHd->AllPolygon().Array());
		slHd->SelectFaceGroup(slHd->AllFaceGroup().Array());
		slHd->SelectConstEdge(slHd->AllConstEdge().Array());

		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX|NEED_REMAKE_DRAWING_SELECTED_POLYGON|
		                         NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_UnselectAll(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		slHd->SelectVertex(0,NULL);
		slHd->SelectPolygon(0,NULL);
		slHd->SelectConstEdge(0,NULL);
		slHd->SelectFaceGroup(0,NULL);

		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX|NEED_REMAKE_DRAWING_SELECTED_POLYGON|
		                         NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_PickVertex(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIModeExceptDNMEditMode();
	undoRedoRequireToClearUIMode=YSFALSE;
	deletionRequireToClearUIMode=YSFALSE;
	LButtonCallBack=Select_PickVertex_LButtonCallBack;
	LButtonBoxCallBack=Select_PickVertex_LButtonBoxCallBack;
	BackSpaceKeyCallBack=Select_PickVertex_BackSpaceKeyCallBack;
	modeMessage=L"Pick Vertices";

	if(YSTRUE==dnmEditMode)
	{
		nextESCOnlyCancelDnmEditSubMode=YSTRUE;
	}

	SetNeedRedraw(YSTRUE);
}

YSRESULT GeblGuiEditorBase::Select_PickVertex_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL,YSBOOL,int mx,int my)
{
	if(NULL!=canvas.slHd && YSTRUE!=lb)
	{
		YsArray <YsShellVertexHandle> selVtHd;
		canvas.slHd->GetSelectedVertex(selVtHd);

		if(0==FsGetKeyState(FSKEY_CTRL))
		{
			YsShellVertexHandle vtHd[1]={canvas.PickedVtHd(mx,my,selVtHd)};

			if(NULL!=vtHd[0])
			{
				canvas.slHd->AddSelectedVertex(1,vtHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
				canvas.SetNeedRedraw(YSTRUE);
				return YSOK;
			}
		}
		else
		{
			YsShellVertexHandle vtHd=canvas.PickedVtHdAmongSelection(mx,my);  // Selection can be already cached in the vertex buffer.

			if(NULL!=vtHd)
			{
				for(YSSIZE_T idx=selVtHd.GetN()-1; 0<=idx; --idx)
				{
					if(selVtHd[idx]==vtHd)
					{
						selVtHd.Delete(idx);
					}
				}

				canvas.slHd->SelectVertex(selVtHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
				canvas.SetNeedRedraw(YSTRUE);
				return YSOK;
			}
		}
	}
	return YSERR;
}

/*static*/ YSRESULT GeblGuiEditorBase::Select_PickVertex_LButtonBoxCallBack(GeblGuiEditorBase &canvas,int xMin,int yMin,int xMax,int yMax)
{
	if(nullptr!=canvas.slHd)
	{
		YsArray <YsShellVertexHandle> selVtHd;
		canvas.slHd->GetSelectedVertex(selVtHd);

		if(0==FsGetKeyState(FSKEY_CTRL))
		{
			YsArray <YsPair <YsShellVertexHandle,double> > vtHdDepthPair=canvas.BoxedVtHd(xMin,yMin,xMax,yMax,selVtHd);
			if(0<vtHdDepthPair.GetN())
			{
				for(auto pair : vtHdDepthPair)
				{
					selVtHd.Append(pair.a);
				}
				canvas.slHd->SelectVertex(selVtHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
				canvas.SetNeedRedraw(YSTRUE);
			}
		}
		else
		{
			YsArray <YsPair <YsShellVertexHandle,double> > vtHdDepthPair=canvas.BoxedVtHdAmongSelection(xMin,yMin,xMax,yMax);
			if(0<vtHdDepthPair.GetN())
			{
				YsKeyStore boxed;
				for(auto pair : vtHdDepthPair)
				{
					boxed.AddKey(canvas.slHd->GetSearchKey(pair.a));
				}
				YsArray <YsShellVertexHandle> newSelVtHd;
				for(YSSIZE_T idx=selVtHd.GetN()-1; 0<=idx; --idx)
				{
					if(YSTRUE!=boxed.IsIncluded(canvas.slHd->GetSearchKey(selVtHd[idx])))
					{
						newSelVtHd.Append(selVtHd[idx]);
					}
				}
				canvas.slHd->SelectVertex(newSelVtHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
				canvas.SetNeedRedraw(YSTRUE);
			}
		}
	}

	return YSERR;
}

/*static*/ YSRESULT GeblGuiEditorBase::Select_PickVertex_BackSpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	if(nullptr!=canvas.slHd)
	{
		auto selVtHd=canvas.slHd->GetSelectedVertex();
		if(0<selVtHd.GetN())
		{
			selVtHd.DeleteLast();
			canvas.slHd->SelectVertex(selVtHd);
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
			canvas.SetNeedRedraw(YSTRUE);
			return YSOK;
		}
	}
	return YSERR;
}

void GeblGuiEditorBase::Select_PickPolygon(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIModeExceptDNMEditMode();
	undoRedoRequireToClearUIMode=YSFALSE;
	deletionRequireToClearUIMode=YSFALSE;
	LButtonCallBack=Select_PickPolygon_LButtonCallBack;
	LButtonBoxCallBack=Select_PickPolygon_LButtonBoxCallBack;
	BackSpaceKeyCallBack=Select_PickPolygon_BackSpaceKeyCallBack;
	modeMessage=L"Pick Polygons";

	if(YSTRUE==dnmEditMode)
	{
		nextESCOnlyCancelDnmEditSubMode=YSTRUE;
	}

	SetNeedRedraw(YSTRUE);
}

YSRESULT GeblGuiEditorBase::Select_PickPolygon_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL,YSBOOL,int mx,int my)
{
	if(NULL!=canvas.slHd && YSTRUE!=lb)
	{
		YsArray <YsShellPolygonHandle> selPlHd;
		canvas.slHd->GetSelectedPolygon(selPlHd);

		if(0==FsGetKeyState(FSKEY_CTRL))
		{
			YsShellPolygonHandle plHd[1]={canvas.PickedPlHd(mx,my,selPlHd)};
			if(NULL!=plHd[0])
			{
				canvas.slHd->AddSelectedPolygon(1,plHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
				canvas.SetNeedRedraw(YSTRUE);

				YsString lastPick;
				lastPick.Printf("Polygon Id=%d (Internal Search Key=%d)",
				    canvas.slHd->GetPolygonIdFromHandle(plHd[0]),
				    canvas.slHd->GetSearchKey(plHd[0]));
				canvas.GetBottomStatusBar()->SetString(1,lastPick);

				return YSOK;
			}
		}
		else 
		{
			YsShellPolygonHandle plHd=canvas.PickedPlHdAmongSelection(mx,my);
			if(NULL!=plHd)
			{
				for(YSSIZE_T idx=selPlHd.GetN()-1; 0<=idx; --idx)
				{
					if(selPlHd[idx]==plHd)
					{
						selPlHd.Delete(idx);
					}
				}
				canvas.slHd->SelectPolygon(selPlHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
				canvas.SetNeedRedraw(YSTRUE);
				return YSOK;
			}
		}
	}
	return YSERR;
}

/*static*/ YSRESULT GeblGuiEditorBase::Select_PickPolygon_LButtonBoxCallBack(GeblGuiEditorBase &canvas,int xMin,int yMin,int xMax,int yMax)
{
	if(nullptr!=canvas.slHd)
	{
		YsArray <YsShellPolygonHandle> selPlHd;
		canvas.slHd->GetSelectedPolygon(selPlHd);

		if(0==FsGetKeyState(FSKEY_CTRL))
		{
			YsArray <YsPair <YsShellPolygonHandle,double> > plHdDepthPairArray=canvas.BoxedPlHd(xMin,yMin,xMax,yMax,selPlHd);
			if(0<plHdDepthPairArray.GetN())
			{
				for(auto pair : plHdDepthPairArray)
				{
					selPlHd.Append(pair.a);
				}

				canvas.slHd->SelectPolygon(selPlHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
				canvas.SetNeedRedraw(YSTRUE);
				return YSOK;
			}
		}
		else 
		{
			YsArray <YsPair <YsShellPolygonHandle,double> > plHdDepthPairArray=canvas.BoxedPlHdAmongSelection(xMin,yMin,xMax,yMax);
			if(0<plHdDepthPairArray.GetN())
			{
				YsShellPolygonStore boxed(*(const YsShell *)canvas.slHd);
				for(auto pair : plHdDepthPairArray)
				{
					boxed.AddPolygon(pair.a);
				}

				YsArray <YsShellPolygonHandle> newSelPlHd;
				for(auto plHd : selPlHd)
				{
					if(YSTRUE!=boxed.IsIncluded(plHd))
					{
						newSelPlHd.Append(plHd);
					}
				}

				canvas.slHd->SelectPolygon(newSelPlHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
				canvas.SetNeedRedraw(YSTRUE);
				return YSOK;
			}
		}
	}
	return YSERR;
}

/*static*/ YSRESULT GeblGuiEditorBase::Select_PickPolygon_BackSpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	if(nullptr!=canvas.slHd)
	{
		auto selPlHd=canvas.slHd->GetSelectedPolygon();
		if(0<selPlHd.GetN())
		{
			selPlHd.DeleteLast();
			canvas.slHd->SelectPolygon(selPlHd);
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
			canvas.SetNeedRedraw(YSTRUE);
			return YSOK;
		}
	}
	return YSERR;
}

void GeblGuiEditorBase::Select_PickConstEdge(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIModeExceptDNMEditMode();
	undoRedoRequireToClearUIMode=YSFALSE;
	deletionRequireToClearUIMode=YSFALSE;
	LButtonCallBack=Select_PickConstEdge_LButtonCallBack;
	LButtonBoxCallBack=Select_PickConstEdge_LButtonBoxCallBack;
	BackSpaceKeyCallBack=Select_PickConstEdge_BackSpaceKeyCallBack;
	modeMessage=L"Pick Const Edges";

	if(YSTRUE==dnmEditMode)
	{
		nextESCOnlyCancelDnmEditSubMode=YSTRUE;
	}

	SetNeedRedraw(YSTRUE);
}

YSRESULT GeblGuiEditorBase::Select_PickConstEdge_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL,YSBOOL,int mx,int my)
{
	if(NULL!=canvas.slHd && YSTRUE!=lb)
	{
		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		canvas.slHd->GetSelectedConstEdge(selCeHd);

		if(0==FsGetKeyState(FSKEY_CTRL))
		{
			YsShellExt::ConstEdgeHandle ceHd[1]={canvas.PickedCeHd(mx,my,selCeHd)};
			if(NULL!=ceHd[0])
			{
				canvas.slHd->AddSelectedConstEdge(1,ceHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
				canvas.SetNeedRedraw(YSTRUE);
				return YSOK;
			}
		}
		else
		{
			YsShellExt::ConstEdgeHandle ceHd=canvas.PickedCeHdAmongSelection(mx,my);
			if(NULL!=ceHd)
			{
				for(YSSIZE_T idx=selCeHd.GetN()-1; 0<=idx; --idx)
				{
					if(ceHd==selCeHd[idx])
					{
						selCeHd.Delete(idx);
					}
				}
				canvas.slHd->SelectConstEdge(selCeHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
				canvas.SetNeedRedraw(YSTRUE);
				return YSOK;
			}
		}
	}
	return YSERR;
}

YSRESULT GeblGuiEditorBase::Select_PickConstEdge_LButtonBoxCallBack(GeblGuiEditorBase &canvas,int x0,int y0,int x1,int y1)
{
	if(nullptr!=canvas.slHd)
	{
		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		canvas.slHd->GetSelectedConstEdge(selCeHd);

		if(0==FsGetKeyState(FSKEY_CTRL))
		{
			YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > boxed=canvas.BoxedCeHd(x0,y0,x1,y1,selCeHd);
			if(0<boxed.GetN())
			{
				for(auto pair : boxed)
				{
					selCeHd.Append(pair.a);
				}
				canvas.slHd->SelectConstEdge(selCeHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
				canvas.SetNeedRedraw(YSTRUE);
				return YSOK;
			}
		}
		else
		{
			YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > boxed=canvas.BoxedCeHdAmongSelection(x0,y0,x1,y1);
			if(0<boxed.GetN())
			{
				YsKeyStore boxedCeKey;
				for(auto pair : boxed)
				{
					boxedCeKey.AddKey(canvas.slHd->GetSearchKey(pair.a));
				}
				YsArray <YsShellExt::ConstEdgeHandle> newSelCeHd;
				for(auto ceHd : selCeHd)
				{
					if(YSTRUE!=boxedCeKey.IsIncluded(canvas.slHd->GetSearchKey(ceHd)))
					{
						newSelCeHd.Append(ceHd);
					}
				}
				canvas.slHd->SelectConstEdge(newSelCeHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
				canvas.SetNeedRedraw(YSTRUE);
				return YSOK;
			}
		}
	}
	return YSERR;
}

/*static*/ YSRESULT GeblGuiEditorBase::Select_PickConstEdge_BackSpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	if(nullptr!=canvas.slHd)
	{
		auto selCeHd=canvas.slHd->GetSelectedConstEdge();
		if(0<selCeHd.GetN())
		{
			selCeHd.DeleteLast();
			canvas.slHd->SelectConstEdge(selCeHd);
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
			canvas.SetNeedRedraw(YSTRUE);
			return YSOK;
		}
	}
	return YSERR;
}

void GeblGuiEditorBase::Select_PickFaceGroup(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIModeExceptDNMEditMode();
	undoRedoRequireToClearUIMode=YSFALSE;
	deletionRequireToClearUIMode=YSFALSE;
	LButtonCallBack=Select_PickFaceGroup_LButtonCallBack;
	BackSpaceKeyCallBack=Select_PickFaceGroup_BackSpaceKeyCallBack;
	modeMessage=L"Pick Face Groups";

	if(YSTRUE==dnmEditMode)
	{
		nextESCOnlyCancelDnmEditSubMode=YSTRUE;
	}

	SetNeedRedraw(YSTRUE);
}

YSRESULT GeblGuiEditorBase::Select_PickFaceGroup_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL,YSBOOL,int mx,int my)
{
	if(NULL!=canvas.slHd && YSTRUE!=lb)
	{
		YsArray <YsShellExt::FaceGroupHandle> selFgHd;
		canvas.slHd->GetSelectedFaceGroup(selFgHd);

		if(0==FsGetKeyState(FSKEY_CTRL))
		{
			YsShellExt::FaceGroupHandle pickedFgHd[1]={canvas.PickedFgHd(mx,my,selFgHd)};
			if(NULL!=pickedFgHd[0])
			{
				canvas.slHd->AddSelectedFaceGroup(1,pickedFgHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
				canvas.SetNeedRedraw(YSTRUE);
			}
		}
		else 
		{
			auto pickedFgHd=canvas.PickedFgHdAmongSelection(mx,my);
			if(NULL!=pickedFgHd)
			{
				for(YSSIZE_T idx=selFgHd.GetN()-1; 0<=idx; --idx)
				{
					if(selFgHd[idx]==pickedFgHd)
					{
						selFgHd.Delete(idx);
					}
				}
				canvas.slHd->SelectFaceGroup(selFgHd);
				canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
				canvas.SetNeedRedraw(YSTRUE);
				return YSOK;
			}
		}

		return YSOK;
	}
	return YSERR;
}

/*static*/ YSRESULT GeblGuiEditorBase::Select_PickFaceGroup_BackSpaceKeyCallBack(GeblGuiEditorBase &canvas)
{
	if(nullptr!=canvas.slHd)
	{
		auto selFgHd=canvas.slHd->GetSelectedFaceGroup();
		if(0<selFgHd.GetN())
		{
			selFgHd.DeleteLast();
			canvas.slHd->SelectFaceGroup(selFgHd);
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
			canvas.SetNeedRedraw(YSTRUE);
			return YSOK;
		}
	}
	return YSERR;
}

void GeblGuiEditorBase::Select_UnselectVertex(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		slHd->SelectVertex(0,NULL);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_AllVertex(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellVertexHandle> allVtHd;
		for(YsShellVertexHandle vtHd=NULL; YSOK==slHd->MoveToNextVertex(vtHd); )
		{
			allVtHd.Append(vtHd);
		}
		slHd->SelectVertex(allVtHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
		SetNeedRedraw(YSTRUE);
	}
}


void GeblGuiEditorBase::Select_InvertVertex(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);

		YsShellVertexStore selVtHash(slHd->Conv(),selVtHd);

		YsArray <YsShellVertexHandle> allVtHd;
		for(YsShellVertexHandle vtHd=NULL; YSOK==slHd->MoveToNextVertex(vtHd); )
		{
			if(YSTRUE!=selVtHash.IsIncluded(vtHd))
			{
				allVtHd.Append(vtHd);
			}
		}
		slHd->SelectVertex(allVtHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
		SetNeedRedraw(YSTRUE);
	}
}


void GeblGuiEditorBase::Select_VertexOfPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);

		YsShellVertexStore visited(slHd->Conv());

		YsArray <YsShellVertexHandle> selVtHd;
		for(YSSIZE_T plIdx=0; plIdx<selPlHd.GetN(); ++plIdx)
		{
			int nPlVt;
			const YsShellVertexHandle *plVtHd;
			slHd->GetVertexListOfPolygon(nPlVt,plVtHd,selPlHd[plIdx]);

			for(YSSIZE_T vtIdx=0; vtIdx<nPlVt; ++vtIdx)
			{
				if(YSTRUE!=visited.IsIncluded(plVtHd[vtIdx]))
				{
					selVtHd.Append(plVtHd[vtIdx]);
					visited.AddVertex(plVtHd[vtIdx]);
				}
			}
		}

		slHd->SelectVertex(selVtHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
		SetNeedRedraw(YSTRUE);
	}
}


void GeblGuiEditorBase::Select_VertexOfConstEdge(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		slHd->GetSelectedConstEdge(selCeHd);

		YsShellVertexStore visited(slHd->Conv());

		YsArray <YsShellVertexHandle> selVtHd;
		for(YSSIZE_T ceIdx=0; ceIdx<selCeHd.GetN(); ++ceIdx)
		{
			YSSIZE_T nCeVt;
			const YsShellVertexHandle *ceVtHd;
			YSBOOL isLoop;
			slHd->GetConstEdge(nCeVt,ceVtHd,isLoop,selCeHd[ceIdx]);

			for(YSSIZE_T vtIdx=0; vtIdx<nCeVt; ++vtIdx)
			{
				if(YSTRUE!=visited.IsIncluded(ceVtHd[vtIdx]))
				{
					selVtHd.Append(ceVtHd[vtIdx]);
					visited.AddVertex(ceVtHd[vtIdx]);
				}
			}
		}

		slHd->SelectVertex(selVtHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_VertexAlongBoundaryOrConstEdge(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto selVtHd=slHd->GetSelectedVertex();
		if(0<selVtHd.GetN())
		{
			YsArray <YsShellVertexHandle> connVtHd;
			slHd->GetConnectedVertex(connVtHd,selVtHd.Last());

			YsArray <YsShellVertexHandle> candidate;
			for(auto vtHd : connVtHd)
			{
				if(1==slHd->GetNumPolygonUsingEdge(selVtHd.Last(),vtHd) &&
				   YSTRUE!=selVtHd.IsIncluded(vtHd))
				{
					candidate.Append(vtHd);
				}
			}

			if(1==candidate.GetN())
			{
				selVtHd.Append(candidate[0]);
				slHd->SelectVertex(selVtHd);
			}
			else if(2<=candidate.GetN())
			{
				if(0==FsGetKeyState(FSKEY_SHIFT))
				{
					selVtHd.Append(candidate[0]);
					slHd->SelectVertex(selVtHd);
				}
				else
				{
					selVtHd.Append(candidate[1]);
					slHd->SelectVertex(selVtHd);
				}
			}
			else
			{
				auto candidate=slHd->GetConstEdgeConnectedVertex(selVtHd.Last());
				for(auto idx=candidate.GetN()-1; 0<=idx; --idx)
				{
					if(YSTRUE==selVtHd.IsIncluded(candidate[idx]))
					{
						candidate.Delete(idx);
					}
				}
				if(1==candidate.GetN())
				{
					selVtHd.Append(candidate[0]);
					slHd->SelectVertex(selVtHd);
				}
				else if(2<=candidate.GetN())
				{
					if(0==FsGetKeyState(FSKEY_SHIFT))
					{
						selVtHd.Append(candidate[0]);
						slHd->SelectVertex(selVtHd);
					}
					else
					{
						selVtHd.Append(candidate[1]);
						slHd->SelectVertex(selVtHd);
					}
				}
			}

			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
			SetNeedRedraw(YSTRUE);
		}
	}
}

void GeblGuiEditorBase::Select_Vertex_SingleUseEdgeLoop(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto &shl=*Slhd();
		YsShellExt_SingleUseEdgeCondition cond;

		auto selVtHd=shl.GetSelectedVertex();
		if(1==selVtHd.GetN())
		{
			auto connVtHd=shl.GetConnectedVertex(selVtHd[0]);
			for(auto vtHd1 : connVtHd)
			{
				auto trk=YsShellExt_TrackingUtil::TrackEdge(shl.Conv(),selVtHd[0],vtHd1,cond);
				if(3<=trk.GetN() && trk[0]==trk.Last())
				{
					shl.SelectVertex(trk);
					break;
				}
			}
		}
		else if(2==selVtHd.GetN())
		{
			auto trk=YsShellExt_TrackingUtil::TrackEdge(shl.Conv(),selVtHd[0],selVtHd[1],cond);
			if(3<=trk.GetN() && trk[0]==trk.Last())
			{
				shl.SelectVertex(trk);
			}
		}

		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_EdgeConnectedVertexBetweenSelection(FsGuiPopUpMenuItem *)
{
	// Should use A* at some point.
	if(NULL!=Slhd())
	{
		auto &shl=*Slhd();
		auto selVtHd=shl.GetSelectedVertex();
		if(2<=selVtHd.GetN())
		{
			YSRESULT atLeastOneFailure=YSOK;

			YsArray <YsShellVertexHandle> newSelVtHd;
			for(YSSIZE_T idx=0; idx<selVtHd.GetN(); ++idx)
			{
				newSelVtHd.Append(selVtHd[idx]);

				if(idx<selVtHd.GetN()-1)
				{
					auto curVtHd=selVtHd[idx];
					auto toVtHd=selVtHd[idx+1];
					// const auto goalPos=shl.GetVertexPosition(toVtHd);
					YsArray <YsShellVertexHandle> midVtHd;

					// Greedy search >>
					// for(;;)
					// {
					// 	const double curDist=(goalPos-shl.GetVertexPosition(curVtHd)).GetSquareLength();
					// 	auto connVtHd=shl.GetConnectedVertex(curVtHd);
					// 	YsShellVertexHandle minDistVtHd=NULL;
					// 	double minDist=curDist;
					// 	for(auto cv : connVtHd)
					// 	{
					// 		if(cv==toVtHd)
					// 		{
					// 			minDistVtHd=cv;
					// 			minDist=0.0;
					// 			break;
					// 		}
					// 		auto cvPos=shl.GetVertexPosition(cv);
					// 		auto dist=(cvPos-goalPos).GetSquareLength();
					// 		if(dist<minDist)
					// 		{
					// 			minDist=dist;
					// 			minDistVtHd=cv;
					// 		}
					// 	}
					// 	if(minDistVtHd==toVtHd)
					// 	{
					// 		break;
					// 	}
					// 	else if(NULL!=minDistVtHd)
					// 	{
					// 		midVtHd.Append(minDistVtHd);
					// 		curVtHd=minDistVtHd;
					// 	}
					// 	else
					// 	{
					// 		midVtHd.CleanUp();
					// 		atLeastOneFailure=YSERR;
					// 		break;
					// 	}
					// }
					// Greedy search <<

					YsShellExt_AstarSearch astar;
					auto mid=astar.FindShortestPath(shl.Conv(),curVtHd,toVtHd);
					mid.Delete(0);
					mid.DeleteLast();
					newSelVtHd.Append(mid);
				}
			}

			if(newSelVtHd.GetN()>selVtHd.GetN())
			{
				shl.SelectVertex(newSelVtHd);
				needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
				SetNeedRedraw(YSTRUE);
			}
			if(YSOK!=atLeastOneFailure)
			{
				// Show error
			}
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTATLEAST2VERTEX);
		}
	}
}

void GeblGuiEditorBase::Select_UnselectPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		slHd->SelectPolygon(0,NULL);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_AllPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellPolygonHandle> allPlHd;
		for(YsShellPolygonHandle plHd=NULL; YSOK==slHd->MoveToNextPolygon(plHd); )
		{
			allPlHd.Append(plHd);
		}

		slHd->SelectPolygon(allPlHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}


void GeblGuiEditorBase::Select_InvertPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);

		YsShellPolygonStore selPlHash(slHd->Conv(),selPlHd);

		YsArray <YsShellPolygonHandle> allPlHd;
		for(YsShellPolygonHandle plHd=NULL; YSOK==slHd->MoveToNextPolygon(plHd); )
		{
			if(YSTRUE!=selPlHash.IsIncluded(plHd))
			{
				allPlHd.Append(plHd);
			}
		}

		slHd->SelectPolygon(allPlHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}


void GeblGuiEditorBase::Select_PolygonOfFaceGroup(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		YsArray <YsShellExt::FaceGroupHandle> selFgHd;
		slHd->GetSelectedFaceGroup(selFgHd);

		YsShellPolygonStore visited(slHd->Conv());

		YsArray <YsShellPolygonHandle> allPlHd;
		for(YSSIZE_T fgIdx=0; fgIdx<selFgHd.GetN(); ++fgIdx)
		{
			YsArray <YsShellPolygonHandle> fgPlHd;
			slHd->GetFaceGroup(fgPlHd,selFgHd[fgIdx]);

			for(YSSIZE_T plIdx=0; plIdx<fgPlHd.GetN(); ++plIdx)
			{
				if(YSTRUE!=visited.IsIncluded(fgPlHd[plIdx]))
				{
					allPlHd.Append(fgPlHd[plIdx]);
					visited.AddPolygon(fgPlHd[plIdx]);
				}
			}
		}

		slHd->SelectPolygon(allPlHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_CurrentShellByPicking(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIModeExceptDNMEditMode();
	undoRedoRequireToClearUIMode=YSFALSE;
	deletionRequireToClearUIMode=YSFALSE;
	LButtonCallBack=Select_CurrentShellByPicking_LButtonCallBack;
	modeMessage=L"Pick Current Shell";

	if(YSTRUE==dnmEditMode)
	{
		nextESCOnlyCancelDnmEditSubMode=YSTRUE;
	}

	SetNeedRedraw(YSTRUE);
}

/*static*/ YSRESULT GeblGuiEditorBase::Select_CurrentShellByPicking_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL,YSBOOL,int mx,int my)
{
	if(YSTRUE!=lb)
	{
		const YsShellDnmContainer <YsShellExtEditGui>::Node *excludeSlHd[1]={canvas.slHd};
		YsShellDnmContainer <YsShellExtEditGui>::Node *pickedSlHd=canvas.PickedSlHd(mx,my,1,excludeSlHd);
		if(NULL!=pickedSlHd)
		{
			canvas.Edit_ClearUIModeExceptDNMEditMode();  // Reset UI once.
			canvas.Select_CurrentShellByPicking(nullptr);
			canvas.slHd=pickedSlHd;
			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			canvas.needRemakeDrawingBuffer_for_OtherShell|=NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
			return YSOK;
		}
	}
	return YSERR;
}

void GeblGuiEditorBase::Select_NextShell(FsGuiPopUpMenuItem *)
{
	YsArray <YsShellDnmContainer <YsShellExtEditGui>::Node *> allNode;
	shlGrp.GetNodePointerAll(allNode);
	if(0<allNode.GetN())
	{
		if(nullptr!=slHd)
		{
			for(YSSIZE_T idx=0; idx<allNode.GetN(); ++idx)
			{
				if(slHd==allNode[idx])
				{
					slHd=allNode.GetCyclic(idx+1);
					needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
					needRemakeDrawingBuffer_for_OtherShell|=NEED_REMAKE_DRAWING_ALL;
					SetNeedRedraw(YSTRUE);
					return;
				}
			}
		}
		else
		{
			slHd=allNode[0];
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			needRemakeDrawingBuffer_for_OtherShell|=NEED_REMAKE_DRAWING_ALL;
			SetNeedRedraw(YSTRUE);
		}
	}
}

void GeblGuiEditorBase::Select_PrevShell(FsGuiPopUpMenuItem *)
{
	YsArray <YsShellDnmContainer <YsShellExtEditGui>::Node *> allNode;
	shlGrp.GetNodePointerAll(allNode);
	if(0<allNode.GetN())
	{
		if(nullptr!=slHd)
		{
			for(YSSIZE_T idx=0; idx<allNode.GetN(); ++idx)
			{
				if(slHd==allNode[idx])
				{
					slHd=allNode.GetCyclic(idx-1);
					needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
					needRemakeDrawingBuffer_for_OtherShell|=NEED_REMAKE_DRAWING_ALL;
					SetNeedRedraw(YSTRUE);
					return;
				}
			}
		}
		else
		{
			slHd=allNode[0];
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
			needRemakeDrawingBuffer_for_OtherShell|=NEED_REMAKE_DRAWING_ALL;
			SetNeedRedraw(YSTRUE);
		}
	}
}

void GeblGuiEditorBase::Select_PolygonConnected(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto &shl=*slHd;
		YsShellPolygonStore newSelPlg(shl.Conv());

		auto selPlHd=shl.GetSelectedPolygon();
		for(auto plHd : selPlHd)
		{
			if(YSTRUE!=newSelPlg.IsIncluded(plHd))
			{
				auto connPlHd=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl.Conv(),plHd,NULL);
				newSelPlg.AddPolygon(connPlHd);
			}
		}

		selPlHd=newSelPlg.GetArray();
		shl.SelectPolygon(selPlHd);

		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_PolygonUsingAtLeastOneOfSelectedVertex(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto &shl=*slHd;
		auto selVtHd=shl.GetSelectedVertex();

		YsShellPolygonStore plg(shl.Conv());
		for(auto vtHd : selVtHd)
		{
			auto vtPlHd=shl.FindPolygonFromVertex(vtHd);
			plg.AddPolygon(vtPlHd);
		}

		shl.SelectPolygon(plg.GetArray());

		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_PolygonShortestPath(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto &shl=*slHd;
		auto selPlHd=shl.GetSelectedPolygon();

		decltype(selPlHd) newSelPlHd;
		for(decltype(selPlHd.GetN()) i=0; i<selPlHd.GetN(); ++i)
		{
			newSelPlHd.Add(selPlHd[i]);
			if(i<selPlHd.GetN()-1)
			{
				YsShellExt_AstarSearch_Polygon astar;
				auto mid=astar.FindShortestPath(shl.Conv(),selPlHd[i],selPlHd[i+1]);
				mid.Delete(0);
				mid.DeleteLast();
				newSelPlHd.Append(mid);
			}
		}

		shl.SelectPolygon(newSelPlHd);

		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_PolygonFromEdge(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto &shl=*slHd;
		auto selVtHd=shl.GetSelectedVertex();

		if(2==selVtHd.GetN())
		{
			auto selPlHd=shl.FindPolygonFromEdgePiece(selVtHd);
			shl.SelectPolygon(selPlHd);
			needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
			SetNeedRedraw(YSTRUE);
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECT2VERTEX);
		}
	}
}

void GeblGuiEditorBase::Select_AllFaceGroup(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto &shl=*slHd;

		YsArray <YsShellExt::FaceGroupHandle> selFgHd=shl.AllFaceGroup().Array();
		shl.SelectFaceGroup(selFgHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_InvertFaceGroup(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto &shl=*slHd;

		auto curSelFgHd=shl.GetSelectedFaceGroup();
		YsShellExt::FaceGroupStore curSel(shl.Conv());
		curSel.AddFaceGroup(curSelFgHd);


		YsArray <YsShellExt::FaceGroupHandle> newSelFgHd;
		for(auto fgHd : shl.AllFaceGroup())
		{
			if(YSTRUE!=curSel.IsIncluded(fgHd))
			{
				newSelFgHd.Append(fgHd);
			}
		}

		shl.SelectFaceGroup(newSelFgHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
		SetNeedRedraw(YSTRUE);
	}

}

void GeblGuiEditorBase::Select_FaceGroup_PlanarFaceGroup(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		auto &shl=*Slhd();
		YsArray <YsShellExt::FaceGroupHandle> selFgHd;
		for(auto fgHd : shl.AllFaceGroup())
		{
			if(YSTRUE==YsShellExt_IsFaceGroupPlanar(shl.Conv(),fgHd))
			{
				selFgHd.Append(fgHd);
			}
		}

		shl.SelectFaceGroup(selFgHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_FaceGroupWithHighDihedralAngle(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiInputNumberDialog>();
	dlg->Make(60.0,1,FSGUI_COMMON_ANGLE,FSGUI_COMMON_ANGLE,FSGUI_COMMON_ANGLE,
	    FSGUI_COMMON_OK,FSGUI_COMMON_CANCEL,(int)YSOK,(int)YSERR);
	dlg->BindCloseModalCallBack(&THISCLASS::Select_FaceGroupWithHighDihedralAngle_AngleSelected,this);
	AttachModalDialog(dlg);
}
void GeblGuiEditorBase::Select_FaceGroupWithHighDihedralAngle_AngleSelected(FsGuiDialog *dlg,int returnCode)
{
	auto numDlg=dynamic_cast <FsGuiInputNumberDialog *>(dlg);
	if(NULL!=Slhd() && nullptr!=numDlg)
	{
		auto thr=YsDegToRad(numDlg->GetNumber());
		auto &shl=*Slhd();
		YsArray <YsShellExt::FaceGroupHandle> selFgHd;
		for(auto fgHd : shl.AllFaceGroup())
		{
			if(thr<YsShellExt_CalculateMaxDihedralAngleInFaceGroup(shl.Conv(),fgHd))
			{
				selFgHd.Add(fgHd);
			}
		}
		shl.SelectFaceGroup(selFgHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_FaceGroup_Connected(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		auto &shl=*Slhd();
		YsShellExt::FaceGroupStore selFgHd(shl.Conv());
		YsArray <YsShellExt::FaceGroupHandle> todo=shl.GetSelectedFaceGroup();
		selFgHd.Add(todo);
		while(0<todo.GetN())
		{
			auto fgHd=todo.Last();
			todo.DeleteLast();

			for(auto neiFgHd : shl.GetEdgeConnectedNeighborFaceGroup(fgHd))
			{
				if(YSTRUE!=selFgHd.IsIncluded(neiFgHd))
				{
					todo.Add(neiFgHd);
					selFgHd.Add(neiFgHd);
				}
			}
		}

		shl.SelectFaceGroup(selFgHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::SelectTangledPolygon(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		auto &shl=*Slhd();

		YsArray <YsShellPolygonHandle> selPlHd;
		for(auto plHd : shl.AllPolygon())
		{
			YsShellExt_PolygonTangleRepair tangleRepair;
			if(YSTRUE==tangleRepair.CheckTangledPolygon(shl.Conv(),plHd))
			{
				selPlHd.Append(plHd);
			}
		}
		shl.SelectPolygon(selPlHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::SelectNonTriangularPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		decltype(shl.GetSelectedPolygon()) selPlHd;

		for(auto plHd : shl.AllPolygon())
		{
			if(3!=shl.GetPolygonNumVertex(plHd))
			{
				selPlHd.Add(plHd);
			}
		}
		shl.SelectPolygon(selPlHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::SelectZeroNormalPolygon(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		auto &shl=*Slhd();

		YsArray <YsShellPolygonHandle> selPlHd;
		for(auto plHd : shl.AllPolygon())
		{
			auto nom=shl.GetNormal(plHd);
			if(nom==YsOrigin())
			{
				selPlHd.Append(plHd);
			}
		}
		shl.SelectPolygon(selPlHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_PolygonByStroke(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode();

	if(nullptr!=slHd)
	{
		auto &shl=*slHd;
		sketchUI->StartSketchInterface(shl.Conv(),drawEnv);
		draw2dCallBack=std::bind(&GeblGuiEditorBase::Select_PolygonByStroke_DrawCallBack2D,this);
		mouseMoveCallBack=std::bind(&GeblGuiEditorBase::Select_PolygonByStroke_MouseMoveCallBack,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5);
		lButtonDownCallBack=std::bind(&GeblGuiEditorBase::Select_PolygonByStroke_LButtonDownCallBack,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5);
		lButtonUpCallBack=std::bind(&GeblGuiEditorBase::Select_PolygonByStroke_LButtonUpCallBack,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5);
		SetNeedRedraw(YSTRUE);
	}
}
void GeblGuiEditorBase::Select_PolygonByStroke_DrawCallBack2D(void)
{
	auto strokePoint=sketchUI->GetStroke();

	YsArray <GLfloat> coord,color;
	for(auto &point : strokePoint)
	{
		coord.Append((GLfloat)point.winCoord.x());
		coord.Append((GLfloat)point.winCoord.y());

		color.Append(0.0f);
		color.Append(1.0f);
		color.Append(0.0f);
		color.Append(1.0f);
	}

	glDepthMask(0);
	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLUseWindowCoordinateInPlain2DDrawing(YsGLSLSharedPlain2DRenderer(),YSTRUE);
	YsGLSLDrawPlain2DPrimitivefv(YsGLSLSharedPlain2DRenderer(),GL_LINE_STRIP,coord.GetN()/2,coord,color);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	glDepthMask(1);
}
YSRESULT GeblGuiEditorBase::Select_PolygonByStroke_LButtonDownCallBack(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	sketchUI->BeginStroke();
	return YSOK;
}
YSRESULT GeblGuiEditorBase::Select_PolygonByStroke_LButtonUpCallBack(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	sketchUI->EndStroke();

	if(nullptr!=slHd)
	{
		auto &shl=*slHd;
		decltype(shl.GetSelectedPolygon()) selPlHd;
		for(auto plHdDepth : EnclosedPlHd(*sketchUI))
		{
			selPlHd.Add(plHdDepth.a);
		}
		shl.SelectPolygon(selPlHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}

	return YSOK;
}
YSRESULT GeblGuiEditorBase::Select_PolygonByStroke_MouseMoveCallBack(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	if(YSTRUE==lb)
	{
		sketchUI->AddStrokePoint(YsVec2(mx,my));
		SetNeedRedraw(YSTRUE);
	}
	return YSOK;
}

class Select_PolygonSameColorFromCurrentSelection_Condition : public YsShellExt_TrackingUtil::SearchCondition
{
public:
	YsShellPolygonStore *alreadyIncludedPtr;
	YsColor baseColor;
	virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const
	{
		if(YSTRUE==alreadyIncludedPtr->IsIncluded(plHd))
		{
			return YSERR;
		}
		if(baseColor!=shl.GetColor(plHd))
		{
			return YSERR;
		}
		return YSOK;
	}
};

void GeblGuiEditorBase::Select_PolygonSameColorFromCurrentSelection(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		auto selPlHd=shl.GetSelectedPolygon();

		YsShellPolygonStore newSel(shl.Conv());
		for(auto plHd : selPlHd)
		{
			if(YSTRUE!=newSel.IsIncluded(plHd))
			{
				Select_PolygonSameColorFromCurrentSelection_Condition cond;
				cond.alreadyIncludedPtr=&newSel;
				cond.baseColor=shl.GetColor(plHd);

				auto connPlHd=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl.Conv(),plHd,&cond);
				newSel.Add(connPlHd);
			}
		}

		shl.SelectPolygon(newSel);

		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_Vertex_UsedByTwoConstEdge(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		auto &shl=*Slhd();

		YsArray <YsShell::VertexHandle> selVtHd;
		for(auto vtHd : shl.AllVertex())
		{
			if(2==shl.GetNumConstEdgeUsingVertex(vtHd))
			{
				selVtHd.Add(vtHd);
			}
		}

		shl.SelectVertex(selVtHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_Vertex_OneNeighborOfSelection(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		auto &shl=*Slhd();
		auto selVtHd=shl.GetSelectedVertex();
		auto newSelVtHd=YsShellExt_TrackingUtil::GetNNeighbor(shl.Conv(),selVtHd,1);
		shl.SelectVertex(newSelVtHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_VERTEX;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_ConstEdge_All(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		auto &shl=*Slhd();
		for(auto ceHd : shl.AllConstEdge())
		{
			selCeHd.Add(ceHd);
		}
		shl.SelectConstEdge(selCeHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
		SetNeedRedraw(YSTRUE);
	}
}
void GeblGuiEditorBase::Select_ConstEdge_InvertSelection(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		YsShellExt::ConstEdgeStore curSelCeHd(shl.Conv());
		curSelCeHd.Add(shl.GetSelectedConstEdge());

		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		for(auto ceHd : shl.AllConstEdge())
		{
			if(YSTRUE!=curSelCeHd.IsIncluded(ceHd))
			{
				selCeHd.Add(ceHd);
			}
		}
		shl.SelectConstEdge(selCeHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
		SetNeedRedraw(YSTRUE);
	}
}
void GeblGuiEditorBase::Select_ConstEdge_DividingSmoothShadingRegion(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		auto &shl=*Slhd();
		for(auto ceHd : shl.AllConstEdge())
		{
			auto ceProp=shl.GetConstEdgeAttrib(ceHd);
			if(YSTRUE==ceProp.IsCrease())
			{
				selCeHd.Add(ceHd);
			}
		}
		shl.SelectConstEdge(selCeHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
		SetNeedRedraw(YSTRUE);
	}
}
void GeblGuiEditorBase::Select_ConstEdge_FromPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		YsShellExt::ConstEdgeStore selCeHd(shl.Conv());
		for(auto plHd : shl.GetSelectedPolygon())
		{
			auto plVtHd=shl.GetPolygonVertex(plHd);
			for(int i=0; i<plVtHd.GetN(); ++i)
			{
				const YsShell::VertexHandle edVtHd[2]=
				{
					plVtHd[i],
					plVtHd.GetCyclic(i+1)
				};
				selCeHd.Add(shl.FindConstEdgeFromEdgePiece(edVtHd));
			}
		}
		shl.SelectConstEdge(selCeHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
		SetNeedRedraw(YSTRUE);
	}
}

// Select_Shell_ByDialog is written in dnmmenu.cpp


void GeblGuiEditorBase::Select_Polygon_Narrow(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiInputNumberDialog>();
	dlg->Make(YsTolerance,20,FSGUI_COMMON_DISTTHR,FSGUI_COMMON_DISTTHR,FSGUI_COMMON_DISTTHR,
	    FSGUI_COMMON_OK,FSGUI_COMMON_CANCEL,(int)YSOK,(int)YSERR);
	dlg->BindCloseModalCallBack(&THISCLASS::Select_Polygon_Narrow_ThresholdEntered,this);
	AttachModalDialog(dlg);
}

void GeblGuiEditorBase::Select_Polygon_Narrow_ThresholdEntered(FsGuiDialog *dlg,int returnCode)
{
	auto numDlg=dynamic_cast <FsGuiInputNumberDialog *>(dlg);
	if(NULL!=Slhd() && nullptr!=numDlg)
	{
		auto dThr=numDlg->GetNumber();
		auto &shl=*Slhd();

		YsShellPolygonStore selPlHd(shl.Conv());
		for(auto plHd : shl.AllPolygon())
		{
			auto plVtPos=shl.GetPolygonVertexPosition(plHd);
			if(0<plVtPos.size())
			{
				YsVec3 longest[2]={plVtPos[0],plVtPos[0]};
				double lMax=0;
				for(YSSIZE_T idx=0; idx<plVtPos.size(); ++idx)
				{
					double l=(plVtPos.GetCyclic(idx+1)-plVtPos[idx]).GetSquareLength();
					if(lMax<l)
					{
						lMax=l;
						longest[0]=plVtPos[idx];
						longest[1]=plVtPos.GetCyclic(idx+1);
					}
				}
				if(sqrt(lMax)<YsTolerance)
				{
					selPlHd.Add(plHd);
				}
				else
				{
					double dMax=0;
					for(auto p : plVtPos)
					{
						double d=YsGetPointLineDistance3(longest[0],longest[1],p);
						if(dMax<d)
						{
							dMax=d;
						}
						if(dThr<dMax)
						{
							break;
						}
					}
					if(dMax<=dThr)
					{
						selPlHd.Add(plHd);
					}
				}
			}
		}
		shl.SelectPolygon(selPlHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Select_Polygon_CannotCalculateNormal(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();

		YsShellPolygonStore selPlHd(shl.Conv());
		for(auto plHd : shl.AllPolygon())
		{
			YsVec3 nom;
			auto plVtPos=shl.GetPolygonVertexPosition(plHd);
			if(YSOK!=YsGetAverageNormalVector(nom,plVtPos))
			{
				selPlHd.Add(plHd);
			}
		}
		shl.SelectPolygon(selPlHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_SELECTED_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
}
