/* ////////////////////////////////////////////////////////////

File Name: ysglfontdata.h
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

#ifndef YSGLFONTDATA_IS_INCLUDED
#define YSGLFONTDATA_IS_INCLUDED
/* { */

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char *YsFont6x7[256];
extern unsigned char *YsFont6x8[256];
extern unsigned char *YsFont6x10[256];
extern unsigned char *YsFont7x10[256];
extern unsigned char *YsFont8x8[256];
extern unsigned char *YsFont8x12[256];
extern unsigned char *YsFont10x14[256];
extern unsigned char *YsFont12x16[256];
extern unsigned char *YsFont16x20[256];
extern unsigned char *YsFont16x24[256];
extern unsigned char *YsFont20x28[256];
extern unsigned char *YsFont20x32[256];
extern unsigned char *YsFont24x40[256];
extern unsigned char *YsFont28x44[256];
extern unsigned char *YsFont32x48[256];

extern unsigned char *YsFont12P_Ptn[256];
extern int YsFont12P_Wid[256];
extern unsigned char *YsFont14P_Ptn[256];
extern int YsFont14P_Wid[256];
extern unsigned char *YsFont16P_Ptn[256];
extern int YsFont16P_Wid[256];

void YsGlMakeFontBitmapDisplayList(int listBase,const unsigned char *const fontPtr[],int wid,int hei);
void YsGlDrawFontBitmapDirect(const char str[],const unsigned char *const fontPtr[],int wid,int hei);
void YsGlDrawFontBitmapDirectWithLength(int nChar,const char str[],const unsigned char *const fontPtr[],int wid,int hei);

void YsGlUseFontBitmap6x7(int listBase);
void YsGlDrawFontBitmap6x7(const char str[]);
void YsGlUseFontBitmap6x8(int listBase);
void YsGlDrawFontBitmap6x8(const char str[]);
void YsGlUseFontBitmap6x10(int listBase);
void YsGlDrawFontBitmap6x10(const char str[]);
void YsGlUseFontBitmap7x10(int listBase);
void YsGlDrawFontBitmap7x10(const char str[]);
void YsGlUseFontBitmap8x8(int listBase);
void YsGlDrawFontBitmap8x8(const char str[]);
void YsGlUseFontBitmap8x12(int listBase);
void YsGlDrawFontBitmap8x12(const char str[]);
void YsGlUseFontBitmap10x14(int listBase);
void YsGlDrawFontBitmap10x14(const char str[]);
void YsGlUseFontBitmap12x16(int listBase);
void YsGlDrawFontBitmap12x16(const char str[]);
void YsGlUseFontBitmap16x20(int listBase);
void YsGlDrawFontBitmap16x20(const char str[]);
void YsGlUseFontBitmap16x24(int listBase);
void YsGlDrawFontBitmap16x24(const char str[]);
void YsGlUseFontBitmap20x28(int listBase);
void YsGlDrawFontBitmap20x28(const char str[]);
void YsGlUseFontBitmap20x32(int listBase);
void YsGlDrawFontBitmap20x32(const char str[]);
void YsGlUseFontBitmap24x40(int listBase);
void YsGlDrawFontBitmap24x40(const char str[]);
void YsGlUseFontBitmap28x44(int listBase);
void YsGlDrawFontBitmap28x44(const char str[]);
void YsGlUseFontBitmap32x48(int listBase);
void YsGlDrawFontBitmap32x48(const char str[]);

void YsGlSelectAndUseFontBitmapByHeight(int *selectedWidth,int *selectedHeight,int listBase,int fontHeight);
const unsigned char * const *YsGlSelectFontBitmapPointerByHeight(int *selectedWidth,int *selectedHeight,int fontHeight);

int YsGlWriteStringToSingleBitBitmap(
	const char str[],
    unsigned char bmpPtr[],unsigned int bmpWid,unsigned int bmpHei,
    int bottomLeftX,int bottomLeftY,
    const unsigned char *const fontPtr[],int fontWid,int fontHei);

int YsGlWriteStringToRGBA8Bitmap(
    const char str[],unsigned int c0,unsigned int c1,unsigned int c2,unsigned int c3,
    unsigned char bmpPtr[],unsigned int bmpWid,unsigned int bmpHei,
    int bottomLeftX,int bottomLeftY,
    const unsigned char *const fontPtr[],int fontWid,int fontHei);
/* In OpenGL:   c0=r  c1=g  c2=b  c3=a
   In Direct3D: c0=a  c1=r  c2=g  c3=b */

int YsGlWriteStringToRGBA8BitmapPropFont(
    const char str[],unsigned int c0,unsigned int c1,unsigned int c2,unsigned int c3,
    unsigned char bmpPtr[],unsigned int bmpWid,unsigned int bmpHei,
    int bottomLeftX,int bottomLeftY,
    const unsigned char *const fontPtr[],const int fontWid[],int fontHei);


#ifdef __cplusplus
}
#endif


/* } */
#endif
