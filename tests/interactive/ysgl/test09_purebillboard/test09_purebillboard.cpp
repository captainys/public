/* ////////////////////////////////////////////////////////////

File Name: test09_purebillboard.cpp
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
#include <math.h>
#include <fssimplewindow.h>
#include <ysgl.h>

#include <ysbitmap.h>


#include <fslazywindow.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;
	YsBitmap bmp[6];
	YsGLSL3DRenderer *renderer;
	GLuint texId[6];
	int bmpId;
	GLfloat x,y;

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
	FsChangeToProgramDir();

	renderer=YsGLSLCreateVariColorBillBoard3DRenderer();
	bmpId=0;
	x=0.0;
	y=0.0;

	bmp[0].LoadPng("explosion01.png");
	bmp[1].LoadPng("explosion02.png");
	bmp[2].LoadPng("explosion03.png");
	bmp[3].LoadPng("explosion04.png");
	bmp[4].LoadPng("explosion05.png");
	bmp[5].LoadPng("flash01.png");

	glGenTextures(6,texId);

	glActiveTexture(GL_TEXTURE0);
	for(int i=0; i<6; ++i)
	{
		glBindTexture(GL_TEXTURE_2D,texId[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,bmp[i].GetWidth(),bmp[i].GetHeight(),1,GL_RGBA,GL_UNSIGNED_BYTE,bmp[i].GetRGBABitmapPointer());
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}
	needRedraw=true;

	switch(key)
	{
	case FSKEY_ENTER:
		bmpId=(bmpId+1)%6;
		glBindTexture(GL_TEXTURE_2D,texId[bmpId]);
		break;
	}

	if(0!=FsGetKeyState(FSKEY_LEFT))
	{
		x-=0.1f;
	}
	if(0!=FsGetKeyState(FSKEY_RIGHT))
	{
		x+=0.1f;
	}
	if(0!=FsGetKeyState(FSKEY_UP))
	{
		y+=0.1f;
	}
	if(0!=FsGetKeyState(FSKEY_DOWN))
	{
		y-=0.1f;
	}

}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	YsGLSLDelete3DRenderer(renderer);
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	needRedraw=false;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	int wid,hei;
	FsGetWindowSize(wid,hei);

	const double aspect=(double)wid/(double)hei;

	GLfloat modelview[16],projection[16];
	YsGLMakePerspectivefv(projection,YSGLPI/6.0,aspect,1.0,50.0);

	YsGLMakeIdentityfv(modelview);


	YsGLSLUse3DRenderer(renderer);
	YsGLSLSet3DRendererProjectionfv(renderer,projection);
	YsGLSLSet3DRendererModelViewfv(renderer,modelview);

	YsGLSLSet3DRendererBillBoardTransformType(renderer,YSGLSL_TEX_BILLBOARD_ORTHO);


	glDisable(GL_CULL_FACE);


	const GLfloat vtx[9]=
	{
		x,y,-15.0f,
		x,y,-15.0f,
		x,y,-15.0f,
	};
	const GLfloat col[12]=
	{
		1.0f,0.0f,0.0f,1.0f,
		1.0f,0.0f,0.0f,1.0f,
		1.0f,0.0f,0.0f,1.0f,
	};
	const GLfloat vOffset[9]=
	{
		-2.154701f,-1.0f,      0.0f,
		 2.154701f,-1.0f,      0.0f,
		 0.0f,      2.7320508f,0.0f,
	};
	const GLfloat texCoord[6]=
	{
		-2.154701f,-1.0f,     
		 2.154701f,-1.0f,     
		 0.0f,      2.7320508f
	};

	YsGLSLDrawPrimitiveVtxColVoffsetTexcoordfv(renderer,GL_TRIANGLES,3,vtx,col,vOffset,texCoord);

	YsGLSLEndUse3DRenderer(renderer);



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


