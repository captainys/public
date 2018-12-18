/* ////////////////////////////////////////////////////////////

File Name: ysglbuffermanager.cpp
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

#include "ysglbuffermanager.h"


YsGLBufferManager::Unit::Unit()
{
	bufferPtr=nullptr;
}

YsGLBufferManager::Unit::~Unit()
{
	CleanUp();
}

void YsGLBufferManager::Unit::CleanUp(void)
{
	if(nullptr!=bufferPtr)
	{
		Delete(bufferPtr);
		bufferPtr=nullptr;
	}
}
YSSIZE_T YsGLBufferManager::Unit::AdjustMaxNumVtxPerVboForPrimitiveType(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx)
{
	switch(primType)
	{
	default:
		break;
	case YsGL::LINES:
		return ((nVtx+1)/2)*2;
	case YsGL::TRIANGLES:
		return ((nVtx+2)/3)*3;
	}
	return nVtx;
}
const YsGLBufferManager::ActualBuffer *YsGLBufferManager::Unit::GetActualBuffer(void) const
{
	return bufferPtr;
}
////////////////////////////////////////////////////////////


/* static */ YsGLBufferManager &YsGLBufferManager::GetSharedBufferManager(void)
{
	static YsGLBufferManager sharedManager;
	return sharedManager;
}

YsGLBufferManager::Handle YsGLBufferManager::Create(void)
{
	auto hd=bufferArray.Create();
	bufferArray[hd]->CleanUp();
	return hd;
}

void YsGLBufferManager::ContextLost(void)
{
	for(auto hd : bufferArray.AllHandle())
	{
		bufferArray[hd]->CleanUp();
	}
}

const YsGLBufferManager::Unit *YsGLBufferManager::GetBufferUnit(Handle hd) const
{
	return bufferArray[hd];
}

const YsGLBufferManager::ActualBuffer *YsGLBufferManager::GetActualBuffer(Handle hd) const
{
	auto unitPtr=GetBufferUnit(hd);
	if(Unit::EMPTY!=unitPtr->GetState())
	{
		return unitPtr->GetActualBuffer();
	}
	return nullptr;
}

void YsGLBufferManager::Delete(Handle hd)
{
	if(nullptr!=hd)
	{
		bufferArray[hd]->CleanUp();
		bufferArray.Delete(hd);
	}
}
void YsGLBufferManager::MakeVtx(Handle bfHd,YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,YSSIZE_T maxNumVtxPerVbo)
{
	bufferArray[bfHd]->MakeVtx(primType,vtxBuf,maxNumVtxPerVbo);
}
void YsGLBufferManager::MakeVtxNom(Handle bfHd,YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,YSSIZE_T maxNumVtxPerVbo)
{
	bufferArray[bfHd]->MakeVtxNom(primType,vtxBuf,nomBuf,maxNumVtxPerVbo);
}
void YsGLBufferManager::MakeVtxCol(Handle bfHd,YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLColorBuffer &colBuf,YSSIZE_T maxNumVtxPerVbo)
{
	bufferArray[bfHd]->MakeVtxCol(primType,vtxBuf,colBuf,maxNumVtxPerVbo);
}
void YsGLBufferManager::MakeVtxNomCol(Handle bfHd,YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,const YsGLColorBuffer &colBuf,YSSIZE_T maxNumVtxPerVbo)
{
	bufferArray[bfHd]->MakeVtxNomCol(primType,vtxBuf,nomBuf,colBuf,maxNumVtxPerVbo);
}
void YsGLBufferManager::MakeVtxColPointSize(Handle bfHd,YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLColorBuffer &colBuf,const YsGLPointSizeBuffer &psBuf,YSSIZE_T maxNumVtxPerVbo)
{
	bufferArray[bfHd]->MakeVtxColPointSize(primType,vtxBuf,colBuf,psBuf,maxNumVtxPerVbo);
}


YSRESULT YsGLBufferManager::CheckIntegrity(void) const
{
	YsArray <unsigned int> allHd;
	for(auto hd : bufferArray.AllHandle())
	{
		allHd.Add(bufferArray[hd]->GetNativeHandle());
	}

	YsQuickSort <unsigned int,int> (allHd.GetN(),allHd.GetEditableArray(),nullptr);

	for(int i=0; i<allHd.GetN()-1; ++i)
	{
		if(allHd[i]==allHd[i+1])
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("Boom!\n");
			exit(1);
		}
	}
	return YSOK;
}
