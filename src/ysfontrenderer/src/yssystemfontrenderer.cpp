/* ////////////////////////////////////////////////////////////

File Name: yssystemfontrenderer.cpp
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

#include "yssystemfontrenderer.h"
#include <yssystemfont.h>

YsSystemFontRenderer::YsSystemFontRenderer()
{
	fontCache=new YsSystemFontCache;
	Initialize();
}

YsSystemFontRenderer::~YsSystemFontRenderer()
{
	delete fontCache;
}

void YsSystemFontRenderer::Initialize(void)
{
	RequestDefaultFontWithPixelHeight(10);
	fontHeightCache=0;
}

YSRESULT YsSystemFontRenderer::RequestDefaultFontWithPixelHeight(unsigned int fontHeightInPix)
{
	fontHeightCache=0;
	return fontCache->RequestDefaultFontWithHeight(fontHeightInPix);
}

YSRESULT YsSystemFontRenderer::RenderString(YsBitmap &bmp,const wchar_t wStr[],const YsColor &fgCol,const YsColor &bgCol) const
{
	unsigned char fg[3]={(unsigned char)fgCol.Ri(),(unsigned char)fgCol.Gi(),(unsigned char)fgCol.Bi()};
	unsigned char bg[3]={(unsigned char)bgCol.Ri(),(unsigned char)bgCol.Gi(),(unsigned char)bgCol.Bi()};

	YsSystemFontTextBitmap tmpBmp;
	if(YSOK==fontCache->MakeRGBABitmap(tmpBmp,wStr,fg,bg,YSTRUE))
	{
		const unsigned int wid=tmpBmp.Width();
		const unsigned int hei=tmpBmp.Height();

		unsigned char *bmpBuf=tmpBmp.TransferBitmap();

		bmp.SetDirect(wid,hei,bmpBuf);

		return YSOK;
	}
	bmp.CleanUp();
	return YSERR;
}

YSRESULT YsSystemFontRenderer::GetTightRenderSize(int &wid,int &hei,const wchar_t wStr[]) const
{
	return fontCache->GetTightBitmapSize(wid,hei,wStr);
}

int YsSystemFontRenderer::GetFontHeight(void) const
{
	if(0==fontHeightCache)
	{
		int x;
		GetTightRenderSize(x,fontHeightCache,L"X");
	}
	return fontHeightCache;
}
