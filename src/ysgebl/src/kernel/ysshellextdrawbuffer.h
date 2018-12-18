/* ////////////////////////////////////////////////////////////

File Name: ysshellextdrawbuffer.h
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

#ifndef YSSHELLEXTDRAWBUFFER_IS_INCLUDED
#define YSSHELLEXTDRAWBUFFER_IS_INCLUDED
/* { */

#include <ysglbuffer.h>
#include "ysshellext.h"

class YsShellExtDrawingBuffer
{
	class RemakePolygonBufferTask;
	class RemakeSelectedPolygonBufferTask;
	class RemakeSelectedFaceGroupBufferTask;

	YSBOOL crossSectionEnabled;
	YsPlane crossSection;

public:
	class ShapeInfo
	{
	public:
		const YsShellExt *shlExtPtr;
		const YsShellExt::TemporaryModification *modPtr;

		ShapeInfo()
		{
			CleanUp();
		}
		void CleanUp(void)
		{
			shlExtPtr=NULL;
			modPtr=NULL;
		}
	};


public:
	YSBOOL viewPortEnabled;
	YsVec3 viewPort[2];

	YSBOOL drawBackFaceInDifferentColor;
	YsColor backFaceColor;

	// Memo: I wish point-sprite feature worked as advertised.  According to my experiments, 
	//       point-sprite implementations in the various vendor drivers are very unreliable.  
	//       Therefore, each vertex is drawn as a triangles.  Therefore, one real vertex 
	//       takes three OpenGL vertices and an index is stored per triangle.

	//       2015/07/03 Now I figured.  It was missing #version tag.  Buffers for drawing vertices
	//       will be changed so that it uses point-sprites.

	YsGLVertexBuffer normalVertexPosBuffer;
	YsArray <int> normalVertexIdxBuffer;

	YsGLVertexBuffer roundVertexPosBuffer;
	YsArray <int> roundVertexIdxBuffer;

	YsGLVertexBuffer selectedVertexPosBuffer;
	YsGLColorBuffer selectedVertexColBuffer;
	YsArray <int> selectedVertexIdxBuffer;

	YsGLVertexBuffer selectedVertexLineBuffer;
	YsGLColorBuffer selectedVertexLineColBuffer;

	YsGLVertexBuffer normalEdgePosBuffer;
	YsArray <int> normalEdgeIdxBuffer;
	YsGLVertexBuffer shrunkEdgePosBuffer;
	YsArray <int> shrunkEdgeIdxBuffer;

	YsGLVertexBuffer solidShadedVtxBuffer;
	YsGLNormalBuffer solidShadedNomBuffer;
	YsGLColorBuffer solidShadedColBuffer;
	YsArray <int> solidShadedIdxBuffer;

	YsGLVertexBuffer solidUnshadedVtxBuffer;
	YsGLColorBuffer solidUnshadedColBuffer;
	YsArray <int> solidUnshadedIdxBuffer;

	YsGLVertexBuffer trspShadedVtxBuffer;
	YsGLNormalBuffer trspShadedNomBuffer;
	YsGLColorBuffer trspShadedColBuffer;
	YsArray <int> trspShadedIdxBuffer;

	YsGLVertexBuffer trspUnshadedVtxBuffer;
	YsGLColorBuffer trspUnshadedColBuffer;
	YsArray <int> trspUnshadedIdxBuffer;

	YsGLVertexBuffer backFaceVtxBuffer;
	YsGLColorBuffer backFaceColBuffer;
	YsArray <int> backFaceIdxBuffer;

	YsGLVertexBuffer invisibleButPickablePolygonVtxBuffer;
	YsArray <int> invisibleButPickablePolygonIdxBuffer;

	YsGLVertexBuffer lightVtxBuffer;	// 2015/07/18 Polygons as light is stored in insivibleButPickablePolygonBuffer.
	YsGLColorBuffer lightColBuffer;		//            These also are populated in the lightVtx,Col,Size buffers.
	YsGLPointSizeBuffer lightSizeBuffer;

	YsGLVertexBuffer selectedPolygonPosBuffer;
	YsGLColorBuffer selectedPolygonColBuffer;
	YsArray <int> selectedPolygonIdxBuffer;

	YsGLVertexBuffer constEdgeVtxBuffer;
	YsArray <int> constEdgeIdxBuffer;

	YsGLVertexBuffer selectedConstEdgeVtxBuffer;
	YsArray <int> selectedConstEdgeIdxBuffer;

	YsGLVertexBuffer selectedConstEdgeArrowHeadVtxBuffer;

	YsGLVertexBuffer selectedFaceGroupVtxBuffer;
	YsArray <int> selectedFaceGroupIdxBuffer;


public:
	YsShellExtDrawingBuffer();
	void EnableViewPort(void);
	void DisableViewPort(void);
	void SetViewPort(const YsVec3 viewPort[2]);
	void GetViewPort(YsVec3 viewPort[2]) const;

