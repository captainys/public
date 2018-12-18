/* ////////////////////////////////////////////////////////////

File Name: ystexturemanager_null.cpp
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

#include "ystexturemanager_nownd.h"

YSRESULT YsTextureManager::Unit::Bind(int) const
{
	return YSERR;
}

YSRESULT YsTextureManager::Unit::BindFrameBuffer(void) const
{
	return YSERR;
}


/* static */ YsTextureManager::ActualTexture *YsTextureManager::Alloc(void)
{
	return nullptr;
}

/* static */ void YsTextureManager::Delete(YsTextureManager::ActualTexture *texPtr)
{
}

/* static */ YSRESULT YsTextureManager::CreateDepthMapRenderTarget(ActualTexture *texPtr,int wid,int hei)
{
	return YSOK;
}

/* static */ YSRESULT YsTextureManager::TransferRGBABitmap(YsTextureManager::ActualTexture *texPtr,int wid,int hei,const unsigned char rgba[])
{
	return YSOK;
}

/* static */ YSBOOL YsTextureManager::IsTextureReady(YsTextureManager::ActualTexture *texPtr)
{
	return YSTRUE;
}

