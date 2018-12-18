/* ////////////////////////////////////////////////////////////

File Name: ysglslcpp.cpp
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

#include "../ysglcpp.h"
#include "ysglslcpp.h"

static GLenum ConvertPrimType(YsGL::PRIMITIVE_TYPE primType)
{
	switch(primType)
	{
	default:
		break;
	case YsGL::POINTS:
		return GL_POINTS;
	case YsGL::LINES:
		return GL_LINES;
	case YsGL::LINE_STRIP:
		return GL_LINE_STRIP;
	case YsGL::LINE_LOOP:
		return GL_LINE_LOOP;
	case YsGL::TRIANGLES:
		return GL_TRIANGLES;
	case YsGL::TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	case YsGL::TRIANGLE_FAN:
		return GL_TRIANGLE_FAN;
	};
	return GL_POINTS;
}



YsGLSLRenderer *YsGLSLRenderer::currentRenderer=nullptr;


/* static */ void YsGLSLRenderer::CreateSharedRenderer(void)
{
	YsGLSLCreateSharedRenderer();
	YsGLSLCreateSharedBitmapFontRenderer();
}
/* static */ void YsGLSLRenderer::SharedRendererLost(void)
{
	YsGLSLSharedRendererLost();
	YsGLSLSharedBitmapFontRendererLost();
}
/* static */ void YsGLSLRenderer::DeleteSharedRenderer(void)
{
	YsGLSLDeleteSharedRenderer();
	YsGLSLDeleteSharedBitmapFontRenderer();
}


YsGLSLRenderer::YsGLSLRenderer(struct YsGLSL3DRenderer *r3ptr)
{
	if(nullptr==r3ptr)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  NULL renderer pointer is given.\n");
		printf("  Did you call YsGLSLRenderer::CreateSharedRenderer()?\n");
		exit(1);
	}
	if(nullptr!=currentRenderer)
	{
		currentRenderer->EndUseRenderer();
	}
	renderer3dPtr=r3ptr;
	renderer2dPtr=nullptr;
	rendererBitmapFontPtr=nullptr;
	YsGLSLUse3DRenderer(r3ptr);

	currentRenderer=this;
}

YsGLSLRenderer::YsGLSLRenderer(struct YsGLSLPlain2DRenderer *r2ptr)
{
	if(nullptr==r2ptr)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  NULL renderer pointer is given.\n");
		printf("  Did you call YsGLSLRenderer::CreateSharedRenderer()?\n");
		exit(1);
	}
	if(nullptr!=currentRenderer)
	{
		currentRenderer->EndUseRenderer();
	}
	renderer3dPtr=nullptr;
	renderer2dPtr=r2ptr;
	rendererBitmapFontPtr=nullptr;
	YsGLSLUsePlain2DRenderer(r2ptr);

	currentRenderer=this;
}

YsGLSLRenderer::YsGLSLRenderer(struct YsGLSLBitmapFontRenderer *rBmfPtr)
{
	if(nullptr==rBmfPtr)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  NULL renderer pointer is given.\n");
		printf("  Did you call YsGLSLRenderer::CreateSharedRenderer()?\n");
		exit(1);
	}
	if(nullptr!=currentRenderer)
	{
		currentRenderer->EndUseRenderer();
	}
	renderer3dPtr=nullptr;
	renderer2dPtr=nullptr;
	rendererBitmapFontPtr=rBmfPtr;
	YsGLSLUseBitmapFontRenderer(rBmfPtr);

	currentRenderer=this;
}

YsGLSLRenderer::~YsGLSLRenderer()
{
	EndUseRenderer();
}

void YsGLSLRenderer::EndUseRenderer(void)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLEndUse3DRenderer(renderer3dPtr);
		renderer3dPtr=nullptr;
	}
	if(nullptr!=renderer2dPtr)
	{
		YsGLSLEndUsePlain2DRenderer(renderer2dPtr);
		renderer2dPtr=nullptr;
	}
	if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLEndUseBitmapFontRenderer(rendererBitmapFontPtr);
		rendererBitmapFontPtr=nullptr;
	}

	if(this==currentRenderer)
	{
		currentRenderer=nullptr;
	}
	else
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("Warning: Current-Renderer has been mixed up somewhere.\n");
	}
}

