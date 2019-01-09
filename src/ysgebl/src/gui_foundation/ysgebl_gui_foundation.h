/* ////////////////////////////////////////////////////////////

File Name: ysgebl_gui_foundation.h
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

#ifndef YSGEBL_GUI_FOUNDATION_IS_INCLUDED
#define YSGEBL_GUI_FOUNDATION_IS_INCLUDED
/* { */

#include <fsgui3d.h>

#include <ysshellkernel.h>
#include <ysglbuffer.h>

#include "ysshellexteditgui.h"

class GeblGuiFoundation :  public FsGuiCanvas
{
public:
	enum ERRORCODE
	{
		GEBLERROR_NOERROR,
		GEBLERROR_FILE_NOT_FOUND,
		GEBLERROR_FILE_CORRUPTED,
		GEBLERROR_FILE_NOT_SUPPORTED
	};

	class SlhdAndErrorCode
	{
	public:
		YsShellDnmContainer <YsShellExtEditGui>::Node *slHd;
		ERRORCODE errCode;
	};


protected:
	YsShellDnmContainer <YsShellExtEditGui> shlGrp;
	YsShellDnmContainer <YsShellExtEditGui>::Node *slHd;
public:
	FsGui3DViewControl drawEnv;

	GLfloat vertexPickRadius;
	GLfloat normalVertexMarkerRadius;
	GLfloat roundVertexMarkerRadius;
	GLfloat selectedVertexMarkerRadius;
	GLfloat polygonShrinkRatioForPicking;

	YSBOOL actuallyDrawVertex;
	YSBOOL actuallyDrawPolygonEdge;
	YSBOOL actuallyDrawPolygon;
	YSBOOL actuallyDrawConstEdge;
	YSBOOL actuallyDrawShrunkPolygon;
	YSBOOL actuallyDrawOtherShell;



public:
	GeblGuiFoundation();

	YsShellExtEditGui *Slhd(void);
	const YsShellExtEditGui *Slhd(void) const;
	YsShellDnmContainer <YsShellExtEditGui> &ShlGrp(void);
	const YsShellDnmContainer <YsShellExtEditGui> &ShlGrp(void) const;

	YSBOOL IsSupportedFileType(const wchar_t fn[]) const;
	SlhdAndErrorCode LoadGeneral(const wchar_t fn[],YsShellDnmContainer <YsShellExtEditGui>::Node *parent);


public:
	void DrawAxis(const double length,const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat) const;

	void DrawVertex(const YsShellExtEditGui &shl,const int viewport[4]) const;
	void DrawSelectedVertex(const YsShellExtEditGui &shl,const int viewport[4]) const;
	void DrawSelectedVertexLine(const YsShellExtEditGui &shl) const;
	void DrawPolygonEdge(const YsShellExtEditGui &shl) const;
	void DrawSelectedPolygon(const YsShellExtEditGui &shl) const;
	void DrawShrunkPolygon(const YsShellExtEditGui &shl) const;
	void DrawConstEdge(const YsShellExtEditGui &shl) const;
	void DrawSelectedConstEdge(const YsShellExtEditGui &shl) const;
	void DrawSelectedFaceGroup(const YsShellExtEditGui &shl) const;


public:
	YsShellVertexHandle PickedVtHd(int mx,int my,YSSIZE_T nExclude,const YsShellVertexHandle excludeVtHd[],const YsMatrix4x4 &modelTfm) const;
	template <const int N>
	YsShellVertexHandle PickedVtHd(int mx,int my,YsArray <YsShellVertexHandle,N> &excludeVtHd,const YsMatrix4x4 &modelTfm) const;

	YsShellVertexHandle PickedVtHdAmongSelection(int mx,int my,const YsMatrix4x4 &modelTfm) const;	// Selected vertices are cached in the vertex buffer.  Can be calculated faster.

