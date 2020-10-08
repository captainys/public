/* ////////////////////////////////////////////////////////////

File Name: sketchmenu.cpp
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

#include <ysshellext.h>

#include "sketch.h"


/* static */ void GeblGuiEditorBase::Sketch_CreateConstEdgeBySketch(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;

	canvas.Edit_ClearUIMode();

	if(NULL!=canvas.slHd)
	{
		auto &shl=*canvas.slHd;
		canvas.sketchUI->StartSketchInterface(shl.Conv(),canvas.drawEnv);
		canvas.LButtonCallBack=Sketch_CreateConstEdge_LButtonCallBack;
		canvas.MouseMoveCallBack=Sketch_CreateConstEdge_MouseMoveCallBack;
		canvas.UIDrawCallBack2D=Sketch_DrawCallBack2D;
		canvas.SetNeedRedraw(YSTRUE);
	}
}

/* static */ void GeblGuiEditorBase::Sketch_DrawCallBack2D(GeblGuiEditorBase &canvas)
{
	auto strokePoint=canvas.sketchUI->GetStroke();

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


	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLUseWindowCoordinateInPlain2DDrawing(YsGLSLSharedPlain2DRenderer(),YSTRUE);

	glDepthMask(0);

	YsGLSLDrawPlain2DPrimitivefv(YsGLSLSharedPlain2DRenderer(),GL_LINE_STRIP,coord.GetN()/2,coord,color);

	glDepthMask(1);

	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
}

/* static */ YSRESULT GeblGuiEditorBase::Sketch_CreateConstEdge_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL ,YSBOOL ,int ,int )
{

	if(YSTRUE==lb) // Button down
	{
		canvas.sketchUI->BeginStroke();
	}
	else // Button up
	{
		canvas.sketchUI->EndStroke();

		if(YSOK==canvas.sketchUI->ProjectToShell())
		{
			YsShellExtEdit::StopIncUndo stopIndUndo(canvas.slHd);

			YsArray <YsShellVertexHandle> ceVtHd;

			auto stroke=canvas.sketchUI->GetStroke();
			for(auto &strokePoint : stroke)
			{
				if(NULL!=strokePoint.prjPlHd)
				{
					ceVtHd.Append(canvas.slHd->AddVertex(strokePoint.prjCoord));
				}
			}

			canvas.slHd->AddConstEdge(ceVtHd,YSFALSE);

			canvas.needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_CONSTEDGE;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
	return YSOK;
}

/* static */ YSRESULT GeblGuiEditorBase::Sketch_CreateConstEdge_MouseMoveCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL ,YSBOOL ,int mx,int my)
{



	if(YSTRUE==lb)
	{
		canvas.sketchUI->AddStrokePoint(YsVec2(mx,my));
		canvas.SetNeedRedraw(YSTRUE);
	}
	return YSOK;
}


void GeblGuiEditorBase::Sketch_MakeSheet(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode();

	if(NULL!=slHd)
	{
		auto &shl=*slHd;
		sketchUI->StartSketchInterface(shl.Conv(),drawEnv);
		BindLButtonDownCallBack(&THISCLASS::Sketch_MakeSheet_LButtonDownCallBack,this);
		BindLButtonUpCallBack(&THISCLASS::Sketch_MakeSheet_LButtonUpCallBack,this);
		BindMouseMoveCallBack(&THISCLASS::Sketch_MakeSheet_MouseMoveCallBack,this);
		UIDrawCallBack2D=Sketch_DrawCallBack2D;
		SetNeedRedraw(YSTRUE);
	}
}
YSRESULT GeblGuiEditorBase::Sketch_MakeSheet_LButtonDownCallBack(FsGuiMouseButtonSet btn,YsVec2i pos)
{
	sketchUI->BeginStroke();
	return YSOK;
}
YSRESULT GeblGuiEditorBase::Sketch_MakeSheet_LButtonUpCallBack(FsGuiMouseButtonSet btn,YsVec2i pos)
{
	sketchUI->EndStroke();
	if(nullptr!=slHd && 2<=sketchUI->GetStroke().size())
	{
		SketchUtil_MakeSheet(*slHd);
		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON;
		SetNeedRedraw(YSTRUE);
	}
	return YSOK;
}
YSRESULT GeblGuiEditorBase::Sketch_MakeSheet_MouseMoveCallBack(FsGuiMouseButtonSet btn,YsVec2i from,YsVec2i to)
{
	if(YSTRUE==btn.lb)
	{
		sketchUI->AddStrokePoint(YsVec2(to.xf(),to.yf()));
		SetNeedRedraw(YSTRUE);
		return YSOK;
	}
	return YSERR;
}

void GeblGuiEditorBase::SketchUtil_MakeSheet(YsShellExtEdit &shl)
{
	const int uDiv=16;
	const int vDiv=16;

	sketchUI->Resample(uDiv);

	YsArray <YsShell::VertexHandle> vtHdArray;
	double nearz,farz;
	sketchUI->CalculateNearFarFromIntersection(nearz,farz);
	if(farz<nearz)
	{
		nearz=drawEnv.GetViewDistance()*0.1;
		farz=drawEnv.GetViewDistance()*1.9;
	}
	else
	{
		double d=farz-nearz;
		farz+=d*0.2;
		nearz-=d*0.2;
	}

	for(auto &p : sketchUI->GetStroke())
	{
		YsVec3 nearPos,farPos;
		drawEnv.TransformScreenCoordTo3DWithZ(nearPos,p.winCoord.x(),p.winCoord.y(),nearz);
		drawEnv.TransformScreenCoordTo3DWithZ(farPos,p.winCoord.x(),p.winCoord.y(),farz);
		for(int i=0; i<=vDiv; ++i)
		{
			double t=(double)i/(double)vDiv;
			vtHdArray.push_back(shl.AddVertex(nearPos*(1.0-t)+farPos*t));
		}
	}

	for(int u=0; u<uDiv; ++u)
	{
		for(int v=0; v<vDiv; ++v)
		{
			YsShell::VertexHandle quadVtHd[4]=
			{
				vtHdArray[(v  )*(uDiv+1)+u],
				vtHdArray[(v+1)*(uDiv+1)+u],
				vtHdArray[(v+1)*(uDiv+1)+u+1],
				vtHdArray[(v  )*(uDiv+1)+u+1],
			};

			YsShell::VertexHandle triVtHd[2][3];
			triVtHd[0][0]=quadVtHd[0];
			triVtHd[0][1]=quadVtHd[1];
			triVtHd[0][2]=quadVtHd[2];
			triVtHd[1][0]=quadVtHd[2];
			triVtHd[1][1]=quadVtHd[3];
			triVtHd[1][2]=quadVtHd[0];
			shl.AddPolygon(3,triVtHd[0]);
			shl.AddPolygon(3,triVtHd[1]);
		}
	}
}
