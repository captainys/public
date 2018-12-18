/* ////////////////////////////////////////////////////////////

File Name: yswin32bitmap.cpp
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

#include <windows.h>

#include "yswin32bitmap.h"




HBITMAP YsCreateWin32BitmapFromYsBitmap(HDC bmpDc,const YsBitmap &ysBmp)  // HDC bmpDc must be created by CreateCompatibleDC
{
	int bmpWid,bmpHei;
	HBITMAP bmp;
	BITMAPINFOHEADER bmiHeader;
	void *bitBuf;

	bmpWid=(ysBmp.GetWidth()+3)&(~3);
	bmpHei=ysBmp.GetHeight();

	bmiHeader.biSize=sizeof(bmiHeader);
	bmiHeader.biWidth=bmpWid;
	bmiHeader.biHeight=bmpHei;
	bmiHeader.biPlanes=1;
	bmiHeader.biBitCount=24;
	bmiHeader.biCompression=BI_RGB;
	bmiHeader.biSizeImage=0;
	bmiHeader.biXPelsPerMeter=2048;
	bmiHeader.biYPelsPerMeter=2048;
	bmiHeader.biClrUsed=0;
	bmiHeader.biClrImportant=0;

	bmp=CreateDIBSection
	    (bmpDc,(BITMAPINFO *)&bmiHeader,DIB_RGB_COLORS,&bitBuf,NULL,0);

	int x,y;
	unsigned char *buf;
	buf=(unsigned char *)bitBuf;
	for(y=0; y<bmpHei; y++)
	{
		for(x=0; x<bmpWid; x++)
		{
			const unsigned char *rgba;
			if(x<ysBmp.GetWidth())
			{
				rgba=ysBmp.GetRGBAPixelPointer(x,y);
			}
			else
			{
				rgba=ysBmp.GetRGBAPixelPointer(ysBmp.GetWidth()-1,y);
			}
			buf[(y*bmpWid+x)*3  ]=rgba[2];
			buf[(y*bmpWid+x)*3+1]=rgba[1];
			buf[(y*bmpWid+x)*3+2]=rgba[0];
		}
	}

	return bmp;
}

