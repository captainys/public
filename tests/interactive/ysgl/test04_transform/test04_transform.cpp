/* ////////////////////////////////////////////////////////////

File Name: test04_transform.cpp
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
#include <ysclass.h>

static void MakeModelViewProjectionMatrix(YsMatrix4x4 &mat,GLfloat modelviewf[16],GLfloat projectionf[16])
{
	YsMatrix4x4 modelview,projection;
	modelview.CreateFromOpenGlCompatibleMatrix <GLfloat> (modelviewf);
	projection.CreateFromOpenGlCompatibleMatrix <GLfloat> (projectionf);
	mat=projection*modelview;
}

#include <fslazywindow.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;
	struct YsGLSL3DRenderer *flatRenderer;
	struct YsGLSL3DRenderer *variColRenderer;

	struct YsGLSLPlain2DRenderer *plain2DDrawingPtr;

	double rot;

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
	flatRenderer=YsGLSLCreateFlat3DRenderer();
	variColRenderer=YsGLSLCreateVariColor3DRenderer();

	plain2DDrawingPtr=YsGLSLCreatePlain2DRenderer();

	rot=0.0;
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}
	if(0!=FsGetKeyState(FSKEY_SPACE))
	{
		rot+=YSGLPI/30.0;
	}
	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	YsGLSLDelete3DRenderer(flatRenderer);
	YsGLSLDelete3DRenderer(variColRenderer);
	YsGLSLDeletePlain2DRenderer(plain2DDrawingPtr);
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	needRedraw=false;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	int wid,hei;
	FsGetWindowSize(wid,hei);

	glViewport(0,0,wid,hei);

	const double aspect=(double)wid/(double)hei;

	GLfloat modelview[16],projection[16],tfm[16];
	YsGLMakePerspectivefv(projection,YSGLPI/6.0,aspect,5.0,25.0);

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


	YsGLSLUsePlain2DRenderer(plain2DDrawingPtr);
	YsGLSLUseWindowCoordinateInPlain2DDrawing(plain2DDrawingPtr,1);

	{
		YsMatrix4x4 tfm;
		MakeModelViewProjectionMatrix(tfm,modelview,projection);
		for(int i=0; i<4; ++i)
		{
			double pos[4]={square2[i*3],square2[i*3+1],square2[i*3+2],1.0};
			const int cenx=wid/2,ceny=hei/2;

#if 1
			tfm.Mul(pos,pos);
			const int scx=cenx+(int)(0.5*(double)wid*pos[0]/pos[3]);
			const int scy=ceny-(int)(0.5*(double)hei*pos[1]/pos[3]);
			const double scz=pos[2]/pos[3];
			printf("Z=%lf\n",scz);
			// Memo: Zview==Znear -> Zdev=-1.0
			//       Zview==Zfar  -> Zdev= 1.0
#else
			// Verification.
			//     http://www.opengl.org/sdk/docs/man/xhtml/gluProject.xml
			GLdouble modeld[16],projd[16],winx,winy,winz;
			for(int j=0; j<16; ++j)
			{
				modeld[j]=(GLdouble)modelview[j];
				projd[j]=(GLdouble)projection[j];
			}
			const int view[4]={0,0,wid,hei};

			gluProject(pos.x(),pos.y(),pos.z(),modeld,projd,view,&winx,&winy,&winz);
			int scx=(int)winx,scy=hei-(int)winy;
#endif

			GLfloat cross[12]=
			{
				(GLfloat)scx-10.0F,(GLfloat)scy      ,(GLfloat)scx+10.0F,(GLfloat)scy   ,
				(GLfloat)scx      ,(GLfloat)scy-10.0F,(GLfloat)scx      ,(GLfloat)scy+10.0F,
				(GLfloat)cenx     ,(GLfloat)ceny     ,(GLfloat)scx      ,(GLfloat)scy
			};
			GLfloat crossColor[24]=
			{
				1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,
				1,0,0,1,1,0,0,1
			};
			YsGLSLDrawPlain2DPrimitivefv(plain2DDrawingPtr,GL_LINES,6,cross,crossColor);


		}
	}

	YsGLSLEndUsePlain2DRenderer(plain2DDrawingPtr);


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


