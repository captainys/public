/* ////////////////////////////////////////////////////////////

File Name: ysshellextgl.h
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

#ifndef YSSHELLEXTGL_IS_INCLUDED
#define YSSHELLEXTGL_IS_INCLUDED
/* { */

#include <ysshellext.h>
#include <ysshellextedit.h>
#include <ysglcpp.h>
#include <ysglbuffermanager.h>



class YsShellExtVboSet
{
public:
	YsGLBufferManager::Handle normalVtxPosHd;
	YsGLBufferManager::Handle roundVtxPosHd;

	YsGLBufferManager::Handle selectedVtxPosColHd;
	YsGLBufferManager::Handle selectedVtxLinePosColHd;

	YsGLBufferManager::Handle selectedPolygonPosColHd;

	YsGLBufferManager::Handle normalEdgePosHd;
	YsGLBufferManager::Handle shrunkEdgePosHd;

	YsGLBufferManager::Handle solidShadedPosNomColHd;
	YsGLBufferManager::Handle solidUnshadedPosColHd;
	YsGLBufferManager::Handle trspShadedPosNomColHd;
	YsGLBufferManager::Handle trspUnshadedPosColHd;

	YsGLBufferManager::Handle plainPlgPosHd[4];  // For shadow

	YsGLBufferManager::Handle backFacePosColHd;

	YsGLBufferManager::Handle lightPosColPointSizeHd;

	YsGLBufferManager::Handle constEdgePosHd;

	YsGLBufferManager::Handle selectedConstEdgePosHd;

	YsGLBufferManager::Handle selectedConstEdgeArrowHeadPosHd;

	YsGLBufferManager::Handle selectedFaceGroupPosHd;

	YsShellExtVboSet();
private:
	void Initialize(void);
public:
	~YsShellExtVboSet();
	void CleanUp(void);
};


class YsHasShellExtVboSet
{
protected:
	mutable YsShellExtVboSet vboSet;

public:
	/*! In most cases, a YsShellExtWithVbo can own a vboSet.  
	    However, there are some exceptions.  
	    For example, a VASI or a PAPI looks differently depending on from which direction the object is seen.
	    Although it can be done on the GPU side, that requires an additional GLSL program.
	    The more GLSL programs, the more troubles.  
	    Obviously, it is impossible to maintain too many shader programs.  VASI and PAPI renderers are too specific and would have little effect for performance.
	    What's the best course of action?  I don't know.
	    One option is let each instance own a vboSet for such objects.

	    If, each instance owns a vboSet, it is a node state, which is a member class of YsShellDnmTemplate.
	*/
	YsShellExtVboSet &GetVboSet(void);
	const YsShellExtVboSet &GetVboSet(void) const;

	void RemakeNormalVertexVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const;
	void RemakeSelectedVertexVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const;
	void RemakePolygonVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const;
	void RemakePlainPolygonVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const;
	void RemakeSelectedPolygonVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const;
	void RemakeConstEdgeVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const;
	void RemakeSelectedConstEdgeVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const;
	void RemakeSelectedFaceGroupVbo(YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer &dBuf) const;

	YSBOOL IsPolygonVboPrepared(void) const;
	YSBOOL IsPlainPolygonVboPrepared(void) const;

	class RenderingOption
	{
	public:
		YSBOOL solidPolygon;
		YSBOOL transparentPolygon;
		YSBOOL light;
		YSBOOL backFace;
		YSBOOL shadow;
		RenderingOption()
		{
			solidPolygon=YSTRUE;
			transparentPolygon=YSTRUE;
			light=YSTRUE;
			backFace=YSFALSE;
			shadow=YSFALSE;
		}
		void TurnOffAll(void)
		{
			solidPolygon=YSFALSE;
			transparentPolygon=YSFALSE;
			light=YSFALSE;
			backFace=YSFALSE;
			shadow=YSFALSE;
		}
	};

