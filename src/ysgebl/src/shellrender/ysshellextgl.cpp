/* ////////////////////////////////////////////////////////////

File Name: ysshellextgl.cpp
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

#include "ysshellextgl.h"



YsShellExtVboSet::YsShellExtVboSet()
{
	Initialize();
}

void YsShellExtVboSet::Initialize(void)
{
	normalVtxPosHd=nullptr;
	roundVtxPosHd=nullptr;

	selectedVtxPosColHd=nullptr;
	selectedVtxLinePosColHd=nullptr;

	selectedPolygonPosColHd=nullptr;

	normalEdgePosHd=nullptr;
	shrunkEdgePosHd=nullptr;

	solidShadedPosNomColHd=nullptr;
	solidUnshadedPosColHd=nullptr;
	trspShadedPosNomColHd=nullptr;
	trspUnshadedPosColHd=nullptr;

	for(auto &hd : plainPlgPosHd)
	{
		hd=nullptr;
	}

	backFacePosColHd=nullptr;

	lightPosColPointSizeHd=nullptr;

	constEdgePosHd=nullptr;

	selectedConstEdgePosHd=nullptr;

	selectedConstEdgeArrowHeadPosHd=nullptr;

	selectedFaceGroupPosHd=nullptr;
}

YsShellExtVboSet::~YsShellExtVboSet()
{
	CleanUp();
}

void YsShellExtVboSet::CleanUp(void)
{
	auto &bufferManager=YsGLBufferManager::GetSharedBufferManager();

	bufferManager.Delete(normalVtxPosHd);
	bufferManager.Delete(roundVtxPosHd);

	bufferManager.Delete(selectedVtxPosColHd);
	bufferManager.Delete(selectedVtxLinePosColHd);

	bufferManager.Delete(selectedPolygonPosColHd);

	bufferManager.Delete(normalEdgePosHd);
	bufferManager.Delete(shrunkEdgePosHd);

	bufferManager.Delete(solidShadedPosNomColHd);
	bufferManager.Delete(solidUnshadedPosColHd);
	bufferManager.Delete(trspShadedPosNomColHd);
	bufferManager.Delete(trspUnshadedPosColHd);

	for(auto hd : plainPlgPosHd)
	{
		bufferManager.Delete(hd);
	}

	bufferManager.Delete(backFacePosColHd);

	bufferManager.Delete(lightPosColPointSizeHd);

	bufferManager.Delete(constEdgePosHd);

	bufferManager.Delete(selectedConstEdgePosHd);

	bufferManager.Delete(selectedConstEdgeArrowHeadPosHd);

	bufferManager.Delete(selectedFaceGroupPosHd);

	Initialize();
}


////////////////////////////////////////////////////////////

YsShellExtVboSet &YsHasShellExtVboSet::GetVboSet(void)
{
	return vboSet;
}

const YsShellExtVboSet &YsHasShellExtVboSet::GetVboSet(void) const
{
	return vboSet;
}

void YsHasShellExtVboSet::RemakeNormalVertexVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const
{
	auto &bufferManager=YsGLBufferManager::GetSharedBufferManager();
	if(vboSet.normalVtxPosHd==nullptr)
	{
		vboSet.normalVtxPosHd=bufferManager.Create();
	}
	if(vboSet.roundVtxPosHd==nullptr)
	{
		vboSet.roundVtxPosHd=bufferManager.Create();
	}
	bufferManager.MakeVtx(vboSet.normalVtxPosHd,YsGL::POINTS,dBuf.normalVertexPosBuffer);
	bufferManager.MakeVtx(vboSet.roundVtxPosHd,YsGL::POINTS,dBuf.roundVertexPosBuffer);
}

void YsHasShellExtVboSet::RemakeSelectedVertexVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const
{
	auto &bufferManager=YsGLBufferManager::GetSharedBufferManager();

	if(vboSet.selectedVtxPosColHd==nullptr)
	{
		vboSet.selectedVtxPosColHd=bufferManager.Create();
	}
	if(vboSet.selectedVtxLinePosColHd==nullptr)
	{
		vboSet.selectedVtxLinePosColHd=bufferManager.Create();
	}
	bufferManager.MakeVtxCol(vboSet.selectedVtxPosColHd,YsGL::POINTS,dBuf.selectedVertexLineBuffer,dBuf.selectedVertexLineColBuffer);
	bufferManager.MakeVtxCol(vboSet.selectedVtxLinePosColHd,YsGL::POINTS,dBuf.selectedVertexLineBuffer,dBuf.selectedVertexLineColBuffer);
}

void YsHasShellExtVboSet::RemakePolygonVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const
{
	auto &bufferManager=YsGLBufferManager::GetSharedBufferManager();

	if(vboSet.normalEdgePosHd==nullptr)
	{
		vboSet.normalEdgePosHd=bufferManager.Create();
	}
	if(vboSet.shrunkEdgePosHd==nullptr)
	{
		vboSet.shrunkEdgePosHd=bufferManager.Create();
	}

	if(vboSet.solidShadedPosNomColHd==nullptr)
	{
		vboSet.solidShadedPosNomColHd=bufferManager.Create();
	}
	if(vboSet.solidUnshadedPosColHd==nullptr)
	{
		vboSet.solidUnshadedPosColHd=bufferManager.Create();
	}
	if(vboSet.trspShadedPosNomColHd==nullptr)
	{
		vboSet.trspShadedPosNomColHd=bufferManager.Create();
	}
	if(vboSet.trspUnshadedPosColHd==nullptr)
	{
		vboSet.trspUnshadedPosColHd=bufferManager.Create();
	}

	if(vboSet.backFacePosColHd==nullptr)
	{
		vboSet.backFacePosColHd=bufferManager.Create();
	}

	if(vboSet.backFacePosColHd==nullptr)
	{
		vboSet.backFacePosColHd=bufferManager.Create();
	}

	if(vboSet.lightPosColPointSizeHd==nullptr)
	{
		vboSet.lightPosColPointSizeHd=bufferManager.Create();
	}


	bufferManager.MakeVtx(vboSet.normalEdgePosHd,YsGL::LINES,dBuf.normalEdgePosBuffer);
	bufferManager.MakeVtx(vboSet.shrunkEdgePosHd,YsGL::LINES,dBuf.shrunkEdgePosBuffer);
	bufferManager.MakeVtxNomCol(vboSet.solidShadedPosNomColHd,YsGL::TRIANGLES,dBuf.solidShadedVtxBuffer,dBuf.solidShadedNomBuffer,dBuf.solidShadedColBuffer);
	bufferManager.MakeVtxCol(vboSet.solidUnshadedPosColHd,YsGL::TRIANGLES,dBuf.solidUnshadedVtxBuffer,dBuf.solidUnshadedColBuffer);
	bufferManager.MakeVtxNomCol(vboSet.trspShadedPosNomColHd,YsGL::TRIANGLES,dBuf.trspShadedVtxBuffer,dBuf.trspShadedNomBuffer,dBuf.trspShadedColBuffer);
	bufferManager.MakeVtxCol(vboSet.trspUnshadedPosColHd,YsGL::TRIANGLES,dBuf.trspUnshadedVtxBuffer,dBuf.trspUnshadedColBuffer);
	bufferManager.MakeVtxCol(vboSet.backFacePosColHd,YsGL::TRIANGLES,dBuf.backFaceVtxBuffer,dBuf.backFaceColBuffer);
	bufferManager.MakeVtxColPointSize(vboSet.lightPosColPointSizeHd,YsGL::POINTS,dBuf.lightVtxBuffer,dBuf.lightColBuffer,dBuf.lightSizeBuffer);
}

void YsHasShellExtVboSet::RemakePlainPolygonVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const
{
	auto &bufferManager=YsGLBufferManager::GetSharedBufferManager();

	for(auto &hd : vboSet.plainPlgPosHd)
	{
		if(nullptr==hd)
		{
			hd=bufferManager.Create();
		}
	}

	bufferManager.MakeVtx(vboSet.plainPlgPosHd[0],YsGL::TRIANGLES,dBuf.solidShadedVtxBuffer);
	bufferManager.MakeVtx(vboSet.plainPlgPosHd[1],YsGL::TRIANGLES,dBuf.solidUnshadedVtxBuffer);
	bufferManager.MakeVtx(vboSet.plainPlgPosHd[2],YsGL::TRIANGLES,dBuf.trspShadedVtxBuffer);
	bufferManager.MakeVtx(vboSet.plainPlgPosHd[3],YsGL::TRIANGLES,dBuf.trspUnshadedVtxBuffer);
}

void YsHasShellExtVboSet::RemakeSelectedPolygonVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const
{
	auto &bufferManager=YsGLBufferManager::GetSharedBufferManager();
	if(nullptr==vboSet.selectedPolygonPosColHd)
	{
		vboSet.selectedPolygonPosColHd=bufferManager.Create();
	}

	bufferManager.MakeVtxCol(vboSet.selectedPolygonPosColHd,YsGL::TRIANGLES,dBuf.selectedPolygonPosBuffer,dBuf.selectedPolygonColBuffer);
}

void YsHasShellExtVboSet::RemakeConstEdgeVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const
{
	auto &bufferManager=YsGLBufferManager::GetSharedBufferManager();
	if(nullptr==vboSet.constEdgePosHd)
	{
		vboSet.constEdgePosHd=bufferManager.Create();
	}
	bufferManager.MakeVtx(vboSet.constEdgePosHd,YsGL::LINES,dBuf.constEdgeVtxBuffer);
}

void YsHasShellExtVboSet::RemakeSelectedConstEdgeVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const
{
	auto &bufferManager=YsGLBufferManager::GetSharedBufferManager();
	if(nullptr==vboSet.selectedConstEdgePosHd)
	{
		vboSet.selectedConstEdgePosHd=bufferManager.Create();
	}
	if(nullptr==vboSet.selectedConstEdgeArrowHeadPosHd)
	{
		vboSet.selectedConstEdgeArrowHeadPosHd=bufferManager.Create();
	}

	bufferManager.MakeVtx(vboSet.selectedConstEdgePosHd,YsGL::LINES,dBuf.selectedConstEdgeVtxBuffer);
	bufferManager.MakeVtx(vboSet.selectedConstEdgeArrowHeadPosHd,YsGL::TRIANGLES,dBuf.selectedConstEdgeArrowHeadVtxBuffer);
}

void YsHasShellExtVboSet::RemakeSelectedFaceGroupVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const
{
	auto &bufferManager=YsGLBufferManager::GetSharedBufferManager();
	if(nullptr==vboSet.selectedFaceGroupPosHd)
	{
		vboSet.selectedFaceGroupPosHd=bufferManager.Create();
	}
	bufferManager.MakeVtx(vboSet.selectedFaceGroupPosHd,YsGL::TRIANGLES,dBuf.selectedFaceGroupVtxBuffer);
}

YSBOOL YsHasShellExtVboSet::IsPolygonVboPrepared(void) const
{
	auto &bufMan=YsGLBufferManager::GetSharedBufferManager();

	if((nullptr!=vboSet.solidShadedPosNomColHd && YsGLBufferManager::Unit::EMPTY!=bufMan.GetBufferUnit(vboSet.solidShadedPosNomColHd)->GetState()) ||
	   (nullptr!=vboSet.solidUnshadedPosColHd  && YsGLBufferManager::Unit::EMPTY!=bufMan.GetBufferUnit(vboSet.solidUnshadedPosColHd)->GetState()) ||
	   (nullptr!=vboSet.trspShadedPosNomColHd  && YsGLBufferManager::Unit::EMPTY!=bufMan.GetBufferUnit(vboSet.trspShadedPosNomColHd)->GetState()) ||
	   (nullptr!=vboSet.trspUnshadedPosColHd   && YsGLBufferManager::Unit::EMPTY!=bufMan.GetBufferUnit(vboSet.trspUnshadedPosColHd)->GetState()) ||
	   (nullptr!=vboSet.lightPosColPointSizeHd && YsGLBufferManager::Unit::EMPTY!=bufMan.GetBufferUnit(vboSet.lightPosColPointSizeHd)->GetState()))
	{
		return YSTRUE;
	}
	return YSFALSE;
}
YSBOOL YsHasShellExtVboSet::IsPlainPolygonVboPrepared(void) const
{
	auto &bufMan=YsGLBufferManager::GetSharedBufferManager();

	for(auto &hd : vboSet.plainPlgPosHd)
	{
		if(nullptr==hd || YsGLBufferManager::Unit::EMPTY==bufMan.GetBufferUnit(hd)->GetState())
		{
			return YSFALSE;
		}
	}
	return YSTRUE;
}