	YsArray <YsPair <YsShellVertexHandle,double> > BoxedVtHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellVertexHandle excludeVtHd[],const YsMatrix4x4 &modelTfm) const;
	template <const int N>
	YsArray <YsPair <YsShellVertexHandle,double> > BoxedVtHd(int x0,int y0,int x1,int y1,const YsArray <YsShellVertexHandle,N> &excludeVtHd,const YsMatrix4x4 &modelTfm) const;

	YsArray <YsPair <YsShellVertexHandle,double> > BoxedVtHdAmongSelection(int x0,int y0,int x1,int y1,const YsMatrix4x4 &modelTfm) const;

	YsShellPolygonHandle PickedPlHd(int mx,int my,YSSIZE_T nExclude,const YsShellPolygonHandle excludePlHd[],const YsMatrix4x4 &modelTfm) const;
	template <const int N>
	YsShellPolygonHandle PickedPlHd(int mx,int my,YsArray <YsShellPolygonHandle,N> &excludePlHd,const YsMatrix4x4 &modelTfm) const;

	YsShellPolygonHandle PickedPlHdAmongSelection(int mx,int my,const YsMatrix4x4 &modelTfm) const;

	YsArray <YsPair <YsShellPolygonHandle,double> > EnclosedPlHd(YsConstArrayMask <YsVec2> strokePnt,const YsMatrix4x4 &modelTfm) const;

	YsArray <YsPair <YsShellPolygonHandle,double> > BoxedPlHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellPolygonHandle excludePlHd[],const YsMatrix4x4 &modelTfm) const;
	template <const int N>
	YsArray <YsPair <YsShellPolygonHandle,double> > BoxedPlHd(int x0,int y0,int x1,int y1,YsArray <YsShellPolygonHandle,N> &excludePlHd,const YsMatrix4x4 &modelTfm) const;

	YsArray <YsPair <YsShellPolygonHandle,double> > BoxedPlHdAmongSelection(int x0,int y0,int x1,int y1,const YsMatrix4x4 &modelTfm) const;

	YsShellExt::ConstEdgeHandle PickedCeHd(int mx,int my,YSSIZE_T nExclude,const YsShellExt::ConstEdgeHandle excludeCeHd[],const YsMatrix4x4 &modelTfm) const;
	template <const int N>
	YsShellExt::ConstEdgeHandle PickedCeHd(int mx,int my,YsArray <YsShellExt::ConstEdgeHandle,N> &excludeCeHd,const YsMatrix4x4 &modelTfm) const;

	YsShellExt::ConstEdgeHandle PickedCeHdAmongSelection(int mx,int my,const YsMatrix4x4 &modelTfm) const;

	YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > BoxedCeHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellExt::ConstEdgeHandle excludeCeHd[],const YsMatrix4x4 &modelTfm) const;
	template <const int N>
	YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > BoxedCeHd(int x0,int y0,int x1,int y1,const YsArray <YsShellExt::ConstEdgeHandle,N> &excludeCeHd,const YsMatrix4x4 &modelTfm) const;

	YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > BoxedCeHdAmongSelection(int x0,int y0,int x1,int y1,const YsMatrix4x4 &modelTfm) const;

	YsShellExt::FaceGroupHandle PickedFgHd(int mx,int my,int nExclude,const YsShellExt::FaceGroupHandle excludeFgHd[],const YsMatrix4x4 &modelTfm) const;
	template <const int N>
	YsShellExt::FaceGroupHandle PickedFgHd(int mx,int my,YsArray <YsShellExt::FaceGroupHandle,N> &excludeFgHd,const YsMatrix4x4 &modelTfm) const;

	YsShellExt::FaceGroupHandle PickedFgHdAmongSelection(int mx,int my,const YsMatrix4x4 &modelTfm) const;

	/*! Returns a picked shell handle.  DnmStatePtr can be nullptr if there is no transformation. */
	const YsShellDnmContainer <YsShellExtEditGui>::Node *PickedSlHd(int mx,int my,YSSIZE_T nExclude,const YsShellDnmContainer <YsShellExtEditGui>::Node *excludeSlHd[],const YsShellDnmContainer <YsShellExtEditGui>::DnmState *DnmStatePtr) const;
	template <const int N>
	const YsShellDnmContainer <YsShellExtEditGui>::Node *PickedSlHd(int mx,int my,YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *> &excludeSlHd,const YsShellDnmContainer <YsShellExtEditGui>::DnmState *DnmStatePtr) const;

	YsShellDnmContainer <YsShellExtEditGui>::Node *PickedSlHd(int mx,int my,YSSIZE_T nExclude,const YsShellDnmContainer <YsShellExtEditGui>::Node *excludeSlHd[],const YsShellDnmContainer <YsShellExtEditGui>::DnmState *DnmStatePtr);
	template <const int N>
	YsShellDnmContainer <YsShellExtEditGui>::Node *PickedSlHd(int mx,int my,YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *> &excludeSlHd,const YsShellDnmContainer <YsShellExtEditGui>::DnmState *DnmStatePtr);

