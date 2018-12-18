/* ////////////////////////////////////////////////////////////

File Name: ysbitmap.h
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

#ifndef YSBITMAP_IS_INCLUDED
#define YSBITMAP_IS_INCLUDED
/* { */

#include <stdio.h>
#include "ysbitmaptemplate.h"

class YsBitmap : public YsBitmapTemplate <unsigned char,4>
{
public:
	using YsBitmapTemplate <unsigned char,4>::CopyFrom;
	using YsBitmapTemplate <unsigned char,4>::MoveFrom;

	YsBitmap();
	~YsBitmap();

	/*! Default copy constructor. */
	YsBitmap(const YsBitmap &incoming);

	/*! Default copy operator. */
	YsBitmap &operator=(const YsBitmap &from);

	/*! Move-assignment constructor. */
	YsBitmap(YsBitmap &&incoming);

	/*! Move-assignment operator. */
	YsBitmap &operator=(YsBitmap &&incoming);

	YSRESULT LoadBmp(const char fn[]);
	/*! fp must be opened with "rb". */
	YSRESULT LoadBmp(FILE *fp);
	YSRESULT LoadPng(const char fn[]);
	/*! fp must be opened with "rb". */
	YSRESULT LoadPng(FILE *fp);

	/*! Load PNG-format data from memory. */
	YSRESULT LoadPng(size_t nByte,const unsigned char binaryData[]);

	/*! Move-assignment operator from a YsRawPngDecoder object. */
	YsBitmap &operator=(class YsRawPngDecoder &&pngDecoder);

	/*! Move bitmap from YsRawPngDecoder. */
	YsBitmap &MoveFrom(class YsRawPngDecoder &pngDecoder);

	/*! Cut out a rectangular part of the bitmap, (x0,y0)-(x0+wid-1,y0+hei-1) and returns it. */
	YsBitmap CutOut(int x0,int y0,int wid,int hei) const;

	/*! Returns a bitmap rotated 90 degrees to the right. */
	YsBitmap Rotate90R(void) const;

	/*! Returns a bitmap rotated 90 degrees to the left. */
	YsBitmap Rotate90L(void) const;

public:
	/*! This function is left for backward-compatibility only.
	    Use Create (YsBitmapTemplate<unsigned char,4>::Create) instead.
	*/
	YSRESULT PrepareBitmap(int wid,int hei);

	/*! Clears the bitmap with the given RGBA values.
	*/
	void Clear(unsigned char r,unsigned char g,unsigned char b,unsigned char a);
	YSRESULT ScaleCopy(int wid,int hei,const YsBitmap &from);
	YSRESULT ScaleCopyRaw(int wid,int hei,const YsBitmap &from);
	YSRESULT Copy(const YsBitmap &from,int x0,int y0);

	/*! Copy from the bitmap incomingBmp starting from (inX0,inY0) to this bitmap from (toX0,toY0) 
	    for the wid pixels horizontally and hei pixels vertically. 
	    This function does not resize this bitmap when the incoming piece of bitmap does not fit
	    within the area of this bitmap. */
	YSRESULT Copy(const YsBitmap &incomingBmp,int inX0,int inY0,int toX0,int toY0,int wid,int hei);

	void NegateRGB(void);
	void NegateRGBA(void);

	YSRESULT InsecureTransfer(const YsBitmap &from,int x0,int y0);  // Incoming bmp must fit in this bmp.
	YSRESULT Transfer(const YsBitmap &from,int x0,int y0,unsigned int flags);
	// Flags:
	//    1: Horizontal Mirror
	//    2: Vertical Mirror
	//    4: Transpose
	//  128: Use Alpha
	YSRESULT VerticalScroll(int dy,unsigned char r,unsigned char g,unsigned char b,unsigned char a);

	YSRESULT SetR(int x,int y,unsigned char r);
	YSRESULT SetG(int x,int y,unsigned char g);
	YSRESULT SetB(int x,int y,unsigned char b);
	YSRESULT SetA(int x,int y,unsigned char a);
	YSRESULT SetRGBA(int x,int y,unsigned char r,unsigned char g,unsigned char b,unsigned char a);

	const unsigned char *GetRGBABitmapPointer(void) const;
	const unsigned char *GetRGBAPixelPointer(int x,int y) const;
	unsigned char *GetEditableRGBABitmapPointer(void);
	unsigned char *GetEditableRGBAPixelPointer(int x,int y);

	YSRESULT SaveBmp(const char fn[]) const;
	YSRESULT SaveBlackAndWhiteBmp(const char fn[],const int thr,YSBOOL invert) const;

	/*! Saves the bitmap in .PNG format.
	    The origin of the .PNG format is bottom-left, while this bitmap top-left.
	    To save it corrctly, use Invert() function before saving.
	    The file must be opened in "wb" mode.
	    Common mistake is ending up with opening "w" mode.  This is good in Unix systems, but breaks in Windows.
	*/
	YSRESULT SavePng(FILE *fp) const;


	/*! Returns true if bitmapB is exactly same as this bitmap. */
	YSBOOL operator==(const YsBitmap &bitmapB) const;

	/*! Returns true if bitmapB is not same as this bitmap. */
	YSBOOL operator!=(const YsBitmap &bitmapB) const;


protected:
	unsigned GetIntelUInt(const unsigned char dat[]);
	unsigned GetIntelUShort(const unsigned char dat[]);
	const unsigned char *GetTopOfLine(int y) const;
};


/* } */
#endif
