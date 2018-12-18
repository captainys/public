/* ////////////////////////////////////////////////////////////

File Name: ysfixedfontrenderer.cpp
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

#include "ysfixedfontrenderer.h"
#include <ysglfontdata.h>

YsFixedFontRenderer::YsFixedFontRenderer()
{
	Initialize();
}

YsFixedFontRenderer::~YsFixedFontRenderer()
{
}

void YsFixedFontRenderer::Initialize(void)
{
	fontWid=12;
	fontHei=16;
	fontPtr=YsFont12x16;
}

int YsFixedFontRenderer::GetFontWidth(void) const
{
	return fontWid;
}

int YsFixedFontRenderer::GetFontHeight(void) const
{
	return fontHei;
}

YSRESULT YsFixedFontRenderer::RequestDefaultFontWithPixelHeight(unsigned int requestedHei)
{
	int fontWid,fontHei;
	const unsigned char *const *fontPtr=YsGlSelectFontBitmapPointerByHeight(&fontWid,&fontHei,requestedHei);
	if(NULL!=fontPtr)
	{
		this->fontWid=fontWid;
		this->fontHei=fontHei;
		this->fontPtr=fontPtr;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsFixedFontRenderer::RenderString(YsBitmap &bmp,const wchar_t wStr[],const YsColor &fgCol,const YsColor &bgCol) const
{
	int nWid,nHei;
	CountStringDimension(nWid,nHei,wStr);

	YsArray <char> cStrBuf(nWid+1,NULL);

	bmp.PrepareBitmap(nWid*GetFontWidth(),nHei*GetFontHeight());
	bmp.Clear((unsigned char)bgCol.Ri(),(unsigned char)bgCol.Gi(),(unsigned char)bgCol.Bi(),0);

	int i=0,i0=0,cStrPtr=0,y=bmp.GetHeight()-GetFontHeight();
	for(;;)
	{
		if(0==wStr[i] || '\n'==wStr[i])
		{
			cStrBuf[cStrPtr]=0;
			YsGlWriteStringToRGBA8Bitmap(
			    cStrBuf,fgCol.Ri(),fgCol.Gi(),fgCol.Bi(),255,
			    bmp.GetEditableRGBABitmapPointer(),bmp.GetWidth(),bmp.GetHeight(),
			    0,y,
			    fontPtr,fontWid,fontHei);
			y-=fontHei;
			i0=i+1;
			cStrPtr=0;
		}
		else
		{
			cStrBuf[cStrPtr++]=(char)wStr[i];
		}

		if(0==wStr[i])
		{
			break;
		}
		i++;
	}

	return YSOK;
}

YSRESULT YsFixedFontRenderer::GetTightRenderSize(int &wid,int &hei,const wchar_t wStr[]) const
{
	int nWid,nHei;
	CountStringDimension(nWid,nHei,wStr);
	wid=fontWid*nWid;
	hei=fontHei*nHei;
	return YSOK;
}

