/* ////////////////////////////////////////////////////////////

File Name: ysglsl3ddrawing.c
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
#include <stdlib.h>
#include <math.h>

#include "ysglsl3ddrawing.h"
#include "ysglslutil.h"
#include "ysglmath.h"
#include "ysglbitmap.h"
#include "ysgldef.h"
#include "ysglslprogram.h"

/* Force Visual C++ to type-mismatching error. */
#pragma warning( error : 4028)
#pragma warning( error : 4047)


/*!
Memo: 2014/07/15
  Until one of my friends kindly told that glVertexAttrib function sets generic attribute in OpenGL ES 2.0,
  I didn't know that there was such thing called generic attribute.  :-P
  
  That's why I wrote single color (mono color) and gradation (vari color) programs separately.

  I'm going to unify them in the future, to simplify things.


2015/11/22
  Found another article.
    http://stackoverflow.com/questions/13348885/why-does-opengl-drawing-fail-when-vertex-attrib-array-zero-is-disabled
  Maybe that's it.  If I make sure one of the required attributes use attribute position 0, 
  maybe I can use generic attributes on a defective OpenGL drivers.
 
  Potentially eliminates the necessity to have vari-color and mono-color renderers.
  Also I should be able to add view-offset and pixel-offset on all shader programs.
*/


struct YsGLSL3DRenderer
{
	int inUse;

	// Shader and Program
	GLuint programId;
	GLuint vertexShaderId,fragmentShaderId;

	// Uniforms
	GLint uniformProjectionPos;
	GLint uniformModelViewPos;
	GLint uniformOffsetTransformPos;

	GLint uniformViewportWidthPos;
	GLint uniformViewportHeightPos;
	GLint uniformViewportOriginPos;

	GLint uniformMarkerTypePos;              // 0:Plain    1:Filled Circle   2:Filled Rect

	GLint uniformLightEnabledPos;     // -1 if no lighting   bool lightEnabled[YSGLMAXNUMLIGHT]
	GLint uniformLightPositionPos;    // -1 if no lighting   vec4 lightPos[YSGLMAXNUMLIGHT]
	GLint uniformLightColorPos;       // -1 if no lighting   vec4 lightColor[YSGLMAXNUMLIGHT]
	GLint uniformSpecularColorPos;    // -1 if no lighting   vec3 specular
	GLint uniformSpecularExponentPos; // -1 if no lighting   float specularExponent
	GLint uniformAmbientColorPos;     // 
	GLint uniformAlphaCutOffPos;

	GLint uniformFogEnabledPos;
	GLint uniformFogDensityPos;
	GLint uniformFogColorPos;

	GLint uniformDimensionPos;  // For flash and marker renderers
	GLint uniformRadius1Pos;  // For flash renderer
	GLint uniformRadius2Pos;  // For flash renderer
	GLint uniformMaxIntensityPos;  // For flash renderer
	GLint uniformAngleIntervalPos;  // For flash renderer
	GLint uniformAngleOffsetPos;  // For flash renderer
	GLint uniformExponentPos;  // For flash renderer

	GLint uniformColorPos;       // -1 if VariColor
	GLfloat uniformColor[4];

	// Texture mapping >>
	GLint uniformTextureTypePos;
	GLint uniformUseNegativeOneToPositiveOnTexCoordPos;
	GLint uniformUseTexturePos;
	GLint uniformTextureSampleCoeff;

	GLint uniformTextureTileTransformPos;
	GLint uniformTexturePos;

	// GLint uniformBillBoardClippingTypePos;  2015/11/22 billBoardClippingType and pointClippingType are merged.
	GLint uniformBillBoardCenterPos;
	GLint uniformBillBoardTransformTypePos;
	GLint uniformBillBoardDimensionPos;
	// Texture mapping <<

	GLint uniformUseZOffsetPos;
	GLint uniformZOffsetPos;

	GLint uniformUsePointSizeInPixelPos;
	GLint uniformUsePointSizeIn3DPos;
	GLint uniformUsePointClippingCirclePos;
	// GLint uniformTextureEnabledPos;  2015/11/22 Merged with textureType
	GLint uniformTexCoordRangePos;

	// For programs that support uniformPointSize;
	GLint uniformPointSizePos;

	// Memo for adding uniforms:
	//   Uniform position must be set to -1 in YsGLSLCreateAndInitialize3DRenderer

	// Shadow map
	GLfloat useShadowMap[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
	GLfloat testShadowMap[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
	GLuint shadowMapTexture[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
	GLfloat shadowMapTransform[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE][16];
	GLfloat lightDistOffset[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
	GLfloat lightDistScale[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];

	GLint uniformUseShadowMapPos;
	GLint uniformTestShadowMapPos;
	GLint uniformShadowMapTransformPos;
	GLint uniformShadowMapTexturePos;
	GLint uniformLightDistOffsetPos;
	GLint uniformLightDistScalePos;



	// Attributes
	GLint attribVertexPos;
	GLint attribColorPos;        // -1 if MonoColor
	YSBOOL colorArrayEnabled;
	GLint attribNormalPos;       // -1 if no normal
	GLint attribTexCoordPos;     // -1 if no tex coord
	YSBOOL texCoordArrayEnabled;
	GLint attribViewOffsetPos;   // -1 if no view offset (offset vector in the view coordinate)
	GLint attribPixelOffsetPos;  // -1 if no pixel offset
	GLint attribPointSizePos;

	// Memo for adding attributes:
	//   Attribute position must be set to -1 in YsGLSLCreateAndInitialize3DRenderer
	//   Attributes array needs to be enabled in YsGLSLUse3DRenderer and disabled in YsGLSLEndUse3DRenderer.
	//   When an attribute is added, make sure enabling/disabling are also added in the above functions.


	// Saving transformations
	double projectionMatrix[16];
	double modelViewMatrix[16];

	// Saving lighting
	GLfloat ambientColor[3];
	GLfloat lightEnabled[YSGLSL_MAX_NUM_LIGHT];
	GLfloat lightPosition[4*YSGLSL_MAX_NUM_LIGHT];
	GLfloat lightColor[3*YSGLSL_MAX_NUM_LIGHT];
	GLfloat specularExponent;
	GLfloat specularColor[3];

	// Saving fog density
	GLfloat fogDensity;
};

static struct YsGLSL3DRenderer *YsGLSLCreateAndInitialize3DRenderer(void)
{
	struct YsGLSL3DRenderer *renderer;
	renderer=(struct YsGLSL3DRenderer *)malloc(sizeof(struct YsGLSL3DRenderer));

	renderer->inUse=0;

	renderer->programId=0;
	renderer->vertexShaderId=0;
	renderer->fragmentShaderId=0;

	renderer->uniformProjectionPos=-1;
	renderer->uniformModelViewPos=-1;
	renderer->uniformOffsetTransformPos=-1;
	renderer->uniformAlphaCutOffPos=-1;

	renderer->uniformViewportWidthPos=-1;
	renderer->uniformViewportHeightPos=-1;
	renderer->uniformViewportOriginPos=-1;

	renderer->uniformMarkerTypePos=-1;

	renderer->uniformLightEnabledPos=-1;
	renderer->uniformLightPositionPos=-1;
	renderer->uniformLightColorPos=-1;
	renderer->uniformSpecularColorPos=-1;
	renderer->uniformSpecularExponentPos=-1;
	renderer->uniformAmbientColorPos=-1;

	renderer->uniformFogEnabledPos=-1;
	renderer->uniformFogDensityPos=-1;
	renderer->uniformFogColorPos=-1;

	renderer->uniformDimensionPos=-1;
	renderer->uniformRadius1Pos=-1;
	renderer->uniformRadius2Pos=-1;
	renderer->uniformMaxIntensityPos=-1;
	renderer->uniformAngleIntervalPos=-1;
	renderer->uniformAngleOffsetPos=-1;
	renderer->uniformExponentPos=-1;

	renderer->uniformColorPos=-1;
	renderer->uniformColor[0]=0.0f;
	renderer->uniformColor[1]=0.0f;
	renderer->uniformColor[2]=0.0f;
	renderer->uniformColor[3]=1.0f;

	renderer->uniformTextureTypePos=-1;
	renderer->uniformUseTexturePos=-1;
	renderer->uniformTextureSampleCoeff=-1;
	renderer->uniformUseNegativeOneToPositiveOnTexCoordPos=-1;

	renderer->uniformTextureTileTransformPos=-1;
	renderer->uniformTexturePos=-1;

	// renderer->uniformBillBoardClippingTypePos=-1;  2015/11/22 billBoardClippingType and pointClippingType are merged.
	renderer->uniformBillBoardCenterPos=-1;
	renderer->uniformBillBoardTransformTypePos=-1;
	renderer->uniformBillBoardDimensionPos=-1;


	renderer->uniformUseZOffsetPos=-1;
	renderer->uniformZOffsetPos=-1;

	renderer->uniformUsePointSizeInPixelPos=-1;
	renderer->uniformUsePointSizeIn3DPos=-1;
	renderer->uniformUsePointClippingCirclePos=-1;
	renderer->uniformTexCoordRangePos=-1;

	renderer->uniformPointSizePos=-1;


	{
		int i,j;
		for(i=0; i<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE; ++i)
		{
			renderer->useShadowMap[i]=0;
			renderer->testShadowMap[i]=0;
			renderer->shadowMapTexture[i]=4+i;
			for(j=0; j<16; ++j)
			{
				renderer->shadowMapTransform[i][j]=0;;
			}
			renderer->lightDistOffset[i]=0.001f;
			renderer->lightDistScale[i]=1.01f;

			renderer->uniformUseShadowMapPos=-1;
			renderer->uniformTestShadowMapPos=-1;
			renderer->uniformShadowMapTransformPos=-1;
			renderer->uniformShadowMapTexturePos=-1;
			renderer->uniformLightDistOffsetPos=-1;
			renderer->uniformLightDistScalePos=-1;
		}
	}



	renderer->attribVertexPos=-1;
	renderer->attribColorPos=-1;
	renderer->colorArrayEnabled=YSFALSE;
	renderer->attribNormalPos=-1;
	renderer->attribTexCoordPos=-1;
	renderer->texCoordArrayEnabled=YSFALSE;
	renderer->attribViewOffsetPos=-1;
	renderer->attribPixelOffsetPos=-1;
	renderer->attribPointSizePos=-1;



	renderer->fogDensity=1.0;

	return renderer;
}

// Returns the error count.
static int YsGLSLGetTextureUniformPos(struct YsGLSL3DRenderer *renderer,const char calledFrom[])
{
	int ec=0;

	renderer->uniformTextureTypePos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"textureType");
	renderer->uniformUseTexturePos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"useTexture");
	renderer->uniformTextureSampleCoeff=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"textureSampleCoeff");
	renderer->uniformUseNegativeOneToPositiveOnTexCoordPos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"useNegativeOneToPositiveOneTexCoord");