	/*! Due to the dependency on what renderer to be used, rendering a vbo cannot be abstracted 
	    inside ysglcpp library.  It needs to be a function of this class.  
	    Is written in API-specific sub-libraries.
	    Parameter viewMdlTfm is the transformation from the shell's own coordinate system to the camera coordinate system.
	    It is a viewTransform x modelTransform.
	*/
	void Render(const YsMatrix4x4 &viewMdlTfm,RenderingOption opt) const;
};


template <class SHLCLASS>
class YsShellExtWithVbo : public SHLCLASS, public YsHasShellExtVboSet
{
public:
	void RemakeNormalVertexBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double normalVertexMarkerRadius,const double roundVertexMarkerRadius) const;
	void RemakeSelectedVertexBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double selectedVertexMarkerRadius,YSSIZE_T nSel,const YsShellVertexHandle selVtHd[]) const;

	template <const int N>
	void RemakeSelectedVertexBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double selectedVertexMarkerRadius,const YsArray <YsShellVertexHandle,N> &selVtHd) const;

	void RemakePolygonBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer::ShapeInfo &shapeInfo,const double polygonShrinkRatioForPicking) const; // Cannot take const because it re-calculates vertex-normal
	void RemakePolygonBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double polygonShrinkRatioForPicking) const; // Cannot take const because it re-calculates vertex-normal

	void RemakeSelectedPolygonBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double polygonShrinkRatio,YSSIZE_T nSel,const YsShellPolygonHandle selPlHd[]) const;
	template <const int N>
	void RemakeSelectedPolygonBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double polygonShrinkRatio,const YsArray <YsShellPolygonHandle,N> &selPlHd) const;

	void RemakeConstEdgeBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,YSSIZE_T nSel,const YsShellExt::ConstEdgeHandle ceHdArray[]) const;
	template <const int N>
	void RemakeConstEdgeBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const YsArray <YsShellExt::ConstEdgeHandle,N> &selCeHd) const;

	void RemakeSelectedConstEdgeBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,YSSIZE_T nSel,const YsShellExt::ConstEdgeHandle ceHdArray[]) const;
	template <const int N>
	void RemakeSelectedConstEdgeBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const YsArray <YsShellExt::ConstEdgeHandle,N> &selCeHd) const;

	void RemakeSelectedFaceGroupBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,YSSIZE_T nFg,const YsShellExt::FaceGroupHandle fgHdArray[]) const;
	template <const int N>
	void RemakeSelectedFaceGroupBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const YsArray <YsShellExt::FaceGroupHandle,N> &fgHdArray) const;
};

template <class SHLCLASS>
void YsShellExtWithVbo<SHLCLASS>::RemakeNormalVertexBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double normalVertexMarkerRadius,const double roundVertexMarkerRadius) const
{
	dBuf.RemakeNormalVertexBuffer(this->Conv(),normalVertexMarkerRadius,roundVertexMarkerRadius);
	this->RemakeNormalVertexVbo(vboSet,dBuf);
}

template <class SHLCLASS>
void YsShellExtWithVbo<SHLCLASS>::RemakeSelectedVertexBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double selectedVertexMarkerRadius,YSSIZE_T nSel,const YsShellVertexHandle selVtHd[]) const
{
	dBuf.RemakeSelectedVertexBuffer(this->Conv(),selectedVertexMarkerRadius,nSel,selVtHd);
	this->RemakeSelectedVertexVbo(vboSet,dBuf);
}

template <class SHLCLASS>
template <const int N>
void YsShellExtWithVbo<SHLCLASS>::RemakeSelectedVertexBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double selectedVertexMarkerRadius,const YsArray <YsShellVertexHandle,N> &selVtHd) const
{
	RemakeSelectedVertexBuffer(dBuf,vboSet,selectedVertexMarkerRadius,selVtHd.GetN(),selVtHd);
}

