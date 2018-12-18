/* ////////////////////////////////////////////////////////////

File Name: yswin32systemfont.cpp
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
#include "../yssystemfont.h"


// The following block is needed for SetDCPenColor >>
#if defined(_WIN32_WINNT) && _WIN32_WINNT<0x0500
#undef _WIN32_WINNT
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
// The following block is needed for SetDCPenColor <<


#include <windows.h>

class YsSystemFontInternalBitmapCache
{
public:
	HBITMAP hBmp;
	unsigned int wid,hei;
	void *bit;
	int bitPerPixel;

	YsSystemFontInternalBitmapCache();
};

YsSystemFontInternalBitmapCache::YsSystemFontInternalBitmapCache()
{
	hBmp=NULL;
	wid=0;
	hei=0;
	bit=NULL;
	bitPerPixel=0;
}

class YsSystemFontCache::InternalData
{
private:
	HDC hDC;
	HGDIOBJ hPrevFont,hPrevBmp;
	YsSystemFontInternalBitmapCache hBmpCache;

public:
	InternalData();
	~InternalData();
	void SetHFont(HFONT hFont);
	void FreeFont(void);

	unsigned char *RGBABitmap(
	    unsigned int &wid,unsigned int &hei,unsigned int &bytePerLine,
	    const wchar_t str[],unsigned int outputBitPerPixel,
	    const unsigned char fgCol[3],const unsigned char bgCol[3],
	    YSBOOL reverse);

	YSRESULT GetTightBitmapSize(int &wid,int &hei,const wchar_t str[]) const;

private:
	HBITMAP CreateDIB(void *&bit,int &srcBitPerPixel,unsigned int wid,unsigned int hei);
};

YsSystemFontCache::InternalData::InternalData()
{
	hDC=CreateCompatibleDC(NULL);
	SetTextAlign(hDC,TA_LEFT|TA_TOP|TA_NOUPDATECP);
	hPrevFont=NULL;

	const unsigned int cacheWid=640;
	const unsigned int cacheHei=120;
	void *bit;
	int bitPerPixel;
	hBmpCache.hBmp=CreateDIB(bit,bitPerPixel,cacheWid,cacheHei);
	if(NULL!=hBmpCache.hBmp)
	{
		hBmpCache.wid=cacheWid;
		hBmpCache.hei=cacheHei;
		hBmpCache.bit=bit;
		hBmpCache.bitPerPixel=bitPerPixel;
		hPrevBmp=SelectObject(hDC,hBmpCache.hBmp);
	}
	else
	{
		hPrevBmp=NULL;
	}
}

YsSystemFontCache::InternalData::~InternalData()
{
	FreeFont();
	if(NULL!=hBmpCache.hBmp)
	{
		DeleteObject(hBmpCache.hBmp);
	}
	if(NULL!=hDC)
	{
		DeleteDC(hDC);
	}
}

HBITMAP YsSystemFontCache::InternalData::CreateDIB(void *&bit,int &srcBitPerPixel,unsigned int wid,unsigned int hei)
{
	YSBOOL reverse=YSFALSE;
	srcBitPerPixel=24;

	BITMAPINFOHEADER hdr=
	{
		sizeof(BITMAPINFOHEADER),
		0,0,1,8,BI_RGB,0,0,0,0,0
	};
	hdr.biWidth=wid;
	if(YSTRUE!=reverse)
	{
		hdr.biHeight=-(int)hei;
	}
	else
	{
		hdr.biHeight=hei;
	}
	hdr.biBitCount=(WORD)srcBitPerPixel;
	hdr.biCompression=BI_RGB;

	return CreateDIBSection(hDC,(BITMAPINFO *)&hdr,DIB_RGB_COLORS,&bit,NULL,0);
}

void YsSystemFontCache::InternalData::SetHFont(HFONT hFont)
{
	if(NULL!=hPrevFont)
	{
		FreeFont();
	}
	hPrevFont=SelectObject(hDC,hFont);
}

void YsSystemFontCache::InternalData::FreeFont(void)
{
	if(NULL!=hPrevFont)
	{
		HGDIOBJ hFontToDel=SelectObject(hDC,hPrevFont);
		DeleteObject(hFontToDel);
		hPrevFont=NULL;
	}
}

unsigned char *YsSystemFontCache::InternalData::RGBABitmap(
    unsigned int &wid,unsigned int &hei,unsigned int &dstBytePerLine,
    const wchar_t str[],unsigned int dstBitPerPixel,
    const unsigned char fgCol[3],const unsigned char bgCol[3],
    YSBOOL reverse)
{
	if(NULL==str || (1!=dstBitPerPixel && 16!=dstBitPerPixel && 32!=dstBitPerPixel))
	{
		// Currently 16bit:GrayScale+Alpha and 32bit:TrueColor+Alpha only.
		wid=0;
		hei=0;
		return NULL;
	}

	wid=0;
	hei=0;

	int i=0,i0=0;
	for(;;)
	{
		if(0==str[i] || '\n'==str[i])
		{
			RECT rect;
			rect.left=0;
			rect.top=0;
			rect.right=100;
			rect.bottom=100;
			if(i0<i)
			{
				DrawTextExW(hDC,(wchar_t *)str+i0,i-i0,&rect,DT_TOP|DT_LEFT|DT_CALCRECT,NULL);
			}
			else
			{
				DrawTextExW(hDC,L" ",1,&rect,DT_TOP|DT_LEFT|DT_CALCRECT,NULL);
			}

			if(wid<(unsigned int)rect.right)
			{
				wid=rect.right;
			}
			hei+=rect.bottom;

			i0=i+1;
		}

		if(0==str[i])
		{
			break;
		}
		i++;
	}

	wid=((wid+3)/4)*4;

	int srcBitPerPixel;
	void *bit;
	HBITMAP hBmp=NULL;
	HGDIOBJ hPrevBmp=NULL;
	int srcWid=0;
	if(NULL!=hBmpCache.hBmp && wid<=hBmpCache.wid && hei<=hBmpCache.hei)
	{
		hBmp=hBmpCache.hBmp;
		srcWid=hBmpCache.wid;
		srcBitPerPixel=hBmpCache.bitPerPixel;
		bit=hBmpCache.bit;
		hPrevBmp=NULL;
	}
	else
	{
		hBmp=CreateDIB(bit,srcBitPerPixel,wid,hei);
		srcWid=wid;
		hPrevBmp=SelectObject(hDC,hBmp);
	}

	int srcBytePerLine=((srcWid+3)/4)*4*srcBitPerPixel/8;  // Needs to be signed.

	ZeroMemory(bit,srcBytePerLine*hei);

	SetTextColor(hDC,RGB(255,255,255));
	SetDCPenColor(hDC,RGB(255,255,255));
	SetBkColor(hDC,RGB(0,0,0));
	SetBkMode(hDC,OPAQUE);


	RECT rect;
	rect.left=0;
	rect.top=0;
	rect.right=wid;
	rect.bottom=hei;

	i=0;
	i0=0;
	for(;;)
	{
		if(0==str[i] || '\n'==str[i])
		{
			int hei;
			if(i0<i)
			{
				hei=DrawTextExW(hDC,(wchar_t *)str+i0,i-i0,&rect,DT_TOP|DT_LEFT,NULL);
			}
			else
			{
				hei=DrawTextExW(hDC,L" ",1,&rect,DT_TOP|DT_LEFT,NULL);
			}
			rect.top+=hei;
			rect.bottom+=hei;
			i0=i+1;
		}

		if(0==str[i])
		{
			break;
		}
		i++;
	}


	unsigned char *retBuf=NULL;
	switch(dstBitPerPixel)
	{
	case 1:
		dstBytePerLine=((wid+31)&~31)/8;
		break;
	case 16:
	case 32:
		dstBytePerLine=wid*dstBitPerPixel/8;
		break;
	}

	retBuf=new unsigned char [dstBytePerLine*hei];
	if(1==dstBitPerPixel)
	{
		for(int i=0; i<(int)(dstBytePerLine*hei); i++)
		{
			retBuf[i]=0;
		}
	}

	unsigned char *srcLineTop=(unsigned char *)bit,*dstLineTop=retBuf;

	if(YSTRUE==reverse)
	{
		srcLineTop+=(hei-1)*srcBytePerLine;
		srcBytePerLine=-srcBytePerLine;
	}

	for(unsigned int y=0; y<hei; y++)
	{
		unsigned char *srcPtr=srcLineTop;

		unsigned char *dstPtr=dstLineTop;
		unsigned char dstMask=0x80;

		switch(dstBitPerPixel)
		{
		case 1:
			for(int x=0; (unsigned int)x<wid; x++,srcPtr+=3)
			{
				if(128<*srcPtr)
				{
					(*dstPtr)|=dstMask;
				}
				if(1==dstMask)
				{
					dstMask=0x80;
					dstPtr++;
				}
				else
				{
					dstMask>>=1;
				}
			}
			break;
		case 16:
			for(int x=0; (unsigned int)x<wid; x++,srcPtr+=3)
			{
				if(128<*srcPtr)
				{
					dstPtr[0]=fgCol[0];
					dstPtr[1]=255;
				}
				else
				{
					dstPtr[0]=bgCol[0];
					dstPtr[1]=0;
				}
				dstPtr+=2;
			}
			break;
		case 32:
			for(int x=0; (unsigned int)x<wid; x++,srcPtr+=3)
			{
				if(128<*srcPtr)
				{
					dstPtr[0]=fgCol[0];
					dstPtr[1]=fgCol[1];
					dstPtr[2]=fgCol[2];
					dstPtr[3]=255;
				}
				else
				{
					dstPtr[0]=bgCol[0];
					dstPtr[1]=bgCol[1];
					dstPtr[2]=bgCol[2];
					dstPtr[3]=0;
				}
				dstPtr+=4;
			}
			break;
		}

		dstLineTop+=dstBytePerLine;
		srcLineTop+=srcBytePerLine;
	}


	if(NULL!=hPrevBmp)
	{
		SelectObject(hDC,hPrevBmp);
	}
	if(hBmpCache.hBmp!=hBmp)
	{
		DeleteObject(hBmp);
	}

	return retBuf;
}

YSRESULT YsSystemFontCache::InternalData::GetTightBitmapSize(int &wid,int &hei,const wchar_t str[]) const
{
	if(NULL==str || 0==str[0])
	{
		wid=0;
		hei=0;
		return YSOK;
	}

	wid=0;
	hei=0;

	int i=0,i0=0;
	for(;;)
	{
		if(0==str[i] || '\n'==str[i])
		{
			RECT rect;
			rect.left=0;
			rect.top=0;
			rect.right=100;
			rect.bottom=100;
			if(i0<i)
			{
				DrawTextExW(hDC,(wchar_t *)str+i0,i-i0,&rect,DT_TOP|DT_LEFT|DT_CALCRECT,NULL);
			}
			else
			{
				DrawTextExW(hDC,L" ",1,&rect,DT_TOP|DT_LEFT|DT_CALCRECT,NULL);
			}

			if(wid<(int)rect.right)
			{
				wid=rect.right;
			}
			hei+=rect.bottom;

			i0=i+1;
		}

		if(0==str[i])
		{
			break;
		}
		i++;
	}

	return YSOK;
}


YsSystemFontCache::YsSystemFontCache()
{
	internal=new YsSystemFontCache::InternalData;
}

YsSystemFontCache::~YsSystemFontCache()
{
	delete internal;
}

YSRESULT YsSystemFontCache::RequestDefaultFont(void)
{
	HFONT hFont=NULL;

	NONCLIENTMETRICSW metric;
	metric.cbSize=sizeof(metric);

	if(0!=SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,sizeof(metric),&metric,0))
	{
		hFont=CreateFontIndirectW(&metric.lfMenuFont);
	}
	else
	{
		HFONT hSysFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
		LOGFONTW logFontW;
		if(NULL!=hSysFont && 0<GetObjectW(hSysFont,sizeof(logFontW),&logFontW))
		{
			// Making a copy of the system font.  Stock object is not supposed to be deleted later.
			hFont=CreateFontIndirectW(&logFontW);
		}
	}

	if(NULL!=hFont)
	{
		internal->SetHFont(hFont);
		return YSOK;
	}

	return YSERR;
}

YSRESULT YsSystemFontCache::RequestDefaultFontWithHeight(int height)
{
	HFONT hFont=NULL;

	NONCLIENTMETRICSW metric;
	metric.cbSize=sizeof(metric);

	hFont=CreateFont(
	    -height,
	    0, // nWidth auto select
		0, // nEscapement
		0, // nOrientation
		FW_DONTCARE, // fnWeight
 		FALSE, // fdwItalic
		FALSE, // fdwUnderline
		FALSE, // fdwStrikeOut
		DEFAULT_CHARSET, // fdwCharSet
		OUT_DEFAULT_PRECIS, // fdwOutputPrecision
		CLIP_DEFAULT_PRECIS, // fdwClipPrecision
		DEFAULT_QUALITY, // WORD fdwQuality
		DEFAULT_PITCH, // fdwPitchAndFamily
		NULL); // lpszFace Use whatever found first.

	if(NULL==hFont)
	{
		if(0!=SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,sizeof(metric),&metric,0))
		{
			LOGFONTW logFontW=metric.lfMenuFont;
			logFontW.lfHeight=-height;

			hFont=CreateFontIndirectW(&logFontW);
		}
	}

	if(NULL==hFont)
	{
		HFONT hSysFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
		LOGFONTW logFontW;
		if(NULL!=hSysFont && 0<GetObjectW(hSysFont,sizeof(logFontW),&logFontW))
		{
			// Making a copy of the system font.  Stock object is not supposed to be deleted later.
			logFontW.lfHeight=-height;
			hFont=CreateFontIndirectW(&logFontW);
		}
	}

	if(NULL!=hFont)
	{
		internal->SetHFont(hFont);
		return YSOK;
	}

	return YSERR;
}

YSRESULT YsSystemFontCache::MakeSingleBitBitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],YSBOOL reverse) const
{
	unsigned int wid,hei,bytePerLine;
	const unsigned int bitPerPixel=1;
	const unsigned char fgCol[3]={255,255,255},bgCol[3]={0,0,0};

	unsigned char *dat=internal->RGBABitmap(wid,hei,bytePerLine,wStr,bitPerPixel,fgCol,bgCol,reverse);
	if(NULL!=dat)
	{
		bmp.SetBitmap(wid,hei,bytePerLine,bitPerPixel,dat);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSystemFontCache::MakeRGBABitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],const unsigned char fgCol[3],const unsigned char bgCol[3],YSBOOL reverse) const
{
	unsigned int wid,hei,bytePerLine;
	const unsigned int bitPerPixel=32;

	unsigned char *dat=internal->RGBABitmap(wid,hei,bytePerLine,wStr,bitPerPixel,fgCol,bgCol,reverse);
	if(NULL!=dat)
	{
		bmp.SetBitmap(wid,hei,bytePerLine,bitPerPixel,dat);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSystemFontCache::MakeGrayScaleAndAlphaBitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],unsigned char fgColS,unsigned char bgColS,YSBOOL reverse) const
{
	const unsigned char fgCol[3]={fgColS,fgColS,fgColS},bgCol[3]={bgColS,bgColS,bgColS};

	unsigned int wid,hei,bytePerLine;
	const unsigned int bitPerPixel=16;

	unsigned char *dat=internal->RGBABitmap(wid,hei,bytePerLine,wStr,bitPerPixel,fgCol,bgCol,reverse);
	if(NULL!=dat)
	{
		bmp.SetBitmap(wid,hei,bytePerLine,bitPerPixel,dat);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSystemFontCache::GetTightBitmapSize(int &wid,int &hei,const wchar_t str[]) const
{
	return internal->GetTightBitmapSize(wid,hei,str);
}