YsGLSLRenderer::operator struct YsGLSL3DRenderer *()
{
	return renderer3dPtr;
}

void YsGLSLRenderer::SetProjection(const float mat[16])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererProjectionfv(renderer3dPtr,mat);
	}
	else if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLSetBitmapFontRendererProjectionfv(rendererBitmapFontPtr,mat);
	}
}
void YsGLSLRenderer::SetProjection(const double mat[16])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererProjectiondv(renderer3dPtr,mat);
	}
	else if(nullptr!=rendererBitmapFontPtr)
	{
		const float matf[16]=
		{
			(float)mat[ 0],(float)mat[ 1],(float)mat[ 2],(float)mat[ 3],
			(float)mat[ 4],(float)mat[ 5],(float)mat[ 6],(float)mat[ 7],
			(float)mat[ 8],(float)mat[ 9],(float)mat[10],(float)mat[11],
			(float)mat[12],(float)mat[13],(float)mat[14],(float)mat[15],
		};
		YsGLSLSetBitmapFontRendererProjectionfv(rendererBitmapFontPtr,matf);
	}
}
void YsGLSLRenderer::GetProjection(float mat[16]) const
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLGet3DRendererProjectionfv(mat,renderer3dPtr);
	}
}
void YsGLSLRenderer::GetProjection(double mat[16]) const
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLGet3DRendererProjectiondv(mat,renderer3dPtr);
	}
}

void YsGLSLRenderer::SetModelView(const float mat[16])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererModelViewfv(renderer3dPtr,mat);
	}
	else if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLSetBitmapFontRendererModelViewfv(rendererBitmapFontPtr,mat);
	}
}
void YsGLSLRenderer::SetModelView(const double mat[16])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererModelViewdv(renderer3dPtr,mat);
	}
	else if(nullptr!=rendererBitmapFontPtr)
	{
		const float matf[16]=
		{
			(float)mat[ 0],(float)mat[ 1],(float)mat[ 2],(float)mat[ 3],
			(float)mat[ 4],(float)mat[ 5],(float)mat[ 6],(float)mat[ 7],
			(float)mat[ 8],(float)mat[ 9],(float)mat[10],(float)mat[11],
			(float)mat[12],(float)mat[13],(float)mat[14],(float)mat[15],
		};
		YsGLSLSetBitmapFontRendererModelViewfv(rendererBitmapFontPtr,matf);
	}
}
void YsGLSLRenderer::GetModelView(float mat[16]) const
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLGet3DRendererModelViewfv(mat,renderer3dPtr);
	}
}
void YsGLSLRenderer::GetModelView(double mat[16]) const
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLGet3DRendererModelViewdv(mat,renderer3dPtr);
	}
}
void YsGLSLRenderer::SetUniformColor(const float col[4])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformColorfv(renderer3dPtr,col);
	}
	else if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLSetBitmapFontRendererColor4f(rendererBitmapFontPtr,(float)col[0],(float)col[1],(float)col[2],(float)col[3]);
	}
	else if(nullptr!=renderer2dPtr)
	{
		YsGLSLSetPlain2DRendererUniformColor(renderer2dPtr,col);
	}
}
void YsGLSLRenderer::SetUniformColor(const double col[4])
{
	if(nullptr!=renderer3dPtr)
	{
		const float cf[4]=
		{
			(float)col[0],(float)col[1],(float)col[2],(float)col[3]
		};
		YsGLSLSet3DRendererUniformColorfv(renderer3dPtr,cf);
	}
	else if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLSetBitmapFontRendererColor4f(rendererBitmapFontPtr,(float)col[0],(float)col[1],(float)col[2],(float)col[3]);
	}
}
void YsGLSLRenderer::SetUniformColor(const unsigned char col[4])
{
	const float cf[4]=
	{
		(float)col[0]/255.0f,(float)col[1]/255.0f,(float)col[2]/255.0f,(float)col[3]/255.0f
	};
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformColorfv(renderer3dPtr,cf);
	}
	else if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLSetBitmapFontRendererColor4f(rendererBitmapFontPtr,cf[0],cf[1],cf[2],cf[3]);
	}
}

