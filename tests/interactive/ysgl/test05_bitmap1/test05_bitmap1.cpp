/* ////////////////////////////////////////////////////////////

File Name: test05_bitmap1.cpp
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
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <ysgl.h>
#include <ysglbitmap.h>


#include <fssimplewindow.h>


#ifndef _WIN32
extern "C" void InspectOpenGLVersionAndExtension(void)
{
	const char *openGlVersion=(const char *)glGetString(GL_VERSION);
	if(NULL!=openGlVersion)
	{
		printf("OpenGL Version=[%s]\n",openGlVersion);
	}
	else
	{
		printf("Cannot retrieve OpenGL version.\n");
	}
}
#endif

unsigned char rgba[4*128*128];

#include <fslazywindow.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;
	int key;
	struct YsGLSLBitmapRenderer *bitmapRenderer;

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
	for(int x=0; x<128; ++x)
	{
		for(int y=0; y<128; ++y)
		{
			int ix=x/16;
			int iy=y/16;
			int ptr=4*(y*128+x);

			if(0==((ix+iy)&1))
			{
				rgba[ptr  ]=0;
				rgba[ptr+1]=255;
				rgba[ptr+2]=0;
				rgba[ptr+3]=255;
			}
			else
			{
				rgba[ptr  ]=0;
				rgba[ptr+1]=0;
				rgba[ptr+2]=255;
				rgba[ptr+3]=255;
			}
		}
	}

	bitmapRenderer=YsGLSLCreateBitmapRenderer();
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}

	int mx,my,lb,mb,rb;
	FsGetMouseState(lb,mb,rb,mx,my);

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	YsGLSLDeleteBitmapRenderer(bitmapRenderer);
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	glViewport(0,0,wid,hei);

	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	YsGLSLUseBitmapRenderer(bitmapRenderer);
	YsGLSLRenderRGBABitmap2D(
	    bitmapRenderer,
	    32,32,
		YSGLSL_HALIGN_LEFT,
		YSGLSL_VALIGN_TOP,
	    128,128,rgba);
	YsGLSLEndUseBitmapRenderer(bitmapRenderer);


	glUseProgram(0);
	glColor3ub(255,255,255);
	glBegin(GL_LINES);
	glVertex2i(270,240);
	glVertex2i(370,240);
	glVertex2i(320,190);
	glVertex2i(320,290);
	glEnd();


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

