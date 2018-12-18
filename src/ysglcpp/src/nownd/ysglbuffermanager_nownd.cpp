/* ////////////////////////////////////////////////////////////

File Name: ysglbuffermanager_nownd.cpp
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

#include "ysglbuffermanager_nownd.h"

////////////////////////////////////////////////////////////

/* static */ YsGLBufferManager::ActualBuffer *YsGLBufferManager::Unit::Create(void)
{
	return new YsGLBufferManager::ActualBuffer;
}
/* static */ void YsGLBufferManager::Unit::Delete(YsGLBufferManager::ActualBuffer *ptr)
{
	delete ptr;
}

YsGLBufferManager::Unit::STATE YsGLBufferManager::Unit::GetState(void) const
{
	return EMPTY;
}

void YsGLBufferManager::Unit::MakeVtx(YsGL::PRIMITIVE_TYPE ,const YsGLVertexBuffer &,YSSIZE_T )
{
}
void YsGLBufferManager::Unit::MakeVtxNom(YsGL::PRIMITIVE_TYPE primType,const YsGLVertexBuffer &vtxBuf,const YsGLNormalBuffer &nomBuf,YSSIZE_T maxNumVtxPerVbo)
{
}
void YsGLBufferManager::Unit::MakeVtxCol(YsGL::PRIMITIVE_TYPE ,const YsGLVertexBuffer &,const YsGLColorBuffer &,YSSIZE_T )
{
}
void YsGLBufferManager::Unit::MakeVtxNomCol(YsGL::PRIMITIVE_TYPE ,const YsGLVertexBuffer &,const YsGLNormalBuffer &,const YsGLColorBuffer &,YSSIZE_T )
{
}
void YsGLBufferManager::Unit::MakeVtxColPointSize(YsGL::PRIMITIVE_TYPE ,const YsGLVertexBuffer &,const YsGLColorBuffer &,const YsGLPointSizeBuffer &,YSSIZE_T )
{
}

YsArray <unsigned int> YsGLBufferManager::Unit::GetNativeHandle(void) const
{
	YsArray <unsigned int> hd;
	return hd;
}
