/* ////////////////////////////////////////////////////////////

File Name: ysgebl_gui_foundation_draw.cpp
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
#include <ysglslcpp.h>
#include <ysglbuffermanager_gl2.h>


#include "ysgebl_gui_foundation.h"



void GeblGuiFoundation::DrawAxis(const double length,const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &viewMat) const
{
	const GLfloat axisLength=(GLfloat)length;
	const GLfloat axisLine[6*3]=
	{
		0,0,0,  axisLength,0,0,
		0,0,0,  0,axisLength,0,
		0,0,0,  0,0,axisLength
	};
	const GLfloat axisColor[4]=
	{
		0,0,0,1.0f
	};

	YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());

	YsGLSLSet3DRendererUniformColorfv(renderer,axisColor);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,6,axisLine);

	int nTri=0;
	GLfloat triCoord[3*3*3];
	YsVec3 tri[3];
	if(YSOK==FsGui3DGetArrowHeadTriangle(tri,YsOrigin(),axisLength*YsXVec(),viewport,projMat,viewMat,0.1,10,8))
	{
		triCoord[nTri*9  ]=(GLfloat)tri[0].x();
		triCoord[nTri*9+1]=(GLfloat)tri[0].y();
		triCoord[nTri*9+2]=(GLfloat)tri[0].z();
		triCoord[nTri*9+3]=(GLfloat)tri[1].x();
		triCoord[nTri*9+4]=(GLfloat)tri[1].y();
		triCoord[nTri*9+5]=(GLfloat)tri[1].z();
		triCoord[nTri*9+6]=(GLfloat)tri[2].x();
		triCoord[nTri*9+7]=(GLfloat)tri[2].y();
		triCoord[nTri*9+8]=(GLfloat)tri[2].z();
		++nTri;
	}
	if(YSOK==FsGui3DGetArrowHeadTriangle(tri,YsOrigin(),axisLength*YsYVec(),viewport,projMat,viewMat,0.1,10,8))
	{
		triCoord[nTri*9  ]=(GLfloat)tri[0].x();
		triCoord[nTri*9+1]=(GLfloat)tri[0].y();
		triCoord[nTri*9+2]=(GLfloat)tri[0].z();
		triCoord[nTri*9+3]=(GLfloat)tri[1].x();
		triCoord[nTri*9+4]=(GLfloat)tri[1].y();
		triCoord[nTri*9+5]=(GLfloat)tri[1].z();
		triCoord[nTri*9+6]=(GLfloat)tri[2].x();
		triCoord[nTri*9+7]=(GLfloat)tri[2].y();
		triCoord[nTri*9+8]=(GLfloat)tri[2].z();
		++nTri;
	}
	if(YSOK==FsGui3DGetArrowHeadTriangle(tri,YsOrigin(),axisLength*YsZVec(),viewport,projMat,viewMat,0.1,10,8))
	{
		triCoord[nTri*9  ]=(GLfloat)tri[0].x();
		triCoord[nTri*9+1]=(GLfloat)tri[0].y();
		triCoord[nTri*9+2]=(GLfloat)tri[0].z();
		triCoord[nTri*9+3]=(GLfloat)tri[1].x();
		triCoord[nTri*9+4]=(GLfloat)tri[1].y();
		triCoord[nTri*9+5]=(GLfloat)tri[1].z();
		triCoord[nTri*9+6]=(GLfloat)tri[2].x();
		triCoord[nTri*9+7]=(GLfloat)tri[2].y();
		triCoord[nTri*9+8]=(GLfloat)tri[2].z();
		++nTri;
	}

	if(0<nTri)
	{
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_TRIANGLES,nTri*3,triCoord);
	}
}



void GeblGuiFoundation::DrawVertex(const YsShellExtEditGui &shl,const int viewport[4]) const
{
#ifdef GL_POINT_SPRITE
    glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
#endif
    
	YsGLSLRenderer renderer(YsGLSLSharedMonoColorMarkerByPointSprite3DRenderer());
	YsShellExtDrawingBuffer &buf=shl.GetDrawingBuffer();

	const GLfloat vtxCol[4]={1.0,0.0,1.0,1.0};
	YsGLSLSet3DRendererUniformColorfv(renderer,vtxCol);
	if(0<buf.normalVertexPosBuffer.GetNumVertex())
	{
		YsGLSLSet3DRendererUniformMarkerDimension(renderer,normalVertexMarkerRadius);
		YsGLSLSet3DRendererUniformMarkerType(renderer,YSGLSL_MARKER_TYPE_RECT);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_POINTS,(int)buf.normalVertexPosBuffer.GetNumVertex(),buf.normalVertexPosBuffer);
	}
	if(0<buf.roundVertexPosBuffer.GetNumVertex())
	{
		YsGLSLSet3DRendererUniformMarkerDimension(renderer,roundVertexMarkerRadius);
		YsGLSLSet3DRendererUniformMarkerType(renderer,YSGLSL_MARKER_TYPE_CIRCLE);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_POINTS,(int)buf.roundVertexPosBuffer.GetNumVertex(),buf.roundVertexPosBuffer);
	}

#ifdef GL_POINT_SPRITE
	glDisable(GL_POINT_SPRITE);
	glDisable(GL_PROGRAM_POINT_SIZE);
#endif
}



void GeblGuiFoundation::DrawSelectedVertex(const YsShellExtEditGui &shl,const int viewport[4]) const
{
	YsGLSLRenderer renderer(YsGLSLSharedMonoColorMarkerByPointSprite3DRenderer());
	YsShellExtDrawingBuffer &buf=shl.GetDrawingBuffer();

#ifdef GL_POINT_SPRITE
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
#endif
    
	const GLfloat selVtxCol[4]={0.0f,0.0f,1.0f,1.0f};
	YsGLSLSet3DRendererUniformColorfv(renderer,selVtxCol);
	YsGLSLSet3DRendererUniformMarkerDimension(renderer,selectedVertexMarkerRadius);
	YsGLSLSet3DRendererUniformMarkerType(renderer,YSGLSL_MARKER_TYPE_EMPTY_RECT);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_POINTS,(int)buf.selectedVertexPosBuffer.GetNumVertex(),buf.selectedVertexPosBuffer);

#ifdef GL_POINT_SPRITE
	glDisable(GL_POINT_SPRITE);
	glDisable(GL_PROGRAM_POINT_SIZE);
#endif
}



void GeblGuiFoundation::DrawSelectedVertexLine(const YsShellExtEditGui &shl) const
{
	YsGLSLRenderer renderer(YsGLSLSharedVariColor3DRenderer());
	YsShellExtDrawingBuffer &buf=shl.GetDrawingBuffer();
	YsGLSLDrawPrimitiveVtxColfv(renderer,GL_LINES,(int)buf.selectedVertexLineBuffer.GetNumVertex(),buf.selectedVertexLineBuffer,buf.selectedVertexLineColBuffer);
}



void GeblGuiFoundation::DrawPolygonEdge(const YsShellExtEditGui &shl) const
{
	auto &vboSet=slHd->GetVboSet();
	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();

	auto bufPtr=bufManager.GetActualBuffer(vboSet.normalEdgePosHd);
	if(nullptr!=bufPtr)
	{
		YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());

		const GLfloat edgeCol[4]={0,0,0,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
		bufPtr->DrawPrimitiveVtx(renderer,GL_LINES);
	}
}



void GeblGuiFoundation::DrawSelectedPolygon(const YsShellExtEditGui &shl) const
{
	auto &vboSet=slHd->GetVboSet();
	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();

	auto unitPtr=bufManager.GetBufferUnit(vboSet.selectedPolygonPosColHd);
	if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
	{
		YsGLSLRenderer renderer(YsGLSLSharedVariColor3DRenderer());

		glLineWidth(3.0f);
		unitPtr->GetActualBuffer()->DrawPrimitiveVtxCol(renderer,GL_LINES);
		glLineWidth(1.0f);
	}
}



void GeblGuiFoundation::DrawShrunkPolygon(const YsShellExtEditGui &shl) const
{
	auto &vboSet=slHd->GetVboSet();
	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();

	auto bufPtr=bufManager.GetActualBuffer(vboSet.shrunkEdgePosHd);
	if(nullptr!=bufPtr)
	{
		YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());
		const GLfloat edgeCol[4]={0,0,0,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
		bufPtr->DrawPrimitiveVtx(renderer,GL_LINES);
	}
}



void GeblGuiFoundation::DrawConstEdge(const YsShellExtEditGui &shl) const
{
	auto &vboSet=shl.GetVboSet();
	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();

	auto unitPtr=bufManager.GetBufferUnit(vboSet.constEdgePosHd);
	if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
	{
		YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());
		glLineWidth(3);

		const GLfloat constEdgeCol[4]={0.0,0.0,0.0,1.0};
		YsGLSLSet3DRendererUniformColorfv(renderer,constEdgeCol);
		unitPtr->GetActualBuffer()->DrawPrimitiveVtx(renderer,GL_LINES);

		glLineWidth(1);
	}
}



void GeblGuiFoundation::DrawSelectedConstEdge(const YsShellExtEditGui &shl) const
{
	auto &vboSet=slHd->GetVboSet();
	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();

	auto lineBufPtr=bufManager.GetActualBuffer(vboSet.selectedConstEdgePosHd);
	auto triBufPtr=bufManager.GetActualBuffer(vboSet.selectedConstEdgeArrowHeadPosHd);
	if(nullptr!=lineBufPtr && nullptr!=triBufPtr)
	{
		YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());

		glLineWidth(4);

		const GLfloat selectedConstEdgeCol[4]={1.0,0.0,1.0,1.0};
		YsGLSLSet3DRendererUniformColorfv(renderer,selectedConstEdgeCol);
		lineBufPtr->DrawPrimitiveVtx(renderer,GL_LINES);
		triBufPtr->DrawPrimitiveVtx(renderer,GL_TRIANGLES);

		glLineWidth(1);
	}
}



void GeblGuiFoundation::DrawSelectedFaceGroup(const YsShellExtEditGui &shl) const
{
	auto &vboSet=slHd->GetVboSet();
	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();
	auto bufPtr=bufManager.GetActualBuffer(vboSet.selectedFaceGroupPosHd);
	if(nullptr!=bufPtr)
	{
		YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());

		const GLfloat selectedFaceGroupCol[4]={0.0,1.0,1.0,1.0};
		YsGLSLSet3DRendererUniformColorfv(renderer,selectedFaceGroupCol);
		bufPtr->DrawPrimitiveVtx(renderer,GL_TRIANGLES);
	}
}