void YsGLSLRenderer::SetUniformColor(const YsColor &col)
{
	const float colf[]=
	{
		col.Rf(),col.Gf(),col.Bf(),col.Af()
	};
	SetUniformColor(colf);
}

void YsGLSLRenderer::SetTextureType(int textureType)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererTextureType(renderer3dPtr,textureType);
	}
}

void YsGLSLRenderer::SetTextureIdent(int textureIdent)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererTextureIdentifier(renderer3dPtr,textureIdent);
	}
}

void YsGLSLRenderer::SetTextureTilingMatrix(const float mat[16])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformTextureTilingMatrixfv(renderer3dPtr,mat);
	}
}

void YsGLSLRenderer::SetBillboardClippingType(int clippingType)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererBillBoardClippingType(renderer3dPtr,clippingType);
	}
}

void YsGLSLRenderer::SetPointSpriteClippingType(int clippingType)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererPointSpriteClippingType(renderer3dPtr,clippingType);
	}
}

void YsGLSLRenderer::SetPointSpriteTextureCoordRange(float texCoordRange)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformTextureCoordRange(renderer3dPtr,texCoordRange);
	}
}

void YsGLSLRenderer::SetBillboardTransformationType(int billboardTfmType)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererBillBoardTransformType(renderer3dPtr,billboardTfmType);
	}
}

void YsGLSLRenderer::SetBillboardCenterAndDimension(const float center[3],const float dimension[2])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererBillBoardfv(renderer3dPtr,center,dimension);
	}
}

void YsGLSLRenderer::SetViewportDimension(float wid,float hei)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererViewportDimensionf(renderer3dPtr,wid,hei);
	}
	else if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLSetBitmapFontRendererViewportSize(rendererBitmapFontPtr,(int)wid,(int)hei);
	}
}

void YsGLSLRenderer::SetAlphaCutOff(float threshold)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererAlphaCutOff(renderer3dPtr,threshold);
	}
}

void YsGLSLRenderer::SetMarkerType(int markerType)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformMarkerType(renderer3dPtr,markerType);
	}
}

void YsGLSLRenderer::SetMarkerDimension(float dim)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformMarkerDimension(renderer3dPtr,dim);
	}
}

void YsGLSLRenderer::SetLightDirection(int lightIndex,const float dir[3])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformLightDirectionfv(renderer3dPtr,lightIndex,dir);
	}
}

void YsGLSLRenderer::SetLightDirectionInCameraCoordinate(int lightIndex,const float dir[3])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformLightDirectionInCameraCoordinatefv(renderer3dPtr,lightIndex,dir);
	}
}

void YsGLSLRenderer::SetLightColor(int lightIndex,const float RGB[3])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererLightColor(renderer3dPtr,lightIndex,RGB);
	}
}
void YsGLSLRenderer::SetAmbientLightColor(int lightIndex,const float RGB[3])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererAmbientColor(renderer3dPtr,RGB);
	}
}
void YsGLSLRenderer::SetSpecularColor(const float RGB[3])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererSpecularColor(renderer3dPtr,RGB);
	}
}
void YsGLSLRenderer::SetSpecularExponent(float exponent)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererSpecularExponent(renderer3dPtr,exponent);
	}
}
void YsGLSLRenderer::SetUniformFlashSize(float flashSize)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformFlashSize(renderer3dPtr,flashSize);
	}
}

void YsGLSLRenderer::SetUniformPointSize(float pointSize)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformPointSize(renderer3dPtr,pointSize);
	}
	if(nullptr!=renderer2dPtr)
	{
		YsGLSLSetPlain2DRendererUniformPointSize(renderer2dPtr,pointSize);
	}
}

void YsGLSLRenderer::SetFlashRadius(float r1,float r2)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererFlashRadius(renderer3dPtr,r1,r2);
	}
}
void YsGLSLRenderer::SetPointSizeMode(int pointSizeMode)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererPointSizeMode(renderer3dPtr,pointSizeMode);
	}
}
void YsGLSLRenderer::EnableFog(void)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformFogEnabled(renderer3dPtr,1);
	}
}
void YsGLSLRenderer::DisableFog(void)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformFogEnabled(renderer3dPtr,0);
	}
}
void YsGLSLRenderer::SetFogDensity(float density)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformFogDensity(renderer3dPtr,density);
	}
}
void YsGLSLRenderer::SetFogColor(const float RGBA[4])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformFogColor(renderer3dPtr,RGBA);
	}
}
void YsGLSLRenderer::EnableZOffset(void)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererZOffsetEnabled(renderer3dPtr,1);
	}
}
void YsGLSLRenderer::DisableZOffset(void)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererZOffsetEnabled(renderer3dPtr,0);
	}
}
void YsGLSLRenderer::SetZOffset(float offset)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererZOffset(renderer3dPtr,offset);
	}
}

