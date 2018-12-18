/* ////////////////////////////////////////////////////////////

File Name: ysfontrenderer.h
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

#ifndef YSFONTRENDERER_IS_INCLUDED
#define YSFONTRENDERER_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysbitmap.h>

class YsFontRenderer
{
public:
	/*! Utility function for counting width and height of a wide-char string.  It takes
	    new-line ('\n') into account and calculates the maximum width and height, which 
	    are returned in nWidth and nLine.
	    */
	template <class CHARTYPE>
	static void CountStringDimension(int &nWidth,int &nLine,const CHARTYPE str[]);

	/*! A convenience function for rendring an ASCII string.  It internally uses wide-char version of 
	    RenderString function. */
	YSRESULT RenderString(YsBitmap &bmp,const char str[],const YsColor &fgCol,const YsColor &bgCol) const;

	/*! Request a font by height in the number of pixels. */
	virtual YSRESULT RequestDefaultFontWithPixelHeight(unsigned int heightInPix)=0;

	/*! Render a wide-char string into a bitmap. */
	virtual YSRESULT RenderString(YsBitmap &bmp,const wchar_t wStr[],const YsColor &fgCol,const YsColor &bgCol) const=0;

	/*! Returns a tight rectangle dimension for the given wide-char string. */
	virtual YSRESULT GetTightRenderSize(int &wid,int &hei,const wchar_t wStr[]) const=0;
};

template <class CHARTYPE>
void YsFontRenderer::CountStringDimension(int &nWidth,int &nLine,const CHARTYPE str[])
{
	if(0!=str[0])
	{
		nLine=1;
		nWidth=0;

		int i,nCurWidth=0;
		for(i=0; 0!=str[i]; i++)
		{
			if('\n'==str[i])
			{
				nLine++;
				nCurWidth=0;
			}
			else
			{
				nCurWidth++;
				if(nWidth<nCurWidth)
				{
					nWidth=nCurWidth;
				}
			}
		}
	}
	else
	{
		nLine=0;
		nWidth=0;
	}
}


#include "ysfixedfontrenderer.h"
#include "yssystemfontrenderer.h"

/* } */
#endif
