/* ////////////////////////////////////////////////////////////

File Name: test02_flat3DDrawing.cpp
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
#include <fslazywindow.h>
#include <ysgl.h>


const unsigned char groundTileTexture[16*16]=
{
	247,235,252,241,238,246,232,247,244,240,245,238,236,230,239,248,
	249,255,237,241,239,239,248,248,237,245,237,239,236,248,237,238,
	233,237,254,236,237,239,245,238,248,236,243,248,251,233,241,251,
	238,240,240,235,250,234,253,238,236,246,251,234,236,238,233,240,
	231,244,240,240,244,253,237,244,234,244,247,244,255,246,242,254,
	237,252,230,235,230,233,242,240,232,242,246,255,232,241,240,251,
	245,254,237,233,251,231,248,246,239,245,232,246,238,254,243,243,
	246,249,247,242,246,252,245,250,254,246,252,242,238,243,236,231,
	255,238,236,244,240,249,232,241,245,253,252,246,249,249,249,241,
	249,237,241,245,231,244,230,249,255,242,239,230,252,255,234,232,
	241,255,253,247,240,248,242,246,241,231,243,253,251,232,250,238,
	250,232,231,231,235,247,238,230,237,233,250,241,255,233,244,240,
	255,246,248,242,232,247,247,237,241,233,254,237,241,241,238,244,
	237,232,232,233,237,239,234,244,251,239,253,255,252,238,232,244,
	231,242,241,246,244,250,253,231,236,254,236,233,251,247,250,240,
	242,252,244,243,234,240,232,244,255,243,234,252,251,245,237,249
};

GLuint MakeTextureTile(void)
{
	unsigned char rgba[16*16*4];
	for(int i=0; i<256; i++)
	{
		rgba[i*4  ]=groundTileTexture[i];
		rgba[i*4+1]=groundTileTexture[i];
		rgba[i*4+2]=groundTileTexture[i];
		rgba[i*4+3]=255;
	}

	GLuint ysScnGlMapTex;
	glGenTextures(1,&ysScnGlMapTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,ysScnGlMapTex);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glTexImage2D
	    (GL_TEXTURE_2D,
	     0,
	     GL_RGBA,
	     16,
	     16,
	     0,
	     GL_RGBA,
	     GL_UNSIGNED_BYTE,
	     rgba);

	return ysScnGlMapTex;
}


class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;
	YsGLSL3DRenderer *flatRenderer;
	YsGLSL3DRenderer *variColRenderer;

	GLfloat texTfmXY[16];
	GLfloat texTfmXZ[16];

	int useTexture;

	double rot;
	int fogEnabled;

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
	for(auto &t : texTfmXY)
	{
		t=0;
	}
	texTfmXY[0]=1;
	texTfmXY[5]=1;
	texTfmXY[10]=1;
	texTfmXY[15]=1;

	for(auto &t : texTfmXZ)
	{
		t=0;
	}
	texTfmXZ[0]=1;
	texTfmXZ[5]=1;
	texTfmXZ[10]=1;
	texTfmXZ[15]=1;

	useTexture=0;

	rot=0.0;
	fogEnabled=0;

	flatRenderer=YsGLSLCreateFlat3DRenderer();
	variColRenderer=YsGLSLCreateVariColor3DRenderer();

	MakeTextureTile();
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}

	if(FSKEY_SPACE==key)
	{
		glEnable(GL_TEXTURE_2D);
		useTexture=1-useTexture;
		YsGLSLUse3DRenderer(flatRenderer);
		YsGLSLSet3DRendererTextureType(flatRenderer,YSGLSL_TEX_TYPE_TILING);
		YsGLSLSet3DRendererTextureIdentifier(flatRenderer,0);
		YsGLSLSet3DRendererUniformTextureTilingMatrixfv(flatRenderer,texTfmXY);
		YsGLSLEndUse3DRenderer(flatRenderer);

		YsGLSLUse3DRenderer(variColRenderer);
		YsGLSLSet3DRendererTextureType(variColRenderer,YSGLSL_TEX_TYPE_TILING);
		YsGLSLSet3DRendererTextureIdentifier(variColRenderer,0);
		YsGLSLSet3DRendererUniformTextureTilingMatrixfv(variColRenderer,texTfmXZ);
		YsGLSLEndUse3DRenderer(variColRenderer);
	}
	if(FSKEY_ENTER==key)
	{
		for(int i=0; i<16; ++i)
		{
			texTfmXY[i]/=1.2f;
			texTfmXZ[i]/=1.2f;
		}
		YsGLSLUse3DRenderer(flatRenderer);
		YsGLSLSet3DRendererUniformTextureTilingMatrixfv(flatRenderer,texTfmXY);
		YsGLSLEndUse3DRenderer(flatRenderer);

		YsGLSLUse3DRenderer(variColRenderer);
		YsGLSLSet3DRendererUniformTextureTilingMatrixfv(variColRenderer,texTfmXZ);
		YsGLSLEndUse3DRenderer(variColRenderer);
	}

	if(FSKEY_F==key)
	{
		fogEnabled=1-fogEnabled;

		YsGLSLUse3DRenderer(flatRenderer);
		YsGLSLSet3DRendererUniformFogEnabled(flatRenderer,fogEnabled);
		YsGLSLSet3DRendererUniformFogDensity(flatRenderer,.07f);
		YsGLSLEndUse3DRenderer(flatRenderer);

		YsGLSLUse3DRenderer(variColRenderer);
		YsGLSLSet3DRendererUniformFogEnabled(variColRenderer,fogEnabled);
		YsGLSLSet3DRendererUniformFogDensity(variColRenderer,.08f);
		YsGLSLEndUse3DRenderer(variColRenderer);
	}

	if(FSKEY_O==key)
	{
		static GLfloat offset=0.001f;
		YsGLSLUse3DRenderer(flatRenderer);
		YsGLSLSet3DRendererZOffsetEnabled(flatRenderer,1);
		YsGLSLSet3DRendererZOffset(flatRenderer,offset);
		YsGLSLEndUse3DRenderer(flatRenderer);
		offset+=0.001f;
	}

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	YsGLSLDelete3DRenderer(flatRenderer);
	YsGLSLDelete3DRenderer(variColRenderer);
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	needRedraw=false;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	int wid,hei;
	FsGetWindowSize(wid,hei);

	const double aspect=(double)wid/(double)hei;

	GLfloat modelview[16],projection[16],tfm[16];
	YsGLMakePerspectivefv(projection,YSGLPI/6.0,aspect,1.0,50.0);

	YsGLMakeIdentityfv(modelview);
	YsGLMakeTranslationfv(tfm,0.0,0.0,-15.0);
	YsGLMultMatrixfv(modelview,modelview,tfm);
	YsGLMakeXZRotationfv(tfm,rot);
	YsGLMultMatrixfv(modelview,modelview,tfm);


	YsGLSLUse3DRenderer(flatRenderer);
	YsGLSLSet3DRendererProjectionfv(flatRenderer,projection);
	YsGLSLSet3DRendererModelViewfv(flatRenderer,modelview);


	GLfloat color[4]={0,0,1,1};
	YsGLSLSet3DRendererUniformColorfv(flatRenderer,color);


	GLfloat square[12]=
	{
		-5,-5,0,
		 5,-5,0,
		 5, 5,0,
		-5, 5,0
	};
	YsGLSLDrawPrimitiveVtxfv(flatRenderer,GL_TRIANGLE_FAN,4,square);
	YsGLSLEndUse3DRenderer(flatRenderer);



	YsGLSLUse3DRenderer(variColRenderer);
	YsGLSLSet3DRendererProjectionfv(variColRenderer,projection);
	YsGLSLSet3DRendererModelViewfv(variColRenderer,modelview);

	GLfloat square2[12]=
	{
		-5,2,-5,
		 5,2,-5,
		 5,2, 5,
		-5,2, 5
	};
	GLfloat color2[16]=
	{
		1,0,0,1,  0,1,0,1,  0,0,1,1,  1,1,0,1
	};

	YsGLSLDrawPrimitiveVtxColfv(variColRenderer,GL_TRIANGLE_FAN,4,square2,color2);

	YsGLSLEndUse3DRenderer(variColRenderer);


	FsSwapBuffers();

	if(0==FsGetKeyState(FSKEY_SHIFT))
	{
		rot+=YSGLPI/90.0;
	}
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
