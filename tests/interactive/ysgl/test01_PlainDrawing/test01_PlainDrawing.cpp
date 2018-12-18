/* ////////////////////////////////////////////////////////////

File Name: test01_PlainDrawing.cpp
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
#include <fssimplewindow.h>
#include <ysgl.h>
#include <fslazywindow.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;
	struct YsGLSLPlain2DRenderer *plain2DDrawingPtr;

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
	const GLubyte *verStr=glGetString(GL_VERSION);
	if(NULL!=verStr)
	{
		printf("OpenGL Version %s\n",verStr);
	}
	else
	{
		printf("Unknown OpenGL version\n");
	}

	plain2DDrawingPtr=YsGLSLCreatePlain2DRenderer();
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}
	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	YsGLSLDeletePlain2DRenderer(plain2DDrawingPtr);
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glViewport(0,0,400,300);

	YsGLSLUsePlain2DRenderer(plain2DDrawingPtr);

	YsGLSLDontUseWindowCoordinateInPlain2DDrawing(plain2DDrawingPtr);
	YsGLSLDrawPlain2DLined(
	    plain2DDrawingPtr,
	    -1.0, 1.0,  // x0,y0
	     1.0,-1.0,  // x1,y1
	     1.0, 0.0, 0.0, 1.0);  // r,g,b,a



	int wid,hei;
	FsGetWindowSize(wid,hei);

	GLfloat triVtx[6]={100,100,200,100,150,200};
	GLfloat triColor[12]={1,0,0,1,  0,0,1,1,  0,1,0,1};


	glViewport(0,0,wid,hei);
	YsGLSLUseWindowCoordinateInPlain2DDrawing(plain2DDrawingPtr,1);

	YsGLSLDrawPlain2DLinef(
	    plain2DDrawingPtr,
	    0,0,300,300,0,0,1,1);

	YsGLSLDrawPlain2DPrimitivefv(plain2DDrawingPtr,GL_TRIANGLES,3,triVtx,triColor);


	YsGLSLUseWindowCoordinateInPlain2DDrawing(plain2DDrawingPtr,0);

	YsGLSLDrawPlain2DLinef(
	    plain2DDrawingPtr,
	    0,0,300,300,0,0,1,1);


	YsGLSLDrawPlain2DPrimitivefv(plain2DDrawingPtr,GL_TRIANGLES,3,triVtx,triColor);

	YsGLSLEndUsePlain2DRenderer(plain2DDrawingPtr);

	FsSwapBuffers();

	needRedraw=false;
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

