/* ////////////////////////////////////////////////////////////

File Name: test13_markerByPointSprite.cpp
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

/*
 *  testapp.cpp
 *  FsSimpleWindow
 *
 *  Created by Soji Yamakawa on 11/22/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <fssimplewindow.h>
#include <ysgl.h>


#if !defined(GL_PROGRAM_POINT_SIZE) && defined(GL_PROGRAM_POINT_SIZE_EXT)
#define GL_PROGRAM_POINT_SIZE GL_PROGRAM_POINT_SIZE_EXT
#endif



static YsGLSL3DRenderer *variColorRenderer,*monoColorRenderer;


#include <fslazywindow.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;
	double rot;
	int fogEnabled;
	double hdg;

public:
	FsLazyWindowApplication();
	virtual void BeforeEverything(int argc,char *argv[]);
	virtual void GetOpenWindowOption(FsOpenWindowOption &OPT) const;
	virtual void Initialize(int argc,char *argv[]);
	virtual void Interval(void);
	virtual void BeforeTerminate(void);
	virtual void Draw(void);
	virtual bool UserWantToCloseProgram(void);
	virtual bool MustTerminate(void) const;
	virtual long long int GetMinimumSleepPerInterval(void) const;
	virtual bool NeedRedraw(void) const;
};

FsLazyWindowApplication::FsLazyWindowApplication()
{
	needRedraw=false;
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int argc,char *argv[])
{
}
/* virtual */ void FsLazyWindowApplication::GetOpenWindowOption(FsOpenWindowOption &opt) const
{
	opt.x0=16;
	opt.y0=16;
	opt.wid=800;
	opt.hei=600;
}
/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
	hdg=0.0;
	rot=0.0;
	fogEnabled=0;

	YsGLSLCreateSharedRenderer();

	variColorRenderer=YsGLSLSharedVariColorMarkerByPointSprite3DRenderer();
	monoColorRenderer=YsGLSLSharedMonoColorMarkerByPointSprite3DRenderer();

	YsGLSLUse3DRenderer(variColorRenderer);
	YsGLSLSet3DRendererUniformMarkerDimension(variColorRenderer,10);
	YsGLSLSet3DRendererUniformMarkerType(variColorRenderer,YSGLSL_MARKER_TYPE_EMPTY_STAR);
	YsGLSLEndUse3DRenderer(variColorRenderer);

	YsGLSLUse3DRenderer(monoColorRenderer);
	YsGLSLSet3DRendererUniformMarkerDimension(monoColorRenderer,10);
	YsGLSLSet3DRendererUniformMarkerType(monoColorRenderer,YSGLSL_MARKER_TYPE_EMPTY_STAR);
	GLfloat col[4]={1,1,1,1};
	YsGLSLSet3DRendererUniformColorfv(monoColorRenderer,col);
	YsGLSLEndUse3DRenderer(monoColorRenderer);

}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}
	needRedraw=true;

	if(FSKEY_F==key)
	{
		fogEnabled=1-fogEnabled;

		//YsGLSLUse3DRenderer(flatRenderer);
		//YsGLSLSet3DRendererUniformFogEnabled(flatRenderer,fogEnabled);
		//YsGLSLSet3DRendererUniformFogDensity(flatRenderer,.07);
		//YsGLSLEndUse3DRenderer(flatRenderer);

		//YsGLSLUse3DRenderer(variColRenderer);
		//YsGLSLSet3DRendererUniformFogEnabled(variColRenderer,fogEnabled);
		//YsGLSLSet3DRendererUniformFogDensity(variColRenderer,.08);
		//YsGLSLEndUse3DRenderer(variColRenderer);
	}

	hdg+=0.01;
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	needRedraw=false;

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);

	int lb,mb,rb,mx,my;
	FsGetMouseEvent(lb,mb,rb,mx,my);

	float r=1.0,g=0.0,b=0.0,a=1.0;


    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	

	GLfloat spriteVtx[]=
	{
		1,1,1,
		-1,1,1,
		1,-1,1,
		-1,-1,1,
		1,1,-1,
		-1,1,-1,
		1,-1,-1,
		-1,-1,-1,
	};
	GLfloat spriteColor[]=
	{
		0,0,1, 1,
		0,1,0, 1,
		1,0,0, 1,
		1,0,1, 1,
		1,1,0, 1,
		1,1,1, 1,
		0,0,1, 1,
		0,1,0, 1,
	};

	GLfloat monoColorVtx[]=
	{
		 1, 0, 0,
		-1, 0, 0,
		 0, 1, 0,
		 0,-1, 0,
		 0, 0, 1,
		 0, 0,-1,
	};


	const int nVtx=(sizeof(spriteVtx)/(3*sizeof(GLfloat)));



	glDisable(GL_DEPTH_TEST);

	glEnable(GL_PROGRAM_POINT_SIZE);  // Needed for enabling gl_PointSize in the vertex shader.
	glEnable(GL_POINT_SPRITE);        // Needed for enabling gl_PointCoord in the fragment shader.

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLUseWindowCoordinateInPlain2DDrawing(YsGLSLSharedPlain2DRenderer(),1);
	YsGLSLDrawPlain2DLinef(YsGLSLSharedPlain2DRenderer(),mx-10,my-10,mx+10,my+10,r,g,b,a);
	YsGLSLDrawPlain2DLinef(YsGLSLSharedPlain2DRenderer(),mx+10,my-10,mx-10,my+10,r,g,b,a);
	
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	
	glEnable(GL_DEPTH_TEST);

	GLfloat modelview[16],projection[16],tfm[16];
	YsGLMakeIdentityfv(modelview);
	YsGLMakeTranslationfv(tfm,0.0f,0.0f,-5.0f);
	YsGLMultMatrixfv(modelview,modelview,tfm);
	YsGLMakeXZRotationfv(tfm,(float)hdg);
	YsGLMultMatrixfv(modelview,modelview,tfm);

	YsGLMakePerspectivefv(projection,YSGLPI/6.0,(float)winWid/(float)winHei,1.0,50.0);


	YsGLSLUse3DRenderer(variColorRenderer);

	YsGLSLSet3DRendererModelViewfv(variColorRenderer,modelview);
	YsGLSLSet3DRendererProjectionfv(variColorRenderer,projection);

	YsGLSLDrawPrimitiveVtxColfv(variColorRenderer,GL_POINTS,nVtx,spriteVtx,spriteColor);

	YsGLSLEndUse3DRenderer(variColorRenderer);


	YsGLSLUse3DRenderer(monoColorRenderer);

	YsGLSLSet3DRendererModelViewfv(monoColorRenderer,modelview);
	YsGLSLSet3DRendererProjectionfv(monoColorRenderer,projection);

	YsGLSLDrawPrimitiveVtxfv(monoColorRenderer,GL_POINTS,6,monoColorVtx);

	YsGLSLEndUse3DRenderer(monoColorRenderer);

	FsSwapBuffers();
}
/* virtual */ bool FsLazyWindowApplication::UserWantToCloseProgram(void)
{
	return true; // Returning true will just close the program.
}
/* virtual */ bool FsLazyWindowApplication::MustTerminate(void) const
{
	return FsLazyWindowApplicationBase::MustTerminate();
}
/* virtual */ long long int FsLazyWindowApplication::GetMinimumSleepPerInterval(void) const
{
	return 10;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	YsGLSLDeleteSharedRenderer();
}
/* virtual */ bool FsLazyWindowApplication::NeedRedraw(void) const
{
	return needRedraw;
}


static FsLazyWindowApplication *appPtr=nullptr;

/* static */ FsLazyWindowApplicationBase *FsLazyWindowApplicationBase::GetApplication(void)
{
	if(nullptr==appPtr)
	{
		appPtr=new FsLazyWindowApplication;
	}
	return appPtr;
}

