/* ////////////////////////////////////////////////////////////

File Name: refbmp.h
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

#ifndef REFBMP_IS_INCLUDED
#define REFBMP_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysbitmap.h>

class PolyCreReferenceBitmap
{
public:
	YsWString fn;
	YSBOOL loaded;
	YsBitmap bmp;
	YsBitmap scaled;

	int scaleWid,scaleHei,paddedWid,paddedHei;

	YsVec3 corner[4];
	YSBOOL show,alignCenter;
	double alpha;
	mutable YSBOOL textureNamePrepared;
	mutable YSBOOL textureCached;
	mutable unsigned int textureName;

	PolyCreReferenceBitmap();
	~PolyCreReferenceBitmap();
	void Initialize(void);
	YSRESULT Load(const char fn[]);
	void CalculateScalingAndPadding(void);
	void ApplyScalingAndPadding(void);
	void MakeTransparency(void);

	void MakeTentativeCorner(void);
	void MakeXY(void);
	void MakeXZ(void);
	void MakeYZ(void);

	void Draw(void) const;
	void DrawHighlight(void) const;
private:
	void CacheTexture(void) const;
};

class PolyCreReferenceBitmapStore
{
public:
	enum 
	{
		MAX_NUM_REFBMP=16
	};
	PolyCreReferenceBitmap refBmp[MAX_NUM_REFBMP];

private:
	PolyCreReferenceBitmapStore();
	~PolyCreReferenceBitmapStore();

public:
	static PolyCreReferenceBitmapStore *Create(void);
	static void Delete(PolyCreReferenceBitmapStore *ptr);

	void Initialize(void);
	YSRESULT Load(const wchar_t fn[]);
	YSRESULT Save(const wchar_t fn[]) const;

	void Draw(void) const;
	void DrawHighlight(int bmpIdx) const;
};

/* } */
#endif
