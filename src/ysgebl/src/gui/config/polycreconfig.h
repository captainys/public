/* ////////////////////////////////////////////////////////////

File Name: polycreconfig.h
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

#ifndef CONFIG_IS_INCLUDED
#define CONFIG_IS_INCLUDED
/* { */

#include <ysclass.h>

class PolyCreConfig
{
public:
	YSCOORDSYSMODEL coordSys;
	YsWString defDataDir;
	YSBOOL perspective;

	YsColor bgColor[3];
	float specular[3],ambient[3],lightColor[3];

	YSBOOL warnSaveAsSTL;
	YSBOOL saveBinarySTL;

	YSBOOL saveWindowPositionAndSize;

	YSBOOL autoFixOrientationInDnm;

	YSBOOL autoAdjustTolerance;

private:
	PolyCreConfig();
	~PolyCreConfig();
public:
	static PolyCreConfig *Create(void);
	static void Delete(PolyCreConfig *ptr);

	void Initialize(void);
	void SetDefault(void);
	const float *GetSpecularColor(void) const;
	void SetSpecularColor(const float specular[3]);
	const float *GetAmbientColor(void) const;
	void SetAmbientColor(const float ambient[3]);
	const float *GetLightColor(void) const;
	void SetLightColor(const float lightCol[3]);
	YSBOOL GetWarnSaveAsSTL(void) const;
	void SetWarnSaveAsSTL(YSBOOL warn);

	YSBOOL SaveBinarySTL(void) const;
	void SetSaveBinarySTL(YSBOOL binStl);

	YSRESULT Load(YsTextInputStream &inStream);
	YSRESULT Load(const wchar_t fn[]);
	YSRESULT Save(YsTextOutputStream &outStream) const;
	YSRESULT Save(const wchar_t fn[]);
};


/* } */
#endif
