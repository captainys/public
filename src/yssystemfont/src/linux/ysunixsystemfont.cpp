/* ////////////////////////////////////////////////////////////

File Name: ysunixsystemfont.cpp
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
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>

#include "../yssystemfont.h"

class YsSystemFontCache::InternalData
{
public:
	Display *dsp;
	Window rootWin;
	XFontSet fontSet;

	InternalData();
	~InternalData();
	void CleanUp(void);
	XFontSet MakeFontSet(int height);
	YSRESULT SetFontByHeight(int height);

	unsigned char *RGBABitmap(
		unsigned int &wid,unsigned int &hei,unsigned int &bytePerLine,
		const wchar_t str[],unsigned int outputBitPerPixel,
	    const unsigned char fgCol[3],const unsigned char bgCol[3],
	    YSBOOL reverse);

	YSRESULT GetTightBitmapSize(int &wid,int &hei,const wchar_t str[]) const;
};

YsSystemFontCache::InternalData::InternalData()
{
	const char *localeStr=setlocale(LC_CTYPE,"");
	if(NULL==localeStr || True!=XSupportsLocale())
	{
		setlocale(LC_CTYPE,"en_US.UTF-8");  // Default to en_US
	}

	dsp=NULL;
	rootWin=NULL;
	fontSet=NULL;

	dsp=XOpenDisplay(NULL);
	if(NULL!=dsp)
	{
		rootWin=DefaultRootWindow(dsp);
		fontSet=MakeFontSet(16);
	}
}

YsSystemFontCache::InternalData::~InternalData()
{
	if(NULL!=dsp)
	{
		CleanUp();
		XCloseDisplay(dsp);
	}
}

void YsSystemFontCache::InternalData::CleanUp(void)
{
	if(NULL!=fontSet)
	{
		XFreeFontSet(dsp,fontSet);
		fontSet=NULL;
	}
}

XFontSet YsSystemFontCache::InternalData::MakeFontSet(int height)
{
	if(NULL!=dsp)
	{
		const char *fontNameFmt=
			"-*-*-*-*-*-*-%d-*-*-*-*-*-iso8859-1,"
			"-*-*-*-*-*-*-%d-*-*-*-*-*-iso8859-2,"
			"-*-*-*-*-*-*-%d-*-*-*-*-*-iso8859-4,"
			"-*-*-*-*-*-*-%d-*-*-*-*-*-iso8859-5,"
			"-*-*-*-*-*-*-%d-*-*-*-*-*-iso8859-6,"
			"-*-*-*-*-*-*-%d-*-*-*-*-*-iso8859-7,"
			"-*-*-*-*-*-*-%d-*-*-*-*-*-iso8859-8,"
			"-*-*-*-*-*-*-%d-*-*-*-*-*-iso8859-9,"
			"-*-*-*-*-*-*-%d-*-*-*-*-*-iso8859-15,"
			"-*-*-*-*-*-*-%d-*-*-*-*-*-iso10646-1";
		char fontNameBuf[1024];
		snprintf(fontNameBuf,1023,fontNameFmt,
				 height,
				 height,
				 height,
				 height,
				 height,
				 height,
				 height,
				 height,
				 height,
				 height);
		fontNameBuf[1023]=0;
		
		int missSetCount;
		char *defStr,**missSet;
		return XCreateFontSet(dsp,fontNameBuf,&missSet,&missSetCount,&defStr);
	}
	return NULL;
}

YSRESULT YsSystemFontCache::InternalData::SetFontByHeight(int height)
{
	if(NULL!=dsp)
	{
		XFontSet newFontSet=MakeFontSet(height);
		if(NULL!=newFontSet)
		{
			CleanUp();
			fontSet=newFontSet;
			return YSOK;
		}
	}
	return YSERR;
}

unsigned char *YsSystemFontCache::InternalData::RGBABitmap(
	unsigned int &wid,unsigned int &hei,unsigned int &dstBytePerLine,
	const wchar_t str[],unsigned int dstBitPerPixel,
	const unsigned char fgCol[3],const unsigned char bgCol[3],
	YSBOOL reverse)
{
	if(NULL==str || 0==str[0] || (1!=dstBitPerPixel && 16!=dstBitPerPixel && 32!=dstBitPerPixel))
	{
		wid=0;
		hei=0;
		return NULL;
	}

	if(NULL!=dsp && NULL!=fontSet)
	{
		wid=0;
		hei=0;
		
		int i=0,i0=0;
		for(;;)
		{
			if(0==str[i] || '\n'==str[i])
			{
				XRectangle inkRect,logRect;
				const int ext=XwcTextExtents(fontSet,str+i0,i-i0,&inkRect,&logRect);
				
				if(wid<ext)
				{
					wid=ext;
				}
				hei+=logRect.height;
				i0=i+1;
			}
			if(0==str[i])
			{
				break;
			}
			i++;
		}
		
		wid=((wid+3)/4*4);
		if(0>=wid || 0>=hei)
		{
			wid=0;
			hei=0;
			return NULL;
		}

		const int defDepth=DefaultDepth(dsp,0);
		Pixmap pixmap=XCreatePixmap(dsp,rootWin,wid,hei,defDepth);
		GC gc;
		
		if(NULL!=(gc=XCreateGC(dsp,pixmap,0,0)))
		{
			XSetForeground(dsp,gc,BlackPixel(dsp,0));
			XFillRectangle(dsp,pixmap,gc,0,0,wid,hei);
		
			XSetForeground(dsp,gc,WhitePixel(dsp,0));
			int y=0;
			i=0;
			i0=0;
			for(;;)
			{
				if(0==str[i] || '\n'==str[i])
				{
					XRectangle inkRect,logRect;
					const int ext=XwcTextExtents(fontSet,str+i0,i-i0,&inkRect,&logRect);
					XwcDrawString(dsp,pixmap,fontSet,gc,0,y+logRect.height-1,str+i0,i-i0);

					y+=logRect.height;
					i0=i+1;
				}
				if(0==str[i])
				{
					break;
				}
				i++;
			}


			unsigned char *retBuf=NULL;
			XImage *image=XGetImage(dsp,pixmap,0,0,wid,hei,~0,XYPixmap);
			if(NULL!=image)
			{
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

				const size_t bufSize=dstBytePerLine*hei;
				retBuf=new unsigned char [bufSize];

				if(1==dstBitPerPixel)
				{
					int i;
					for(i=0; i<bufSize; i++)
					{
						retBuf[i]=0;
					}
				}

				int x,y;
				unsigned char *dstLineTop=retBuf;
				for(y=0; y<image->height; y++)
				{
					const int yInImage=(YSTRUE!=reverse ? y : image->height-1-y);
					unsigned char srcMask=1;
					unsigned char *srcLineTop=(unsigned char *)image->data+yInImage*image->bytes_per_line;

					unsigned char dstMask=0x80;
					unsigned char *dstPtr=dstLineTop;

					for(x=0; x<image->width; x++)
					{
						const unsigned char *col;
						unsigned char alpha;
						if(0!=((*srcLineTop)&srcMask))
						{
							col=fgCol;
							alpha=255;
						}
						else
						{
							col=bgCol;
							alpha=0;
						}

						switch(dstBitPerPixel)
						{
						case 1:
							if(255==alpha)
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
							break;
						case 16:
							dstLineTop[x*2  ]=col[0];
							dstLineTop[x*2+1]=alpha;
							break;
						case 32:
							dstLineTop[x*4  ]=col[0];
							dstLineTop[x*4+1]=col[1];
							dstLineTop[x*4+2]=col[2];
							dstLineTop[x*4+3]=alpha;
							break;
						}
						if(0x80==srcMask)
						{
							srcMask=1;
							srcLineTop++;
						}
						else
						{
							srcMask<<=1;
						}
					}
					for(x=x; x<wid; x++)
					{
						switch(dstBitPerPixel)
						{
						case 16:
							dstLineTop[x*2  ]=bgCol[0];
							dstLineTop[x*2+1]=0;
							break;
						case 32:
							dstLineTop[x*4  ]=bgCol[0];
							dstLineTop[x*4+1]=bgCol[1];
							dstLineTop[x*4+2]=bgCol[2];
							dstLineTop[x*4+3]=0;
							break;
						}
					}
					dstLineTop+=dstBytePerLine;
				}
			}

			XFreeGC(dsp,gc);
			XFreePixmap(dsp,pixmap);
			return retBuf;
		}
	}

	return NULL;
}

YSRESULT YsSystemFontCache::InternalData::GetTightBitmapSize(int &wid,int &hei,const wchar_t str[]) const
{
	if(NULL==str || 0==str[0])
	{
		wid=0;
		hei=0;
		return YSOK;
	}

	if(NULL!=dsp && NULL!=fontSet)
	{
		wid=0;
		hei=0;

		int i=0,i0=0;
		for(;;)
		{
			if(0==str[i] || '\n'==str[i])
			{
				XRectangle inkRect,logRect;
				const int ext=XwcTextExtents(fontSet,str+i0,i-i0,&inkRect,&logRect);
				
				if(wid<ext)
				{
					wid=ext;
				}
				hei+=logRect.height;
				i0=i+1;
			}
			if(0==str[i])
			{
				break;
			}
			i++;
		}

		if(0>=wid || 0>=hei)
		{
			wid=0;
			hei=0;
			return YSERR;
		}
	}
	return YSOK;
}

YsSystemFontCache::YsSystemFontCache()
{
	internal=new InternalData;
}

YsSystemFontCache::~YsSystemFontCache()
{
	delete internal;
}

YSRESULT YsSystemFontCache::RequestDefaultFont(void)
{
	return internal->SetFontByHeight(16);
}

YSRESULT YsSystemFontCache::RequestDefaultFontWithHeight(int height)
{
	return internal->SetFontByHeight(height);
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
