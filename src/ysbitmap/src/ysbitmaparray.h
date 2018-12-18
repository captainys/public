/* ////////////////////////////////////////////////////////////

File Name: ysbitmaparray.h
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

#ifndef FSGUIBITMAP_IS_INCLUDED
#define FSGUIBITMAP_IS_INCLUDED
/* { */

#include "ysbitmap.h"

/*! This class makes an array of bitmap.  Can be used for breaking down a large bitmap into 
    pieces of bitmaps that each can be drawn by a poor-man's graphics toolkit such as OpenGL ES.
    Typical OpenGL ES cannot draw any bitmap greater than 64x64, and the size need to be 2^n times 2^m.

	Note that the tile in the last row or last column may not have a same size as the other tiles.
	For example, totalWidth is 260 and the maxWidth is 16, the width of the last tile will be 4.
	Size will always be 2^n.
    */
class YsBitmapArray
{
private:
	int totalWid,totalHei;
	int tileWid,tileHei;
	int nDivX,nDivY;
	YsBitmap *bmpArray;

public:
	/*! Default constructor. */
	YsBitmapArray();

	/*! Copy constructor .*/
	YsBitmapArray(const YsBitmapArray &from);

	/*! Copy operator. */
	const YsBitmapArray &operator=(const YsBitmapArray &from);

private:
	void CopyFrom(const YsBitmapArray &from);

public:
	/*! Default destructor. */
	~YsBitmapArray();

	/*! Initializes the bitmap array. */
	void CleanUp(void);

	/*! Copys a bitmap to this bitmap array. The bitmap will be divided up into an array of bitmaps.
	    One piece of a bitmap will be 2^n times 2^m pixels that does not exceed maxWidth, maxHeight. */
	YSRESULT SetAndBreakUp(const YsBitmap &from,int maxWidth,int maxHeight);

private:
	void ResizeArray(int n);

public:
	/*! Returns the total width of the bitmap. */
	int GetTotalWidth(void) const;

	/*! Returns the total height of the bitmap. */
	int GetTotalHeight(void) const;

	/*! Returns number of pieces of bitmap in X direction */
	int GetNumBlockX(void) const;

	/*! Returns number of pieces of bitmap in Y direction */
	int GetNumBlockY(void) const;

	/*! Returns width of a tile in number of pixels */
	int GetTileWidth(void) const;

	/*! Returns height of a tile in number of pixels */
	int GetTileHeight(void) const;

	/*! Returns a constant pointer to a tile bitmap.  If x or y is not in range,
	    it will return NULL. */
	const YsBitmap *GetTileBitmap(int x,int y) const;

	/*! Returns a pointer to a tile bitmap.  If x or y is not in range,
	    it will return NULL. */
	YsBitmap *GetTileBitmap(int x,int y);
};

/* } */
#endif
