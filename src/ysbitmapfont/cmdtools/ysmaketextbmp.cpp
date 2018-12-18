/* ////////////////////////////////////////////////////////////

File Name: ysmaketextbmp.cpp
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

#include <stdlib.h>

#include <ysclass.h>
#include <ysbitmap.h>
#include <yspngenc.h>
#include <ysglfontdata.h>




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

int main(int ac,char *av[])
{
	if(4>ac)
	{
		printf("Usage: ysmaketextbmp output.png \"text\" fontHei\n");
		return 1;
	}

	const int fontHeiRequest=atoi(av[3]);

	int fontWid,fontHei;
	auto fontPtr=YsGlSelectFontBitmapPointerByHeight(&fontWid,&fontHei,fontHeiRequest);

	const int len=strlen(av[2]);

	YsBitmap bmp;
	bmp.PrepareBitmap(len*fontWid,fontHei);
	YsGlWriteStringToRGBA8Bitmap(av[2],255,255,255,255,
		bmp.GetEditableRGBABitmapPointer(),bmp.GetWidth(),bmp.GetHeight(),
		0,0,
		fontPtr,fontWid,fontHei);
	bmp.Invert();

	if(YSOK==SaveBitmap(av[1],bmp))
	{
		printf("Saved %s\n",av[1]);
	}
	else
	{
		printf("Cannot save %s\n",av[1]);
		return 1;
	}
	return 0;
}