private:
	int PickedIndexInPointBuffer(double &z,int mx,int my,const YsGLVertexBuffer &pointVtxBuffer,const YsArray <int> &pointIdxBuffer,const YsKeyStore &exclusion,const YsMatrix4x4 &modelTfm) const;
	int PickedIndexInEdgeBuffer(double &z,int mx,int my,const YsGLVertexBuffer &edgeVtxBuffer,const YsArray <int> &edgeIdxBuffer,const YsKeyStore &exclusion,const YsMatrix4x4 &modelTfm) const;
	int PickedIndexInTriangleBuffer(double &z,int mx,int my,const YsGLVertexBuffer &triVtxBuffer,const YsArray <int> &triIdxBuffer,const YsKeyStore *limit,const YsKeyStore *exclusion,const YsMatrix4x4 &modelTfm) const;

	YsArray <YsPair <int,double> > BoxedIndexInPointBuffer(int x0,int y0,int x1,int y1,const YsGLVertexBuffer &pointVtxBuffer,const YsArray <int> &pointIdxBuffer,const YsKeyStore &exclusion,const YsMatrix4x4 &modelTfm) const;
	YsArray <YsPair <int,double> > BoxedIndexInEdgeBuffer(
	    int x0,int y0,int x1,int y1,const YsGLVertexBuffer &edgeVtxBuffer,const YsArray <int> &edgeIdxBuffer,const YsKeyStore *limit,const YsKeyStore *exclusion,const YsMatrix4x4 &modelTfm) const;
	YsArray <YsPair <int,double> > BoxedIndexInTriangleBuffer(
	    int x0,int y0,int x1,int y1,const YsGLVertexBuffer &triVtxBuffer,const YsArray <int> &triIdxBuffer,const YsKeyStore *limit,const YsKeyStore *exclusion,const YsMatrix4x4 &modelTfm) const;

	YsArray <YsPair <int,double> > EnclosedIndexInEdgeBuffer(
	    const YsShell2d &shl,const YsShell2dLattice &ltc,
	    const YsGLVertexBuffer &edgeVtxBuffer,const YsArray <int> &edgeIdxBuffer,const YsKeyStore *limit,const YsKeyStore *exclusion,const YsMatrix4x4 &modelTfm) const;
	YsArray <YsPair <int,double> > EnclosedIndexInTriangleBuffer(
	    const YsShell2d &shl,const YsShell2dLattice &ltc,
	    const YsGLVertexBuffer &triVtxBuffer,const YsArray <int> &triIdxBuffer,const YsKeyStore *limit,const YsKeyStore *exclusion,const YsMatrix4x4 &modelTfm) const;

};

template <const int N>
YsShellVertexHandle GeblGuiFoundation::PickedVtHd(int mx,int my,YsArray <YsShellVertexHandle,N> &excludeVtHd,const YsMatrix4x4 &modelTfm) const
{
	return PickedVtHd(mx,my,excludeVtHd.GetN(),excludeVtHd,modelTfm);
}

template <const int N>
YsArray <YsPair <YsShellVertexHandle,double> > GeblGuiFoundation::BoxedVtHd(int x0,int y0,int x1,int y1,const YsArray <YsShellVertexHandle,N> &excludeVtHd,const YsMatrix4x4 &modelTfm) const
{
	return BoxedVtHd(x0,y0,x1,y1,excludeVtHd.GetN(),excludeVtHd,modelTfm);
}

template <const int N>
YsShellPolygonHandle GeblGuiFoundation::PickedPlHd(int mx,int my,YsArray <YsShellPolygonHandle,N> &excludePlHd,const YsMatrix4x4 &modelTfm) const
{
	return PickedPlHd(mx,my,excludePlHd.GetN(),excludePlHd,modelTfm);
}

template <const int N>
YsArray <YsPair <YsShellPolygonHandle,double> > GeblGuiFoundation::BoxedPlHd(int x0,int y0,int x1,int y1,YsArray <YsShellPolygonHandle,N> &excludePlHd,const YsMatrix4x4 &modelTfm) const
{
	return BoxedPlHd(x0,y0,x1,y1,excludePlHd.GetN(),excludePlHd,modelTfm);
}

template <const int N>
YsShellExt::ConstEdgeHandle GeblGuiFoundation::PickedCeHd(int mx,int my,YsArray <YsShellExt::ConstEdgeHandle,N> &excludeCeHd,const YsMatrix4x4 &modelTfm) const
{
	return PickedCeHd(mx,my,excludeCeHd.GetN(),excludeCeHd,modelTfm);
}

template <const int N>
YsShellExt::FaceGroupHandle GeblGuiFoundation::PickedFgHd(int mx,int my,YsArray <YsShellExt::FaceGroupHandle,N> &excludeFgHd,const YsMatrix4x4 &modelTfm) const
{
	return PickedFgHd(mx,my,(int)excludeFgHd.GetN(),excludeFgHd,modelTfm);
}

template <const int N>
YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > GeblGuiFoundation::BoxedCeHd(int x0,int y0,int x1,int y1,const YsArray <YsShellExt::ConstEdgeHandle,N> &excludeCeHd,const YsMatrix4x4 &modelTfm) const
{
	return BoxedCeHd(x0,y0,x1,y1,excludeCeHd.GetN(),excludeCeHd,modelTfm);
}

template <const int N>
const YsShellDnmContainer <YsShellExtEditGui>::Node *GeblGuiFoundation::PickedSlHd(int mx,int my,YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *> &excludeSlHd,const YsShellDnmContainer <YsShellExtEditGui>::DnmState *dnmState) const
{
	return PickedSlHd(mx,my,excludeSlHd.GetN(),excludeSlHd,dnmState);
}

template <const int N>
YsShellDnmContainer <YsShellExtEditGui>::Node *GeblGuiFoundation::PickedSlHd(int mx,int my,YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *> &excludeSlHd,const YsShellDnmContainer <YsShellExtEditGui>::DnmState *dnmState)
{
	return PickedSlHd(mx,my,excludeSlHd.GetN(),excludeSlHd,dnmState);
}


/* } */
#endif
