/* ////////////////////////////////////////////////////////////

File Name: ysuwpsystemfont.cpp
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
#include <time.h>
#include <vector>

#include <agile.h>

#include "../yssystemfont.h"



using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System;
using namespace Platform;



class YsSystemFontCache::InternalData
{
public:
	const int bmpWid=800,bmpHei=400;
	Microsoft::Graphics::Canvas::CanvasDevice^ sharedDevice=nullptr;
	Microsoft::Graphics::Canvas::CanvasRenderTarget^ bitmap=nullptr;
	Microsoft::Graphics::Canvas::Text::CanvasTextFormat^ textFormat=nullptr;

	InternalData();
	~InternalData();
	void Initialize(void);
	void CleanUp(void);
	void RequestFontHeight(int fontHeight);

	unsigned char *RenderTextToBitmap(int &w,int &h,const wchar_t msg[],const unsigned char fgCol[3],const unsigned char bgCol[3]);
	YSRESULT GetTightBitmapSize(int &wid,int &hei,const wchar_t str[]) const;
};
YsSystemFontCache::InternalData::InternalData()
{
}
YsSystemFontCache::InternalData::~InternalData()
{
	CleanUp();
}
void YsSystemFontCache::InternalData::Initialize(void)
{
	CleanUp();
	sharedDevice=Microsoft::Graphics::Canvas::CanvasDevice::GetSharedDevice();
	bitmap=ref new Microsoft::Graphics::Canvas::CanvasRenderTarget(sharedDevice,(float)bmpWid,(float)bmpHei,96);
	textFormat=ref new Microsoft::Graphics::Canvas::Text::CanvasTextFormat();
	textFormat->FontSize=bitmap->ConvertPixelsToDips(48);
}
void YsSystemFontCache::InternalData::CleanUp(void)
{
	if(nullptr!=bitmap)
	{
		delete bitmap;
		bitmap=nullptr;
	}
	if(nullptr!=textFormat)
	{
		delete textFormat;
		textFormat=nullptr;
	}
}
void YsSystemFontCache::InternalData::RequestFontHeight(int fontHeight)
{
	textFormat->FontSize=bitmap->ConvertPixelsToDips(fontHeight);
}
unsigned char *YsSystemFontCache::InternalData::RenderTextToBitmap(int &w,int &h,const wchar_t wMsg[],const unsigned char fgCol[3],const unsigned char bgCol[3])
{
	Windows::UI::Color fg,bg;
	fg.R=fgCol[0];
	fg.G=fgCol[1];
	fg.B=fgCol[2];
	fg.A=255;
	bg.R=bgCol[0];
	bg.G=bgCol[1];
	bg.B=bgCol[2];
	bg.A=0;

	auto ds=bitmap->CreateDrawingSession();
	ds->Clear(bg); // Windows::UI::Colors::Green

	auto wStr=ref new Platform::String(wMsg);

	auto txtLayout=ref new Microsoft::Graphics::Canvas::Text::CanvasTextLayout(ds,wStr,textFormat,(float)bmpWid,(float)bmpHei);
	auto rounding=Microsoft::Graphics::Canvas::CanvasDpiRounding::Floor;
	int renderX=(int)bitmap->ConvertDipsToPixels(txtLayout->LayoutBounds.X,rounding);
	int renderY=(int)bitmap->ConvertDipsToPixels(txtLayout->LayoutBounds.Y,rounding);
	int renderW=(int)bitmap->ConvertDipsToPixels(txtLayout->LayoutBounds.Width,rounding);
	int renderH=(int)bitmap->ConvertDipsToPixels(txtLayout->LayoutBounds.Height,rounding);

	ds->DrawTextLayout(txtLayout,0,0,fg); // Windows::UI::Colors::Orange

	ds->Flush(); // <- Don't forget this, or you get nothing.

	// When you need the bitmap size of the text on the bitmap,
	// you can do it by CanvasTextLayout.
	// http://stackoverflow.com/questions/30696838/how-to-calculate-the-size-of-a-piece-of-text-in-win2d

	// Windows bitmap starts at top-left, while OpenGL bitmap at bottom-left.
	// Bitmap needs to be flipped.
	w=renderW;
	h=renderH;

	auto rawBitmap=bitmap->GetPixelBytes();
	auto reallyRawBitmap=new unsigned char [renderW*renderH*4];
	{
		int x=0,y=0;
		for(int y=0; y<renderH; ++y)
		{
			for(int x=0; x<renderW; ++x)
			{
				if(x<bmpWid && y<bmpHei)
				{
					// Windows -> BGRA   OpenGL -> RGBA  Need conversion.
					reallyRawBitmap[(y*w+x)*4  ]=rawBitmap[(y*bmpWid+x)*4+2];
					reallyRawBitmap[(y*w+x)*4+1]=rawBitmap[(y*bmpWid+x)*4+1];
					reallyRawBitmap[(y*w+x)*4+2]=rawBitmap[(y*bmpWid+x)*4];
					reallyRawBitmap[(y*w+x)*4+3]=rawBitmap[(y*bmpWid+x)*4+3];
				}
				else
				{
					reallyRawBitmap[(y*w+x)*4  ]=0;
					reallyRawBitmap[(y*w+x)*4+1]=0;
					reallyRawBitmap[(y*w+x)*4+2]=0;
					reallyRawBitmap[(y*w+x)*4+3]=0;
				}
			}
		}
	}

	return reallyRawBitmap;
}

YSRESULT YsSystemFontCache::InternalData::GetTightBitmapSize(int &wid,int &hei,const wchar_t str[]) const
{
	auto ds=bitmap->CreateDrawingSession();

	auto wStr=ref new Platform::String(str);

	auto txtLayout=ref new Microsoft::Graphics::Canvas::Text::CanvasTextLayout(ds,wStr,textFormat,(float)bmpWid,(float)bmpHei);
	auto rounding=Microsoft::Graphics::Canvas::CanvasDpiRounding::Floor;
	int renderX=(int)bitmap->ConvertDipsToPixels(txtLayout->LayoutBounds.X,rounding);
	int renderY=(int)bitmap->ConvertDipsToPixels(txtLayout->LayoutBounds.Y,rounding);
	int renderW=(int)bitmap->ConvertDipsToPixels(txtLayout->LayoutBounds.Width,rounding);
	int renderH=(int)bitmap->ConvertDipsToPixels(txtLayout->LayoutBounds.Height,rounding);

	wid=renderW;
	hei=renderH;

	return YSOK;
}

////////////////////////////////////////////////////////////

YsSystemFontCache::YsSystemFontCache()
{
	internal=new YsSystemFontCache::InternalData;
	internal->Initialize();
}

YsSystemFontCache::~YsSystemFontCache()
{
	delete internal;
}

YSRESULT YsSystemFontCache::RequestDefaultFont(void)
{
	internal->Initialize();
	return YSERR;
}

YSRESULT YsSystemFontCache::RequestDefaultFontWithHeight(int height)
{
	internal->RequestFontHeight(height);
	return YSOK;
}

YSRESULT YsSystemFontCache::MakeSingleBitBitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],YSBOOL reverse) const
{
	const unsigned char fgCol[3]={255,255,255},bgCol[3]={0,0,0};
	int w,h;
	auto rgba=internal->RenderTextToBitmap(w,h,wStr,fgCol,bgCol);

	const int bitPerPixel=1;
	int bytePerLine=(w+7)&(~7);
	auto *bitArray=new unsigned char [bytePerLine*h];
	for(int y=0; y<h; ++y)
	{
		auto rgbaTop=(YSTRUE!=reverse ? rgba+y*w*4 : rgba+(h-1-y)*4);
		auto bitTop=bitArray+y*bytePerLine;
		unsigned char orBit=0x80;
		for(int x=0; x<w; ++x)
		{
			if(0==x%8)
			{
				bitArray[x/8]=0;
			}
			if(255==rgbaTop[x*4])
			{
				bitTop[x/8]|=(0x80>>(x%8));
			}
		}
	}
	delete [] rgba;
	bmp.SetBitmap(w,h,bytePerLine,bitPerPixel,bitArray);
	return YSOK;
}

YSRESULT YsSystemFontCache::MakeRGBABitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],const unsigned char fgCol[3],const unsigned char bgCol[3],YSBOOL reverse) const
{
	int w,h;
	auto rgba=internal->RenderTextToBitmap(w,h,wStr,fgCol,bgCol);

	if(YSTRUE==reverse)
	{
		for(int y=0; y<h/2; ++y)
		{
			auto lineTop0=rgba+y*w*4;
			auto lineTop1=rgba+(h-1-y)*w*4;
			for(int i=0; i<w*4; ++i)
			{
				auto c=lineTop0[i];
				lineTop0[i]=lineTop1[i];
				lineTop1[i]=c;
			}
		}
	}

	auto bytePerLine=w*4;
	auto bitPerPixel=32;
	bmp.SetBitmap(w,h,bytePerLine,bitPerPixel,rgba);
	return YSOK;
}

YSRESULT YsSystemFontCache::MakeGrayScaleAndAlphaBitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],unsigned char fgColS,unsigned char bgColS,YSBOOL reverse) const
{
	const unsigned char fgCol[3]={fgColS,fgColS,fgColS},bgCol[3]={bgColS,bgColS,bgColS};
	int w,h;
	auto rgba=internal->RenderTextToBitmap(w,h,wStr,fgCol,bgCol);

	const int bytePerLine=2*w;
	const int bitPerPixel=16;

	unsigned char *ra=new unsigned char [bytePerLine*h];
	for(int y=0; y<h; ++y)
	{
		auto rgbaTop=(YSTRUE!=reverse ? rgba+y*4 : rgba+(h-1-y)*4);
		auto raTop=ra+bytePerLine*y;
		for(int x=0; x<w; ++x)
		{
			raTop[x*2  ]=rgbaTop[x*4];
			raTop[x*2+1]=rgbaTop[x*4+3];
		}
	}
	delete [] rgba;

	bmp.SetBitmap(w,h,bytePerLine,bitPerPixel,ra);
	return YSOK;
}

YSRESULT YsSystemFontCache::GetTightBitmapSize(int &wid,int &hei,const wchar_t str[]) const
{
	return internal->GetTightBitmapSize(wid,hei,str);
}
