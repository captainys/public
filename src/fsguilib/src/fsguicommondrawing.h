/* ////////////////////////////////////////////////////////////

File Name: fsguicommondrawing.h
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

#ifndef FSGUICOMMONDRAWING_IS_INCLUDED
#define FSGUICOMMONDRAWING_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysbitmap.h>
#include <ysbitmaparray.h>

class FsGuiCommonDrawing
{
public:
	static void DrawLine(double x1,double y1,double x2,double y2,const YsColor &col);
	static void DrawLineStrip(int n,const double plg[],const YsColor &col);
	static void DrawRect(double x1,double y1,double x2,double y2,const YsColor &col,YSBOOL fill);
	static void DrawCircle(double x,double y,double rad,const YsColor &col,YSBOOL fill);
	static void DrawPolygon(int n,const double plg[],const YsColor &col);
	static void DrawGradationPolygon(int n,const double plg[],const YsColor col[]);
	static void DrawBmp(const YsBitmap &bmp,double x,double y);
	static void DrawBmpArray(const YsBitmapArray &bmp,double x,double y);
	static void DrawMask(const YsColor &fgCol,const YsColor &bgCol,double x0,double y0,double wid,double hei);
	static void SetClipRect(double x0,double y0,double wid,double hei);
	static void ClearClipRect(void);
};

/* } */
#endif