	renderer->uniformTexturePos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"textureIdent");

	renderer->uniformTextureTileTransformPos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"textureTileTransform");

	renderer->uniformBillBoardCenterPos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"billBoardCenter");
	renderer->uniformBillBoardTransformTypePos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"billBoardTransformType");
	renderer->uniformBillBoardDimensionPos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"billBoardDimension");

	return ec;
}

// Returns the error count.
static int YsGLSLGetShadowMapUniformPos(struct YsGLSL3DRenderer *renderer,const char calledFrom[])
{
	int ec=0;
	renderer->uniformUseShadowMapPos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"useShadowMap");
	renderer->uniformTestShadowMapPos=YsGLSLGetUniformLocation(NULL,calledFrom,renderer->programId,"useShadowTest");
	renderer->uniformShadowMapTransformPos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"shadowMapTransform");
	renderer->uniformShadowMapTexturePos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"shadowMapTexture");
	renderer->uniformLightDistOffsetPos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"lightDistOffset");
	renderer->uniformLightDistScalePos=YsGLSLGetUniformLocation(&ec,calledFrom,renderer->programId,"lightDistScale");

	return ec;
}

static void YsGLSLTexCoordIsNotGivenAsArray(struct YsGLSL3DRenderer *renderer)
{
	if(0<=renderer->attribTexCoordPos && YSTRUE==renderer->texCoordArrayEnabled)
	{
		glDisableVertexAttribArray(renderer->attribTexCoordPos);
		renderer->texCoordArrayEnabled=YSFALSE;
		glVertexAttrib2f(renderer->attribTexCoordPos,0.0f,0.0f);
	}
}

static void YsGLSLTexCoordIsGivenAsArray(struct YsGLSL3DRenderer *renderer)
{
	if(0<=renderer->attribTexCoordPos && YSTRUE!=renderer->texCoordArrayEnabled)
	{
		glEnableVertexAttribArray(renderer->attribTexCoordPos);
		renderer->texCoordArrayEnabled=YSTRUE;
	}
}

static void YsGLSLColorIsNotGivenAsArray(struct YsGLSL3DRenderer *renderer)
{
	if(0<=renderer->attribColorPos)
	{
		if(YSTRUE==renderer->colorArrayEnabled)
		{
			glDisableVertexAttribArray(renderer->attribColorPos);
			renderer->colorArrayEnabled=YSFALSE;
		}
		glVertexAttrib4f(renderer->attribColorPos,renderer->uniformColor[0],renderer->uniformColor[1],renderer->uniformColor[2],renderer->uniformColor[3]);
	}
}

static void YsGLSLColorIsGivenAsArray(struct YsGLSL3DRenderer *renderer)
{
	if(0<=renderer->attribColorPos && YSTRUE!=renderer->colorArrayEnabled)
	{
		glEnableVertexAttribArray(renderer->attribColorPos);
		renderer->colorArrayEnabled=YSTRUE;
	}
}

void YsGLSLDelete3DRenderer(struct YsGLSL3DRenderer *renderer)
{
	if(NULL!=renderer)
	{
		glDeleteProgram(renderer->programId);
		glDeleteShader(renderer->vertexShaderId);
		glDeleteShader(renderer->fragmentShaderId);

		free(renderer);
	}
}

void YsGLSLSet3DRendererProjectionfv(struct YsGLSL3DRenderer *renderer,const GLfloat mat[16])
{
	if(NULL!=renderer)
	{
		int i;
		glUniformMatrix4fv(renderer->uniformProjectionPos,1,GL_FALSE,mat);
		for(i=0; i<16; ++i)
		{
			renderer->projectionMatrix[i]=mat[i];
		}
	}
}

void YsGLSLSet3DRendererProjectiondv(struct YsGLSL3DRenderer *renderer,const double mat[16])
{
	if(NULL!=renderer)
	{
		int i;
		GLfloat matf[16];
		for(i=0; i<16; ++i)
		{
			matf[i]=(GLfloat)mat[i];
			renderer->projectionMatrix[i]=mat[i];
		}
		glUniformMatrix4fv(renderer->uniformProjectionPos,1,GL_FALSE,matf);
	}
}

void YsGLSLGet3DRendererProjectionfv(GLfloat mat[16],const struct YsGLSL3DRenderer *renderer)
{
	if(NULL!=renderer)
	{
		int i;
		for(i=0; i<16; ++i)
		{
			mat[i]=(GLfloat)renderer->projectionMatrix[i];
		}
	}
}

void YsGLSLGet3DRendererProjectiondv(double mat[16],const struct YsGLSL3DRenderer *renderer)
{
	if(NULL!=renderer)
	{
		int i;
		for(i=0; i<16; ++i)
		{
			mat[i]=renderer->projectionMatrix[i];
		}
	}
}

void YsGLSLSet3DRendererModelViewfv(struct YsGLSL3DRenderer *renderer,const GLfloat mat[16])
{
	if(NULL!=renderer)
	{
		int i;
		for(i=0; i<16; ++i)
		{
			renderer->modelViewMatrix[i]=mat[i];
		}
		glUniformMatrix4fv(renderer->uniformModelViewPos,1,GL_FALSE,mat);
	}
}

void YsGLSLSet3DRendererModelViewdv(struct YsGLSL3DRenderer *renderer,const double mat[16])
{
	if(NULL!=renderer)
	{
		int i;
		GLfloat matf[16];
		for(i=0; i<16; ++i)
		{
			matf[i]=(GLfloat)mat[i];
			renderer->modelViewMatrix[i]=mat[i];
		}
		glUniformMatrix4fv(renderer->uniformModelViewPos,1,GL_FALSE,matf);
	}
}

void YsGLSLGet3DRendererModelViewfv(GLfloat mat[16],const struct YsGLSL3DRenderer *renderer)
{
	if(NULL!=renderer)
	{
		int i;
		for(i=0; i<16; ++i)
		{
			mat[i]=(GLfloat)renderer->modelViewMatrix[i];
		}
	}
}

void YsGLSLGet3DRendererModelViewdv(double mat[16],const struct YsGLSL3DRenderer *renderer)
{
	if(NULL!=renderer)
	{
		int i;
		for(i=0; i<16; ++i)
		{
			mat[i]=renderer->modelViewMatrix[i];
		}
	}
}

