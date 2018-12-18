/* ////////////////////////////////////////////////////////////

File Name: test.cpp
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

#include <ysbitmap.h>
#include <yspng.h>
#include <ysport.h>

#include <string>



bool TestPngDecoder(const char pngFn[],const char refBmpFn[])
{
	printf("Checking %s agains %s...\n",pngFn,refBmpFn);

	YsBitmap png,bmp;
	if(YSOK!=png.LoadPng(pngFn))
	{
		fprintf(stderr,"Cannot load .PNG\n");
		return false;
	}
	if(YSOK!=bmp.LoadBmp(refBmpFn))
	{
		fprintf(stderr,"Cannot load .BMP\n");
		return false;
	}

	if(png.GetWidth()!=bmp.GetWidth() || png.GetHeight()!=bmp.GetHeight())
	{
		fprintf(stderr,"Size does not match.\n");
		return false;
	}

	for(YSSIZE_T i=0; i<bmp.GetWidth()*bmp.GetHeight(); ++i)
	{
		auto pngRGBA=png.GetRGBABitmapPointer();
		auto bmpRGBA=bmp.GetRGBABitmapPointer();
		if(pngRGBA[i*4  ]!=bmpRGBA[i*4  ] ||
		   pngRGBA[i*4+1]!=bmpRGBA[i*4+1] ||
		   pngRGBA[i*4+2]!=bmpRGBA[i*4+2])
		{
			YSSIZE_T x=i%bmp.GetWidth();
			YSSIZE_T y=i/bmp.GetWidth();
			fprintf(stderr,"Pixel colors do not match.\n");
			fprintf(stderr,"  Coordinate (%d,%d)\n",(int)x,(int)y);
			fprintf(stderr,"  RGB from PNG (%d,%d,%d)\n",pngRGBA[i*4],pngRGBA[i*4+1],pngRGBA[i*4+2]);
			fprintf(stderr,"  RGB from BMP (%d,%d,%d)\n",bmpRGBA[i*4],bmpRGBA[i*4+1],bmpRGBA[i*4+2]);
			return false;
		}
	}

	printf("OK!\n");
	return true;
}

int main(void)
{
	YsFileIO::ChDir(YsSpecialPath::GetProgramBaseDirW());

	const char *const pngBmp[]=
	{
		"pictures/BabyBlueJay",
		"pictures/Concorde",
		"pictures/Foliage",
		"pictures/heading",
	};

	int nFail=0;
	for(auto pb : pngBmp)
	{
		std::string png(pb),bmp(pb);
		png+=".png";
		bmp+=".bmp";
		if(true!=TestPngDecoder(png.c_str(),bmp.c_str()))
		{
			fprintf(stderr,"Error! Failed: %s\n",pb);
			++nFail;
		}
	}
	fprintf(stderr,"%d test failed\n",nFail);
	if(0<nFail)
	{
		return 1;
	}
	return 0;
}
