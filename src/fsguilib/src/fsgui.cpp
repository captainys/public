/* ////////////////////////////////////////////////////////////

File Name: fsgui.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>



#include <ysclass.h>

#include <fssimplewindow.h>
#include "fsguiclipboard.h"

#include <ysbitmap.h>
#include <ysfontrenderer.h>
#include <ysfixedfontrenderer.h>

#include "fsgui.h"





#include <time.h>


YsColor FsGuiObject::defBgCol(0.0,0.0,1.0);
YsColor FsGuiObject::defFgCol(1.0,1.0,1.0);
YsColor FsGuiObject::defActiveBgCol(0.0,0.0,0.4);
YsColor FsGuiObject::defActiveFgCol(1.0,1.0,1.0);
YsColor FsGuiObject::defFrameCol(1.0,1.0,1.0);

YsColor FsGuiObject::defDialogBgCol(0.2,0.2,0.8);

YsColor FsGuiObject::defTabBgCol(0.1,0.1,0.9);
YsColor FsGuiObject::defTabClosedBgCol(0.0,0.0,1.0);
YsColor FsGuiObject::defTabClosedFgCol(1.0,1.0,1.0);

double FsGuiObject::defRoundRadius=0.0;

YsColor FsGuiObject::defListBgCol(0.0,0.0,0.0);
YsColor FsGuiObject::defListFgCol(1.0,1.0,1.0);
YsColor FsGuiObject::defListActiveBgCol(1.0,1.0,1.0);
YsColor FsGuiObject::defListActiveFgCol(0.0,0.0,1.0);

int FsGuiObject::defHSpaceUnit=10;
int FsGuiObject::defVSpaceUnit=16;
int FsGuiObject::defHAnnotation=12;
int FsGuiObject::defHScrollBar=12;


static YsFixedFontRenderer defaultRenderer;
class YsFontRenderer *FsGuiObject::defUnicodeRenderer=NULL;
class YsFontRenderer *FsGuiObject::defAsciiRenderer=&defaultRenderer;

////////////////////////////////////////////////////////////

YSRESULT FsGuiObject::GetUnicodeCharacterBitmapSize(int &wid,int &hei)  // Based on the size of L"X"
{
	YsBitmap bmp;
	const wchar_t xray[]={'X',0}; // Workaround to cope with 64-bit GCC bug in Mac OSX
	if(YSOK==RenderUnicodeString(bmp,xray,defFgCol,defBgCol))
	{
		wid=bmp.GetWidth();
		hei=bmp.GetHeight();
		return YSOK;
	}
	wid=0;
	hei=0;
	return YSERR;
}

YSRESULT FsGuiObject::RenderUnicodeString(YsBitmap &bmp,const char str[],const YsColor &fgCol,const YsColor &bgCol)
{
	if(NULL!=defUnicodeRenderer)
	{
		YsWString wStr;
		wStr.SetUTF8String(str);
		return defUnicodeRenderer->RenderString(bmp,wStr,fgCol,bgCol);
	}
	return YSERR;
}

YSRESULT FsGuiObject::RenderUnicodeString(YsBitmap &bmp,const wchar_t wStr[],const YsColor &fgCol,const YsColor &bgCol)
{
	if(NULL!=defUnicodeRenderer)
	{
		return defUnicodeRenderer->RenderString(bmp,wStr,fgCol,bgCol);
	}
	return YSERR;
}

YSRESULT FsGuiObject::GetTightUnicodeRenderSize(int &wid,int &hei,const wchar_t wStr[])
{
	if(NULL!=defUnicodeRenderer)
	{
		return defUnicodeRenderer->GetTightRenderSize(wid,hei,wStr);
	}
	return YSERR;
}

YSRESULT FsGuiObject::RenderAsciiString(YsBitmap &bmp,const char str[],const YsColor &fgCol,const YsColor &bgCol)
{
	if(NULL!=defAsciiRenderer)
	{
		YsWString wStr;
		wStr.SetUTF8String(str);
		return defAsciiRenderer->RenderString(bmp,wStr,fgCol,bgCol);
	}
	return YSERR;
}

YSRESULT FsGuiObject::RenderAsciiString(YsBitmap &bmp,const wchar_t str[],const YsColor &fgCol,const YsColor &bgCol)
{
	if(NULL!=defAsciiRenderer)
	{
		return defAsciiRenderer->RenderString(bmp,str,fgCol,bgCol);
	}
	return YSERR;
}

YSRESULT FsGuiObject::GetTightAsciiRenderSize(int &wid,int &hei,const char str[])
{
	if(NULL!=defAsciiRenderer)
	{
		YsWString wStr;
		wStr.SetUTF8String(str);
		return defAsciiRenderer->GetTightRenderSize(wid,hei,wStr);
	}
	return YSERR;
}

/* static */ FsGuiObject::TouchState FsGuiObject::MatchTouch(const YsArray <YsVec2> &newTouchIn,const YsArray <YsVec2> &prevTouchIn)
{
	TouchState trans;
	auto newTouch=newTouchIn;
	auto prevTouch=prevTouchIn;
	while(0<newTouch.GetN() && 0<prevTouch.GetN())
	{
		int nearIdx=0;
		double nearDist=(prevTouch[0]-newTouch[0]).GetSquareLength();
		for(int i=1; i<newTouch.GetN(); ++i)
		{
			const double dist=(prevTouch[0]-newTouch[i]).GetSquareLength();
			if(dist<nearDist)
			{
				nearIdx=i;
				nearDist=dist;
			}
		}

		trans.movement.Increment();
		trans.movement.Last().from=prevTouch[0];
		trans.movement.Last().to=newTouch[nearIdx];

		prevTouch.Delete(0);
		newTouch.Delete(nearIdx);
	}

	trans.newTouch.MoveFrom(newTouch);
	trans.endedTouch.MoveFrom(prevTouch);

	return trans;
}

/* static */ YsArray <YsVec2> FsGuiObject::ConvertTouch(YSSIZE_T nTouch,const FsVec2i touch[])
{
	YsArray <YsVec2> newTouch(nTouch,nullptr);
	for(YSSIZE_T idx=0; idx<nTouch; ++idx)
	{
		newTouch[idx].Set(touch[idx].x(),touch[idx].y());
	}
	return newTouch;
}

////////////////////////////////////////////////////////////

YSRESULT FsGuiObject::GetAsciiCharacterBitmapSize(int &wid,int &hei)  // Based on the size of "X"
{
	return GetAsciiStringBitmapSize(wid,hei,"X");
}

YSRESULT FsGuiObject::GetAsciiStringBitmapSize(int &wid,int &hei,const char str[])
{
	if(NULL!=defAsciiRenderer)
	{
		YsBitmap testBmp;
		YsWString wStr;
		wStr.SetUTF8String(str);
		defAsciiRenderer->RenderString(testBmp,wStr,defFgCol,defBgCol);
		wid=testBmp.GetWidth();
		hei=testBmp.GetHeight();
		return YSOK;
	}
	printf("Warning: ASCII Renderer is not set!\n");
	wid=0;
	hei=0;
	return YSERR;
}

