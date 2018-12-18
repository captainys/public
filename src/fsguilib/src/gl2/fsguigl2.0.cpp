/* ////////////////////////////////////////////////////////////

File Name: fsguigl2.0.cpp
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

// Depends on ysgl2lib

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

#include <ysclass.h>

#include <ysgl.h>

#include <ysbitmaparray.h>
#include "fsgui.h"
#include "fsguicommondrawing.h"



void FsGuiCommonDrawing::DrawLine(double x1,double y1,double x2,double y2,const YsColor &col)
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLDrawPlain2DLinef(YsGLSLSharedPlain2DRenderer(),
	   (float)x1,(float)y1,(float)x2,(float)y2,
	   col.Rf(),col.Gf(),col.Bf(),col.Af());
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawLineStrip(int n,const double plg[],const YsColor &c)
{
	YsArray <float,32> vtx(n*2,NULL);
	YsArray <float,64> col(n*4,NULL);

	for(int i=0; i<n; ++i)
	{
		vtx[i*2  ]=(float)plg[i*2];
		vtx[i*2+1]=(float)plg[i*2+1];

		col[i*4  ]=c.Rf();
		col[i*4+1]=c.Gf();
		col[i*4+2]=c.Bf();
		col[i*4+3]=c.Af();
	}

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLDrawPlain2DPrimitivefv(YsGLSLSharedPlain2DRenderer(),
	    GL_LINE_STRIP,
	    n,vtx,col);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
}

void FsGuiCommonDrawing::DrawRect(double x1,double y1,double x2,double y2,const YsColor &c,YSBOOL fill)
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	const float vtx[10]=
	{
		(float)x1,(float)y1,
		(float)x2,(float)y1,
		(float)x2,(float)y2,
		(float)x1,(float)y2,
		(float)x1,(float)y1  // <- to compensate missing dot problem.
	};
	const float col[20]=
	{
		c.Rf(),c.Gf(),c.Bf(),c.Af(),
		c.Rf(),c.Gf(),c.Bf(),c.Af(),
		c.Rf(),c.Gf(),c.Bf(),c.Af(),
		c.Rf(),c.Gf(),c.Bf(),c.Af(),
		c.Rf(),c.Gf(),c.Bf(),c.Af()
	};

	GLenum primType=(YSTRUE==fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
	const int nVtx=(YSTRUE==fill ? 4 : 5);

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLDrawPlain2DPrimitivefv(YsGLSLSharedPlain2DRenderer(),
	    primType,
	    nVtx,vtx,col);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawCircle(double x,double y,double rad,const YsColor &c,YSBOOL fill)
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	const int nDiv=32;

	float vtx[(nDiv+1)*2];
	float col[(nDiv+1)*4];

	for(int i=0; i<=nDiv; i++)
	{
		const float a=2.0f*(float)YsPi*(float)i/(float)nDiv;
		vtx[i*2  ]=(float)x+(float)rad*cosf(a);
		vtx[i*2+1]=(float)y+(float)rad*sinf(a);

		col[i*4  ]=c.Rf();
		col[i*4+1]=c.Gf();
		col[i*4+2]=c.Bf();
		col[i*4+3]=c.Af();
	}

	const GLenum primType=(YSTRUE==fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
	const int nVtx=(YSTRUE==fill ? nDiv : nDiv+1);

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLDrawPlain2DPrimitivefv(YsGLSLSharedPlain2DRenderer(),
	    primType,
	    nVtx,vtx,col);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawPolygon(int n,const double plg[],const YsColor &c)
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	YsArray <float,32> vtx(n*2,NULL);
	YsArray <float,64> col(n*4,NULL);

	for(int i=0; i<n; ++i)
	{
		vtx[i*2  ]=(float)plg[i*2];
		vtx[i*2+1]=(float)plg[i*2+1];

		col[i*4  ]=c.Rf();
		col[i*4+1]=c.Gf();
		col[i*4+2]=c.Bf();
		col[i*4+3]=c.Af();
	}

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLDrawPlain2DPrimitivefv(YsGLSLSharedPlain2DRenderer(),
	    GL_TRIANGLE_FAN,
	    n,vtx,col);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawGradationPolygon(int n,const double plg[],const YsColor c[])
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	YsArray <float,32> vtx(n*2,NULL);
	YsArray <float,64> col(n*4,NULL);

	for(int i=0; i<n; ++i)
	{
		vtx[i*2  ]=(float)plg[i*2];
		vtx[i*2+1]=(float)plg[i*2+1];

		col[i*4  ]=c[i].Rf();
		col[i*4+1]=c[i].Gf();
		col[i*4+2]=c[i].Bf();
		col[i*4+3]=c[i].Af();
	}

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLDrawPlain2DPrimitivefv(YsGLSLSharedPlain2DRenderer(),
	    GL_TRIANGLE_FAN,
	    n,vtx,col);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s Out",__FUNCTION__);
#endif
}

void FsGuiCommonDrawing::DrawBmp(const YsBitmap &bmp,double x,double y)
{
#ifdef YSOGLERRORCHECK
	FsOpenGlShowError("%s In",__FUNCTION__);
#endif

	const int bmpWid=bmp.GetWidth();
	const int bmpHei=bmp.GetHeight();

	if(0<bmpWid && 0<bmpHei)
	{
		YsGLSLUseBitmapRenderer(YsGLSLSharedBitmapRenderer());

		YsGLSLRenderRGBABitmap2D(YsGLSLSharedBitmapRenderer(),
			(int)x,(int)y,
			YSGLSL_HALIGN_LEFT,YSGLSL_VALIGN_TOP,
			bmpWid,bmpHei,bmp.GetRGBABitmapPointer());

		YsGLSLEndUseBitmapRenderer(YsGLSLSharedBitmapRenderer());
	}

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
	for(int i=0; i<bmpAry.GetNumBlockY(); ++i)
	{
		int nextLowY=lowY;
		for(int j=0; j<bmpAry.GetNumBlockX(); ++j)
		{
			const YsBitmap *bmp=bmpAry.GetTileBitmap(j,i);
			if(NULL!=bmp)
			{
				DrawBmp(*bmp,x0+j*(int)bmpAry.GetTileWidth(),lowY-(int)bmp->GetHeight());
			}
			nextLowY=lowY-(int)bmp->GetHeight();
		}
		lowY=nextLowY;
	}
}

void FsGuiCommonDrawing::DrawMask(const YsColor &fgCol,const YsColor & /*bgCol*/,double x0,double y0,double wid,double hei)
{
	float r,g,b,a;

	if(fgCol.Ri()>200 && fgCol.Gi()>200 && fgCol.Bi()>200)
	{
		r=0.2f;
		g=0.2f;
		b=0.2f;
		a=0.8f;
	}
	else
	{
		r=0.8f;
		g=0.8f;
		b=0.8f;
		a=0.8f;
	}

	const int x1=(int)(x0+wid);
	const int y1=(int)(y0+hei);

	const float vtx[10]=
	{
		(float)x0,(float)y0,
		(float)x1,(float)y0,
		(float)x1,(float)y1,
		(float)x0,(float)y1,
		(float)x0,(float)y0  // <- to compensate missing dot problem.
	};
	const float col[20]=
	{
		r,g,b,a,
		r,g,b,a,
		r,g,b,a,
		r,g,b,a,
		r,g,b,a
	};

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLDrawPlain2DPrimitivefv(YsGLSLSharedPlain2DRenderer(),
	    GL_TRIANGLE_FAN,
	    5,vtx,col);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
}

void FsGuiCommonDrawing::SetClipRect(double x0,double y0,double wid,double hei)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);

	glScissor((int)x0,viewport[3]-(int)(y0+hei),(int)wid,(int)hei+1);

	glEnable(GL_SCISSOR_TEST);
}

void FsGuiCommonDrawing::ClearClipRect(void)
{
	glDisable(GL_SCISSOR_TEST);
}