void YsGLSLRenderer::DrawVtx(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const float vtx[]) const
{
	return DrawVtx(ConvertPrimType(primType),nVtx,vtx);
}
void YsGLSLRenderer::DrawVtx(GLenum primType,YSSIZE_T nVtx,const float vtx[]) const
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxfv(renderer3dPtr,primType,(GLsizei)nVtx,vtx);
	}
	else if(nullptr!=renderer2dPtr)
	{
		YsGLSLDrawPlain2DPrimitiveVtxfv(renderer2dPtr,primType,(GLsizei)nVtx,vtx);
	}
}

void YsGLSLRenderer::DrawVtxCol(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const float vtx[],const float col[]) const
{
	DrawVtxCol(ConvertPrimType(primType),nVtx,vtx,col);
}
void YsGLSLRenderer::DrawVtxCol(GLenum primType,YSSIZE_T nVtx,const float vtx[],const float col[]) const
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxColfv(renderer3dPtr,primType,(GLsizei)nVtx,vtx,col);
	}
	else if(nullptr!=renderer2dPtr)
	{
		YsGLSLDrawPlain2DPrimitiveVtxColfv(renderer2dPtr,primType,(GLsizei)nVtx,vtx,col);
	}
}

void YsGLSLRenderer::DrawVtxTexCoordCol(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat col[])
{
	DrawVtxTexCoordCol(ConvertPrimType(primType),nVtx,vtx,texCoord,col);
}
void YsGLSLRenderer::DrawVtxTexCoordCol(GLenum primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat col[])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxTexCoordColfv(renderer3dPtr,primType,(GLsizei)nVtx,vtx,texCoord,col);
	}
}

void YsGLSLRenderer::DrawVtxNom(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const float vtx[],const float nom[]) const
{
	DrawVtxNom(ConvertPrimType(primType),nVtx,vtx,nom);
}
void YsGLSLRenderer::DrawVtxNom(GLenum primType,YSSIZE_T nVtx,const float vtx[],const float nom[]) const
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxNomfv(renderer3dPtr,primType,(GLsizei)nVtx,vtx,nom);
	}
}

void YsGLSLRenderer::DrawVtxTexCoordNom(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[]) const
{
	DrawVtxTexCoordNom(ConvertPrimType(primType),nVtx,vtx,texCoord,nom);
}
void YsGLSLRenderer::DrawVtxTexCoordNom(GLenum primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[]) const
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxTexCoordNomfv(renderer3dPtr,primType,(GLsizei)nVtx,vtx,texCoord,nom);
	}
}

void YsGLSLRenderer::DrawVtxNomCol(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat nom[],const GLfloat col[])
{
	DrawVtxNomCol(ConvertPrimType(primType),nVtx,vtx,nom,col);
}
void YsGLSLRenderer::DrawVtxNomCol(GLenum primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat nom[],const GLfloat col[])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxNomColfv(renderer3dPtr,primType,(GLsizei)nVtx,vtx,nom,col);
	}
}

void YsGLSLRenderer::DrawVtxTexCoordNomCol(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[],const GLfloat col[])
{
	DrawVtxTexCoordNomCol(ConvertPrimType(primType),nVtx,vtx,texCoord,nom,col);
}
void YsGLSLRenderer::DrawVtxTexCoordNomCol(GLenum primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[],const GLfloat col[])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxTexCoordNomColfv(renderer3dPtr,primType,(GLsizei)nVtx,vtx,texCoord,nom,col);
	}
}

