/* ////////////////////////////////////////////////////////////

File Name: ysglbuffermanager_gl2.cpp
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

#include "ysglbuffermanager_gl2.h"

void YsGLBufferManager::ActualBuffer::PrepareVbo(YSSIZE_T nVbo)
{
	auto nCurrent=bufSet.GetN();

	for(auto i=nVbo; i<nCurrent; ++i)
	{
		if(GL_TRUE==glIsBuffer(bufSet[i].vboIdent))
		{
			glDeleteBuffers(1,&bufSet[i].vboIdent);
		}
	}

	bufSet.Resize(nVbo);
	for(auto i=nCurrent; i<nVbo; ++i)
	{
		GLuint newVboIdent[1];
		glGenBuffers(1,newVboIdent);
		bufSet[i].vboIdent=newVboIdent[0];

		// OpenGL ES for Android
		//   OpenGL ES calls from outside of the graphics thread may fail.
		//   Must check that the name is valid, or an empty name is cached,
		//   and later crashes.
		if(0==newVboIdent[0])
		{
			bufSet.clear();
			break;
		}
	}
}

////////////////////////////////////////////////////////////

/* static */ YsGLBufferManager::ActualBuffer *YsGLBufferManager::Unit::Create(void)
{
	return new YsGLBufferManager::ActualBuffer;
}
/* static */ void YsGLBufferManager::Unit::Delete(YsGLBufferManager::ActualBuffer *ptr)
{
	for(auto &buf : ptr->bufSet)
	{
		if(GL_TRUE==glIsBuffer(buf.vboIdent))
		{
			glDeleteBuffers(1,&buf.vboIdent);
		}
	}
	delete ptr;
}

YsGLBufferManager::Unit::STATE YsGLBufferManager::Unit::GetState(void) const
{
	if(nullptr==bufferPtr || 0==bufferPtr->bufSet.size())
	{
		return EMPTY;
	}
	return POPULATED;
}

template <class T>
static int PushBufferSubData(YSSIZE_T &bufPtr,const T &buf,YSSIZE_T vtxTop,YSSIZE_T nVtx)
{
	auto currentPtr=bufPtr;
	auto bufLength=buf.GetNBytePerElem()*nVtx;

	glBufferSubData(GL_ARRAY_BUFFER,bufPtr,bufLength,buf.GetArrayPointer()+buf.GetNPerElem()*vtxTop);

	bufPtr+=bufLength;
	return (int)currentPtr;   // <- By wrongfully writing bufPtr, I spent about four hours to debug it....
}