YSRESULT YsGLSLSet3DRendererUniformColorfv(struct YsGLSL3DRenderer *renderer,const GLfloat color[4])
{
	if(NULL!=renderer)
	{
		renderer->uniformColor[0]=color[0];
		renderer->uniformColor[1]=color[1];
		renderer->uniformColor[2]=color[2];
		renderer->uniformColor[3]=color[3];
		if(0<=renderer->uniformColorPos)
		{
			glUniform4fv(renderer->uniformColorPos,1,color);
			return YSOK;
		}
		else if(0<=renderer->attribColorPos)
		{
			// May be used as a generic color.
			return YSOK;
		}
		else
		{
			return YSERR;
		}
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererViewportDimensionf(const struct YsGLSL3DRenderer *renderer,const GLfloat width,const GLfloat height)
{
	if(NULL!=renderer && (0<=renderer->uniformViewportWidthPos || 0<=renderer->uniformViewportHeightPos))
	{
		if(0<=renderer->uniformViewportWidthPos)
		{
			glUniform1f(renderer->uniformViewportWidthPos,width);
		}
		if(0<=renderer->uniformViewportHeightPos)
		{
			glUniform1f(renderer->uniformViewportHeightPos,height);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererAlphaCutOff(const struct YsGLSL3DRenderer *renderer,GLfloat alphaCutOff)
{
	if(NULL!=renderer && 0<=renderer->uniformAlphaCutOffPos)
	{
		glUniform1f(renderer->uniformAlphaCutOffPos,alphaCutOff);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererTextureType(const struct YsGLSL3DRenderer *renderer,int textureType)
{
	YSRESULT res=YSERR;

	if(NULL!=renderer)
	{
		float useTexture=0;
		float texCoeff[3]={0,0,0};
		if(YSGLSL_TEX_TYPE_NONE==textureType)
		{
			useTexture=0;
		}
		else if(YSGLSL_TEX_TYPE_TILING==textureType ||
		        YSGLSL_TEX_TYPE_BILLBOARD==textureType ||
		        YSGLSL_TEX_TYPE_ATTRIBUTE==textureType)
		{
			useTexture=1;
			texCoeff[0]=1;
		}
		else if(YSGLSL_TEX_TYPE_3DTILING==textureType)
		{
			useTexture=1;
			texCoeff[0]=1.0f/3.0f;
			texCoeff[1]=1.0f/3.0f;
			texCoeff[2]=1.0f/3.0f;
		}

		// Point-Sprite renderer may have non-zero useTexturePos and zero textureSampleCoeffPos
		if(0<=renderer->uniformUseTexturePos)
		{
			glUniform1f(renderer->uniformUseTexturePos,useTexture);
			res=YSOK;
		}
		if(0<=renderer->uniformTextureSampleCoeff)
		{
			glUniform3fv(renderer->uniformTextureSampleCoeff,1,texCoeff);
		}
	}


	if(NULL!=renderer && 0<=renderer->uniformTextureTypePos)
	{
		glUniform1i(renderer->uniformTextureTypePos,textureType);
		res=YSOK;
	}
	return res;
}

YSRESULT YsGLSLSet3DRendererTextureIdentifier(const struct YsGLSL3DRenderer *renderer,GLuint textureIdent)
{
	if(NULL!=renderer && 0<=renderer->uniformTexturePos)
	{
		glUniform1i(renderer->uniformTexturePos,textureIdent);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformTextureTilingMatrixfv(const struct YsGLSL3DRenderer *renderer,const GLfloat texTfm[16])
{
	if(NULL!=renderer && 0<=renderer->uniformTextureTileTransformPos)
	{
		glUniformMatrix4fv(renderer->uniformTextureTileTransformPos,1,GL_FALSE,texTfm);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererBillBoardClippingType(const struct YsGLSL3DRenderer *renderer,int billBoardClippingType)
{
	return YsGLSLSet3DRendererPointSpriteClippingType(renderer,billBoardClippingType);
}

YSRESULT YsGLSLSet3DRendererBillBoardTransformType(const struct YsGLSL3DRenderer *renderer,int billBoardTfmType)
{
	if(NULL!=renderer && 0<=renderer->uniformBillBoardTransformTypePos)
	{
		glUniform1i(renderer->uniformBillBoardTransformTypePos,billBoardTfmType);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererBillBoardfv(const struct YsGLSL3DRenderer *renderer,const GLfloat center[3],const GLfloat dimension[2])
{
	if(NULL!=renderer && 0<=renderer->uniformBillBoardCenterPos && 0<=renderer->uniformBillBoardDimensionPos)
	{
		glUniform3fv(renderer->uniformBillBoardCenterPos,1,center);
		glUniform2fv(renderer->uniformBillBoardDimensionPos,1,dimension);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformLightDirectionfv(struct YsGLSL3DRenderer *renderer,int i,const GLfloat dir[3])
{
	if(NULL!=renderer && 0<=i && i<YSGLSL_MAX_NUM_LIGHT && 0<=renderer->uniformLightPositionPos)
	{
		GLdouble l;
		GLdouble dir4d[4];
		dir4d[0]=dir[0];
		dir4d[1]=dir[1];
		dir4d[2]=dir[2];
		dir4d[3]=0.0;

		YsGLMultMatrixVectordv(dir4d,renderer->modelViewMatrix,dir4d);

		l=sqrt(dir4d[0]*dir4d[0]+dir4d[1]*dir4d[1]+dir4d[2]*dir4d[2]);
		renderer->lightPosition[i*4  ]=(GLfloat)(dir4d[0]/l);
		renderer->lightPosition[i*4+1]=(GLfloat)(dir4d[1]/l);
		renderer->lightPosition[i*4+2]=(GLfloat)(dir4d[2]/l);
		renderer->lightPosition[i*4+3]=0.0f;

		glUniform4fv(renderer->uniformLightPositionPos,YSGLSL_MAX_NUM_LIGHT,renderer->lightPosition);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformLightDirectionInCameraCoordinatefv(struct YsGLSL3DRenderer *renderer,int i,const GLfloat dir[3])
{
	if(NULL!=renderer && 0<=i && i<YSGLSL_MAX_NUM_LIGHT && 0<=renderer->uniformLightPositionPos)
	{
		GLdouble l;
		GLdouble dir4d[4];
		dir4d[0]=dir[0];
		dir4d[1]=dir[1];
		dir4d[2]=dir[2];
		dir4d[3]=0.0;

		l=sqrt(dir4d[0]*dir4d[0]+dir4d[1]*dir4d[1]+dir4d[2]*dir4d[2]);
		renderer->lightPosition[i*4  ]=(GLfloat)(dir4d[0]/l);
		renderer->lightPosition[i*4+1]=(GLfloat)(dir4d[1]/l);
		renderer->lightPosition[i*4+2]=(GLfloat)(dir4d[2]/l);
		renderer->lightPosition[i*4+3]=0.0f;

		glUniform4fv(renderer->uniformLightPositionPos,YSGLSL_MAX_NUM_LIGHT,renderer->lightPosition);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererLightColor(struct YsGLSL3DRenderer *renderer,int idx,const GLfloat color[3])
{
	if(NULL!=renderer && 0<=idx && idx<YSGLSL_MAX_NUM_LIGHT && 0<=renderer->uniformLightColorPos)
	{
		renderer->lightColor[idx*3  ]=color[0];
		renderer->lightColor[idx*3+1]=color[1];
		renderer->lightColor[idx*3+2]=color[2];
		glUniform3fv(renderer->uniformLightColorPos,YSGLSL_MAX_NUM_LIGHT,renderer->lightColor);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererAmbientColor(struct YsGLSL3DRenderer *renderer,const GLfloat color[3])
{
	if(NULL!=renderer && 0<=renderer->uniformAmbientColorPos)
	{
		renderer->ambientColor[0]=color[0];
		renderer->ambientColor[1]=color[1];
		renderer->ambientColor[2]=color[2];
		glUniform3fv(renderer->uniformAmbientColorPos,1,color);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererSpecularColor(struct YsGLSL3DRenderer *renderer,const GLfloat color[3])
{
	if(NULL!=renderer && 0<=renderer->uniformSpecularColorPos)
	{
		renderer->specularColor[0]=color[0];
		renderer->specularColor[1]=color[1];
		renderer->specularColor[2]=color[2];
		glUniform3fv(renderer->uniformSpecularColorPos,1,color);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLGet3DRendererSpecularColor(GLfloat color[3],struct YsGLSL3DRenderer *renderer)
{
	if(NULL!=renderer && 0<=renderer->uniformSpecularColorPos)
	{
		color[0]=renderer->specularColor[0];
		color[1]=renderer->specularColor[1];
		color[2]=renderer->specularColor[2];
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererSpecularExponent(struct YsGLSL3DRenderer *renderer,const GLfloat exponent)
{
	if(NULL!=renderer && 0<=renderer->uniformSpecularExponentPos)
	{
		renderer->specularExponent=exponent;
		glUniform1f(renderer->uniformSpecularExponentPos,exponent);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformFlashSize(struct YsGLSL3DRenderer *renderer,GLfloat flashSize)
{
	if(NULL!=renderer && 0<=renderer->uniformDimensionPos)
	{
		glUniform1f(renderer->uniformDimensionPos,flashSize);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererFlashRadius(struct YsGLSL3DRenderer *renderer,GLfloat r1,GLfloat r2)
{
	if(NULL!=renderer && 0<=renderer->uniformRadius1Pos && 0<=renderer->uniformRadius2Pos)
	{
		glUniform1f(renderer->uniformRadius1Pos,r1);
		glUniform1f(renderer->uniformRadius2Pos,r2);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformFogEnabled(struct YsGLSL3DRenderer *renderer,int enableFog)
{
	if(NULL!=renderer && 0<=renderer->uniformFogEnabledPos)
	{
		if(0!=enableFog)
		{
			glUniform1f(renderer->uniformFogEnabledPos,1);
		}
		else
		{
			glUniform1f(renderer->uniformFogEnabledPos,0);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformFogDensity(struct YsGLSL3DRenderer *renderer,const GLfloat density)
{
	if(NULL!=renderer && 0<=renderer->uniformFogDensityPos)
	{
		renderer->fogDensity=density;
		glUniform1f(renderer->uniformFogDensityPos,density);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformFogColor(struct YsGLSL3DRenderer *renderer,const GLfloat fogColor[4])
{
	if(NULL!=renderer && 0<=renderer->uniformFogColorPos)
	{
		glUniform4fv(renderer->uniformFogColorPos,1,fogColor);
		return YSOK;
	}
	return YSERR;
}

GLfloat YsGLSLGet3DRendererFogDensity(struct YsGLSL3DRenderer *renderer)
{
	if(NULL!=renderer)
	{
		return renderer->fogDensity;
	}
	return 0.0;
}

YSRESULT YsGLSLSet3DRendererZOffsetEnabled(struct YsGLSL3DRenderer *renderer,int enableZOffset)
{
	if(NULL!=renderer && 0<=renderer->uniformUseZOffsetPos)
	{
		glUniform1i(renderer->uniformUseZOffsetPos,enableZOffset);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererZOffset(struct YsGLSL3DRenderer *renderer,GLfloat zOffset)
{
	if(NULL!=renderer && 0<=renderer->uniformZOffsetPos)
	{
		glUniform1f(renderer->uniformZOffsetPos,zOffset);
		return YSOK;
	}
	return YSERR;
}

GLuint YsGLSLUse3DRenderer(struct YsGLSL3DRenderer *renderer)
{
	GLuint prevProgramId;
	glGetIntegerv(GL_CURRENT_PROGRAM,(GLint *)&prevProgramId);

	if(NULL!=renderer)
	{
		if(1==renderer->inUse)
		{
			printf("Warning!!!! A 3DRenderer is activated again without being de-activated after last use!!!!\n");
		}

		glUseProgram(renderer->programId);

		if(0<=renderer->attribVertexPos)
		{
			glEnableVertexAttribArray(renderer->attribVertexPos);
		}
		if(0<=renderer->attribColorPos)
		{
			glEnableVertexAttribArray(renderer->attribColorPos);
			renderer->colorArrayEnabled=YSTRUE;
		}
		if(0<=renderer->attribNormalPos)
		{
			glEnableVertexAttribArray(renderer->attribNormalPos);
		}
		if(0<=renderer->attribTexCoordPos)
		{
			glEnableVertexAttribArray(renderer->attribTexCoordPos);
			renderer->texCoordArrayEnabled=YSTRUE;
		}
		if(0<=renderer->attribPixelOffsetPos)
		{
			glEnableVertexAttribArray(renderer->attribPixelOffsetPos);
		}
		if(0<=renderer->attribViewOffsetPos)
		{
			glEnableVertexAttribArray(renderer->attribViewOffsetPos);
		}
		if(0<=renderer->attribPointSizePos)
		{
			glEnableVertexAttribArray(renderer->attribPointSizePos);
		}

		renderer->inUse=1;
	}

	return prevProgramId;
}

void YsGLSLEndUse3DRenderer(struct YsGLSL3DRenderer *renderer)
{
	if(NULL!=renderer)
	{
		if(0<=renderer->attribVertexPos)
		{
			glDisableVertexAttribArray(renderer->attribVertexPos);
		}
		if(0<=renderer->attribColorPos)
		{
			glDisableVertexAttribArray(renderer->attribColorPos);
		}
		renderer->colorArrayEnabled=YSFALSE;
		if(0<=renderer->attribNormalPos)
		{
			glDisableVertexAttribArray(renderer->attribNormalPos);
		}
		if(0<=renderer->attribTexCoordPos && YSTRUE==renderer->texCoordArrayEnabled)
		{
			glDisableVertexAttribArray(renderer->attribTexCoordPos);
		}
		renderer->texCoordArrayEnabled=YSFALSE;
		if(0<=renderer->attribPixelOffsetPos)
		{
			glDisableVertexAttribArray(renderer->attribPixelOffsetPos);
		}
		if(0<=renderer->attribViewOffsetPos)
		{
			glDisableVertexAttribArray(renderer->attribViewOffsetPos);
		}
		if(0<=renderer->attribPointSizePos)
		{
			glDisableVertexAttribArray(renderer->attribPointSizePos);
		}

		renderer->inUse=0;
	}
}

int YsGLSLCheck3DRendererIsInUse(struct YsGLSL3DRenderer *renderer)
{
	return renderer->inUse;
}

int YsGLSL3DRendererApplySphereMap(int w,int h,GLfloat *rgba,struct YsGLSL3DRenderer *renderer,int useSpecular)
{
	if(NULL!=renderer)
	{
		YsGLApplySphereMap(
		    w,h,rgba,
		    renderer->ambientColor,
		    YSGLSL_MAX_NUM_LIGHT,
		    renderer->lightEnabled,
		    renderer->lightPosition,
		    renderer->lightColor,
		    useSpecular,
		    renderer->specularColor,
		    renderer->specularExponent);
		return YSOK;
	}
	return YSERR;
}

static void YsGLSL3DRendererInitializeUniform(struct YsGLSL3DRenderer *renderer)
{
	int i;
	GLuint prevProgramId;
	const GLfloat color[4]={1,1,1,1};
	const GLfloat fogColor[4]=
	{
		0,0,0,0
	};
	const GLfloat specularColor[3]={1,1,1};
	const GLfloat ambientColor[3]={0,0,0};

	const double identityMatrixd[16]=
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	glGetIntegerv(GL_CURRENT_PROGRAM,(GLint *)&prevProgramId);
	glUseProgram(renderer->programId);

	for(i=0; i<16; ++i)
	{
		renderer->modelViewMatrix[i]=identityMatrixd[i];
		renderer->projectionMatrix[i]=identityMatrixd[i];
	}

	YsGLSLLoadIdentityMatrix4(renderer->uniformProjectionPos);
	YsGLSLLoadIdentityMatrix4(renderer->uniformModelViewPos);
	if(0<=renderer->uniformColorPos)
	{
		glUniform4fv(renderer->uniformColorPos,1,color);
	}


	if(0<=renderer->uniformViewportWidthPos)
	{
		glUniform1f(renderer->uniformViewportWidthPos,640);
	}
	if(0<=renderer->uniformViewportHeightPos)
	{
		glUniform1f(renderer->uniformViewportHeightPos,320);
	}
	if(0<=renderer->uniformViewportOriginPos)
	{
		glUniform1f(renderer->uniformViewportOriginPos,0);
	}

	if(0<=renderer->uniformMarkerTypePos)
	{
		glUniform1i(renderer->uniformMarkerTypePos,0);
	}

	if(0<=renderer->uniformAlphaCutOffPos)
	{
		glUniform1f(renderer->uniformAlphaCutOffPos,0.005f);
	}


	if(0<=renderer->uniformTextureTypePos)
	{
		glUniform1i(renderer->uniformTextureTypePos,YSGLSL_TEX_TYPE_NONE);
	}
	if(0<=renderer->uniformUseTexturePos)
	{
		glUniform1f(renderer->uniformUseTexturePos,0);
	}
	if(0<=renderer->uniformTextureSampleCoeff)
	{
		glUniform1f(renderer->uniformTextureSampleCoeff,0);
	}
	if(0<=renderer->uniformUseNegativeOneToPositiveOnTexCoordPos)
	{
		glUniform1i(renderer->uniformUseNegativeOneToPositiveOnTexCoordPos,1);
	}
	if(0<=renderer->uniformTextureTileTransformPos)
	{
		YsGLSLLoadIdentityMatrix4(renderer->uniformTextureTileTransformPos);
	}
	if(0<=renderer->uniformTexturePos)
	{
		glUniform1i(renderer->uniformTexturePos,0);
	}

	if(0<=renderer->uniformBillBoardTransformTypePos)
	{
		glUniform1i(renderer->uniformBillBoardTransformTypePos,YSGLSL_TEX_BILLBOARD_PERS);
	}
	if(0<=renderer->uniformBillBoardCenterPos)
	{
		const GLfloat zero[3]={0.0f,0.0f,0.0f};
		glUniform3fv(renderer->uniformBillBoardCenterPos,1,zero);
	}
	if(0<=renderer->uniformBillBoardDimensionPos)
	{
		const GLfloat unit[2]={1.0f,1.0f};
		glUniform2fv(renderer->uniformBillBoardDimensionPos,1,unit);
	}

	if(0<=renderer->uniformFogEnabledPos)
	{
		glUniform1i(renderer->uniformFogEnabledPos,0);
	}
	if(0<=renderer->uniformFogDensityPos)
	{
		glUniform1f(renderer->uniformFogDensityPos,1.0f);
	}
	if(0<=renderer->uniformFogColorPos)
	{
		glUniform4fv(renderer->uniformFogColorPos,1,fogColor);
	}


	if(0<=renderer->uniformDimensionPos)
	{
		glUniform1f(renderer->uniformDimensionPos,0.5);
	}
	if(0<=renderer->uniformRadius1Pos)
	{
		glUniform1f(renderer->uniformRadius1Pos,0.2f);
	}
	if(0<=renderer->uniformRadius2Pos)
	{
		glUniform1f(renderer->uniformRadius2Pos,1.0f);
	}
	if(0<=renderer->uniformMaxIntensityPos)
	{
		glUniform1f(renderer->uniformMaxIntensityPos,1.1f);
	}
	if(0<=renderer->uniformAngleIntervalPos)
	{
		glUniform1f(renderer->uniformAngleIntervalPos,(GLfloat)YSGLPI/3.0);
	}
	if(0<=renderer->uniformAngleOffsetPos)
	{
		glUniform1f(renderer->uniformAngleOffsetPos,(GLfloat)YSGLPI/6.0);
	}
	if(0<=renderer->uniformExponentPos)
	{
		glUniform1f(renderer->uniformExponentPos,4);
	}


	for(i=0; i<YSGLSL_MAX_NUM_LIGHT; i++)
	{
		renderer->lightEnabled[i]=(i==0 ? 1.0f : 0.0f);
		renderer->lightPosition[i*4  ]=0;
		renderer->lightPosition[i*4+1]=0;
		renderer->lightPosition[i*4+2]=1;
		renderer->lightPosition[i*4+3]=0;
		renderer->lightColor[i*3  ]=1;
		renderer->lightColor[i*3+1]=1;
		renderer->lightColor[i*3+2]=1;
	}
	if(0<=renderer->uniformLightEnabledPos)
	{
		glUniform1fv(renderer->uniformLightEnabledPos,YSGLSL_MAX_NUM_LIGHT,renderer->lightEnabled);
	}
	if(0<=renderer->uniformLightPositionPos)
	{
		glUniform4fv(renderer->uniformLightPositionPos,YSGLSL_MAX_NUM_LIGHT,renderer->lightPosition);
	}
	if(0<=renderer->uniformLightColorPos)
	{
		glUniform3fv(renderer->uniformLightColorPos,YSGLSL_MAX_NUM_LIGHT,renderer->lightColor);
	}
	if(0<=renderer->uniformColorPos)
	{
		glUniform4fv(renderer->uniformColorPos,1,color);
	}

	renderer->ambientColor[0]=ambientColor[0];
	renderer->ambientColor[1]=ambientColor[1];
	renderer->ambientColor[2]=ambientColor[2];
	if(0<=renderer->uniformAmbientColorPos)
	{
		glUniform3fv(renderer->uniformAmbientColorPos,1,ambientColor);
	}

	renderer->specularExponent=80.0f;
	if(0<=renderer->uniformSpecularExponentPos)
	{
		glUniform1f(renderer->uniformSpecularExponentPos,80.0f);
	}
	renderer->specularColor[0]=specularColor[0];
	renderer->specularColor[1]=specularColor[1];
	renderer->specularColor[2]=specularColor[2];
	if(0<=renderer->uniformSpecularColorPos)
	{
		glUniform3fv(renderer->uniformSpecularColorPos,1,specularColor);
	}


	if(0<=renderer->uniformUseZOffsetPos)
	{
		glUniform1i(renderer->uniformUseZOffsetPos,0);
	}
	if(0<=renderer->uniformZOffsetPos)
	{
		glUniform1f(renderer->uniformZOffsetPos,0.0f);
	}


	if(0<=renderer->uniformPointSizePos)
	{
		glUniform1f(renderer->uniformPointSizePos,1.0f);
	}


	if(0<=renderer->uniformTexCoordRangePos)
	{
		glUniform1f(renderer->uniformTexCoordRangePos,1.0f);
	}

	if(0<=renderer->uniformUsePointSizeInPixelPos)
	{
		glUniform1f(renderer->uniformUsePointSizeInPixelPos,1);
	}
	if(0<=renderer->uniformUsePointSizeIn3DPos)
	{
		glUniform1f(renderer->uniformUsePointSizeIn3DPos,0);
	}
	if(0<=renderer->uniformUsePointClippingCirclePos)
	{
		glUniform1f(renderer->uniformUsePointClippingCirclePos,0);
	}


	{
		if(0<=renderer->uniformUseShadowMapPos)
		{
			glUniform1fv(renderer->uniformUseShadowMapPos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,renderer->useShadowMap);
		}
		if(0<=renderer->uniformTestShadowMapPos)
		{
			glUniform1fv(renderer->uniformTestShadowMapPos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,renderer->testShadowMap);
		}
		if(0<=renderer->uniformShadowMapTransformPos)
		{
			glUniformMatrix4fv(renderer->uniformShadowMapTransformPos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,GL_FALSE,&renderer->shadowMapTransform[0][0]);
		}
		if(0<=renderer->uniformShadowMapTexturePos)
		{
			glUniform1iv(renderer->uniformShadowMapTexturePos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,(GLint *)renderer->shadowMapTexture);
		}
		if(0<=renderer->uniformLightDistOffsetPos)
		{
			glUniform1fv(renderer->uniformLightDistOffsetPos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,renderer->lightDistOffset);
		}
		if(0<=renderer->uniformLightDistScalePos)
		{
			glUniform1fv(renderer->uniformLightDistScalePos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,renderer->lightDistScale);
		}
	}


	glUseProgram(prevProgramId);
}

////////////////////////////////////////////////////////////

struct YsGLSL3DRenderer *YsGLSLCreateFlat3DRenderer(void)
{
	// 2015/12/09 Finally figured how to use generic attribute.  MonoColor renderer no longer needed.
	return YsGLSLCreateVariColor3DRenderer();
}

YSRESULT YsGLSLDrawPrimitiveVtxfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[])
{
	YsGLSLTexCoordIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When texCoord is not given as an array.
	YsGLSLColorIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When color is not given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0>renderer->attribNormalPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


////////////////////////////////////////////////////////////

struct YsGLSL3DRenderer *YsGLSLCreateMonoColorBillBoard3DRenderer(void)
{
	// 2015/12/09 Finally figured how to use generic attribute.  MonoColor renderer no longer needed.
	return YsGLSLCreateVariColorBillBoard3DRenderer();
}


////////////////////////////////////////////////////////////
static int YsGLSLVariColorBillBoard3DRendererCreateProgram(struct YsGLSL3DRenderer *renderer)
{
	int ec=0;

	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	if(YSOK!=YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_variColorBillBoard3DDrawingVertexShader_nLine,
	    YSGLSL_variColorBillBoard3DDrawingVertexShader,
	    renderer->fragmentShaderId,
	    YSGLSL_variColorBillBoard3DDrawingFragmentShader_nLine,
	    YSGLSL_variColorBillBoard3DDrawingFragmentShader))
	{
		++ec;
	}

	// Uniforms
	renderer->uniformProjectionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"projection");
	renderer->uniformModelViewPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"modelView");
	renderer->uniformUsePointClippingCirclePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"usePointClippingCircle");
	renderer->uniformBillBoardTransformTypePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"billBoardTransformType");
	renderer->uniformTexturePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"textureIdent");
	renderer->uniformUseNegativeOneToPositiveOnTexCoordPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useNegativeOneToPositiveOneTexCoord");


	renderer->uniformFogEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogEnabled");
	renderer->uniformFogDensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogDensity");
	renderer->uniformFogColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogColor");

	renderer->uniformUseZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useZOffset");
	renderer->uniformZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"zOffset");

	renderer->uniformAlphaCutOffPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"alphaCutOff");


	// Attributes
	renderer->attribVertexPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"vertex");
	renderer->attribColorPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"colorIn");
	renderer->attribTexCoordPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"texCoord");
	renderer->attribViewOffsetPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"viewOffset");
	if(0!=renderer->attribVertexPos)
	{
		printf("Verify Vertex Attrib Location (Must be zero)=%d\n",renderer->attribVertexPos);
		++ec;
	}

	return ec;
}

struct YsGLSL3DRenderer *YsGLSLCreateVariColorBillBoard3DRenderer(void)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLCreateAndInitialize3DRenderer();
	if(NULL!=renderer)
	{
		if(0<YsGLSLVariColorBillBoard3DRendererCreateProgram(renderer))
		{
			YsGLSLDelete3DRenderer(renderer);
			return NULL;
		}
		YsGLSL3DRendererInitializeUniform(renderer);
	}
	return renderer;
}

YSRESULT YsGLSLDrawPrimitiveVtxColVoffsetTexcoordfv(
    const struct YsGLSL3DRenderer *renderer,GLenum mode,
    GLsizei nVtx,
    const GLfloat vtx[],     // 3*nVtx
    const GLfloat col[],     // 4*nVtx
    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
    const GLfloat texCoord[] // 2*nVtx
    )
{
	YsGLSLTexCoordIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // Whenever texCoord is given as an array.
	YsGLSLColorIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // Whenever color is given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribColorPos &&
	   0<=renderer->attribViewOffsetPos &&
	   0<=renderer->attribTexCoordPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribColorPos,4,GL_FLOAT,GL_FALSE,0,col);
		glVertexAttribPointer(renderer->attribViewOffsetPos,3,GL_FLOAT,GL_FALSE,0,vOffset);
		glVertexAttribPointer(renderer->attribTexCoordPos,2,GL_FLOAT,GL_FALSE,0,texCoord);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


YSRESULT YsGLSLDrawPrimitiveVtxVoffsetTexcoordfv(
    const struct YsGLSL3DRenderer *renderer,GLenum mode,
    GLsizei nVtx,
    const GLfloat vtx[],     // 3*nVtx
    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
    const GLfloat texCoord[] // 2*nVtx
    )
{
	YsGLSLTexCoordIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // Whenever texCoord is given as an array.
	YsGLSLColorIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When color is not given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribViewOffsetPos &&
	   0<=renderer->attribTexCoordPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribViewOffsetPos,3,GL_FLOAT,GL_FALSE,0,vOffset);
		glVertexAttribPointer(renderer->attribTexCoordPos,2,GL_FLOAT,GL_FALSE,0,texCoord);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


////////////////////////////////////////////////////////////
static int YsGLSLFlash3DRendererCreateProgram(struct YsGLSL3DRenderer *renderer)
{
	int ec=0;

	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	if(YSOK!=YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_flash3DDrawingVertexShader_nLine,
	    YSGLSL_flash3DDrawingVertexShader,
	    renderer->fragmentShaderId,
	    YSGLSL_flash3DDrawingFragmentShader_nLine,
	    YSGLSL_flash3DDrawingFragmentShader))
	{
		++ec;
	}

	// Uniforms
	renderer->uniformProjectionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"projection");
	renderer->uniformModelViewPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"modelView");

	renderer->uniformDimensionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"dimension");
	renderer->uniformRadius1Pos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"radius1");
	renderer->uniformRadius2Pos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"radius2");
	renderer->uniformMaxIntensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"maxIntensity");
	renderer->uniformAngleIntervalPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"angleInterval");
	renderer->uniformAngleOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"angleOffset");
	renderer->uniformExponentPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"exponent");

	renderer->uniformFogEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogEnabled");
	renderer->uniformFogDensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogDensity");
	renderer->uniformFogColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogColor");

	renderer->uniformUseZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useZOffset");
	renderer->uniformZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"zOffset");


	// Attributes
	renderer->attribVertexPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"vertex");
	renderer->attribViewOffsetPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"viewOffset");
	renderer->attribColorPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"colorIn");
	if(0!=renderer->attribVertexPos)
	{
		printf("Verify Vertex Attrib Location (Must be zero)=%d\n",renderer->attribVertexPos);
		++ec;
	}

	return ec;
}

struct YsGLSL3DRenderer *YsGLSLCreateFlash3DRenderer(void)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLCreateAndInitialize3DRenderer();
	if(NULL!=renderer)
	{
		if(0<YsGLSLFlash3DRendererCreateProgram(renderer))
		{
			YsGLSLDelete3DRenderer(renderer);
			return NULL;
		}
		YsGLSL3DRendererInitializeUniform(renderer);
	}
	return renderer;
}

////////////////////////////////////////////////////////////

static int YsGLSLFlashByPointSprite3DRendererCreateProgram(struct YsGLSL3DRenderer *renderer)
{
	int ec=0;

	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	if(YSOK!=YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_flashByPointSprite3DDrawingVertexShader_nLine,
	    YSGLSL_flashByPointSprite3DDrawingVertexShader,
	    renderer->fragmentShaderId,
	    YSGLSL_flashByPointSprite3DDrawingFragmentShader_nLine,
	    YSGLSL_flashByPointSprite3DDrawingFragmentShader))
	{
		++ec;
	}

	// Uniforms
	renderer->uniformProjectionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"projection");
	renderer->uniformModelViewPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"modelView");

	renderer->uniformRadius1Pos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"radius1");
	renderer->uniformRadius2Pos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"radius2");
	renderer->uniformMaxIntensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"maxIntensity");
	renderer->uniformAngleIntervalPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"angleInterval");
	renderer->uniformAngleOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"angleOffset");
	renderer->uniformExponentPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"exponent");

	renderer->uniformFogEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogEnabled");
	renderer->uniformFogDensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogDensity");
	renderer->uniformFogColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogColor");

	renderer->uniformUseZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useZOffset");
	renderer->uniformZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"zOffset");

	// "viewportWid" is defined in the vertex shader, but not referenced.
	renderer->uniformViewportWidthPos=YsGLSLGetUniformLocation(NULL,__FUNCTION__,renderer->programId,"viewportWid");
	renderer->uniformViewportHeightPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"viewportHei");

	renderer->uniformUsePointSizeInPixelPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"usePointSizeInPixel");
	renderer->uniformUsePointSizeIn3DPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"usePointSizeIn3D");


	// Attributes
	renderer->attribVertexPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"vertex");
	renderer->attribColorPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"colorIn");
	renderer->attribPointSizePos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"pointSizeIn");
	if(0!=renderer->attribVertexPos)
	{
		printf("Verify Vertex Attrib Location (Must be zero)=%d\n",renderer->attribVertexPos);
		++ec;
	}
	return ec;
}