void YsGLSLRenderer::DrawVtxColVoffsetTexCoord(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],     // 3*nVtx
	    const GLfloat col[],     // 4*nVtx
	    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
	    const GLfloat texCoord[] // 2*nVtx
	)
{
	DrawVtxColVoffsetTexCoord(ConvertPrimType(primType),nVtx,
	    vtx,     // 3*nVtx
	    col,     // 4*nVtx
	    vOffset, // 3*nVtx   Let it modify Z as well
	    texCoord // 2*nVtx
	);
}
void YsGLSLRenderer::DrawVtxColVoffsetTexCoord(GLenum primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],     // 3*nVtx
	    const GLfloat col[],     // 4*nVtx
	    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
	    const GLfloat texCoord[] // 2*nVtx
	)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxColVoffsetTexcoordfv(
		    renderer3dPtr,primType,(GLsizei)nVtx,vtx,col,vOffset,texCoord);
	}
}

void YsGLSLRenderer::DrawVtxVoffsetTexCoord(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],     // 3*nVtx
	    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
	    const GLfloat texCoord[] // 2*nVtx
	)
{
	DrawVtxVoffsetTexCoord(ConvertPrimType(primType),nVtx,
	    vtx,     // 3*nVtx
	    vOffset, // 3*nVtx   Let it modify Z as well
	    texCoord // 2*nVtx
	);
}
void YsGLSLRenderer::DrawVtxVoffsetTexCoord(GLenum primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],     // 3*nVtx
	    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
	    const GLfloat texCoord[] // 2*nVtx
	)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxVoffsetTexcoordfv(
		    renderer3dPtr,primType,(GLsizei)nVtx,vtx,vOffset,texCoord);
	}
}

void YsGLSLRenderer::DrawVtxColPointSize(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],
	    const GLfloat color[],
	    const GLfloat pointSize[])
{
	DrawVtxColPointSize(ConvertPrimType(primType),nVtx,
	    vtx,
	    color,
	    pointSize);
}
void YsGLSLRenderer::DrawVtxColPointSize(GLenum primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],
	    const GLfloat color[],
	    const GLfloat pointSize[])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxColPointSizefv(renderer3dPtr,primType,(GLsizei)nVtx,vtx,color,pointSize);
	}
}

void YsGLSLRenderer::DrawVtxTexCoordColPointSize(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],const GLfloat texCoord[],const GLfloat color[],const GLfloat pointSize[])
{
	DrawVtxTexCoordColPointSize(ConvertPrimType(primType),nVtx,vtx,texCoord,color,pointSize);
}
void YsGLSLRenderer::DrawVtxTexCoordColPointSize(GLenum primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],const GLfloat texCoord[],const GLfloat color[],const GLfloat pointSize[])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxTexCoordColPointSizefv(renderer3dPtr,primType,(GLsizei)nVtx,vtx,texCoord,color,pointSize);
	}
}
void YsGLSLRenderer::DrawVtxTexCoordPointSize(GLenum primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],const GLfloat texCoord[],const GLfloat pointSize[])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLDrawPrimitiveVtxTexCoordPointSizefv(renderer3dPtr,primType,(GLsizei)nVtx,vtx,texCoord,pointSize);
	}
}

////////////////////////////////////////////////////////////

void YsGLSLRenderer::SetViewportOrigin(int viewportOrigin)
{
	if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLSetBitmapFontRendererViewportOrigin(rendererBitmapFontPtr,(YSGLSL_BMPFONT_VIEWPORT_ORIGIN)viewportOrigin);
	}
}

void YsGLSLRenderer::SetFontFirstLineAlignment(int alignment)
{
	if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLSetBitmapFontRendererFirstLineAlignment(rendererBitmapFontPtr,(YSGLSL_BMPFONT_FIRST_LINE_ALIGNMENT)alignment);
	}
}

void YsGLSLRenderer::RequestFontSize(int wid,int hei)
{
	if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLBitmapFontRendererRequestFontSize(rendererBitmapFontPtr,wid,hei);
	}
}

void YsGLSLRenderer::DrawString(int x,int y,const char str[])
{
	if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLRenderBitmapFontString2D(rendererBitmapFontPtr,x,y,str);
	}
}

void YsGLSLRenderer::DrawString(float x,float y,float z,const char str[])
{
	if(nullptr!=rendererBitmapFontPtr)
	{
		YsGLSLRenderBitmapFontString3D(rendererBitmapFontPtr,x,y,z,str);
	}
}

