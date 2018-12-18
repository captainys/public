/* ////////////////////////////////////////////////////////////

File Name: ysmakeblackwhite.cpp
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

#include <ysclass.h>
#include <ysbitmap.h>
#include <yspngenc.h>
#include <stdlib.h>



YSRESULT LoadBitmap(YsBitmap &bmp,const char fn[])
{
	const YsString inFn(fn);
	YsString ext;
	inFn.GetExtension(ext);
	ext.Capitalize();
	if(0==strcmp(ext,".PNG"))
	{
		return bmp.LoadPng(inFn);
	}
	else if(0==strcmp(ext,".BMP"))
	{
		return bmp.LoadBmp(inFn);
	}
	return YSERR;
}

YSRESULT SaveBitmap(const char fn[],const YsBitmap &bmp)
{
	const YsString inFn(fn);
	YsString ext;
	inFn.GetExtension(ext);
	ext.Capitalize();
	if(0==strcmp(ext,".PNG"))
	{
		YsRawPngEncoder encoder;
		return (YSRESULT)encoder.EncodeToFile(fn,bmp.GetWidth(),bmp.GetHeight(),8,6,bmp.GetRGBABitmapPointer());
	}
	else if(0==strcmp(ext,".BMP"))
	{
		return (YSRESULT)bmp.SaveBmp(inFn);
	}
	return YSERR;
}

void MakeBlackAndWhite(YsBitmap &bmp,int rThr,int gThr,int bThr)
{
	for(int y=0; y<bmp.GetHeight(); ++y)
	{
		for(int x=0; x<bmp.GetWidth(); ++x)
		{
			auto pix=bmp.GetEditableRGBAPixelPointer(x,y);
			if(rThr<=pix[0] && gThr<pix[1] && bThr<pix[2])
			{
				pix[0]=255;
				pix[1]=255;
				pix[2]=255;
			}
			else
			{
				pix[0]=0;
				pix[1]=0;
				pix[2]=0;
			}
		}
	}
}

int main(int ac,char *av[])
{
	if(6>ac)
	{
		printf("Usage: ysmakeblackwhite infile outfile RThr(0-255) GThr(0-255) BThr(0-255)\n");
		return 1;
	}

	const int rThr=atoi(av[3]);
	const int gThr=atoi(av[4]);
	const int bThr=atoi(av[5]);

	YsBitmap bmp;
	if(YSOK==LoadBitmap(bmp,av[1]))
	{
		MakeBlackAndWhite(bmp,rThr,gThr,bThr);
		if(YSOK==SaveBitmap(av[2],bmp))
		{
			printf("Saved %s\n",av[2]);
		}
		else
		{
			printf("Cannot save %s\n",av[2]);
			return 1;
		}
	}
	else
	{
		printf("Cannot load %s\n",av[1]);
		return 1;
	}
	return 0;
}