struct YsGLSL3DRenderer *YsGLSLCreateFlashByPointSprite3DRenderer(void)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLCreateAndInitialize3DRenderer();
	if(NULL!=renderer)
	{
		if(0<YsGLSLFlashByPointSprite3DRendererCreateProgram(renderer))
		{
			YsGLSLDelete3DRenderer(renderer);
			return NULL;
		}
		YsGLSL3DRendererInitializeUniform(renderer);
	}
	return renderer;
}

////////////////////////////////////////////////////////////

struct YsGLSL3DRenderer *YsGLSLCreateMonoColorMarker3DRenderer(void)
{
	// 2015/12/09 Finally figured how to use generic attribute.  MonoColor renderer no longer needed.
	return YsGLSLCreateVariColorMarker3DRenderer();
}

YSRESULT YsGLSLDrawPrimitiveVtxPixelOffsetfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat pixelOffset[])
{
	YsGLSLTexCoordIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When texCoord is not given as an input array.
	YsGLSLColorIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When color is not given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribPixelOffsetPos &&
	   0<=renderer->attribVertexPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribPixelOffsetPos,2,GL_FLOAT,GL_FALSE,0,pixelOffset);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsGLSLSet3DRendererUniformMarkerDimension(struct YsGLSL3DRenderer *renderer,GLfloat dimension)
{
	if(NULL!=renderer && 0<=renderer->uniformDimensionPos)
	{
		glUniform1f(renderer->uniformDimensionPos,dimension);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformTextureCoordRange(struct YsGLSL3DRenderer *renderer,GLfloat texCoordRange)
{
	if(NULL!=renderer && 0<=renderer->uniformTexCoordRangePos)
	{
		glUniform1f(renderer->uniformTexCoordRangePos,texCoordRange);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformMarkerType(struct YsGLSL3DRenderer *renderer,int markerType)
{
	if(NULL!=renderer && 0<=renderer->uniformMarkerTypePos)
	{
		glUniform1i(renderer->uniformMarkerTypePos,markerType);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformShadowMapMode(struct YsGLSL3DRenderer *renderer,int shadowMapId,int shadowMapMode)
{
	if(NULL!=renderer && 0<=renderer->uniformUseShadowMapPos && 0<=shadowMapId && shadowMapId<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE)
	{
		switch(shadowMapMode)
		{
		default:
			renderer->useShadowMap[shadowMapId]=0;
			renderer->testShadowMap[shadowMapId]=0;
			break;
		case YSGLSL_SHADOWMAP_USE:
			renderer->useShadowMap[shadowMapId]=1;
			renderer->testShadowMap[shadowMapId]=0;
			break;
		case YSGLSL_SHADOWMAP_DEBUG:
			renderer->useShadowMap[shadowMapId]=0;
			renderer->testShadowMap[shadowMapId]=1;
			break;
		}
		glUniform1fv(renderer->uniformUseShadowMapPos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,renderer->useShadowMap);
		if(0<=renderer->uniformTestShadowMapPos)
		{
			glUniform1fv(renderer->uniformTestShadowMapPos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,renderer->testShadowMap);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformShadowMapTexture(struct YsGLSL3DRenderer *renderer,int shadowMapId,int textureIdent)
{
	if(NULL!=renderer && 0<=renderer->uniformShadowMapTexturePos && 0<=shadowMapId && shadowMapId<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE)
	{
		renderer->shadowMapTexture[shadowMapId]=textureIdent;
		glUniform1iv(renderer->uniformShadowMapTexturePos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,(GLint *)renderer->shadowMapTexture);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformShadowMapTransformation(struct YsGLSL3DRenderer *renderer,int shadowMapId,const GLfloat tfm[16])
{
	if(NULL!=renderer && 0<=renderer->uniformShadowMapTransformPos && 0<=shadowMapId && shadowMapId<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE)
	{
		int i;
		for(i=0; i<16; ++i)
		{
			renderer->shadowMapTransform[shadowMapId][i]=tfm[i];
		}
		glUniformMatrix4fv(renderer->uniformShadowMapTransformPos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,GL_FALSE,&renderer->shadowMapTransform[0][0]);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformLightDistOffset(struct YsGLSL3DRenderer *renderer,int shadowMapId,const GLfloat offset)
{
	if(NULL!=renderer && 0<=renderer->uniformLightDistOffsetPos && 0<=shadowMapId && shadowMapId<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE)
	{
		renderer->lightDistOffset[shadowMapId]=offset;
		glUniform1fv(renderer->uniformLightDistOffsetPos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,renderer->lightDistOffset);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformLightDistScaling(struct YsGLSL3DRenderer *renderer,int shadowMapId,const GLfloat scaling)
{
	if(NULL!=renderer && 0<=renderer->uniformLightDistScalePos && 0<=shadowMapId && shadowMapId<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE)
	{
		renderer->lightDistScale[shadowMapId]=scaling;
		glUniform1fv(renderer->uniformLightDistScalePos,YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE,renderer->lightDistScale);
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

static int YsGLSLVariColorMarker3DRendererCreateProgram(struct YsGLSL3DRenderer *renderer)
{
	int ec=0;

	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	if(YSOK!=YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_variColorMarker3DDrawingVertexShader_nLine,
	    YSGLSL_variColorMarker3DDrawingVertexShader,
	    renderer->fragmentShaderId,
	    YSGLSL_variColorMarker3DDrawingFragmentShader_nLine,
	    YSGLSL_variColorMarker3DDrawingFragmentShader))
	{
		++ec;
	}

	// Uniforms
	renderer->uniformProjectionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"projection");
	renderer->uniformModelViewPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"modelView");

	renderer->uniformViewportWidthPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"viewportWid");
	renderer->uniformViewportHeightPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"viewportHei");

	renderer->uniformMarkerTypePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"markerType");
	renderer->uniformDimensionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"dimension");

	renderer->uniformFogEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogEnabled");
	renderer->uniformFogDensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogDensity");
	renderer->uniformFogColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogColor");

	renderer->uniformUseZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useZOffset");
	renderer->uniformZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"zOffset");


	// Attributes
	renderer->attribVertexPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"vertex");
	renderer->attribPixelOffsetPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"pixelOffset");
	renderer->attribColorPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"colorIn");
	if(0!=renderer->attribVertexPos)
	{
		printf("Verify Vertex Attrib Location (Must be zero)=%d\n",renderer->attribVertexPos);
		++ec;
	}
	return ec;
}

struct YsGLSL3DRenderer *YsGLSLCreateVariColorMarker3DRenderer(void)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLCreateAndInitialize3DRenderer();
	if(NULL!=renderer)
	{
		if(0<YsGLSLVariColorMarker3DRendererCreateProgram(renderer))
		{
			YsGLSLDelete3DRenderer(renderer);
			return NULL;
		}
		YsGLSL3DRendererInitializeUniform(renderer);
	}
	return renderer;
}

YSRESULT YsGLSLDrawPrimitiveVtxColPixelOffsetfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat col[],const GLfloat pixelOffset[])
{
	YsGLSLTexCoordIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When texCoord is not given as an input array.
	YsGLSLColorIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // Whenever color is given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribPixelOffsetPos &&
	   0<=renderer->attribColorPos &&
	   0<=renderer->attribVertexPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribColorPos,4,GL_FLOAT,GL_FALSE,0,col);
		glVertexAttribPointer(renderer->attribPixelOffsetPos,2,GL_FLOAT,GL_FALSE,0,pixelOffset);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

////////////////////////////////////////////////////////////

static int YsGLSLVariColorMarkerByPointSprite3DRenderer_CreateProgram(struct YsGLSL3DRenderer *renderer)
{
	int ec=0;

	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	if(YSOK!=YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_variColorMarkerByPointSprite3DDrawingVertexShader_nLine,
	    YSGLSL_variColorMarkerByPointSprite3DDrawingVertexShader,
	    renderer->fragmentShaderId,
	    YSGLSL_variColorMarkerByPointSprite3DDrawingFragmentShader_nLine,
	    YSGLSL_variColorMarkerByPointSprite3DDrawingFragmentShader))
	{
		++ec;
	}

	// Uniforms
	renderer->uniformProjectionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"projection");
	renderer->uniformModelViewPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"modelView");

	renderer->uniformMarkerTypePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"markerType");
	renderer->uniformDimensionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"dimension");

	renderer->uniformFogEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogEnabled");
	renderer->uniformFogDensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogDensity");
	renderer->uniformFogColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogColor");

	renderer->uniformUseZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useZOffset");
	renderer->uniformZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"zOffset");


	// Attributes
	renderer->attribVertexPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"vertex");
	renderer->attribColorPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"colorIn");
	if(0!=renderer->attribVertexPos)
	{
		printf("Verify Vertex Attrib Location (Must be zero)=%d\n",renderer->attribVertexPos);
		++ec;
	}
	return ec;
}

