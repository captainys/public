/* ////////////////////////////////////////////////////////////

File Name: test10_particle.cpp
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
#include <time.h>
#include <fssimplewindow.h>
#include <ysgl.h>

#include <ysbitmap.h>
#include <ysclass.h>

static void FsMakeAlphaMask(YsBitmap &bmp)
{
	const int nPix=bmp.GetWidth()*bmp.GetHeight();
	unsigned char *rgba=bmp.GetEditableRGBABitmapPointer();
	for(int i=0; i<nPix; ++i)
	{
		rgba[i*4+3]=rgba[i*4];

		rgba[i*4  ]=255;
		rgba[i*4+1]=255;
		rgba[i*4+2]=255;
	}
}

static void ClearEdge(YsBitmap &bmp)
{
	for(int x=0; x<bmp.GetWidth(); ++x)
	{
		unsigned char *pix=bmp.GetEditableRGBAPixelPointer(x,0);
		pix[0]=0;
		pix[1]=0;
		pix[2]=0;
		pix[3]=0;
		pix=bmp.GetEditableRGBAPixelPointer(x,1);
		pix[0]=0;
		pix[1]=0;
		pix[2]=0;
		pix[3]=0;
		pix=bmp.GetEditableRGBAPixelPointer(x,bmp.GetHeight()-1);
		pix[0]=0;
		pix[1]=0;
		pix[2]=0;
		pix[3]=0;
		pix=bmp.GetEditableRGBAPixelPointer(x,bmp.GetHeight()-2);
		pix[0]=0;
		pix[1]=0;
		pix[2]=0;
		pix[3]=0;
	}
	for(int y=0; y<bmp.GetHeight(); ++y)
	{
		unsigned char *pix=bmp.GetEditableRGBAPixelPointer(0,y);
		pix[0]=0;
		pix[1]=0;
		pix[2]=0;
		pix[3]=0;
		pix=bmp.GetEditableRGBAPixelPointer(1,y);
		pix[0]=0;
		pix[1]=0;
		pix[2]=0;
		pix[3]=0;
		pix=bmp.GetEditableRGBAPixelPointer(bmp.GetWidth()-1,y);
		pix[0]=0;
		pix[1]=0;
		pix[2]=0;
		pix[3]=0;
		pix=bmp.GetEditableRGBAPixelPointer(bmp.GetWidth()-2,y);
		pix[0]=0;
		pix[1]=0;
		pix[2]=0;
		pix[3]=0;
	}
}

static 	const int maxNParticle=50000;

void GenerateCloud(
    GLuint vboId,int nParticle,GLfloat vtx[],GLfloat col[],GLfloat vOffset[],GLfloat texCoord[],
    unsigned int &vtxPtr,unsigned int &colPtr,unsigned int &vOffsetPtr,unsigned int &texCoordPtr,
    GLfloat radius,GLfloat alpha)
{
	srand(12345);

	static YsVec3 partPos[maxNParticle];
	static double partDist[maxNParticle];
	for(int i=0; i<nParticle; ++i)
	{
		const double x=(double)rand()/(double)RAND_MAX*8.0-4.0;
		const double y=(double)rand()/(double)RAND_MAX*4.0-2.0;
		const double z=(double)rand()/(double)RAND_MAX*8.0-4.0;
		const GLfloat random=(GLfloat)rand()/(GLfloat)RAND_MAX;
		const GLfloat realRadius=radius*(1.0f+random)/2.0f;
		partPos[i].Set(x,y,z);
		partDist[i]=partPos[i].z(); // GetLength();

		col[i*12   ]=1.0f;
		col[i*12+ 1]=1.0f;
		col[i*12+ 2]=1.0f;
		col[i*12+ 3]=alpha;
		col[i*12+ 4]=1.0f;
		col[i*12+ 5]=1.0f;
		col[i*12+ 6]=1.0f;
		col[i*12+ 7]=alpha;
		col[i*12+ 8]=1.0f;
		col[i*12+ 9]=1.0f;
		col[i*12+10]=1.0f;
		col[i*12+11]=alpha;
		
		vOffset[i*9  ]=-realRadius*2.154701f;
		vOffset[i*9+1]=-realRadius*1.0f;
		vOffset[i*9+2]= 0.0f;
		vOffset[i*9+3]= realRadius*2.154701f;
		vOffset[i*9+4]=-realRadius*1.0f;
		vOffset[i*9+5]= 0.0f;
		vOffset[i*9+6]= realRadius*0.0f;
		vOffset[i*9+7]= realRadius*2.7320508f;
		vOffset[i*9+8]= 0.0f;

		texCoord[i*6  ]=-2.154701f;
		texCoord[i*6+1]=-1.0f;
		texCoord[i*6+2]= 2.154701f;
		texCoord[i*6+3]=-1.0f;
		texCoord[i*6+4]= 0.0f;
		texCoord[i*6+5]= 2.7320508f;
	}
	YsQuickSort <double,YsVec3> (nParticle,partDist,partPos);
	for(int i=0; i<nParticle; ++i)
	{
		vtx[i*9  ]=(GLfloat)partPos[i].x();
		vtx[i*9+1]=(GLfloat)partPos[i].y();
		vtx[i*9+2]=(GLfloat)partPos[i].z();
		vtx[i*9+3]=(GLfloat)partPos[i].x();
		vtx[i*9+4]=(GLfloat)partPos[i].y();
		vtx[i*9+5]=(GLfloat)partPos[i].z();
		vtx[i*9+6]=(GLfloat)partPos[i].x();
		vtx[i*9+7]=(GLfloat)partPos[i].y();
		vtx[i*9+8]=(GLfloat)partPos[i].z();
	}



	glBindBuffer(GL_ARRAY_BUFFER,vboId);

	unsigned int vtxSize=sizeof(GLfloat)*maxNParticle*9;
	unsigned int colSize=sizeof(GLfloat)*maxNParticle*12;
	unsigned int vOffsetSize=sizeof(GLfloat)*maxNParticle*9;
	unsigned int texCoordSize=sizeof(GLfloat)*maxNParticle*6;

	unsigned int totalBufSize=vtxSize+colSize+vOffsetSize+texCoordSize;
	glBufferData(GL_ARRAY_BUFFER,totalBufSize,NULL,GL_STATIC_DRAW);

	unsigned int bufPtr=0;

	vtxPtr=bufPtr;
	glBufferSubData(GL_ARRAY_BUFFER,bufPtr,vtxSize,vtx);
	bufPtr+=vtxSize;

	colPtr=bufPtr;
	glBufferSubData(GL_ARRAY_BUFFER,bufPtr,colSize,col);
	bufPtr+=colSize;

	vOffsetPtr=bufPtr;
	glBufferSubData(GL_ARRAY_BUFFER,bufPtr,vOffsetSize,vOffset);
	bufPtr+=vOffsetSize;

	texCoordPtr=bufPtr;
	glBufferSubData(GL_ARRAY_BUFFER,bufPtr,texCoordSize,texCoord);
	bufPtr+=texCoordSize;

	glBindBuffer(GL_ARRAY_BUFFER,0);
}

class YsGLBitmap
{
public:
	int w,h;
	GLfloat *rgba;

	YsGLBitmap();
	~YsGLBitmap();
	void CleanUp(void);
	void MakeEmpty(int w,int h);
	void Copy(const YsBitmap &from);
};

YsGLBitmap::YsGLBitmap()
{
	w=0;
	h=0;
	rgba=NULL;
}

YsGLBitmap::~YsGLBitmap()
{
	CleanUp();
}

void YsGLBitmap::CleanUp(void)
{
	if(NULL!=rgba)
	{
		delete [] rgba;
	}
	rgba=NULL;
	w=0;
	h=0;
}

void YsGLBitmap::MakeEmpty(int w,int h)
{
	CleanUp();
	this->w=w;
	this->h=h;
	this->rgba=new GLfloat[w*h*4];
}

void YsGLBitmap::Copy(const YsBitmap &from)
{
	MakeEmpty(from.GetWidth(),from.GetHeight());
	YsGLCopyUCharBitmapToGLFloatBitmap(w,h,rgba,from.GetRGBABitmapPointer());
}

#include <fslazywindow.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	GLfloat ang;

	YsBitmap bmp[6];
	YsGLBitmap ysglBmp[6];
	YsGLSL3DRenderer *shadedRenderer;

	YsGLSL3DRenderer *renderer[2];
	int useRenderer;

	GLuint texId[6];
	int bmpId;
	GLfloat x,y,z,alpha,radius;
	GLint lighting;


	int nParticle;
	GLfloat vtx[maxNParticle*9];
	GLfloat col[maxNParticle*12];
	GLfloat vOffset[maxNParticle*9];
	GLfloat texCoord[maxNParticle*6];

	GLuint vboId;
	unsigned int vtxPtr,colPtr,vOffsetPtr,texCoordPtr;
	YsVec3 lightDir;
	GLfloat lightDirf[4];
	GLfloat specularColor[4];
	GLfloat ambient[4];
	int useVbo;
	int nFrame;
	time_t t0;


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
	opt.x0=0;
	opt.y0=0;
	opt.wid=1200;
	opt.hei=800;
}
/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
	ang=0.0;

	shadedRenderer=YsGLSLCreateMonoColorPerPixShading3DRenderer();

	renderer[0]=YsGLSLCreateVariColorBillBoard3DRenderer();
	renderer[1]=YsGLSLCreateMonoColorBillBoard3DRenderer();
	useRenderer=0;

	bmpId=0;
	x=0.0;
	y=0.0;
	z=-15.0f;
	alpha=0.2f;
	radius=1.0f;
	lighting=1;


	nParticle=100;

	lightDir.Set(1.0,1.0,1.0);
	lightDirf[0]=lightDir.xf();
	lightDirf[1]=lightDir.yf();
	lightDirf[2]=lightDir.zf();
	lightDirf[3]=0.0f;

	specularColor[0]=0;
	specularColor[1]=0;
	specularColor[2]=0;
	specularColor[3]=0;
	
	ambient[0]=0.3f;
	ambient[1]=0.3f;
	ambient[2]=0.3f;
	ambient[3]=1.0f;

	useVbo=1;
	nFrame=0;
	t0=time(nullptr);

	FsChangeToProgramDir();

	bmp[0].LoadPng("explosion01.png");
	bmp[1].LoadPng("explosion02.png");
	bmp[2].LoadPng("explosion03.png");
	bmp[3].LoadPng("explosion04.png");
	bmp[4].LoadPng("explosion05.png");
	bmp[5].LoadPng("flash01.png");

	ClearEdge(bmp[0]);
	ClearEdge(bmp[1]);
	ClearEdge(bmp[2]);
	ClearEdge(bmp[3]);
	ClearEdge(bmp[4]);
	ClearEdge(bmp[5]);

	FsMakeAlphaMask(bmp[0]);
	FsMakeAlphaMask(bmp[1]);
	FsMakeAlphaMask(bmp[2]);
	FsMakeAlphaMask(bmp[3]);
	FsMakeAlphaMask(bmp[4]);
	FsMakeAlphaMask(bmp[5]);

	ysglBmp[0].Copy(bmp[0]);
	ysglBmp[1].Copy(bmp[1]);
	ysglBmp[2].Copy(bmp[2]);
	ysglBmp[3].Copy(bmp[3]);
	ysglBmp[4].Copy(bmp[4]);
	ysglBmp[5].Copy(bmp[5]);



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

	glBindTexture(GL_TEXTURE_2D,texId[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


	glGenBuffers(1,&vboId);

	GenerateCloud(vboId,nParticle,vtx,col,vOffset,texCoord,vtxPtr,colPtr,vOffsetPtr,texCoordPtr,radius,alpha);


	YsGLSLUse3DRenderer(shadedRenderer);
	lightDir.Normalize();

	YsGLSLSet3DRendererUniformLightDirectionfv(shadedRenderer,0,lightDirf);

	YsGLSLSet3DRendererSpecularColor(shadedRenderer,specularColor);

	YsGLSLSet3DRendererAmbientColor(shadedRenderer,ambient);
	YsGLSLEndUse3DRenderer(shadedRenderer);

	for(int i=0; i<2; ++i)
	{
		YsGLSLUse3DRenderer(renderer[i]);
		YsGLSLSet3DRendererBillBoardClippingType(renderer[i],YSGLSL_BILLBOARD_CLIPPING_CIRCLE);
		YsGLSLEndUse3DRenderer(renderer[i]);
	}

	x=0.0f;
	z=-15.0f;

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
		glBindTexture(GL_TEXTURE_2D,texId[bmpId]);
		break;
	case FSKEY_SPACE:
		useVbo=1-useVbo;
		printf("useVbo=%d\n",useVbo);
		break;
	case FSKEY_TAB:
		useRenderer=1-useRenderer;
		printf("useRenderer=%d\n",useRenderer);
		break;
	case FSKEY_L:
		lighting=1-lighting;
		break;
	case FSKEY_E:
		nParticle*=2;
		if(nParticle>maxNParticle)
		{
			nParticle=maxNParticle;
		}
		GenerateCloud(vboId,nParticle,vtx,col,vOffset,texCoord,vtxPtr,colPtr,vOffsetPtr,texCoordPtr,radius,alpha);
		printf("%d\n",nParticle);
		break;
	case FSKEY_D:
		nParticle/=2;
		if(100>nParticle)
		{
			nParticle=100;
		}
		GenerateCloud(vboId,nParticle,vtx,col,vOffset,texCoord,vtxPtr,colPtr,vOffsetPtr,texCoordPtr,radius,alpha);
		printf("%d\n",nParticle);
		break;
	case FSKEY_Q:
		alpha+=0.1f;
		GenerateCloud(vboId,nParticle,vtx,col,vOffset,texCoord,vtxPtr,colPtr,vOffsetPtr,texCoordPtr,radius,alpha);
		printf("%f\n",alpha);
		break;
	case FSKEY_A:
		alpha-=0.1f;
		GenerateCloud(vboId,nParticle,vtx,col,vOffset,texCoord,vtxPtr,colPtr,vOffsetPtr,texCoordPtr,radius,alpha);
		printf("%f\n",alpha);
		break;
	case FSKEY_W:
		radius+=1.0f;
		GenerateCloud(vboId,nParticle,vtx,col,vOffset,texCoord,vtxPtr,colPtr,vOffsetPtr,texCoordPtr,radius,alpha);
		printf("%f\n",radius);
		break;
	case FSKEY_S:
		radius-=1.0f;
		GenerateCloud(vboId,nParticle,vtx,col,vOffset,texCoord,vtxPtr,colPtr,vOffsetPtr,texCoordPtr,radius,alpha);
		printf("%f\n",radius);
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
	if(0!=FsGetKeyState(FSKEY_PAGEUP))
	{
		z+=0.1f;
	}
	if(0!=FsGetKeyState(FSKEY_PAGEDOWN))
	{
		z-=0.1f;
	}

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	needRedraw=false;

	if(0==lighting)
	{
		glBindTexture(GL_TEXTURE_2D,texId[bmpId]);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,bmp[bmpId].GetWidth(),bmp[bmpId].GetHeight(),1,GL_RGBA,GL_UNSIGNED_BYTE,bmp[bmpId].GetRGBABitmapPointer());
	}
	else
	{
		YsGLBitmap lit;
		lit.Copy(bmp[bmpId]);
		YsGLSLUse3DRenderer(shadedRenderer);
		YsGLSL3DRendererApplySphereMap(lit.w,lit.h,lit.rgba,shadedRenderer,0);
		YsGLSLEndUse3DRenderer(shadedRenderer);
		glBindTexture(GL_TEXTURE_2D,texId[bmpId]);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,lit.w,lit.h,1,GL_RGBA,GL_FLOAT,lit.rgba);
	}


	glClearColor(0.5f,0.5f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	int wid,hei;
	FsGetWindowSize(wid,hei);

	const double aspect=(double)wid/(double)hei;

	GLfloat modelview[16],projection[16];
	YsGLMakePerspectivefv(projection,YSGLPI/6.0,aspect,1.0,50.0);

	YsGLMakeIdentityfv(modelview);
	YsGLMultMatrixTranslationfv(modelview,x,y,z);
	YsGLMultMatrixRotationXZfv(modelview,ang);
	// ang+=0.02f;


	YsGLSLUse3DRenderer(renderer[useRenderer]);

	YsGLSLSet3DRendererAlphaCutOff(renderer[useRenderer],0.02f);
	YsGLSLSet3DRendererProjectionfv(renderer[useRenderer],projection);
	YsGLSLSet3DRendererModelViewfv(renderer[useRenderer],modelview);

	YsGLSLSet3DRendererBillBoardTransformType(renderer[useRenderer],YSGLSL_TEX_BILLBOARD_ORTHO);


	glDisable(GL_CULL_FACE);

	if(0==useRenderer)
	{
		if(0!=useVbo)
		{
			glBindBuffer(GL_ARRAY_BUFFER,vboId);
			YsGLSLDrawPrimitiveVtxColVoffsetTexcoordfv(
			    renderer[useRenderer],GL_TRIANGLES,nParticle*3,
			    (const GLfloat *)vtxPtr,
			    (const GLfloat *)colPtr,
			    (const GLfloat *)vOffsetPtr,
			    (const GLfloat *)texCoordPtr);
			glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		else
		{
			YsGLSLDrawPrimitiveVtxColVoffsetTexcoordfv(renderer[useRenderer],GL_TRIANGLES,nParticle*3,vtx,col,vOffset,texCoord);
		}
	}
	else
	{
		GLfloat white[4]={1.0f,1.0f,1.0f,alpha};
		YsGLSLSet3DRendererUniformColorfv(renderer[useRenderer],white);
		if(0!=useVbo)
		{
			glBindBuffer(GL_ARRAY_BUFFER,vboId);
			YsGLSLDrawPrimitiveVtxVoffsetTexcoordfv(
			    renderer[useRenderer],GL_TRIANGLES,nParticle*3,
			    (const GLfloat *)vtxPtr,
			    (const GLfloat *)vOffsetPtr,
			    (const GLfloat *)texCoordPtr);
			glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		else
		{
			YsGLSLDrawPrimitiveVtxVoffsetTexcoordfv(renderer[useRenderer],GL_TRIANGLES,nParticle*3,vtx,vOffset,texCoord);
		}
	}

	YsGLSLEndUse3DRenderer(renderer[useRenderer]);

	FsSwapBuffers();

	++nFrame;
	if(time(nullptr)!=t0)
	{
		printf("%d fps\n",nFrame);
		nFrame=0;
		t0=time(NULL);
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
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	for(int i=0; i<2; ++i)
	{
		YsGLSLDelete3DRenderer(renderer[i]);
	}

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

