/* ////////////////////////////////////////////////////////////

File Name: ys_bitmap_command_base.cpp
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

int main(int ac,char *av[])
{
	if(5>ac)
	{
		printf("Usage: yspastebmp <destination.png> <source.png> <x> <y>\n");
		return 1;
	}

	YsBitmap dst,src;
	if(YSOK==LoadBitmap(dst,av[1]) && YSOK==LoadBitmap(src,av[2]))
	{
		dst.Transfer(src,atoi(av[3]),atoi(av[4]),0);
		dst.Invert();

		// Do processing
		if(YSOK==SaveBitmap(av[1],dst))
		{
			printf("Saved %s\n",av[1]);
		}
		else
		{
			printf("Cannot save %s\n",av[1]);
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