struct YsGLSL3DRenderer *YsGLSLCreateVariColorMarkerByPointSprite3DRenderer(void)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLCreateAndInitialize3DRenderer();
	if(NULL!=renderer)
	{
		if(0<YsGLSLVariColorMarkerByPointSprite3DRenderer_CreateProgram(renderer))
		{
			YsGLSLDelete3DRenderer(renderer);
			return NULL;
		}
		YsGLSL3DRendererInitializeUniform(renderer);
	}
	return renderer;
}

////////////////////////////////////////////////////////////

struct YsGLSL3DRenderer *YsGLSLCreateMonoColorMarkerByPointSprite3DRenderer(void)
{
	// 2015/12/09 Finally figured how to use generic attribute.  MonoColor renderer no longer needed.
	return YsGLSLCreateVariColorMarkerByPointSprite3DRenderer();
}

////////////////////////////////////////////////////////////

YSRESULT YsGLSLDrawPrimitiveVtxColVoffsetfv(const struct YsGLSL3DRenderer *renderer,GLsizei nVtx,const GLfloat vtx[],const GLfloat col[],const GLfloat viewOffset[])
{
	YsGLSLTexCoordIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When texCoord is not given as an input array.
	YsGLSLColorIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // Whenever color is given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribViewOffsetPos &&
	   0<=renderer->attribVertexPos && 
	   0<=renderer->attribColorPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribViewOffsetPos,3,GL_FLOAT,GL_FALSE,0,viewOffset);
		glVertexAttribPointer(renderer->attribColorPos,4,GL_FLOAT,GL_FALSE,0,col);
		glDrawArrays(GL_TRIANGLES,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


////////////////////////////////////////////////////////////

// Returns the error count.
static int YsGLSLVariColor3DRendererCreateProgram(struct YsGLSL3DRenderer *renderer)
{
	int ec=0;

	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	if(YSOK!=YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_variColor3DDrawingVertexShader_nLine,
	    YSGLSL_variColor3DDrawingVertexShader,
	    renderer->fragmentShaderId,
	    YSGLSL_variColor3DDrawingFragmentShader_nLine,
	    YSGLSL_variColor3DDrawingFragmentShader))
	{
		++ec;
	}

	// Uniforms
	renderer->uniformProjectionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"projection");
	renderer->uniformModelViewPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"modelView");

	renderer->uniformFogEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogEnabled");
	renderer->uniformFogDensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogDensity");
	renderer->uniformFogColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogColor");

	renderer->uniformUseZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useZOffset");
	renderer->uniformZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"zOffset");

	renderer->uniformAlphaCutOffPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"alphaCutOff");

	renderer->uniformTexturePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"textureIdent");
	renderer->uniformTextureTypePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"textureType");

	renderer->uniformPointSizePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"pointSizeIn");

	ec+=YsGLSLGetTextureUniformPos(renderer,__FUNCTION__);


	// Attributes
	renderer->attribVertexPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"vertex");
	renderer->attribColorPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"colorIn");
	renderer->attribTexCoordPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"texCoord");

	if(0!=renderer->attribVertexPos)
	{
		printf("Verify Vertex Attrib Location (Must be zero)=%d\n",renderer->attribVertexPos);
		++ec;
	}

	return ec;
}