	void EnableCrossSection(void);
	void DisableCrossSection(void);
	void ToggleCrossSection(void);
	void SetCrossSection(const YsPlane &pln);
	YSBOOL CrossSectionEnabled(void) const;
	YsPlane GetCrossSection(void) const;

	void RemakeNormalVertexBuffer(const class YsShellExt &shl,const double normalVertexMarkerRadius,const double roundVertexMarkerRadius);
	void RemakeSelectedVertexBuffer(const class YsShellExt &shl,const double selectedVertexMarkerRadius,YSSIZE_T nSel,const YsShellVertexHandle selVtHd[]);

	template <const int N>
	void RemakeSelectedVertexBuffer(const class YsShellExt &shl,const double selectedVertexMarkerRadius,const YsArray <YsShellVertexHandle,N> &selVtHd);

	void RemakePolygonBuffer(const ShapeInfo &shapeInfo,const double polygonShrinkRatioForPicking);
	void RemakePolygonBuffer(const class YsShellExt &shl,const double polygonShrinkRatioForPicking);

	void AddPolygonAsLight(const class YsShellExt &shl,const double lightSize);

	void RemakeSelectedPolygonBuffer(const class YsShellExt &shl,const double polygonShrinkRatio,YSSIZE_T nSel,const YsShellPolygonHandle selPlHd[]);
	template <const int N>
	void RemakeSelectedPolygonBuffer(const class YsShellExt &shl,const double polygonShrinkRatio,const YsArray <YsShellPolygonHandle,N> &selPlHd);

	void RemakeConstEdgeBuffer(const class YsShellExt &shl,YSSIZE_T nSel,const YsShellExt::ConstEdgeHandle ceHdArray[]);
	template <const int N>
	void RemakeConstEdgeBuffer(const class YsShellExt &shl,const YsArray <YsShellExt::ConstEdgeHandle,N> &selCeHd);

	void RemakeSelectedConstEdgeBuffer(const class YsShellExt &shl,YSSIZE_T nSel,const YsShellExt::ConstEdgeHandle ceHdArray[]);
	template <const int N>
	void RemakeSelectedConstEdgeBuffer(const class YsShellExt &shl,const YsArray <YsShellExt::ConstEdgeHandle,N> &selCeHd);

	void RemakeSelectedFaceGroupBuffer(const class YsShellExt &shl,YSSIZE_T nFg,const YsShellExt::FaceGroupHandle fgHdArray[]);
	template <const int N>
	void RemakeSelectedFaceGroupBuffer(const class YsShellExt &shl,const YsArray <YsShellExt::FaceGroupHandle,N> &fgHdArray);

private:
	YSRESULT ClipLine(YsVec3 clippedEdVtPos[2],const YsVec3 edVtPos[2]) const;

public:
	static void MakeVertexNormalCache(YsHashTable <YsVec3> &vtxNomHash,const YsShellExt &shl);

private:
	static YsColor ColorGradation(const double t);
	static YsColor ColorGradation(YSSIZE_T idx,YSSIZE_T n);
};

template <const int N>
void YsShellExtDrawingBuffer::RemakeSelectedVertexBuffer(const class YsShellExt &shl,const double selectedVertexMarkerRadius,const YsArray <YsShellVertexHandle,N> &selVtHd)
{
	RemakeSelectedVertexBuffer(shl,selectedVertexMarkerRadius,selVtHd.GetN(),selVtHd);
}

template <const int N>
void YsShellExtDrawingBuffer::RemakeSelectedPolygonBuffer(const class YsShellExt &shl,const double polygonShrinkRatio,const YsArray <YsShellPolygonHandle,N> &selPlHd)
{
	RemakeSelectedPolygonBuffer(shl,polygonShrinkRatio,selPlHd.GetN(),selPlHd);
}

template <const int N>
void YsShellExtDrawingBuffer::RemakeConstEdgeBuffer(const class YsShellExt &shl,const YsArray <YsShellExt::ConstEdgeHandle,N> &selCeHd)
{
	RemakeConstEdgeBuffer(shl,selCeHd.GetN(),selCeHd);
}

template <const int N>
void YsShellExtDrawingBuffer::RemakeSelectedConstEdgeBuffer(const class YsShellExt &shl,const YsArray <YsShellExt::ConstEdgeHandle,N> &selCeHd)
{
	RemakeSelectedConstEdgeBuffer(shl,selCeHd.GetN(),selCeHd);
}

template <const int N>
void YsShellExtDrawingBuffer::RemakeSelectedFaceGroupBuffer(const class YsShellExt &shl,const YsArray <YsShellExt::FaceGroupHandle,N> &fgHdArray)
{
	RemakeSelectedFaceGroupBuffer(shl,fgHdArray.GetN(),fgHdArray);
}

/* } */
#endif