void YsGLSLRenderer::SetUniformShadowMapMode(int shadowMapId,int shadowMapMode)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformShadowMapMode(renderer3dPtr,shadowMapId,shadowMapMode);
	}
}
void YsGLSLRenderer::SetUniformShadowMapTexture(int shadowMapId,int texIdent)
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformShadowMapTexture(renderer3dPtr,shadowMapId,texIdent);
	}
}
void YsGLSLRenderer::SetUniformShadowMapTransformation(int shadowMapId,const float lightTfm[16])
{
	if(nullptr!=renderer3dPtr)
	{
		YsGLSLSet3DRendererUniformShadowMapTransformation(renderer3dPtr,shadowMapId,lightTfm);
	}
}


////////////////////////////////////////////////////////////

YsGLSL3DRendererBase::YsGLSL3DRendererBase(struct YsGLSL3DRenderer *r3ptr) : YsGLSLRenderer(r3ptr)
{
}

YsGLSL3DRendererBase::operator struct YsGLSL3DRenderer *()
{
	return renderer3dPtr;
}

YsGLSLPlain3DRenderer::YsGLSLPlain3DRenderer() : YsGLSL3DRendererBase(YsGLSLSharedVariColor3DRenderer())
{
}

YsGLSLPerVertexShaded3DRenderer::YsGLSLPerVertexShaded3DRenderer() : YsGLSL3DRendererBase(YsGLSLSharedVariColorPerVtxShading3DRenderer())
{
}

YsGLSLShaded3DRenderer::YsGLSLShaded3DRenderer() : YsGLSL3DRendererBase(YsGLSLSharedVariColorPerPixShading3DRenderer())
{
}

YsGLSLBillboard3DRenderer::YsGLSLBillboard3DRenderer() : YsGLSL3DRendererBase(YsGLSLSharedVariColorBillBoard3DRenderer())
{
}

YsGLSLMarker3DRenderer::YsGLSLMarker3DRenderer() : YsGLSL3DRendererBase(YsGLSLSharedVariColorMarkerByPointSprite3DRenderer())
{
}

YsGLSLFlash3DRenderer::YsGLSLFlash3DRenderer() : YsGLSL3DRendererBase(YsGLSLSharedFlashByPointSprite3DRenderer())
{
}

YsGLSLPointSprite3DRenderer::YsGLSLPointSprite3DRenderer() : YsGLSL3DRendererBase(YsGLSLSharedVariColorPointSprite3DRenderer())
{
}

////////////////////////////////////////////////////////////

YsGLSLBitmapFontRendererClass::YsGLSLBitmapFontRendererClass() : YsGLSLRenderer(YsGLSLSharedBitmapFontRenderer())
{
}

void YsGLSL2DRendererBase::UseWindowCoordinateTopLeftAsOrigin(void)
{
	if(this==currentRenderer)
	{
		if(nullptr!=renderer2dPtr)
		{
			YsGLSLUseWindowCoordinateInPlain2DDrawing(renderer2dPtr,1);
		}
	}
	else
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Error!  Trying to do something with non-current renderer.\n");
	}
}
void YsGLSL2DRendererBase::UseWindowCoordinateBottomLeftAsOrigin(void)
{
	if(this==currentRenderer)
	{
		if(nullptr!=renderer2dPtr)
		{
			YsGLSLUseWindowCoordinateInPlain2DDrawing(renderer2dPtr,0);
		}
	}
	else
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Error!  Trying to do something with non-current renderer.\n");
	}
}

void YsGLSL2DRendererBase::DontUseWindowCoordinate(void)
{
	if(this==currentRenderer)
	{
		if(nullptr!=renderer2dPtr)
		{
			YsGLSLDontUseWindowCoordinateInPlain2DDrawing(renderer2dPtr);
		}
	}
	else
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Error!  Trying to do something with non-current renderer.\n");
	}
}

////////////////////////////////////////////////////////////

YsGLSL2DRendererBase::YsGLSL2DRendererBase(struct YsGLSLPlain2DRenderer *r2ptr) : YsGLSLRenderer(r2ptr)
{
}

YsGLSL2DRenderer::YsGLSL2DRenderer() : YsGLSL2DRendererBase(YsGLSLSharedPlain2DRenderer())
{
}