struct YsGLSL3DRenderer *YsGLSLCreateVariColor3DRenderer(void)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLCreateAndInitialize3DRenderer();
	if(NULL!=renderer)
	{
		if(0<YsGLSLVariColor3DRendererCreateProgram(renderer))
		{
			YsGLSLDelete3DRenderer(renderer);
			return NULL;
		}
		YsGLSL3DRendererInitializeUniform(renderer);
	}
	return renderer;
}

YSRESULT YsGLSLDrawPrimitiveVtxColfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat col[])
{
	YsGLSLTexCoordIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When texCoord is not given as an input array.
	YsGLSLColorIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // Whenever color is given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribColorPos &&
	   0>renderer->attribNormalPos &&
	   0>renderer->attribPixelOffsetPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribColorPos,4,GL_FLOAT,GL_FALSE,0,col);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsGLSLDrawPrimitiveVtxTexCoordColfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat col[])
{
	YsGLSLTexCoordIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When texCoord is not given as an input array.
	YsGLSLColorIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // Whenever color is given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribColorPos &&
	   0<=renderer->attribTexCoordPos &&
	   0>renderer->attribNormalPos &&
	   0>renderer->attribPixelOffsetPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribColorPos,4,GL_FLOAT,GL_FALSE,0,col);
		glVertexAttribPointer(renderer->attribTexCoordPos,2,GL_FLOAT,GL_FALSE,0,texCoord);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

