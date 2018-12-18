/* ////////////////////////////////////////////////////////////

File Name: ysglbuffermanager.h
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

#ifndef YSGLBUFFERMANAGER_IS_INCLUDED
#define YSGLBUFFERMANAGER_IS_INCLUDED
/* { */

#include "ysglcpp.h"
#include "ysglbuffer.h"
#include <ysclass.h>

/*!
YsGLBufferManager detaches high-level vertex-buffer data structure and the graphics tookit's own vertex-buffer management structure.
The concept of the vertex-buffer is independent of OpenGL, Direct3D, or anything else.
Native handle to the vertex-buffer is necessary only when the vertex-buffer object is populated and when the primitives are drawn.
Until then, the program should be free of native handles.

The goal of this class is to encapculate native handles in the implementation library (ysglbuffermanager_gl2, ysglbuffermanager_gl1)
and reduce the toolkit dependent code as much as possible.
*/
class YsGLBufferManager
{
public:
	static YsGLBufferManager &GetSharedBufferManager(void);

public:
	friend class Unit;

	class ActualBuffer;
	class Unit
	{
	private:
		ActualBuffer *bufferPtr;
	public:
		enum STATE
		{
			EMPTY,
			POPULATED
		};

		Unit();
		~Unit();
		void CleanUp(void);
		const ActualBuffer *GetActualBuffer(void) const;
		YSSIZE_T AdjustMaxNumVtxPerVboForPrimitiveType(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx);

		// Implemented in toolkit-specific library >>
		static ActualBuffer *Create(void);
		static void Delete(ActualBuffer *ptr);
		STATE GetState(void) const;
		YsArray <unsigned int> GetNativeHandle(void) const;
		void MakeVtx(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,YSSIZE_T maxNumVtxPerVbo);
		void MakeVtxNom(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,YSSIZE_T maxNumVtxPerVbo);
		void MakeVtxCol(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLColorBuffer &colBuf,YSSIZE_T maxNumVtxPerVbo);
		void MakeVtxNomCol(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,const YsGLColorBuffer &colBuf,YSSIZE_T maxNumVtxPerVbo);
		void MakeVtxColPointSize(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLColorBuffer &colBuf,const YsGLPointSizeBuffer &psBuf,YSSIZE_T maxNumVtxPerVbo);
		// Implemented in toolkit-specific library <<
	};

	typedef YsEditArrayObjectHandle <Unit,8> Handle;

private:
	YsEditArray <Unit,8> bufferArray;

public:
	Handle Create(void);

	/*! Call this function when a graphics context is lost due to an irresponsible implementation of the driver.
	    This will keep all handles valid, but makes them EMPTY so that whoever uses the handle can
	    check and re-create buffers.
	*/
	void ContextLost(void);

	const Unit *GetBufferUnit(Handle hd) const;
	const ActualBuffer *GetActualBuffer(Handle hd) const;
	void Delete(Handle hd);
	void MakeVtx(Handle bfHd,YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,YSSIZE_T maxNumVtxPerVbo=0x1fffffff);
	void MakeVtxNom(Handle bfHd,YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,YSSIZE_T maxNumVtxPerVbo=0x1fffffff);
	void MakeVtxCol(Handle bfHd,YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLColorBuffer &colBuf,YSSIZE_T maxNumVtxPerVbo=0x1fffffff);
	void MakeVtxNomCol(Handle bfHd,YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,const YsGLColorBuffer &colBuf,YSSIZE_T maxNumVtxPerVbo=0x1fffffff);
	void MakeVtxColPointSize(Handle bfHd,YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLColorBuffer &colBuf,const YsGLPointSizeBuffer &psBuf,YSSIZE_T maxNumVtxPerVbo=0x1fffffff);

	YSRESULT CheckIntegrity(void) const;
};


/* } */
#endif
