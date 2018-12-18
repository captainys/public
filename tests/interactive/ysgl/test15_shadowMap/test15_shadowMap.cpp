/* ////////////////////////////////////////////////////////////

File Name: test15_shadowMap.cpp
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

int MakeSphere(GLfloat vtx[],GLfloat nom[],GLfloat col[],int nh,int nv)
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
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v0)*cos(h1));
			vtx[nVtx*3+1]=(GLfloat)(sin(v0));
			vtx[nVtx*3+2]=(GLfloat)(cos(v0)*sin(h1));
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v1)*cos(h0));
			vtx[nVtx*3+1]=(GLfloat)(sin(v1));
			vtx[nVtx*3+2]=(GLfloat)(cos(v1)*sin(h0));
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v0)*cos(h1));
			vtx[nVtx*3+1]=(GLfloat)(sin(v0));
			vtx[nVtx*3+2]=(GLfloat)(cos(v0)*sin(h1));
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v1)*cos(h1));
			vtx[nVtx*3+1]=(GLfloat)(sin(v1));
			vtx[nVtx*3+2]=(GLfloat)(cos(v1)*sin(h1));
			col[nVtx*4  ]=(GLfloat)(colIndex&1);
			col[nVtx*4+1]=(GLfloat)((colIndex>>1)&1);
			col[nVtx*4+2]=(GLfloat)((colIndex>>2)&1);
			col[nVtx*4+3]=1;
			nVtx++;

			vtx[nVtx*3  ]=(GLfloat)(cos(v1)*cos(h0));
			vtx[nVtx*3+1]=(GLfloat)(sin(v1));
			vtx[nVtx*3+2]=(GLfloat)(cos(v1)*sin(h0));
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

	int nSphereVtx;
	GLfloat sphereVtx[3*3*2*8*16],sphereNom[3*3*2*8*16],sphereCol[3*4*2*8*16];

	static const float cubeCol[4*6*6];
	static const float cubeNom[3*6*6];
	static const float cubeVtx[3*6*6];

	double rot;

	YsGLSL3DRenderer *renderer3d;
	YsGLSLBitmapRenderer *bitmapRenderer;

    int shadowTexWid,shadowTexHei;
	GLuint frameBufferIdent[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
	GLuint shadowTexIdent[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
	GLfloat shadowMapTfmFull[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE][16];

	int shadowMode[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
	int showShadowTextureIdent;

	GLfloat lightDistOffset;
	GLfloat lightDistScaling;

	void GetSceneTransformation(GLfloat projection[16],GLfloat modelview[16]);
	void GetShadowTransformation(GLfloat projection[16],GLfloat modelview[16],const GLfloat lightDir[3],int bufferIdent);
	void GenerateShadowTexture(GLuint &frameBufferIdent,GLuint &textureIdent);
	void GenerateShadowTexture(void);

	void DrawScene(const GLfloat viewTfm[16]);
	void DrawCube(GLfloat x,GLfloat y,GLfloat z,GLfloat h,GLfloat p,GLfloat b,const GLfloat viewTfm[16]);
	void DrawSphere(GLfloat x,GLfloat y,GLfloat z,const GLfloat viewTfm[16]);
	void DrawFloor(void);
	void DrawShadow(int shadowBufferId,const GLfloat lightDir[3]); // 0 or 1

	int CycleShadowMapUse(int shadowMapUse) const;

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

const float FsLazyWindowApplication::cubeVtx[3*6*6]=
{
	-1,-1,-1,
	 1,-1,-1,
	 1, 1,-1,

	 1, 1,-1,
	-1, 1,-1,
	-1,-1,-1,

	-1,-1, 1,
	 1,-1, 1,
	 1, 1, 1,

	 1, 1, 1,
	-1, 1, 1,
	-1,-1, 1,

	-1,-1,-1,
	 1,-1,-1,
	 1,-1, 1,

	 1,-1, 1,
	-1,-1, 1,
	-1,-1,-1,

	-1, 1,-1,
	 1, 1,-1,
	 1, 1, 1,

	 1, 1, 1,
	-1, 1, 1,
	-1, 1,-1,

	-1,-1,-1,
	-1, 1,-1,
	-1, 1, 1,

	-1, 1, 1,
	-1,-1, 1,
	-1,-1,-1,

	 1,-1,-1,
	 1, 1,-1,
	 1, 1, 1,

	 1, 1, 1,
	 1,-1, 1,
	 1,-1,-1,
};
const float FsLazyWindowApplication::cubeNom[3*6*6]=
{
	 0, 0,-1,
	 0, 0,-1,
	 0, 0,-1,

	 0, 0,-1,
	 0, 0,-1,
	 0, 0,-1,

	 0, 0, 1,
	 0, 0, 1,
	 0, 0, 1,

	 0, 0, 1,
	 0, 0, 1,
	 0, 0, 1,

	 0,-1, 0,
	 0,-1, 0,
	 0,-1, 0,

	 0,-1, 0,
	 0,-1, 0,
	 0,-1, 0,

	 0, 1, 0,
	 0, 1, 0,
	 0, 1, 0,

	 0, 1, 0,
	 0, 1, 0,
	 0, 1, 0,

	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,

	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,

	 1, 0, 0,
	 1, 0, 0,
	 1, 0, 0,

	 1, 0, 0,
	 1, 0, 0,
	 1, 0, 0,
};
const float FsLazyWindowApplication::cubeCol[4*6*6]=
{
	1,0,0,1,
	1,0,0,1,
	1,0,0,1,
	1,0,0,1,
	1,0,0,1,
	1,0,0,1,

	0,1,0,1,
	0,1,0,1,
	0,1,0,1,
	0,1,0,1,
	0,1,0,1,
	0,1,0,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,
	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,1,1,1,
	0,1,1,1,
	0,1,1,1,
	0,1,1,1,
	0,1,1,1,
	0,1,1,1,

	1,0,1,1,
	1,0,1,1,
	1,0,1,1,
	1,0,1,1,
	1,0,1,1,
	1,0,1,1,

	1,1,0,1,
	1,1,0,1,
	1,1,0,1,
	1,1,0,1,
	1,1,0,1,
	1,1,0,1,
};

void FsLazyWindowApplication::GetSceneTransformation(GLfloat projection[16],GLfloat modelview[16])
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	const double aspect=(double)wid/(double)hei;

	YsGLMakePerspectivefv(projection,YSGLPI/6.0,aspect,1.0,50.0);

	YsGLMakeIdentityfv(modelview);
	YsGLMultMatrixTranslationfv(modelview,0.0f,0.0f,-10.0f);
	YsGLMultMatrixRotationZYfv(modelview,-YSGLPI/4.0);
	YsGLMultMatrixRotationXZfv(modelview,rot);
	YsGLMultMatrixTranslationfv(modelview,0.0f,-2.0f,0.0f);

}
void FsLazyWindowApplication::GetShadowTransformation(GLfloat projection[16],GLfloat modelview[16],const GLfloat lightDir[3],int bufferIdent)
{
	GLfloat yVec[3];
	YsGLGetArbitraryPerpendicularVectorfv(yVec,lightDir);

	GLfloat xVec[3];
	YsGLCrossProductfv(xVec,yVec,lightDir);

	GLfloat rotMat[16]=
	{
		xVec[0],yVec[0],lightDir[0],  0,
		xVec[1],yVec[1],lightDir[1],  0,
		xVec[2],yVec[2],lightDir[2],  0,
		0,0,0,1
	};

	if(0==bufferIdent)
	{
		YsGLMakeOrthographicfv(projection,-1,1,-1,1,1.0,14);
	}
	else if(1==bufferIdent)
	{
#if 3<=YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE
		YsGLMakeOrthographicfv(projection,-2,2,-2,2,1.0,15);
#else
		YsGLMakeOrthographicfv(projection,-50,50,-50,50,1.0,100);
#endif
	}
	else
	{
		YsGLMakeOrthographicfv(projection,-50,50,-50,50,1.0,100);
	}

	YsGLMakeTranslationfv(modelview,0,0,-15.0f);
	YsGLMultMatrixfv(modelview,modelview,rotMat);
	YsGLMultMatrixTranslationfv(modelview,0,-2,0);
}

void FsLazyWindowApplication::GenerateShadowTexture(GLuint &frameBufferIdent,GLuint &textureIdent)
{
	shadowTexWid=1024;
	shadowTexHei=1024;

    glGenTextures(1,&textureIdent);
	glBindTexture(GL_TEXTURE_2D,textureIdent);
#if (!defined(GL_ES) || GL_ES==0) && !defined(GL_ES_VERSION_2_0)
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32,shadowTexWid,shadowTexHei,0,GL_DEPTH_COMPONENT,GL_FLOAT,nullptr);
#else
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,shadowTexWid,shadowTexHei,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,nullptr); // ES needs to use GL_UNSIGNED_INT
#endif
    

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1,&frameBufferIdent);
	glBindFramebuffer(GL_FRAMEBUFFER,frameBufferIdent);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,textureIdent,0);
#if (!defined(GL_ES) || GL_ES==0) && !defined(GL_ES_VERSION_2_0)
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE); // <- Without this, frame buffer status becomes 36060, which seems to be GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
#else
//	Do I really need a color-component in OpenGL ES2?
//    glGenTextures(1,&colorTexIdent);
//    glBindTexture(GL_TEXTURE_2D,colorTexIdent);
//    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,shadowTexWid,shadowTexHei,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);

//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

//    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,colorTexIdent,0);
#endif
    
	printf("%d %d\n",frameBufferIdent,textureIdent);

	auto sta=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(sta!=GL_FRAMEBUFFER_COMPLETE)
	{
		switch(sta)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
			break;
		//case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		//	printf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
		//	break;
#if (!defined(GL_ES) || GL_ES==0) && !defined(GL_ES_VERSION_2_0)
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			printf("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n");
			break;
#endif
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			printf("GL_FRAMEBUFFER_UNSUPPORTED\n");
			break;
		default:
			printf("Unknown error %d\n",sta);
			break;
		}

		printf("Cannot generate a frame buffer.\n");
		frameBufferIdent=0x7fffffff;
	}

	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void FsLazyWindowApplication::GenerateShadowTexture(void)
{
	for(int i=0; i<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE; ++i)
	{
		GenerateShadowTexture(frameBufferIdent[i],shadowTexIdent[i]);
	}
}

void FsLazyWindowApplication::DrawScene(const GLfloat viewTfm[16])
{
	YsGLSLUse3DRenderer(renderer3d);

	DrawCube(-1.5f,2,-1.5f,0,0,0,viewTfm);
	DrawCube( 1.5f,2, 1.5f,0,0,0,viewTfm);

	DrawSphere(3.0f,5.0f,0.0f,viewTfm);

	DrawSphere( 1.5f,2,-1.5f,viewTfm);
	DrawSphere(-1.5f,2, 1.5f,viewTfm);

	YsGLSLEndUse3DRenderer(renderer3d);
}
void FsLazyWindowApplication::DrawCube(GLfloat x,GLfloat y,GLfloat z,GLfloat h,GLfloat p,GLfloat b,const GLfloat viewTfm[16])
{
	GLfloat tfm[16];
	for(int i=0; i<16; ++i)
	{
		tfm[i]=viewTfm[i];
	}
	YsGLMultMatrixTranslationfv(tfm,x,y,z);
	YsGLMultMatrixRotationXZfv(tfm,h);
	YsGLMultMatrixRotationYZfv(tfm,p);
	YsGLMultMatrixRotationXYfv(tfm,b);

	YsGLSLSet3DRendererModelViewfv(renderer3d,tfm);
	YsGLSLDrawPrimitiveVtxNomColfv(renderer3d,GL_TRIANGLES,36,cubeVtx,cubeNom,cubeCol);
}
void FsLazyWindowApplication::DrawSphere(GLfloat x,GLfloat y,GLfloat z,const GLfloat viewTfm[16])
{
	GLfloat tfm[16];
	for(int i=0; i<16; ++i)
	{
		tfm[i]=viewTfm[i];
	}
	YsGLMultMatrixTranslationfv(tfm,x,y,z);

	YsGLSLSet3DRendererModelViewfv(renderer3d,tfm);
	YsGLSLDrawPrimitiveVtxNomColfv(renderer3d,GL_TRIANGLES,nSphereVtx,sphereVtx,sphereNom,sphereCol);
}

void FsLazyWindowApplication::DrawFloor(void)
{
	GLfloat floorVtx[6*3]=
	{
		-10,0,-10,
		 10,0,-10,
		 10,0, 10,

		 10,0, 10,
		-10,0, 10,
		-10,0,-10,
	};
	GLfloat floorNom[6*3]=
	{
		0,1,0,
		0,1,0,
		0,1,0,

		0,1,0,
		0,1,0,
		0,1,0,
	};
	GLfloat floorCol[6*4]=
	{
		0,0,1,1,
		0,0,1,1,
		0,0,1,1,
		0,0,1,1,
		0,0,1,1,
		0,0,1,1,
	};

	YsGLSLDrawPrimitiveVtxNomColfv(renderer3d,GL_TRIANGLES,6,floorVtx,floorNom,floorCol);
}

void FsLazyWindowApplication::DrawShadow(int bufferId,const GLfloat lightDir[3])
{
	glBindTexture(GL_TEXTURE_2D,0);  // This is needed in MacOSX.
	                                 // It actually makes sense.  Unless the texture is unbound before being written,
	                                 // GPU may still be using it for drawing a fragment, and as a result,
	                                 // drawing to the texture may fail.  It was exactly what was happening, and
	                                 // glClear could clear only a part of the texture unless the texture is unbound.

	glBindFramebuffer(GL_FRAMEBUFFER,frameBufferIdent[bufferId]);

	glViewport(0,0,shadowTexWid,shadowTexHei);

	GLfloat projection[16],modelview[16];
	GetShadowTransformation(projection,modelview,lightDir,bufferId);

	YsGLMultMatrixfv(shadowMapTfmFull[bufferId],projection,modelview);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	YsGLSLUse3DRenderer(renderer3d);
	for(int i=0; i<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE; ++i)
	{
		YsGLSLSet3DRendererUniformShadowMapMode(renderer3d,i,YSGLSL_SHADOWMAP_NONE);
	}
	YsGLSLSet3DRendererProjectionfv(renderer3d,projection);
	YsGLSLSet3DRendererModelViewfv(renderer3d,modelview);
	YsGLSLEndUse3DRenderer(renderer3d);

	DrawScene(modelview);

	glBindFramebuffer(GL_FRAMEBUFFER,0);

//YsGLSLSet3DRendererUniformShadowMapTexture(const struct YsGLSL3DRenderer *renderer,int shadowMapId,int textureIdent);
}

int FsLazyWindowApplication::CycleShadowMapUse(int shadowMapUse) const
{
	if(YSGLSL_SHADOWMAP_USE==shadowMapUse)
	{
		return YSGLSL_SHADOWMAP_DEBUG;
	}
	else if(YSGLSL_SHADOWMAP_DEBUG==shadowMapUse)
	{
		return YSGLSL_SHADOWMAP_NONE;
	}
	else
	{
		return YSGLSL_SHADOWMAP_USE;
	}
}


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
	shadowMode[0]=YSGLSL_SHADOWMAP_USE;
	shadowMode[1]=YSGLSL_SHADOWMAP_USE;
	shadowMode[2]=YSGLSL_SHADOWMAP_USE;
	showShadowTextureIdent=0;

	lightDistOffset=0.001;
	lightDistScaling=1.01;

	nSphereVtx=MakeSphere(sphereVtx,sphereNom,sphereCol,16,8);
	rot=0.0;

	renderer3d=YsGLSLCreateVariColorPerPixShading3DRenderer();
	bitmapRenderer=YsGLSLCreateBitmapRenderer();

	GenerateShadowTexture();
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}
	if(FSKEY_1==key)
	{
		showShadowTextureIdent=0;
		shadowMode[0]=CycleShadowMapUse(shadowMode[0]);
	}
	if(FSKEY_2==key)
	{
		showShadowTextureIdent=1;
		shadowMode[1]=CycleShadowMapUse(shadowMode[1]);
	}
	if(FSKEY_3==key && 3<=YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE)
	{
		showShadowTextureIdent=2;
		shadowMode[2]=CycleShadowMapUse(shadowMode[2]);
	}
	if(FSKEY_Q==key)
	{
		lightDistOffset=0.001;
		lightDistScaling=1.01;
	}
	if(FSKEY_A==key)
	{
		lightDistOffset=0;
		lightDistScaling=1;
	}
	needRedraw=true;

	rot+=YSGLPI/180.0;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	YsGLSLDelete3DRenderer(renderer3d);
	YsGLSLDeleteBitmapRenderer(bitmapRenderer);
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	needRedraw=false;

	GLfloat lightDir[3]={1,1,0};
	{
		GLfloat l=sqrt(lightDir[0]*lightDir[0]+lightDir[1]*lightDir[1]+lightDir[2]*lightDir[2]);
		lightDir[0]/=l;
		lightDir[1]/=l;
		lightDir[2]/=l;
	}

	for(int i=0; i<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE; ++i)
	{
		DrawShadow(i,lightDir);
	}


	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	int wid,hei;
	FsGetWindowSize(wid,hei);
	glViewport(0,0,wid,hei);


	GLfloat modelview[16],projection[16];

	if(0==FsGetKeyState(FSKEY_SHIFT))
	{
		GetSceneTransformation(projection,modelview);
	}
	else
	{
		GetShadowTransformation(projection,modelview,lightDir,showShadowTextureIdent);
	}


	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D,shadowTexIdent[0]);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D,shadowTexIdent[1]);

#if 3<=YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D,shadowTexIdent[2]);
#endif
	const int shadowActiveTexture[3]={4,5,6};


	YsGLSLUse3DRenderer(renderer3d);

	GLfloat modelviewInv[16],shadowMapTfm[16];
	YsGLInvertMatrixfv(modelviewInv,modelview);

	const GLfloat ambient[3]={0.2,0.2,0.2};
	YsGLSLSet3DRendererAmbientColor(renderer3d,ambient);

	for(int i=0; i<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE; ++i)
	{
		YsGLMultMatrixfv(shadowMapTfm,shadowMapTfmFull[i],modelviewInv);
		YsGLSLSet3DRendererUniformShadowMapMode(renderer3d,i,shadowMode[i]);
		YsGLSLSet3DRendererUniformShadowMapTexture(renderer3d,i,shadowActiveTexture[i]);
		YsGLSLSet3DRendererUniformShadowMapTransformation(renderer3d,i,shadowMapTfm);

		YsGLSLSet3DRendererUniformLightDistOffset(renderer3d,i,lightDistOffset);
		YsGLSLSet3DRendererUniformLightDistScaling(renderer3d,i,lightDistScaling);
	}


	YsGLSLSet3DRendererProjectionfv(renderer3d,projection);
	YsGLSLSet3DRendererModelViewfv(renderer3d,modelview);
	YsGLSLSet3DRendererUniformLightDirectionfv(renderer3d,0,lightDir);
	YsGLSLEndUse3DRenderer(renderer3d);

	DrawScene(modelview);

	YsGLSLUse3DRenderer(renderer3d);
	YsGLSLSet3DRendererModelViewfv(renderer3d,modelview);
	DrawFloor();
	YsGLSLEndUse3DRenderer(renderer3d);

	YsGLSLUseBitmapRenderer(bitmapRenderer);
	YsGLSLRenderTexture2D(bitmapRenderer,0,0,YSGLSL_HALIGN_LEFT,YSGLSL_VALIGN_TOP,256,256,shadowTexIdent[showShadowTextureIdent]);
	YsGLSLEndUseBitmapRenderer(bitmapRenderer);


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