////////////////////////////////////////////////////////////

struct YsGLSL3DRenderer *YsGLSLCreateMonoColorPerVtxShading3DRenderer(void)
{
	// 2015/12/09 Finally figured how to use generic attribute.  MonoColor renderer no longer needed.
	return YsGLSLCreateVariColorPerVtxShading3DRenderer();
}

YSRESULT YsGLSLDrawPrimitiveVtxNomfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat nom[])
{
	YsGLSLTexCoordIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When texCoord is not given as an input array.
	YsGLSLColorIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When color is not given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribNormalPos &&
	   0>renderer->attribPixelOffsetPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribNormalPos,3,GL_FLOAT,GL_FALSE,0,nom);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsGLSLDrawPrimitiveVtxTexCoordNomfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[])
{
	YsGLSLTexCoordIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // Whenever texCoord is given as an array.
	YsGLSLColorIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When color is not given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribNormalPos &&
	   0<=renderer->attribTexCoordPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribNormalPos,3,GL_FLOAT,GL_FALSE,0,nom);
		glVertexAttribPointer(renderer->attribTexCoordPos,2,GL_FLOAT,GL_FALSE,0,texCoord);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

////////////////////////////////////////////////////////////

struct YsGLSL3DRenderer *YsGLSLCreateMonoColorPerPixShading3DRenderer(void)
{
	// 2015/12/09 Finally figured how to use generic attribute.  MonoColor renderer no longer needed.
	return YsGLSLCreateVariColorPerPixShading3DRenderer();
}

////////////////////////////////////////////////////////////

struct YsGLSL3DRenderer *YsGLSLCreateMonoColorPerPixShadingWithTexCoord3DRenderer(void)
{
	// 2015/12/09 Finally figured how to use generic attribute.  MonoColor and WithTexCoord renderer no longer needed.
	return YsGLSLCreateVariColorPerPixShading3DRenderer();
}

////////////////////////////////////////////////////////////

struct YsGLSL3DRenderer *YsGLSLCreateMonoColorPerVtxShadingWithTexCoord3DRenderer(void)
{
	// 2015/12/09 Finally figured how to use generic attribute.  MonoColor and WithTexCoord renderer no longer needed.
	return YsGLSLCreateVariColorPerVtxShading3DRenderer();
}

////////////////////////////////////////////////////////////

static int YsGLSLVariColorPerVtxShading3DRendererCreateProgram(struct YsGLSL3DRenderer *renderer)
{
	int ec=0;

	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	if(YSOK!=YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_variColorPerVtxShading3DDrawingVertexShader_nLine,
	    YSGLSL_variColorPerVtxShading3DDrawingVertexShader,
	    renderer->fragmentShaderId,
	    YSGLSL_variColorPerVtxShading3DDrawingFragmentShader_nLine,
	    YSGLSL_variColorPerVtxShading3DDrawingFragmentShader))
	{
		++ec;
	}

	// Uniforms
	renderer->uniformProjectionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"projection");
	renderer->uniformModelViewPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"modelView");

	renderer->uniformLightEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"lightEnabled");
	renderer->uniformLightPositionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"lightPos");
	renderer->uniformLightColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"lightColor");
	renderer->uniformSpecularColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"specularColor");
	renderer->uniformSpecularExponentPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"specularExponent");
	renderer->uniformAmbientColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"ambientColor");

	renderer->uniformFogEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogEnabled");
	renderer->uniformFogDensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogDensity");
	renderer->uniformFogColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogColor");

	renderer->uniformUseZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useZOffset");
	renderer->uniformZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"zOffset");

	renderer->uniformAlphaCutOffPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"alphaCutOff");

	YsGLSLGetTextureUniformPos(renderer,__FUNCTION__);
	// YsGLSLGetShadowMapUniformPos(renderer,__FUNCTION__); not supported yet

	// Attributes
	renderer->attribVertexPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"vertex");
	renderer->attribNormalPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"normal");
	renderer->attribColorPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"color");
	renderer->attribTexCoordPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"texCoord");
	if(0!=renderer->attribVertexPos)
	{
		printf("Verify Vertex Attrib Location (Must be zero)=%d\n",renderer->attribVertexPos);
		++ec;
	}
	return ec;
}

struct YsGLSL3DRenderer *YsGLSLCreateVariColorPerVtxShading3DRenderer(void)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLCreateAndInitialize3DRenderer();
	if(NULL!=renderer)
	{
		if(0<YsGLSLVariColorPerVtxShading3DRendererCreateProgram(renderer))
		{
			YsGLSLDelete3DRenderer(renderer);
			return NULL;
		}
		YsGLSL3DRendererInitializeUniform(renderer);
	}
	return renderer;
}

YSRESULT YsGLSLDrawPrimitiveVtxNomColfv(const struct YsGLSL3DRenderer *renderer, GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat nom[],const GLfloat col[])
{
	if(0>renderer->attribColorPos)
	{
		YsGLSLSet3DRendererUniformColorfv((struct YsGLSL3DRenderer *)renderer,col);
	}

	YsGLSLTexCoordIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When texCoord is not given as an input array.
	YsGLSLColorIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // Whenever color is given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribColorPos &&
	   0<=renderer->attribNormalPos &&
	   0>renderer->attribPixelOffsetPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribNormalPos,3,GL_FLOAT,GL_FALSE,0,nom);
		glVertexAttribPointer(renderer->attribColorPos,4,GL_FLOAT,GL_FALSE,0,col);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else if(0<=renderer->attribVertexPos &&
	        0>renderer->attribColorPos &&
	        0<=renderer->attribNormalPos &&
	        0>renderer->attribTexCoordPos &&
	        0>renderer->attribPixelOffsetPos &&
	        0<=renderer->uniformColorPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribNormalPos,3,GL_FLOAT,GL_FALSE,0,nom);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsGLSLDrawPrimitiveVtxTexCoordNomColfv(const struct YsGLSL3DRenderer *renderer, GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[],const GLfloat col[])
{
	if(0>renderer->attribColorPos)
	{
		YsGLSLSet3DRendererUniformColorfv((struct YsGLSL3DRenderer *)renderer,col);
	}

	YsGLSLTexCoordIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // When texCoord is not given as an input array.
	YsGLSLColorIsGivenAsArray((struct YsGLSL3DRenderer *)renderer); // Whenever color is given as an array.

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribTexCoordPos &&
	   0<=renderer->attribColorPos &&
	   0<=renderer->attribNormalPos &&
	   0>renderer->attribPixelOffsetPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribTexCoordPos,2,GL_FLOAT,GL_FALSE,0,texCoord);
		glVertexAttribPointer(renderer->attribNormalPos,3,GL_FLOAT,GL_FALSE,0,nom);
		glVertexAttribPointer(renderer->attribColorPos,4,GL_FLOAT,GL_FALSE,0,col);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else if(0<=renderer->attribVertexPos &&
	        0<=renderer->attribTexCoordPos &&
	        0>renderer->attribColorPos &&
	        0<=renderer->attribNormalPos &&
	        0>renderer->attribTexCoordPos &&
	        0>renderer->attribPixelOffsetPos &&
	        0<=renderer->uniformColorPos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribTexCoordPos,2,GL_FLOAT,GL_FALSE,0,texCoord);
		glVertexAttribPointer(renderer->attribNormalPos,3,GL_FLOAT,GL_FALSE,0,nom);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