void YsGLBufferManager::Unit::MakeVtx(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,YSSIZE_T maxNumVtxPerVbo)
{
	maxNumVtxPerVbo=AdjustMaxNumVtxPerVboForPrimitiveType(primType,maxNumVtxPerVbo);
	if(nullptr==bufferPtr)
	{
		bufferPtr=YsGLBufferManager::Unit::Create();
	}
	bufferPtr->PrepareVbo(1+vtxBuf.GetN()/maxNumVtxPerVbo);
	for(YSSIZE_T vboIndex=0; vboIndex<bufferPtr->bufSet.GetN(); ++vboIndex)
	{
		auto &buf=bufferPtr->bufSet[vboIndex];
		auto vtxTop=maxNumVtxPerVbo*vboIndex;
		auto nVtx=YsSmaller(maxNumVtxPerVbo,vtxBuf.GetN()-vtxTop);

		buf.nVtx=nVtx;

		glBindBuffer(GL_ARRAY_BUFFER,buf.vboIdent);
		YSSIZE_T totalBufSize=vtxBuf.GetNBytePerElem()*nVtx;

		glBufferData(GL_ARRAY_BUFFER,totalBufSize,nullptr,GL_STATIC_DRAW);

		YSSIZE_T bufPtr=0;
		buf.vtxPtr=PushBufferSubData(bufPtr,vtxBuf,vtxTop,nVtx);
	}
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void YsGLBufferManager::Unit::MakeVtxNom(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,YSSIZE_T maxNumVtxPerVbo)
{
	maxNumVtxPerVbo=AdjustMaxNumVtxPerVboForPrimitiveType(primType,maxNumVtxPerVbo);
	if(nullptr==bufferPtr)
	{
		bufferPtr=YsGLBufferManager::Unit::Create();
	}
	bufferPtr->PrepareVbo(1+vtxBuf.GetN()/maxNumVtxPerVbo);
	for(YSSIZE_T vboIndex=0; vboIndex<bufferPtr->bufSet.GetN(); ++vboIndex)
	{
		auto &buf=bufferPtr->bufSet[vboIndex];
		auto vtxTop=maxNumVtxPerVbo*vboIndex;
		auto nVtx=YsSmaller(maxNumVtxPerVbo,vtxBuf.GetN()-vtxTop);

		buf.nVtx=nVtx;

		glBindBuffer(GL_ARRAY_BUFFER,buf.vboIdent);
		YSSIZE_T totalBufSize=(vtxBuf.GetNBytePerElem()+nomBuf.GetNBytePerElem())*nVtx;

		glBufferData(GL_ARRAY_BUFFER,totalBufSize,nullptr,GL_STATIC_DRAW);

		YSSIZE_T bufPtr=0;
		buf.vtxPtr=PushBufferSubData(bufPtr,vtxBuf,vtxTop,nVtx);
		buf.nomPtr=PushBufferSubData(bufPtr,nomBuf,vtxTop,nVtx);
	}
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void YsGLBufferManager::ActualBuffer::DrawPrimitiveVtx(YsGLSL3DRenderer *renderer,GLenum primType) const
{
	for(auto &buf : bufSet)
	{
		glBindBuffer(GL_ARRAY_BUFFER,buf.vboIdent);
		YsGLSLDrawPrimitiveVtxfv(renderer,primType,(int)buf.nVtx,(GLfloat *)((long long int)buf.vtxPtr));
	}
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void YsGLBufferManager::ActualBuffer::DrawPrimitiveVtxNom(YsGLSL3DRenderer *renderer,GLenum primType) const
{
	for(auto &buf : bufSet)
	{
		glBindBuffer(GL_ARRAY_BUFFER,buf.vboIdent);
		YsGLSLDrawPrimitiveVtxNomfv(renderer,primType,(int)buf.nVtx,(GLfloat *)((long long int)buf.vtxPtr),(GLfloat *)((long long int)buf.nomPtr));
	}
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void YsGLBufferManager::ActualBuffer::DrawPrimitiveVtxCol(YsGLSL3DRenderer *renderer,GLenum primType) const
{
	for(auto &buf : bufSet)
	{
		glBindBuffer(GL_ARRAY_BUFFER,buf.vboIdent);
		YsGLSLDrawPrimitiveVtxColfv(renderer,primType,(int)buf.nVtx,(GLfloat *)((long long int)buf.vtxPtr),(GLfloat *)((long long int)buf.colPtr));
	}
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void YsGLBufferManager::ActualBuffer::DrawPrimitiveVtxNomCol(YsGLSL3DRenderer *renderer,GLenum primType) const
{
	for(auto &buf : bufSet)
	{
		glBindBuffer(GL_ARRAY_BUFFER,buf.vboIdent);
		YsGLSLDrawPrimitiveVtxNomColfv(renderer,primType,(int)buf.nVtx,(GLfloat *)((long long int)buf.vtxPtr),(GLfloat *)((long long int)buf.nomPtr),(GLfloat *)((long long int)buf.colPtr));
	}
	glBindBuffer(GL_ARRAY_BUFFER,0);
}
void YsGLBufferManager::ActualBuffer::DrawPrimitiveVtxColPointSize(YsGLSL3DRenderer *renderer,GLenum primType) const
{
	for(auto &buf : bufSet)
	{
		glBindBuffer(GL_ARRAY_BUFFER,buf.vboIdent);
		YsGLSLDrawPrimitiveVtxColPointSizefv(renderer,primType,(int)buf.nVtx,(GLfloat *)((long long int)buf.vtxPtr),(GLfloat *)((long long int)buf.colPtr),(GLfloat *)((long long int)buf.pointSizePtr));
	}
	glBindBuffer(GL_ARRAY_BUFFER,0);
}


void YsGLBufferManager::Unit::MakeVtxCol(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLColorBuffer &colBuf,YSSIZE_T maxNumVtxPerVbo)
{
	maxNumVtxPerVbo=AdjustMaxNumVtxPerVboForPrimitiveType(primType,maxNumVtxPerVbo);
	if(nullptr==bufferPtr)
	{
		bufferPtr=YsGLBufferManager::Unit::Create();
	}
	bufferPtr->PrepareVbo(1+vtxBuf.GetN()/maxNumVtxPerVbo);
	for(YSSIZE_T vboIndex=0; vboIndex<bufferPtr->bufSet.GetN(); ++vboIndex)
	{
		auto &buf=bufferPtr->bufSet[vboIndex];
		auto vtxTop=maxNumVtxPerVbo*vboIndex;
		auto nVtx=YsSmaller(maxNumVtxPerVbo,vtxBuf.GetN()-vtxTop);

		buf.nVtx=nVtx;

		glBindBuffer(GL_ARRAY_BUFFER,buf.vboIdent);
		YSSIZE_T totalBufSize=(vtxBuf.GetNBytePerElem()+colBuf.GetNBytePerElem())*nVtx;

		glBufferData(GL_ARRAY_BUFFER,totalBufSize,nullptr,GL_STATIC_DRAW);

		YSSIZE_T bufPtr=0;
		buf.vtxPtr=PushBufferSubData(bufPtr,vtxBuf,vtxTop,nVtx);
		buf.colPtr=PushBufferSubData(bufPtr,colBuf,vtxTop,nVtx);
	}

	glBindBuffer(GL_ARRAY_BUFFER,0);
}
void YsGLBufferManager::Unit::MakeVtxNomCol(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,const YsGLColorBuffer &colBuf,YSSIZE_T maxNumVtxPerVbo)
{
	maxNumVtxPerVbo=AdjustMaxNumVtxPerVboForPrimitiveType(primType,maxNumVtxPerVbo);
	if(nullptr==bufferPtr)
	{
		bufferPtr=YsGLBufferManager::Unit::Create();
	}
	bufferPtr->PrepareVbo(1+vtxBuf.GetN()/maxNumVtxPerVbo);
	for(YSSIZE_T vboIndex=0; vboIndex<bufferPtr->bufSet.GetN(); ++vboIndex)
	{
		auto &buf=bufferPtr->bufSet[vboIndex];
		auto vtxTop=maxNumVtxPerVbo*vboIndex;
		auto nVtx=YsSmaller(maxNumVtxPerVbo,vtxBuf.GetN()-vtxTop);

		buf.nVtx=nVtx;

		glBindBuffer(GL_ARRAY_BUFFER,buf.vboIdent);
		// Memo: Be careful about totalBufSize.  Took two hours to debug.  2015/09/19
		YSSIZE_T totalBufSize=(vtxBuf.GetNBytePerElem()+colBuf.GetNBytePerElem()+nomBuf.GetNBytePerElem())*nVtx;

		glBufferData(GL_ARRAY_BUFFER,totalBufSize,nullptr,GL_STATIC_DRAW);

		YSSIZE_T bufPtr=0;
		buf.vtxPtr=PushBufferSubData(bufPtr,vtxBuf,vtxTop,nVtx);
		buf.colPtr=PushBufferSubData(bufPtr,colBuf,vtxTop,nVtx);
		buf.nomPtr=PushBufferSubData(bufPtr,nomBuf,vtxTop,nVtx);
	}

	glBindBuffer(GL_ARRAY_BUFFER,0);
}
void YsGLBufferManager::Unit::MakeVtxColPointSize(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLColorBuffer &colBuf,const YsGLPointSizeBuffer &psBuf,YSSIZE_T maxNumVtxPerVbo)
{
	maxNumVtxPerVbo=AdjustMaxNumVtxPerVboForPrimitiveType(primType,maxNumVtxPerVbo);
	if(nullptr==bufferPtr)
	{
		bufferPtr=YsGLBufferManager::Unit::Create();
	}
	bufferPtr->PrepareVbo(1+vtxBuf.GetN()/maxNumVtxPerVbo);
	for(YSSIZE_T vboIndex=0; vboIndex<bufferPtr->bufSet.GetN(); ++vboIndex)
	{
		auto &buf=bufferPtr->bufSet[vboIndex];
		auto vtxTop=maxNumVtxPerVbo*vboIndex;
		auto nVtx=YsSmaller(maxNumVtxPerVbo,vtxBuf.GetN()-vtxTop);

		buf.nVtx=nVtx;

		glBindBuffer(GL_ARRAY_BUFFER,buf.vboIdent);
		YSSIZE_T totalBufSize=(vtxBuf.GetNBytePerElem()+colBuf.GetNBytePerElem()+psBuf.GetNBytePerElem())*nVtx;

		glBufferData(GL_ARRAY_BUFFER,totalBufSize,nullptr,GL_STATIC_DRAW);

		YSSIZE_T bufPtr=0;
		buf.vtxPtr=PushBufferSubData(bufPtr,vtxBuf,vtxTop,nVtx);
		buf.colPtr=PushBufferSubData(bufPtr,colBuf,vtxTop,nVtx);
		buf.pointSizePtr=PushBufferSubData(bufPtr,psBuf,vtxTop,nVtx);
	}

	glBindBuffer(GL_ARRAY_BUFFER,0);
}

YsArray <unsigned int> YsGLBufferManager::Unit::GetNativeHandle(void) const
{
	YsArray <unsigned int> hd;
	return hd;
}
