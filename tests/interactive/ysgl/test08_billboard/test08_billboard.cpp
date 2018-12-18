/* ////////////////////////////////////////////////////////////

File Name: test08_billboard.cpp
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

int MakeSphere(GLfloat vtx[],GLfloat texCoord[],GLfloat nom[],GLfloat col[],int nh,int nv)
{
	int nVtx=0;
	for(int v=0; v<nv; v++)
	{
		const double v0=-YSGLPI/2.0+YSGLPI*(double)v/(double)nv;
		const double v1=-YSGLPI/2.0+YSGLPI*(double)(v+1)/(double)nv;

		for(int h=0; h<nh; h++)
		{
			const int colIndex=1+(v*2/nv)+(h*4/nh)*2;

			const double h0=YSGLPI*2.0*(double)h/(double)nh;
			const double h1=YSGLPI*2.0*(double)(h+1)/(double)nh;

			vtx[nVtx*3  ]=(GLfloat)(cos(v0)*cos(h0));
			vtx[nVtx*3+1]=(GLfloat)(sin(v0));
			vtx[nVtx*3+2]=(GLfloat)(cos(v0)*sin(h0));
			texCoord[nVtx*2  ]=vtx[nVtx*3  ];
			texCoord[nVtx*2+1]=vtx[nVtx*3+1];
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v0)*cos(h1));
			vtx[nVtx*3+1]=(GLfloat)(sin(v0));
			vtx[nVtx*3+2]=(GLfloat)(cos(v0)*sin(h1));
			texCoord[nVtx*2  ]=vtx[nVtx*3  ];
			texCoord[nVtx*2+1]=vtx[nVtx*3+1];
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v1)*cos(h0));
			vtx[nVtx*3+1]=(GLfloat)(sin(v1));
			vtx[nVtx*3+2]=(GLfloat)(cos(v1)*sin(h0));
			texCoord[nVtx*2  ]=vtx[nVtx*3  ];
			texCoord[nVtx*2+1]=vtx[nVtx*3+1];
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v0)*cos(h1));
			vtx[nVtx*3+1]=(GLfloat)(sin(v0));
			vtx[nVtx*3+2]=(GLfloat)(cos(v0)*sin(h1));
			texCoord[nVtx*2  ]=vtx[nVtx*3  ];
			texCoord[nVtx*2+1]=vtx[nVtx*3+1];
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v1)*cos(h1));
			vtx[nVtx*3+1]=(GLfloat)(sin(v1));
			vtx[nVtx*3+2]=(GLfloat)(cos(v1)*sin(h1));
			texCoord[nVtx*2  ]=vtx[nVtx*3  ];
			texCoord[nVtx*2+1]=vtx[nVtx*3+1];
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v1)*cos(h0));
			vtx[nVtx*3+1]=(GLfloat)(sin(v1));
			vtx[nVtx*3+2]=(GLfloat)(cos(v1)*sin(h0));
			texCoord[nVtx*2  ]=vtx[nVtx*3  ];
			texCoord[nVtx*2+1]=vtx[nVtx*3+1];
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;
		}
	}

	for(int i=0; i<nVtx; i++)
	{
		nom[i*3  ]=vtx[i*3  ];
		nom[i*3+1]=vtx[i*3+1];
		nom[i*3+2]=vtx[i*3+2];
	}

	return nVtx;
}


#include <fslazywindow.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;
	GLfloat vtx[3*3*2*8*16],texCoord[3*3*2*8*16],nom[3*3*2*8*16],col[3*4*2*8*16];
	int nVtx;

	YsBitmap bmp[6];
	GLuint texId;
	double rot;
	int bmpId;

	YsGLSL3DRenderer *variColorPerVtxShadingRenderer;
	YsGLSL3DRenderer *monoColorPerVtxShadingRenderer;
	YsGLSL3DRenderer *monoColorPerPixShadingRenderer;
	YsGLSL3DRenderer *variColorPerPixShadingRenderer;
	YsGLSL3DRenderer *monoColorPerVtxShadingWithTexRenderer;
	YsGLSL3DRenderer *variColorPerVtxShadingWithTexRenderer;
	YsGLSL3DRenderer *monoColorPerPixShadingWithTexRenderer;
	YsGLSL3DRenderer *variColorPerPixShadingWithTexRenderer;
	YsGLSL3DRenderer *renderer3d;

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

	nVtx=MakeSphere(vtx,texCoord,nom,col,16,8);

	rot=0.0;
	bmpId=0;

	bmp[0].LoadPng("explosion01.png");
	bmp[1].LoadPng("explosion02.png");
	bmp[2].LoadPng("explosion03.png");
	bmp[3].LoadPng("explosion04.png");
	bmp[4].LoadPng("explosion05.png");
	bmp[5].LoadPng("flash01.png");

	variColorPerVtxShadingRenderer=YsGLSLCreateVariColorPerVtxShading3DRenderer();
	monoColorPerVtxShadingRenderer=YsGLSLCreateMonoColorPerVtxShading3DRenderer();
	monoColorPerPixShadingRenderer=YsGLSLCreateMonoColorPerPixShading3DRenderer();
	variColorPerPixShadingRenderer=YsGLSLCreateVariColorPerPixShading3DRenderer();
	monoColorPerVtxShadingWithTexRenderer=YsGLSLCreateMonoColorPerVtxShadingWithTexCoord3DRenderer();
	variColorPerVtxShadingWithTexRenderer=YsGLSLCreateVariColorPerVtxShadingWithTexCoord3DRenderer();
	monoColorPerPixShadingWithTexRenderer=YsGLSLCreateMonoColorPerPixShadingWithTexCoord3DRenderer();
	variColorPerPixShadingWithTexRenderer=YsGLSLCreateVariColorPerPixShadingWithTexCoord3DRenderer();
	renderer3d=monoColorPerVtxShadingRenderer;

	printf("SPACE   Switch between Per Vertex and Per Pixel renderers.\n");


	glGenTextures(1,&texId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,bmp[0].GetWidth(),bmp[0].GetHeight(),0,GL_RGBA,GL_UNSIGNED_BYTE,bmp[0].GetRGBABitmapPointer());

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

	switch(key)
	{
	case FSKEY_ENTER:
		bmpId=(bmpId+1)%6;
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,bmp[bmpId].GetWidth(),bmp[bmpId].GetHeight(),0,GL_RGBA,GL_UNSIGNED_BYTE,bmp[bmpId].GetRGBABitmapPointer());
		break;
	case FSKEY_SPACE:
		if(renderer3d==monoColorPerVtxShadingRenderer)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			renderer3d=monoColorPerPixShadingRenderer;
		}
		else if(renderer3d==monoColorPerPixShadingRenderer)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			renderer3d=variColorPerVtxShadingRenderer;
		}
		else if(renderer3d==variColorPerVtxShadingRenderer)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			renderer3d=variColorPerPixShadingRenderer;
		}
		else if(renderer3d==variColorPerPixShadingRenderer)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			renderer3d=monoColorPerVtxShadingWithTexRenderer;
		}
		else if(renderer3d==monoColorPerVtxShadingWithTexRenderer)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			renderer3d=variColorPerVtxShadingWithTexRenderer;
		}
		else if(renderer3d==variColorPerVtxShadingWithTexRenderer)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			renderer3d=monoColorPerPixShadingWithTexRenderer;
		}
		else if(renderer3d==monoColorPerPixShadingWithTexRenderer)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			renderer3d=variColorPerPixShadingWithTexRenderer;
		}
		else
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			renderer3d=monoColorPerVtxShadingRenderer;
		}
		break;
	}

	rot+=YSGLPI/30.0;

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	YsGLSLDelete3DRenderer(monoColorPerVtxShadingRenderer);
	YsGLSLDelete3DRenderer(variColorPerVtxShadingRenderer);
	YsGLSLDelete3DRenderer(monoColorPerPixShadingRenderer);
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
	YsGLMakeScalingfv(tfm,3.0,3.0,3.0);
	YsGLMultMatrixfv(modelview,modelview,tfm);


	YsGLSLUse3DRenderer(monoColorPerVtxShadingRenderer);
	YsGLSLSet3DRendererProjectionfv(monoColorPerVtxShadingRenderer,projection);
	YsGLSLSet3DRendererModelViewfv(monoColorPerVtxShadingRenderer,modelview);
	YsGLSLEndUse3DRenderer(monoColorPerVtxShadingRenderer);

	YsGLSLUse3DRenderer(variColorPerVtxShadingRenderer);
	YsGLSLSet3DRendererProjectionfv(variColorPerVtxShadingRenderer,projection);
	YsGLSLSet3DRendererModelViewfv(variColorPerVtxShadingRenderer,modelview);
	YsGLSLEndUse3DRenderer(variColorPerVtxShadingRenderer);

	YsGLSLUse3DRenderer(monoColorPerPixShadingRenderer);
	YsGLSLSet3DRendererProjectionfv(monoColorPerPixShadingRenderer,projection);
	YsGLSLSet3DRendererModelViewfv(monoColorPerPixShadingRenderer,modelview);
	YsGLSLEndUse3DRenderer(monoColorPerPixShadingRenderer);

	YsGLSLUse3DRenderer(variColorPerPixShadingRenderer);
	YsGLSLSet3DRendererProjectionfv(variColorPerPixShadingRenderer,projection);
	YsGLSLSet3DRendererModelViewfv(variColorPerPixShadingRenderer,modelview);
	YsGLSLEndUse3DRenderer(variColorPerPixShadingRenderer);

	YsGLSLUse3DRenderer(monoColorPerVtxShadingWithTexRenderer);
	YsGLSLSet3DRendererProjectionfv(monoColorPerVtxShadingWithTexRenderer,projection);
	YsGLSLSet3DRendererModelViewfv(monoColorPerVtxShadingWithTexRenderer,modelview);
	YsGLSLEndUse3DRenderer(monoColorPerVtxShadingWithTexRenderer);

	YsGLSLUse3DRenderer(variColorPerVtxShadingWithTexRenderer);
	YsGLSLSet3DRendererProjectionfv(variColorPerVtxShadingWithTexRenderer,projection);
	YsGLSLSet3DRendererModelViewfv(variColorPerVtxShadingWithTexRenderer,modelview);
	YsGLSLEndUse3DRenderer(variColorPerVtxShadingWithTexRenderer);

	YsGLSLUse3DRenderer(monoColorPerPixShadingWithTexRenderer);
	YsGLSLSet3DRendererProjectionfv(monoColorPerPixShadingWithTexRenderer,projection);
	YsGLSLSet3DRendererModelViewfv(monoColorPerPixShadingWithTexRenderer,modelview);
	YsGLSLEndUse3DRenderer(monoColorPerPixShadingWithTexRenderer);

	YsGLSLUse3DRenderer(variColorPerPixShadingWithTexRenderer);
	YsGLSLSet3DRendererProjectionfv(variColorPerPixShadingWithTexRenderer,projection);
	YsGLSLSet3DRendererModelViewfv(variColorPerPixShadingWithTexRenderer,modelview);
	YsGLSLEndUse3DRenderer(variColorPerPixShadingWithTexRenderer);


	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	YsGLSLUse3DRenderer(renderer3d);

	YsGLSLSet3DRendererTextureType(renderer3d,YSGLSL_TEX_TYPE_BILLBOARD);

	const GLfloat billBoardCenter[3]={0,0,0};
	const GLfloat billBoardDimension[2]={3.0f,3.0f};
	YsGLSLSet3DRendererBillBoardfv(renderer3d,billBoardCenter,billBoardDimension);

	GLfloat color[4]={0,0,1,1};
	YsGLSLSet3DRendererUniformColorfv(renderer3d,color);

	if(renderer3d!=monoColorPerPixShadingWithTexRenderer &&
	   renderer3d!=variColorPerPixShadingWithTexRenderer &&
	   renderer3d!=monoColorPerVtxShadingWithTexRenderer &&
	   renderer3d!=variColorPerVtxShadingWithTexRenderer)
	{
		YsGLSLDrawPrimitiveVtxNomColfv(renderer3d,GL_TRIANGLES,nVtx,vtx,nom,col);
	}
	else if(renderer3d==monoColorPerPixShadingWithTexRenderer || renderer3d==monoColorPerVtxShadingWithTexRenderer)
	{
		YsGLSLDrawPrimitiveVtxTexCoordNomfv(renderer3d,GL_TRIANGLES,nVtx,vtx,texCoord,nom);
	}
	else if(renderer3d==variColorPerPixShadingWithTexRenderer || renderer3d==variColorPerVtxShadingWithTexRenderer)
	{
		YsGLSLDrawPrimitiveVtxTexCoordNomColfv(renderer3d,GL_TRIANGLES,nVtx,vtx,texCoord,nom,col);
	}

	YsGLSLEndUse3DRenderer(renderer3d);



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