////////////////////////////////////////////////////////////

static int YsGLSLVariColorPerPixShading3DRendererCreateProgram(struct YsGLSL3DRenderer *renderer)
{
	int ec=0;

	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	if(YSOK!=YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_variColorPerPixShading3DDrawingVertexShader_nLine,
	    YSGLSL_variColorPerPixShading3DDrawingVertexShader,
	    renderer->fragmentShaderId,
	    YSGLSL_variColorPerPixShading3DDrawingFragmentShader_nLine,
	    YSGLSL_variColorPerPixShading3DDrawingFragmentShader))
	{
		++ec;
	}

	// Uniforms
	renderer->uniformProjectionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"projection");
	renderer->uniformModelViewPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"modelView");

	renderer->uniformLightEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"lightEnabled");
	renderer->uniformLightPositionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"lightPos");
	renderer->uniformLightColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"lightColor");
	renderer->uniformSpecularColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"specularColor");
	renderer->uniformSpecularExponentPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"specularExponent");
	renderer->uniformAmbientColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"ambientColor");

	renderer->uniformFogEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogEnabled");
	renderer->uniformFogDensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogDensity");
	renderer->uniformFogColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogColor");

	renderer->uniformUseZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useZOffset");
	renderer->uniformZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"zOffset");

	renderer->uniformAlphaCutOffPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"alphaCutOff");

	YsGLSLGetTextureUniformPos(renderer,__FUNCTION__);
	YsGLSLGetShadowMapUniformPos(renderer,__FUNCTION__);


	// Attributes
	renderer->attribVertexPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"vertex");
	renderer->attribNormalPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"normal");
	renderer->attribColorPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"colorIn");
	renderer->attribTexCoordPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"texCoord");
	if(0!=renderer->attribVertexPos)
	{
		printf("Verify Vertex Attrib Location (Must be zero)=%d\n",renderer->attribVertexPos);
		++ec;
	}

	return ec;
}

struct YsGLSL3DRenderer *YsGLSLCreateVariColorPerPixShading3DRenderer(void)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLCreateAndInitialize3DRenderer();
	if(NULL!=renderer)
	{
		if(0<YsGLSLVariColorPerPixShading3DRendererCreateProgram(renderer))
		{
			YsGLSLDelete3DRenderer(renderer);
			return NULL;
		}
		YsGLSL3DRendererInitializeUniform(renderer);
	}
	return renderer;
}

////////////////////////////////////////////////////////////

struct YsGLSL3DRenderer *YsGLSLCreateVariColorPerPixShadingWithTexCoord3DRenderer(void)
{
	// 2015/12/09 Finally figured how to use generic attribute.  WithTexCoord renderer no longer needed.
	return YsGLSLCreateVariColorPerPixShading3DRenderer();
}

////////////////////////////////////////////////////////////

struct YsGLSL3DRenderer *YsGLSLCreateVariColorPerVtxShadingWithTexCoord3DRenderer(void)
{
	// 2015/12/09 Finally figured how to use generic attribute.  WithTexCoord renderer no longer needed.
	return YsGLSLCreateVariColorPerVtxShading3DRenderer();
}

////////////////////////////////////////////////////////////

static int YsGLSLVariColorPointSprite3DRendererCreateProgram(struct YsGLSL3DRenderer *renderer)
{
	int ec=0;

	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	if(YSOK!=YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_variColorPointSprite3DDrawingVertexShader_nLine,
	    YSGLSL_variColorPointSprite3DDrawingVertexShader,
	    renderer->fragmentShaderId,
	    YSGLSL_variColorPointSprite3DDrawingFragmentShader_nLine,
	    YSGLSL_variColorPointSprite3DDrawingFragmentShader))
	{
		++ec;
	}


	// Uniforms
	renderer->uniformProjectionPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"projection");
	renderer->uniformModelViewPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"modelView");
	renderer->uniformTexturePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"textureIdent");

	renderer->uniformFogEnabledPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogEnabled");
	renderer->uniformFogDensityPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogDensity");
	renderer->uniformFogColorPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"fogColor");

	renderer->uniformUseZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useZOffset");
	renderer->uniformZOffsetPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"zOffset");

	// viewportWid is defined in the vertex shader, but is not used.
	renderer->uniformViewportWidthPos=YsGLSLGetUniformLocation(NULL,__FUNCTION__,renderer->programId,"viewportWid");
	renderer->uniformViewportHeightPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"viewportHei");

	renderer->uniformUsePointSizeInPixelPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"usePointSizeInPixel");
	renderer->uniformUsePointSizeIn3DPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"usePointSizeIn3D");
	renderer->uniformUsePointClippingCirclePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"usePointClippingCircle");
	renderer->uniformUseTexturePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"useTexture");
	renderer->uniformTexCoordRangePos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"texCoordRange");

	renderer->uniformAlphaCutOffPos=YsGLSLGetUniformLocation(&ec,__FUNCTION__,renderer->programId,"alphaCutOff");

	// Attributes
	renderer->attribVertexPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"vertex");
	renderer->attribColorPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"colorIn");
	renderer->attribTexCoordPos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"texCoordIn");
	renderer->attribPointSizePos=YsGLSLGetAttribLocation(&ec,__FUNCTION__,renderer->programId,"pointSizeIn");

	if(0!=renderer->attribVertexPos)
	{
		++ec;
	}
	return ec;
}

struct YsGLSL3DRenderer *YsGLSLCreateVariColorPointSprite3DRenderer(void)
{
	struct YsGLSL3DRenderer *renderer=YsGLSLCreateAndInitialize3DRenderer();
	if(NULL!=renderer)
	{
		if(0<YsGLSLVariColorPointSprite3DRendererCreateProgram(renderer))
		{
			YsGLSLDelete3DRenderer(renderer);
			return NULL;
		}
		YsGLSL3DRendererInitializeUniform(renderer);
	}
	return renderer;
}

YSRESULT YsGLSLSet3DRendererPointSpriteClippingType(const struct YsGLSL3DRenderer *renderer,int pointSpriteClippingType)
{
	if(NULL!=renderer && 0<=renderer->uniformUsePointClippingCirclePos)
	{
		switch(pointSpriteClippingType)
		{
		case YSGLSL_POINTSPRITE_CLIPPING_NONE:
			glUniform1f(renderer->uniformUsePointClippingCirclePos,0);
			break;
		case YSGLSL_POINTSPRITE_CLIPPING_CIRCLE:
			glUniform1f(renderer->uniformUsePointClippingCirclePos,1);
			break;
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererPointSizeMode(const struct YsGLSL3DRenderer *renderer,int pointSizeMode)
{
	if(NULL!=renderer && 0<=renderer->uniformUsePointSizeInPixelPos && 0<=renderer->uniformUsePointSizeIn3DPos)
	{
		GLfloat useSizeInPixel=0,useSizeIn3D=0;

		switch(pointSizeMode)
		{
		case YSGLSL_POINTSPRITE_SIZE_IN_PIXEL:
			useSizeInPixel=1;
			break;
		case YSGLSL_POINTSPRITE_SIZE_IN_3DSPACE:
			useSizeIn3D=1;
			break;
		}

		glUniform1f(renderer->uniformUsePointSizeInPixelPos,useSizeInPixel);
		glUniform1f(renderer->uniformUsePointSizeIn3DPos,useSizeIn3D);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLSet3DRendererUniformPointSize(const struct YsGLSL3DRenderer *renderer,float pointSize)
{
	if(NULL!=renderer && 0<=renderer->uniformPointSizePos)
	{
		glUniform1f(renderer->uniformPointSizePos,pointSize);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLDrawPrimitiveVtxTexCoordColPointSizefv(
    const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,
    const GLfloat vtx[],const GLfloat texCoord[],const GLfloat color[],const GLfloat pointSize[])
{
	YsGLSLTexCoordIsGivenAsArray((struct YsGLSL3DRenderer *)renderer);
	YsGLSLColorIsGivenAsArray((struct YsGLSL3DRenderer *)renderer);

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribTexCoordPos &&
	   0<=renderer->attribColorPos &&
	   0<=renderer->attribPointSizePos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribTexCoordPos,2,GL_FLOAT,GL_FALSE,0,texCoord);
		glVertexAttribPointer(renderer->attribColorPos,4,GL_FLOAT,GL_FALSE,0,color);
		glVertexAttribPointer(renderer->attribPointSizePos,1,GL_FLOAT,GL_FALSE,0,pointSize);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLDrawPrimitiveVtxTexCoordPointSizefv(
    const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,
    const GLfloat vtx[],const GLfloat texCoord[],const GLfloat pointSize[])
{
	YsGLSLTexCoordIsGivenAsArray((struct YsGLSL3DRenderer *)renderer);
	YsGLSLColorIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer);

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribTexCoordPos &&
	   0<=renderer->attribPointSizePos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribTexCoordPos,2,GL_FLOAT,GL_FALSE,0,texCoord);
		glVertexAttribPointer(renderer->attribPointSizePos,1,GL_FLOAT,GL_FALSE,0,pointSize);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLSLDrawPrimitiveVtxColPointSizefv(
    const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,
    const GLfloat vtx[],const GLfloat color[],const GLfloat pointSize[])
{
	YsGLSLTexCoordIsNotGivenAsArray((struct YsGLSL3DRenderer *)renderer);
	YsGLSLColorIsGivenAsArray((struct YsGLSL3DRenderer *)renderer);

	if(NULL!=renderer &&
	   0<=renderer->attribVertexPos &&
	   0<=renderer->attribColorPos &&
	   0<=renderer->attribPointSizePos)
	{
		glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
		glVertexAttribPointer(renderer->attribColorPos,4,GL_FLOAT,GL_FALSE,0,color);
		glVertexAttribPointer(renderer->attribPointSizePos,1,GL_FLOAT,GL_FALSE,0,pointSize);
		glDrawArrays(mode,0,nVtx);
		return YSOK;
	}
	return YSERR;
}


