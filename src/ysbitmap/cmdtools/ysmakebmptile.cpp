/* ////////////////////////////////////////////////////////////

File Name: ysmakebmptile.cpp
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


/*
    eg.
	ysmakebmptile pattern.png 1024 1024 bmp1.png bmp2.png bmp3.png ...
*/


int main(int ac,char *av[])
{
	if(4>ac)
	{
		printf("Eg. ysmakebmptile pattern.png 1024 1024 bmp1.png bmp2.png bmp3.png ...\n");
		return 1;
	}

	const YsString outFn(av[1]);
	const int allWid=atoi(av[2]);
	const int allHei=atoi(av[3]);

	printf("Output File Name : %s\n",outFn.Txt());
	printf("Output Width : %d\n",allWid);
	printf("Output Width : %d\n",allHei);



	YsBitmap bmp;
	bmp.PrepareBitmap(allWid,allHei);
	bmp.Clear(0,0,0,0);



	int curX=0,curY=0,nextY=0;
	for(int i=4; i<ac; ++i)
	{
		YsBitmap tile;
		const YsString inFn(av[i]);
		YsString ext;
		inFn.GetExtension(ext);
		ext.Capitalize();
		if(0==strcmp(ext,".PNG"))
		{
			tile.LoadPng(inFn);
		}
		else if(0==strcmp(ext,".BMP"))
		{
			tile.LoadBmp(inFn);
		}

		nextY=YsGreater(nextY,curY+tile.GetHeight());
		if(allWid<curX+tile.GetWidth())
		{
			curX=0;
			curY=nextY;
		}

		bmp.Copy(tile,curX,curY);
		curX+=tile.GetWidth();
	}



	YsRawPngEncoder encoder;
	encoder.EncodeToFile(outFn,bmp.GetWidth(),bmp.GetHeight(),8,6,bmp.GetRGBABitmapPointer());

	return 0;
}

