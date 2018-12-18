/* ////////////////////////////////////////////////////////////

File Name: fsgui3dapp_draw.cpp
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
#include <ysclass.h>
#include <ysglslcpp.h>
#include <ysglbuffermanager_gl2.h>

#include "ysgebl_gui_editor_base.h"
#include "ysgebl_gui_extension_base.h"
#include "miscdlg.h"
#include "refbmp/refbmp.h"
#include "ysglbuffer.h"
#include "config/polycreconfig.h"


void GeblGuiEditorBase::DrawGuiOnly(void)
{
	int viewport[4];
	drawEnv.GetOpenGlCompatibleViewport(viewport);
	drawEnv.SetVerticalOrientation(YSTRUE);
	drawEnv.UpdateNearFar();

	SetNeedRedraw(YSFALSE);
	drawEnv.SetNeedRedraw(YSFALSE);
	threeDInterface.SetNeedRedraw(YSFALSE);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);

	// glMatrixMode(GL_PROJECTION);
	// glLoadIdentity();
	// glOrtho(0.0,(double)viewport[2],(double)viewport[3],0.0,-1.0,1.0);
	// glMatrixMode(GL_MODELVIEW);
	// glLoadIdentity();

	FsGuiCanvas::Show();
	FsSwapBuffers();
}

void GeblGuiEditorBase::Draw(void)
{
	if(YSTRUE!=drawingMasterSwitch)
	{
		DrawGuiOnly();
		return;
	}

	// Do this at the beginning of Draw funtion.  This will allow one of the elements set SetNeedRedraw(YSTRUE) 
	// within drawing function so that Draw function will be called again in the next iteragion. >>
	SetNeedRedraw(YSFALSE);
	drawEnv.SetNeedRedraw(YSFALSE);
	threeDInterface.SetNeedRedraw(YSFALSE);
	// <<


	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1,1);


	if(NULL==slHd)
	{
		if(NULL!=GetTopStatusBar())
		{
			GetTopStatusBar()->ClearStringAll();
		}
		if(NULL!=GetBottomStatusBar())
		{
			GetBottomStatusBar()->ClearStringAll();
		}
	}

	if(NULL!=slHd && NULL!=GetTopStatusBar())
	{
		YsWString ful,pth,fil;
		slHd->GetFileName(ful);
		if(0==ful.size() && 0<lastAccessedFileName.size())
		{
			ful=lastAccessedFileName;
		}
		ful.SeparatePathFile(pth,fil);
		if(YSTRUE==slHd->IsModified())
		{
			fil.Append(L"(*)");
		}

		YsWString curMsg;
		if(0!=YsWString::Strcmp(fil,GetTopStatusBar()->GetString(curMsg,0)))
		{
			GetTopStatusBar()->SetString(0,fil);
		}


		if(YSTRUE==dnmEditMode)
		{
			if(0>dnmCurrentState)
			{
				GetTopStatusBar()->SetString(1,"DNM-Edit Mode: STATIC mode");
			}
			else
			{
				YsString str;
				str.Printf("DNM-Edit Mode: State%d",dnmCurrentState);
				GetTopStatusBar()->SetString(1,str);
			}
		}
		else
		{
			GetTopStatusBar()->SetString(1,"");
		}
	}

	if(NULL!=slHd && NULL!=GetBottomStatusBar())
	{
		YsString str("Selection");
		YsString append;

		{
			YsArray <YsShellVertexHandle> selVtHd;
			slHd->GetSelectedVertex(selVtHd);
			if(0<selVtHd.GetN())
			{
				append.Printf("  Vertex:%d",(int)selVtHd.GetN());
				str.Append(append);
			}
		}
		{
			YsArray <YsShellPolygonHandle> selPlHd;
			slHd->GetSelectedPolygon(selPlHd);
			if(0<selPlHd.GetN())
			{
				append.Printf("  Polygon:%d",(int)selPlHd.GetN());
				str.Append(append);
			}
		}
		{
			YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
			slHd->GetSelectedConstEdge(selCeHd);
			if(0<selCeHd.GetN())
			{
				append.Printf("  ConstEdge:%d",(int)selCeHd.GetN());
				str.Append(append);
			}
		}
		{
			YsArray <YsShellExt::FaceGroupHandle> selFgHd;
			slHd->GetSelectedFaceGroup(selFgHd);
			if(0<selFgHd.GetN())
			{
				append.Printf("  FaceGroup:%d",(int)selFgHd.GetN());
				str.Append(append);
			}
		}
		GetBottomStatusBar()->SetString(0,str);
		GetBottomStatusBar()->SetString(1,modeMessage);
	}


	int viewport[4];
	drawEnv.GetOpenGlCompatibleViewport(viewport);
	drawEnv.SetVerticalOrientation(YSTRUE);
	drawEnv.UpdateNearFar();


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	if(YsCoordSysModel==YSOPENGL)
	{
		glFrontFace(GL_CCW);
	}
	else
	{
		glFrontFace(GL_CW);
	}


	const YsMatrix4x4 &projMat=drawEnv.GetProjectionMatrix();
	const YsMatrix4x4 &viewMat=drawEnv.GetViewMatrix();
	const YsMatrix4x4 projViewMat=(projMat*viewMat);

	threeDInterface.SetViewport(viewport);
	threeDInterface.SetProjViewModelMatrix(projViewMat);


	glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);

	GLfloat glProjMat[16];
	projMat.GetOpenGlCompatibleMatrix(glProjMat);

	GLfloat glModelviewMat[16];
	viewMat.GetOpenGlCompatibleMatrix(glModelviewMat);


	// glMatrixMode(GL_PROJECTION);
	// glLoadIdentity();
	// glMultMatrixf(glProjMat);
	// glMatrixMode(GL_MODELVIEW);
	// glLoadIdentity();
	// glMultMatrixf(glModelviewMat);


	YsGLSLSetShared3DRendererProjection(glProjMat);
	YsGLSLSetShared3DRendererModelView(glModelviewMat);
	{
		YsGLSLBitmapFontRendererClass renderer;
		renderer.SetProjection(glProjMat);
		renderer.SetModelView(glModelviewMat);
	}


	YsVec3 lightDir=YsUnitVector(YsVec3(0.1,1.0,3.0));
	const GLfloat lightDirf[]={lightDir.xf(),lightDir.yf(),lightDir.zf()};
	YsGLSLSetShared3DRendererDirectionalLightInCameraCoordinatefv(0,lightDirf);
	YsGLSLSetShared3DRendererSpecularColor(config->GetSpecularColor());
	YsGLSLSetShared3DRendererAmbientColor(config->GetAmbientColor());
	YsGLSLSetShared3DRendererLightColor(0,config->GetLightColor());



	actuallyDrawVertex=drawVertex;
	actuallyDrawConstEdge=drawConstEdge;
	actuallyDrawPolygonEdge=drawPolygonEdge;
	actuallyDrawPolygon=drawPolygon;
	actuallyDrawShrunkPolygon=drawShrunkPolygon;
	actuallyDrawNonManifoldEdge=drawNonManifoldEdge;
	if(YSTRUE==drawOtherShellWireframe || YSTRUE==drawOtherShellFill)
	{
		actuallyDrawOtherShell=YSTRUE;
	}
	else
	{
		actuallyDrawOtherShell=YSFALSE;
	}

	if(NULL!=UIBeforeDrawCallBack)
	{
		(*UIBeforeDrawCallBack)(*this);
	}

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


	if(YSTRUE==FsGuiPopUpMenuItem::GetCheck(viewDrawBackgroundGradation) && (NULL==guiExtension || YSTRUE!=guiExtension->DrawBackground()))
	{
// Linear interpolation looks to be too fast.  Probably need three colors.
		YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
		YsGLSLUseWindowCoordinateInPlain2DDrawing(YsGLSLSharedPlain2DRenderer(),YSTRUE);

		glDepthMask(0);

		const GLfloat rect[12]=
		{
			0                   ,0,
			(GLfloat)viewport[2],0,
			0                   ,(GLfloat)viewport[3]/2.0f,
			(GLfloat)viewport[2],(GLfloat)viewport[3]/2.0f,
			0                   ,(GLfloat)viewport[3],
			(GLfloat)viewport[2],(GLfloat)viewport[3]
		};
		const GLfloat color[24]=
		{
			config->bgColor[0].Rf(),config->bgColor[0].Gf(),config->bgColor[0].Bf(),1,
			config->bgColor[0].Rf(),config->bgColor[0].Gf(),config->bgColor[0].Bf(),1,
			config->bgColor[1].Rf(),config->bgColor[1].Gf(),config->bgColor[1].Bf(),1,
			config->bgColor[1].Rf(),config->bgColor[1].Gf(),config->bgColor[1].Bf(),1,
			config->bgColor[2].Rf(),config->bgColor[2].Gf(),config->bgColor[2].Bf(),1,
			config->bgColor[2].Rf(),config->bgColor[2].Gf(),config->bgColor[2].Bf(),1
		};
		YsGLSLDrawPlain2DPrimitivefv(YsGLSLSharedPlain2DRenderer(),GL_TRIANGLE_STRIP,6,rect,color);

		glDepthMask(1);

		YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	}



	threeDInterface.Draw();


	if(NULL!=slHd && YSTRUE==slHd->NeedRemakeHighlightVertexBuffer())
	{
		slHd->GetHighlight().RemakeVertexBuffer(*slHd);
		slHd->SetNeedRemakeHighlightVertexBuffer(YSFALSE);
	}

	if(NEED_REMAKE_DRAWING_NONE!=needRemakeDrawingBuffer || (nullptr!=slHd && YSTRUE==slHd->GetTemporaryModification().Changed()))
	{
		RemakeDrawingBuffer(slHd,needRemakeDrawingBuffer);
		needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_NONE;
		slHd->GetTemporaryModification().ClearChangeFlag();
	}
	if(NEED_REMAKE_DRAWING_NONE!=needRemakeDrawingBuffer_for_OtherShell && YSTRUE==actuallyDrawOtherShell)
	{
		YsArray <YsShellDnmContainer <YsShellExtEditGui>::Node *> allNode;
		shlGrp.GetNodePointerAll(allNode);
		for(YSSIZE_T nodeIdx=0; nodeIdx<allNode.GetN(); ++nodeIdx)
		{
			if(slHd!=allNode[nodeIdx])
			{
				RemakeDrawingBuffer(allNode[nodeIdx],needRemakeDrawingBuffer_for_OtherShell);
			}
		}
		needRemakeDrawingBuffer_for_OtherShell=NEED_REMAKE_DRAWING_NONE;
	}


	{
		GLfloat prevTfm[16];
		GLfloat curNodeTfm[16];
		if(YSTRUE==dnmEditMode)
		{
			YsMatrix4x4 curMat;
			{
				YsGLSLShaded3DRenderer renderer;
				renderer.GetModelView(prevTfm);
				curMat.CreateFromOpenGlCompatibleMatrix(prevTfm);
			}
			YsMatrix4x4 tfm=curMat*dnmState.GetNodeToRootTransformation(slHd);
			tfm.GetOpenGlCompatibleMatrix(curNodeTfm);
			YsGLSLSetShared3DRendererModelView(curNodeTfm);

			if(YSTRUE!=dnmState.GetShow(slHd))
			{
			#ifdef GL_LINE_STIPPLE
				glEnable(GL_LINE_STIPPLE);
				glLineStipple(1,0x1111);
			#endif
			}
		}


		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();
		if(YSTRUE==actuallyDrawVertex && NULL!=slHd)
		{
			GeblGuiFoundation::DrawVertex(*slHd,viewport);
		}
		if(0<drawBuf.selectedVertexPosBuffer.GetNumVertex() && NULL!=slHd)
		{
			GeblGuiFoundation::DrawSelectedVertex(*slHd,viewport);
		}
		if(0<drawBuf.selectedVertexLineBuffer.GetNumVertex() && NULL!=slHd)
		{
			GeblGuiFoundation::DrawSelectedVertexLine(*slHd);
		}
		if(YSTRUE==actuallyDrawPolygonEdge && NULL!=slHd)
		{
			GeblGuiFoundation::DrawPolygonEdge(*slHd);
		}
		if(YSTRUE==actuallyDrawShrunkPolygon && NULL!=slHd)
		{
			GeblGuiFoundation::DrawShrunkPolygon(*slHd);

		}
		if(0<drawBuf.selectedPolygonPosBuffer.GetNumVertex() && NULL!=slHd)
		{
			GeblGuiFoundation::DrawSelectedPolygon(*slHd);
		}
		if(YSTRUE==actuallyDrawConstEdge && 0<drawBuf.constEdgeVtxBuffer.GetNumVertex() && NULL!=slHd)
		{
			GeblGuiFoundation::DrawConstEdge(*slHd);
		}
		if(0<drawBuf.selectedConstEdgeVtxBuffer.GetNumVertex() && NULL!=slHd)
		{
			GeblGuiFoundation::DrawSelectedConstEdge(*slHd);
		}
		if(YSTRUE==dnmEditMode)
		{
			YsGLSLSetShared3DRendererModelView(prevTfm);
		}


		if(YSTRUE!=dnmEditMode)
		{
			DrawInGeometryEditMode();
		}
		else
		{
			DrawInDnmEditMode();
		}

		if(YSTRUE==drawDnmConnection)
		{
			DrawDnmConnection();
		}


		if(YSTRUE==dnmEditMode)
		{
			YsGLSLSetShared3DRendererModelView(curNodeTfm);
		}
		if(0<=drawBuf.selectedFaceGroupVtxBuffer.GetNumVertex() && NULL!=slHd)
		{
			GeblGuiFoundation::DrawSelectedFaceGroup(*slHd);
		}
		if(YSTRUE==dnmEditMode)
		{
			YsGLSLSetShared3DRendererModelView(prevTfm);
		}

		if(YSTRUE==dnmEditMode && YSTRUE!=dnmState.GetShow(slHd))
		{
		#ifdef GL_LINE_STIPPLE
			glDisable(GL_LINE_STIPPLE);
		#endif
		}
	}




	if(NULL!=slHd && YSTRUE==actuallyDrawNonManifoldEdge)
	{
		if(YSTRUE==nonManifoldEdgeCache.IsModified(slHd))
		{
			nonManifoldEdgeCache.CacheNonManifoldEdge(slHd);
		}
		struct YsGLSL3DRenderer *renderer=YsGLSLSharedVariColor3DRenderer();
		YsGLSLUse3DRenderer(renderer);

		glLineWidth(3);
		YsGLSLDrawPrimitiveVtxColfv(renderer,GL_LINES,nonManifoldEdgeCache.lineVtxBuf.GetNi(),nonManifoldEdgeCache.lineVtxBuf,nonManifoldEdgeCache.lineColBuf);
		glLineWidth(1);

		YsGLSLEndUse3DRenderer(renderer);

		// The following could have easily be done by glPointSize.  However, stupid OpenGL ES 2.0 got rid of glPointSize.
		// OpenGL ES should be gone.  The hardware on the tablets and smart phones are good enough to support full-blown
		// OpenGL.  OpenGL ES will be remembered as a dark history of OpenGL.  That is bad.  Not as bad as Direct 3D though.
		renderer=YsGLSLSharedVariColorMarkerByPointSprite3DRenderer();
		YsGLSLUse3DRenderer(renderer);

#ifdef GL_PROGRAM_POINT_SIZE
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);
#endif
		YsGLSLSet3DRendererUniformMarkerType(renderer,YSGLSL_MARKER_TYPE_RECT);
		YsGLSLSet3DRendererUniformMarkerDimension(renderer,3.0f);
		YsGLSLDrawPrimitiveVtxColfv(
		    renderer,GL_POINTS,
		    nonManifoldEdgeCache.pointVtxBuf.GetNi(),
		    nonManifoldEdgeCache.pointVtxBuf,
		    nonManifoldEdgeCache.pointColBuf);
#ifdef GL_PROGRAM_POINT_SIZE
		glDisable(GL_POINT_SPRITE);
		glDisable(GL_PROGRAM_POINT_SIZE);
#endif
		YsGLSLEndUse3DRenderer(renderer);
	}

	if(YSTRUE==drawHighlight)
	{
		auto &bufManager=YsGLBufferManager::GetSharedBufferManager();
		auto &highlight=slHd->GetHighlight();

		YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());

		const GLfloat blue[]={0.0f,0.0f,1.0f,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,blue);

		{
			auto unitPtr=bufManager.GetBufferUnit(highlight.highlightedEdgeVboHandle);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				unitPtr->GetActualBuffer()->DrawPrimitiveVtx(renderer,GL_LINES);
			}
		}

		glLineWidth(2);
		{
			auto unitPtr=bufManager.GetBufferUnit(highlight.highlightedThickEdgeVboHandle);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				unitPtr->GetActualBuffer()->DrawPrimitiveVtx(renderer,GL_LINES);
			}
		}
		glLineWidth(1);

		const GLfloat darkBlue[]={0.0f,0.0f,0.7f,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,darkBlue);
		{
			auto unitPtr=bufManager.GetBufferUnit(highlight.highlightedPolygonVboHandle);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				unitPtr->GetActualBuffer()->DrawPrimitiveVtx(renderer,GL_TRIANGLES);
			}
		}
	}

	if(NULL!=UIDrawCallBack3D)
	{
		UIDrawCallBack3D(*this);
	}
	if(draw3dCallBack)
	{
		draw3dCallBack();
	}


	guiExtension->OnDraw3d();

	refBmpStore->Draw();


	if(YSTRUE==drawAxis)
	{
		const GLfloat axisLength=5.0;
		GeblGuiFoundation::DrawAxis(axisLength,viewport,projMat,viewMat);
	}



	// 2D Drawing
	// glMatrixMode(GL_PROJECTION);
	// glLoadIdentity();
	// glOrtho(0.0,(double)viewport[2],(double)viewport[3],0.0,-1.0,1.0);
	// glMatrixMode(GL_MODELVIEW);
	// glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLUseWindowCoordinateInPlain2DDrawing(YsGLSLSharedPlain2DRenderer(),YSTRUE);
	{
		if(YSTRUE==lBtnDown)
		{
			const GLfloat rect[8]=
			{
				(GLfloat)lBtnDownMx,(GLfloat)lBtnDownMy,
				(GLfloat)moveMx,    (GLfloat)lBtnDownMy,
				(GLfloat)moveMx,    (GLfloat)moveMy,
				(GLfloat)lBtnDownMx,(GLfloat)moveMy
			};
			const GLfloat color[16]=
			{
				0,0,0,0.8f,
				0,0,0,0.8f,
				0,0,0,0.8f,
				0,0,0,0.8f
			};
			YsGLSLDrawPlain2DPrimitivefv(YsGLSLSharedPlain2DRenderer(),GL_LINE_LOOP,4,rect,color);
		}
	}
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

	FsGuiCanvas::Show();

	if(NULL!=UIDrawCallBack2D)
	{
		(*UIDrawCallBack2D)(*this);
	}
	if(draw2dCallBack)
	{
		draw2dCallBack();
	}

	FsSwapBuffers();
}

void GeblGuiEditorBase::DrawInGeometryEditMode(void)
{
	if(YSTRUE==actuallyDrawPolygon && NULL!=slHd)
	{
		DrawShellPlainFill(slHd);
	}

	if(YSTRUE==actuallyDrawOtherShell)
	{
		YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *,16> allNode;
		shlGrp.GetNodePointerAll(allNode);
		for(YSSIZE_T nodeIdx=0; nodeIdx<allNode.GetN(); ++nodeIdx)
		{
			if(slHd!=allNode[nodeIdx])
			{
				if(YSTRUE==drawOtherShellWireframe)
				{
					YsShellExtDrawingBuffer &buf=allNode[nodeIdx]->GetDrawingBuffer();

					YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());

					const GLfloat edgeCol[4]={0,0,0,0.2f};
					YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
					YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,buf.normalEdgePosBuffer.GetNi(),buf.normalEdgePosBuffer);
					YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,buf.constEdgeVtxBuffer.GetNi(),buf.constEdgeVtxBuffer);
				}
				if(YSTRUE==drawOtherShellFill)
				{
					DrawShellPlainFill(allNode[nodeIdx]);
				}
			}
		}
	}
}

void GeblGuiEditorBase::DrawShellPlainFill(const YsShellDnmContainer <YsShellExtEditGui>::Node *slHd)
{
	// auto &buf=slHd->GetDrawingBuffer();
	auto &vboSet=slHd->GetVboSet();
	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();

	glEnable(GL_CULL_FACE);

	if(NULL!=YsGLSLSharedVariColorShaded3DRenderer())
	{
		auto unitPtr=bufManager.GetBufferUnit(vboSet.solidShadedPosNomColHd);
		if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
		{
			YsGLSLRenderer renderer(YsGLSLSharedVariColorShaded3DRenderer());
			unitPtr->GetActualBuffer()->DrawPrimitiveVtxNomCol(renderer,GL_TRIANGLES);
		}
	}
	if(NULL!=YsGLSLSharedVariColor3DRenderer())
	{
		YsGLSLRenderer renderer(YsGLSLSharedVariColor3DRenderer());

		{
			auto unitPtr=bufManager.GetBufferUnit(vboSet.solidUnshadedPosColHd);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				unitPtr->GetActualBuffer()->DrawPrimitiveVtxCol(renderer,GL_TRIANGLES);
			}
		}
		{
			auto unitPtr=bufManager.GetBufferUnit(vboSet.backFacePosColHd);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				unitPtr->GetActualBuffer()->DrawPrimitiveVtxCol(renderer,GL_TRIANGLES);
			}
		}
	}
	if(NULL!=YsGLSLSharedVariColorShaded3DRenderer())
	{
		auto unitPtr=bufManager.GetBufferUnit(vboSet.trspShadedPosNomColHd);
		if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
		{
			YsGLSLRenderer renderer(YsGLSLSharedVariColorShaded3DRenderer());
			unitPtr->GetActualBuffer()->DrawPrimitiveVtxNomCol(renderer,GL_TRIANGLES);
		}
	}
	if(NULL!=YsGLSLSharedVariColor3DRenderer())
	{
		auto unitPtr=bufManager.GetBufferUnit(vboSet.trspUnshadedPosColHd);
		if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
		{
			YsGLSLRenderer renderer(YsGLSLSharedVariColor3DRenderer());
			unitPtr->GetActualBuffer()->DrawPrimitiveVtxNomCol(renderer,GL_TRIANGLES);
		}
	}
	if(NULL!=YsGLSLSharedFlashByPointSprite3DRenderer())
	{
		auto unitPtr=bufManager.GetBufferUnit(vboSet.lightPosColPointSizeHd);
		if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
		{
			int viewport[4];
			glGetIntegerv(GL_VIEWPORT,viewport);

#ifdef GL_PROGRAM_POINT_SIZE
			glEnable(GL_PROGRAM_POINT_SIZE);  // Needed for enabling gl_PointSize in the vertex shader.
			glEnable(GL_POINT_SPRITE);        // Needed for enabling gl_PointCoord in the fragment shader.
#endif
			YsGLSLRenderer renderer(YsGLSLSharedFlashByPointSprite3DRenderer());
			YsGLSLSet3DRendererViewportDimensionf(renderer,(float)viewport[2],(float)viewport[3]);
			YsGLSLSet3DRendererPointSizeMode(renderer,YSGLSL_POINTSPRITE_SIZE_IN_3DSPACE);

			unitPtr->GetActualBuffer()->DrawPrimitiveVtxColPointSize(renderer,GL_POINTS);

#ifdef GL_PROGRAM_POINT_SIZE
			glDisable(GL_PROGRAM_POINT_SIZE);
			glDisable(GL_POINT_SPRITE);
#endif
		}
	}
	glDisable(GL_CULL_FACE);
}

void GeblGuiEditorBase::DrawInDnmEditMode(void)
{
	auto renderer=YsGLSLSharedVariColorShaded3DRenderer();
	GLfloat curGLTfm[16];

	YsGLSLUse3DRenderer(renderer);
	YsGLSLGet3DRendererModelViewfv(curGLTfm,renderer);
	YsGLSLEndUse3DRenderer(renderer);

	YsMatrix4x4 curTfm(YSFALSE);
	curTfm.CreateFromOpenGlCompatibleMatrix(curGLTfm);

	glEnable(GL_CULL_FACE);

	auto rootArray=shlGrp.GetRootNodeArray();
	for(auto node : rootArray)
	{
		DrawInDnmEditModeRecursive(node,dnmState,curTfm);
	}

	YsGLSL3DRenderer *usedRenderer[4]=
	{
		YsGLSLSharedVariColorShaded3DRenderer(),
		YsGLSLSharedVariColor3DRenderer(),
		YsGLSLSharedVariColorShaded3DRenderer(),
		YsGLSLSharedVariColor3DRenderer(),
	};
	for(int i=0; i<4; ++i)
	{
		YsGLSLUse3DRenderer(usedRenderer[i]);
		YsGLSLSet3DRendererModelViewfv(usedRenderer[i],curGLTfm);
		YsGLSLEndUse3DRenderer(usedRenderer[i]);
	}

	glDisable(GL_CULL_FACE);
}

void GeblGuiEditorBase::DrawInDnmEditModeRecursive(YsShellDnmContainer <YsShellExtEditGui>::Node *node,const YsShellDnmContainer <YsShellExtEditGui>::DnmState &state,const YsMatrix4x4 &curTfm)
{
	const YsMatrix4x4 tfm=curTfm*state.GetTransformation(node);

	GLfloat glTfm[16];
	tfm.GetOpenGlCompatibleMatrix(glTfm);

	for(auto childNode : node->children)
	{
		DrawInDnmEditModeRecursive(childNode,state,tfm);
	}

	auto &buf=node->GetDrawingBuffer();
	if(YSTRUE==state.GetShow(node))
	{
		YSBOOL fill=YSFALSE,wire=YSFALSE;

		if(node!=slHd)
		{
			fill=drawOtherShellFill;
			wire=drawOtherShellWireframe;
		}
		else
		{
			fill=drawPolygon;
			wire=YSFALSE; // Already taken care.
		}

		if(YSTRUE==fill)
		{
			if(NULL!=YsGLSLSharedVariColorShaded3DRenderer())
			{
				auto renderer=YsGLSLSharedVariColorShaded3DRenderer();
				YsGLSLUse3DRenderer(renderer);

				YsGLSLSet3DRendererModelViewfv(renderer,glTfm);
				YsGLSLDrawPrimitiveVtxNomColfv(
				    renderer,
				    GL_TRIANGLES,
				    buf.solidShadedVtxBuffer.GetNi(),
				    buf.solidShadedVtxBuffer,
				    buf.solidShadedNomBuffer,
				    buf.solidShadedColBuffer);
				YsGLSLEndUse3DRenderer(renderer);
			}
			if(NULL!=YsGLSLSharedVariColor3DRenderer())
			{
				auto renderer=YsGLSLSharedVariColor3DRenderer();
				YsGLSLUse3DRenderer(renderer);

				YsGLSLSet3DRendererModelViewfv(renderer,glTfm);
				YsGLSLDrawPrimitiveVtxColfv(
				    renderer,
				    GL_TRIANGLES,
				    buf.solidUnshadedVtxBuffer.GetNi(),
				    buf.solidUnshadedVtxBuffer,
				    buf.solidUnshadedColBuffer);

				YsGLSLEndUse3DRenderer(renderer);
			}
			if(NULL!=YsGLSLSharedVariColorShaded3DRenderer())
			{
				auto renderer=YsGLSLSharedVariColorShaded3DRenderer();
				YsGLSLUse3DRenderer(renderer);

				YsGLSLSet3DRendererModelViewfv(renderer,glTfm);
				YsGLSLDrawPrimitiveVtxNomColfv(
				    renderer,
				    GL_TRIANGLES,
				    buf.trspShadedVtxBuffer.GetNi(),
				    buf.trspShadedVtxBuffer,
				    buf.trspShadedNomBuffer,
				    buf.trspShadedColBuffer);
				YsGLSLEndUse3DRenderer(renderer);
			}
			if(NULL!=YsGLSLSharedVariColor3DRenderer())
			{
				auto renderer=YsGLSLSharedVariColor3DRenderer();
				YsGLSLUse3DRenderer(renderer);

				YsGLSLSet3DRendererModelViewfv(renderer,glTfm);
				YsGLSLDrawPrimitiveVtxColfv(
				    renderer,
				    GL_TRIANGLES,
				    buf.trspUnshadedVtxBuffer.GetNi(),
				    buf.trspUnshadedVtxBuffer,
				    buf.trspUnshadedColBuffer);
				YsGLSLEndUse3DRenderer(renderer);
			}
			if(NULL!=YsGLSLSharedFlashByPointSprite3DRenderer())
			{
				int viewport[4];
				glGetIntegerv(GL_VIEWPORT,viewport);

	#ifdef GL_PROGRAM_POINT_SIZE
				glEnable(GL_PROGRAM_POINT_SIZE);  // Needed for enabling gl_PointSize in the vertex shader.
				glEnable(GL_POINT_SPRITE);        // Needed for enabling gl_PointCoord in the fragment shader.
	#endif
				auto renderer=YsGLSLSharedFlashByPointSprite3DRenderer();
				YsGLSLUse3DRenderer(renderer);
				YsGLSLSet3DRendererViewportDimensionf(renderer,(float)viewport[2],(float)viewport[3]);
				YsGLSLSet3DRendererPointSizeMode(renderer,YSGLSL_POINTSPRITE_SIZE_IN_3DSPACE);
				YsGLSLDrawPrimitiveVtxColPointSizefv(
					renderer,
					GL_POINTS,
					buf.lightVtxBuffer.GetNi(),
					buf.lightVtxBuffer,
					buf.lightColBuffer,
					buf.lightSizeBuffer);
				YsGLSLEndUse3DRenderer(renderer);

	#ifdef GL_PROGRAM_POINT_SIZE
				glDisable(GL_PROGRAM_POINT_SIZE);
				glDisable(GL_POINT_SPRITE);
	#endif
			}
		}
		if(YSTRUE==wire)
		{
			YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());
			renderer.SetModelView(glTfm);

			const GLfloat edgeCol[4]={0,1,0,0.5f};
			YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
			YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,buf.normalEdgePosBuffer.GetNi(),buf.normalEdgePosBuffer);
		}
	}
	else if(YSTRUE!=state.GetShow(node) && YSTRUE==drawOtherShellWireframe)
	{
	#ifdef GL_LINE_STIPPLE
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1,0x1111);
	#endif

		YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());
		renderer.SetModelView(glTfm);

		const GLfloat edgeCol[4]={0,1,0,0.5f};
		YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,buf.normalEdgePosBuffer.GetNi(),buf.normalEdgePosBuffer);

	#ifdef GL_LINE_STIPPLE
		glDisable(GL_LINE_STIPPLE);
	#endif
	}
}

void GeblGuiEditorBase::DrawDnmConnection(void)
{
	glEnable(GL_CULL_FACE);

	auto rootArray=shlGrp.GetRootNodeArray();
	for(auto node : rootArray)
	{
		DrawDnmConnectionRecursive(YsVec3::Origin(),node,YsIdentity4x4());
	}

	glDisable(GL_CULL_FACE);
}
void GeblGuiEditorBase::DrawDnmConnectionRecursive(const YsVec3 &parentPos,YsShellDnmContainer <YsShellExtEditGui>::Node *node,const YsMatrix4x4 &curTfm)
{
	auto posInParent=curTfm*node->posInParent;

	YsMatrix4x4 tfm;
	if(YSTRUE==dnmEditMode)
	{
		tfm=curTfm*dnmState.GetTransformation(node);
	}
	else
	{
		tfm=curTfm;
	}

	auto localRotationCenter=tfm*node->localRotationCenter;
	YsVec3 localRotationAxis;
	tfm.Mul(localRotationAxis,node->localRotationAxis,0.0);

	YsGLVertexBuffer vtxBuf;
	vtxBuf.Add(parentPos);
	vtxBuf.Add(posInParent);
	vtxBuf.Add(localRotationCenter);

	{
		YsGLSLPlain3DRenderer renderer;
		renderer.SetUniformColor(YsGreen());
		renderer.DrawVtx(GL_LINE_STRIP,vtxBuf.GetN(),vtxBuf);
	}

	if(YSTRUE==dnmEditMode)
	{
		YsGLVertexBuffer vtxBuf;
		YsGLNormalBuffer nomBuf;

		YsGLCreateArrowForGL_TRIANGLES(vtxBuf,nomBuf,localRotationCenter,localRotationCenter+localRotationAxis,YSTRUE,YSFALSE,0.01,0.03,0.3);

		YsGLSLShaded3DRenderer renderer;
		renderer.SetUniformColor(YsGreen());
		renderer.DrawVtxNom(GL_TRIANGLES,vtxBuf.GetN(),vtxBuf,nomBuf);
	}

	for(auto childNode : node->children)
	{
		DrawDnmConnectionRecursive(localRotationCenter,childNode,tfm);
	}
}

void GeblGuiEditorBase::RemakeDrawingBuffer(
    YsShellDnmContainer <YsShellExtEditGui>::Node *slHd,
    unsigned int needRemakeDrawingBuffer)
{
	auto &buf=slHd->GetDrawingBuffer();
	auto &vboSet=slHd->GetVboSet();

	YsShellExtDrawingBuffer::ShapeInfo shapeInfo;
	const YsShellExt &shlExt=slHd->Conv();
	const auto &mod=slHd->GetTemporaryModification();

	shapeInfo.shlExtPtr=&shlExt;
	shapeInfo.modPtr=&mod;

	buf.drawBackFaceInDifferentColor=drawBackFaceInDifferentColor;
	buf.backFaceColor=backFaceColor;

	if(0!=(needRemakeDrawingBuffer & NEED_REMAKE_DRAWING_VERTEX) && NULL!=slHd)
	{
		slHd->RemakeNormalVertexBuffer(buf,vboSet,normalVertexMarkerRadius,roundVertexMarkerRadius);
	}

	if(0!=(needRemakeDrawingBuffer & NEED_REMAKE_DRAWING_SELECTED_VERTEX))
	{
		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);
		slHd->RemakeSelectedVertexBuffer(buf,vboSet,selectedVertexMarkerRadius,selVtHd);
	}

	if(0!=(needRemakeDrawingBuffer & NEED_REMAKE_DRAWING_POLYGON) || 
	   YSTRUE==mod.PolygonColorChanged())
	{
		slHd->RemakePolygonBuffer(buf,vboSet,shapeInfo,polygonShrinkRatioForPicking);
	}

	if(0!=(needRemakeDrawingBuffer & NEED_REMAKE_DRAWING_SELECTED_POLYGON))
	{
		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);
		slHd->RemakeSelectedPolygonBuffer(buf,vboSet,polygonShrinkRatioForPicking,selPlHd);
	}

	if(0!=(needRemakeDrawingBuffer & (NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE)))
	{
		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		slHd->GetSelectedConstEdge(selCeHd);
		slHd->RemakeConstEdgeBuffer(buf,vboSet,selCeHd);
		slHd->RemakeSelectedConstEdgeBuffer(buf,vboSet,selCeHd);
	}

	if(0!=(needRemakeDrawingBuffer & (NEED_REMAKE_DRAWING_SELECTED_FACEGROUP)))
	{
		YsArray <YsShellExt::FaceGroupHandle> selFgHd;
		slHd->GetSelectedFaceGroup(selFgHd);
		slHd->RemakeSelectedFaceGroupBuffer(buf,vboSet,selFgHd);
	}
}

