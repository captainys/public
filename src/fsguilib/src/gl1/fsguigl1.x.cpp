/* ////////////////////////////////////////////////////////////

File Name: fsguigl1.x.cpp
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
#include <string.h>
#include <stddef.h>
#include <time.h>

#include <ysclass.h>

#ifdef _WIN32
	#ifndef _WINSOCKAPI_
		// Prevent inclusion of winsock.h
		#define _WINSOCKAPI_
		#include <windows.h>
		#undef _WINSOCKAPI_
	#else
		// Too late.  Just include it.
		#include <windows.h>
	#endif
#endif

#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif


#include <ysbitmaparray.h>
#include "fsgui.h"
#include "fsguicommondrawing.h"


void FsGuiCommonDrawing::DrawLine(double x1,double y1,double x2,double y2,const YsColor &col)
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	glColor4f(col.Rf(),col.Gf(),col.Bf(),col.Af());
	glBegin(GL_LINES);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glEnd();

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawLineStrip(int n,const double  plg[],const YsColor &col)
{
	glColor4f(col.Rf(),col.Gf(),col.Bf(),col.Af());
	glBegin(GL_LINE_STRIP);
	for(int i=0; i<n; ++i)
	{
		glVertex2d(plg[i*2],plg[i*2+1]);
	}
	glEnd();
}

void FsGuiCommonDrawing::DrawRect(double x1,double y1,double x2,double y2,const YsColor &col,YSBOOL fill)
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	glColor4f(col.Rf(),col.Gf(),col.Bf(),col.Af());
	if(fill==YSTRUE)
	{
		glBegin(GL_QUADS);
		glVertex2d(x1,y1);
		glVertex2d(x2,y1);
		glVertex2d(x2,y2);
		glVertex2d(x1,y2);
		glEnd();

		glBegin(GL_LINE_LOOP);  // <- This may be needed due to OpenGL driver's implementation error.
		glVertex2d(x1,y1);
		glVertex2d(x2,y1);
		glVertex2d(x2,y2);
		glVertex2d(x1,y2);
		glVertex2d(x1,y1);  // Need to wrap around, or it will miss the last point.
		glEnd();
	}
	else
	{
		glBegin(GL_LINE_LOOP);
		glVertex2d(x1,y1);
		glVertex2d(x2,y1);
		glVertex2d(x2,y2);
		glVertex2d(x1,y2);
		glVertex2d(x1,y1);  // Need to wrap around, or it will miss the last point.
		glEnd();
	}

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawCircle(double x,double y,double rad,const YsColor &col,YSBOOL fill)
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	static YsArray <YsVec2> circle;

	glColor4f(col.Rf(),col.Gf(),col.Bf(),col.Af());

	if(circle.GetN()<65)
	{
		circle.Set(65,NULL);
		for(int i=0; i<=64; i++)
		{
			const double a=YsPi*((double)i)/32.0;
			circle[i].Set(cos(a),sin(a));
		}
	}

	glPushMatrix();
	glTranslatef((float)x,(float)y,(float)0);
	glScalef((float)rad,(float)rad,(float)rad);

	if(fill==YSTRUE)
	{
		glBegin(GL_POLYGON);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}

	for(int i=0; i<64; i++)
	{
		glVertex2dv(circle[i]);
	}

	glEnd();
	glPopMatrix();

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawPolygon(int n,const double plg[],const YsColor &col)
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	glColor4f(col.Rf(),col.Gf(),col.Bf(),col.Af());
	glBegin(GL_POLYGON);
	for(int i=0; i<n; i++)
	{
		glVertex2d(plg[i*2],plg[i*2+1]);
	}
	glEnd();

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawGradationPolygon(int n,const double plg[],const YsColor col[])
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	glBegin(GL_POLYGON);
	for(int i=0; i<n; i++)
	{
		glColor4f(col[i].Rf(),col[i].Gf(),col[i].Bf(),col[i].Af());
		glVertex2d(plg[i*2],plg[i*2+1]);
	}
	glEnd();

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawBmp(const YsBitmap &bmp,double x,double y)
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	int bmpHei,bmpWid;
	bmpWid=bmp.GetWidth();
	bmpHei=bmp.GetHeight();

	glRasterPos2d(x,y+(double)(bmpHei-1));
	glDrawPixels(bmpWid,bmpHei,GL_RGBA,GL_UNSIGNED_BYTE,bmp.GetRGBABitmapPointer());

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawBmpArray(const YsBitmapArray &bmpAry,double x0,double y0)
{
	// 20 21 22  <- Some blank above.  Height of this row may be lower than other rows.
	// 10 11 12
	// 00 01 02  <- Packed from lower left.

	int lowY=(int)y0+bmpAry.GetTotalHeight();
	for(int i=0; i<(int)bmpAry.GetNumBlockY(); ++i)
	{
		int nextLowY=lowY;
		for(int j=0; j<(int)bmpAry.GetNumBlockX(); ++j)
		{
			const YsBitmap *bmp=bmpAry.GetTileBitmap(j,i);
			if(NULL!=bmp)
			{
				DrawBmp(*bmp,(int)x0+j*(int)bmpAry.GetTileWidth(),lowY-(int)bmp->GetHeight());
			}
			nextLowY=lowY-(int)bmp->GetHeight();
		}
		lowY=nextLowY;
	}
}

void FsGuiCommonDrawing::DrawMask(const YsColor &fgCol,const YsColor & /*bgCol*/,double x0,double y0,double wid,double hei)
{
	if(fgCol.Ri()>200 && fgCol.Gi()>200 && fgCol.Bi()>200)
	{
		glColor4f(0.2f,0.2f,0.2f,0.8f);
	}
	else
	{
		glColor4f(0.8f,0.8f,0.8f,0.8f);
	}

	glBegin(GL_QUADS);
	glVertex2d(x0    ,y0);
	glVertex2d(x0+wid,y0);
	glVertex2d(x0+wid,y0+hei);
	glVertex2d(x0    ,y0+hei);
	glEnd();
}

void FsGuiCommonDrawing::SetClipRect(double x0,double y0,double wid,double hei)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);

	glScissor((int)x0,viewport[3]-(int)y0-(int)hei,(int)wid,(int)hei+1);

	glEnable(GL_SCISSOR_TEST);
}

void FsGuiCommonDrawing::ClearClipRect(void)
{
	glDisable(GL_SCISSOR_TEST);
}

