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
