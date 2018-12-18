/* ////////////////////////////////////////////////////////////

File Name: ysglusefontbitmap.c
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

#include <ysglheader.h>

#include "ysglfontdata.h"


void YsGlMakeFontBitmapDisplayList(int listBase,const unsigned char *const fontPtr[],int wid,int hei)
{
	int i;
	glRasterPos2i(0,0);
	for(i=0; i<256; i++)
	{
		glNewList(listBase+i,GL_COMPILE);
		glBitmap(wid,hei,0,0,(GLfloat)wid,0,fontPtr[i]);
		glEndList();
	}
}

void YsGlDrawFontBitmapDirect(const char str[],const unsigned char *const fontPtr[],int wid,int hei)
{
	int i;
	for(i=0; 0!=str[i]; i++)
	{
		glBitmap(wid,hei,0,0,(GLfloat)wid,0,fontPtr[((unsigned char *)str)[i]]);
	}
}

void YsGlDrawFontBitmapDirectWithLength(int nChar,const char str[],const unsigned char *const fontPtr[],int wid,int hei)
{
	int i;
	for(i=0; i<nChar; i++)
	{
		glBitmap(wid,hei,0,0,(GLfloat)wid,0,fontPtr[((unsigned char *)str)[i]]);
	}
}



void YsGlUseFontBitmap6x7(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont6x7,6,7);
}
void YsGlDrawFontBitmap6x7(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont6x7,6,7);
}


void YsGlUseFontBitmap6x8(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont6x8,6,8);
}
void YsGlDrawFontBitmap6x8(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont6x8,6,8);
}


void YsGlUseFontBitmap6x10(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont6x10,6,10);
}
void YsGlDrawFontBitmap6x10(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont6x10,6,10);
}


void YsGlUseFontBitmap7x10(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont7x10,7,10);
}
void YsGlDrawFontBitmap7x10(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont7x10,7,10);
}


void YsGlUseFontBitmap8x8(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont8x8,8,8);
}
void YsGlDrawFontBitmap8x8(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont8x8,8,8);
}


void YsGlUseFontBitmap8x12(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont8x12,8,12);
}
void YsGlDrawFontBitmap8x12(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont8x12,8,12);
}


void YsGlUseFontBitmap10x14(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont10x14,10,14);
}
void YsGlDrawFontBitmap10x14(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont10x14,10,14);
}


void YsGlUseFontBitmap12x16(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont12x16,12,16);
}
void YsGlDrawFontBitmap12x16(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont12x16,12,16);
}


void YsGlUseFontBitmap16x20(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont16x20,16,20);
}
void YsGlDrawFontBitmap16x20(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont16x20,16,20);
}


void YsGlUseFontBitmap16x24(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont16x24,16,24);
}
void YsGlDrawFontBitmap16x24(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont16x24,16,24);
}


void YsGlUseFontBitmap20x28(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont20x28,20,28);
}
void YsGlDrawFontBitmap20x28(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont20x28,20,28);
}


void YsGlUseFontBitmap20x32(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont20x32,20,32);
}
void YsGlDrawFontBitmap20x32(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont20x32,20,32);
}


void YsGlUseFontBitmap24x40(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont24x40,24,40);
}
void YsGlDrawFontBitmap24x40(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont24x40,24,40);
}


void YsGlUseFontBitmap32x48(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont32x48,32,48);
}
void YsGlDrawFontBitmap32x48(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont32x48,32,48);
}


void YsGlUseFontBitmap28x44(int listBase)
{
	YsGlMakeFontBitmapDisplayList(listBase,YsFont28x44,28,44);
}
void YsGlDrawFontBitmap28x44(const char str[])
{
	YsGlDrawFontBitmapDirect(str,YsFont28x44,28,44);
}



void YsGlSelectAndUseFontBitmapByHeight(int *selectedWidth,int *selectedHeight,int listBase,int fontHeight)
{
	const unsigned char * const *ptr=YsGlSelectFontBitmapPointerByHeight(selectedWidth,selectedHeight,fontHeight);
	if(NULL!=ptr)
	{
		YsGlMakeFontBitmapDisplayList(listBase,ptr,*selectedWidth,*selectedHeight);
	}
}