template <class SHLCLASS>
void YsShellExtWithVbo<SHLCLASS>::RemakePolygonBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const YsShellExtDrawingBuffer::ShapeInfo &shapeInfo,const double polygonShrinkRatioForPicking) const
{
	dBuf.RemakePolygonBuffer(shapeInfo,polygonShrinkRatioForPicking);
	this->RemakePolygonVbo(vboSet,dBuf);
}

template <class SHLCLASS>
void YsShellExtWithVbo<SHLCLASS>::RemakePolygonBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double polygonShrinkRatioForPicking) const
{
	dBuf.RemakePolygonBuffer(this->Conv(),polygonShrinkRatioForPicking);
	this->RemakePolygonVbo(vboSet,dBuf);
}

template <class SHLCLASS>
void YsShellExtWithVbo<SHLCLASS>::RemakeSelectedPolygonBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double polygonShrinkRatio,YSSIZE_T nSel,const YsShellPolygonHandle selPlHd[]) const
{
	dBuf.RemakeSelectedPolygonBuffer(this->Conv(),polygonShrinkRatio,nSel,selPlHd);
	this->RemakeSelectedPolygonVbo(vboSet,dBuf);
}

template <class SHLCLASS>
template <const int N>
void YsShellExtWithVbo<SHLCLASS>::RemakeSelectedPolygonBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const double polygonShrinkRatio,const YsArray <YsShellPolygonHandle,N> &selPlHd) const
{
	RemakeSelectedPolygonBuffer(dBuf,vboSet,polygonShrinkRatio,selPlHd.GetN(),selPlHd);
}

template <class SHLCLASS>
void YsShellExtWithVbo<SHLCLASS>::RemakeConstEdgeBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,YSSIZE_T nSel,const YsShellExt::ConstEdgeHandle ceHdArray[]) const
{
	dBuf.RemakeConstEdgeBuffer(this->Conv(),nSel,ceHdArray);
	this->RemakeConstEdgeVbo(vboSet,dBuf);
}

template <class SHLCLASS>
template <const int N>
void YsShellExtWithVbo<SHLCLASS>::RemakeConstEdgeBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const YsArray <YsShellExt::ConstEdgeHandle,N> &selCeHd) const
{
	RemakeConstEdgeBuffer(dBuf,vboSet,selCeHd.GetN(),selCeHd);
}

template <class SHLCLASS>
void YsShellExtWithVbo<SHLCLASS>::RemakeSelectedConstEdgeBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,YSSIZE_T nSel,const YsShellExt::ConstEdgeHandle ceHdArray[]) const
{
	dBuf.RemakeSelectedConstEdgeBuffer(this->Conv(),nSel,ceHdArray);
	this->RemakeSelectedConstEdgeVbo(vboSet,dBuf);

}

template <class SHLCLASS>
template <const int N>
void YsShellExtWithVbo<SHLCLASS>::RemakeSelectedConstEdgeBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const YsArray <YsShellExt::ConstEdgeHandle,N> &selCeHd) const
{
	RemakeSelectedConstEdgeBuffer(dBuf,vboSet,selCeHd.GetN(),selCeHd);
}

template <class SHLCLASS>
void YsShellExtWithVbo<SHLCLASS>::RemakeSelectedFaceGroupBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,YSSIZE_T nFg,const YsShellExt::FaceGroupHandle fgHdArray[]) const
{
	dBuf.RemakeSelectedFaceGroupBuffer(this->Conv(),nFg,fgHdArray);
	this->RemakeSelectedFaceGroupVbo(vboSet,dBuf);
}

template <class SHLCLASS>
template <const int N>
void YsShellExtWithVbo<SHLCLASS>::RemakeSelectedFaceGroupBuffer(YsShellExtDrawingBuffer &dBuf,YsShellExtVboSet &vboSet,const YsArray <YsShellExt::FaceGroupHandle,N> &fgHdArray) const
{
	RemakeSelectedFaceGroupBuffer(dBuf,vboSet,fgHdArray.GetN(),fgHdArray);
}


/* } */
#endif
