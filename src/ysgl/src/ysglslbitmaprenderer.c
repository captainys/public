/* ////////////////////////////////////////////////////////////

File Name: ysglslbitmaprenderer.c
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

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef _WIN32
#define GL_GLEXT_PROTOTYPES
#endif

// I didn't find a convenient macro to identify iOS.  Please add a macro __APPLE_IOS__ to the preprocessor macro option.

#include "ysglheader.h"
#include "ysgldef.h"
#include "ysglslbitmaprenderer.h"
#include "ysglslprogram.h"
#include "ysglslutil.h"

struct YsGLSLBitmapRenderer
{
	int inUse;

	// Shader and Program
	GLuint programId;
	GLuint vertexShaderId,fragmentShaderId;

	// Reserve texture
	GLuint reserveTextureId;

	// Uniform positions
	GLint projectionPos;
	GLint modelViewPos;

	GLint viewportWidthPos;
	GLint viewportHeightPos;

	GLint texturePos;
	GLint transformationTypePos;
	GLint viewportOriginPos;
	GLint alphaCutOffPos;

	// Attribute positions
	GLint vertexPos;
	GLint offsetPos;
	GLint texCoordPos;
};

static struct YsGLSLBitmapRenderer *YsGLSLAllocBitmapRenderer(void)
{
	struct YsGLSLBitmapRenderer *renderer=(struct YsGLSLBitmapRenderer *)malloc(sizeof(struct YsGLSLBitmapRenderer));

	renderer->inUse=0;

	// Shader and Program
	renderer->programId=0;
	renderer->vertexShaderId=0;
	renderer->fragmentShaderId=0;

	// Reserve texture
	renderer->reserveTextureId=0;

	// Uniform positions
	renderer->projectionPos=-1;
	renderer->modelViewPos=-1;

	renderer->viewportWidthPos=-1;
	renderer->viewportHeightPos=-1;

	renderer->texturePos=-1;
	renderer->transformationTypePos=-1;
	renderer->viewportOriginPos=-1;
	renderer->alphaCutOffPos=-1;

	// Attribute positions
	renderer->vertexPos=-1;
	renderer->offsetPos=-1;
	renderer->texCoordPos=-1;


	return renderer;
}

static void YsGLSLBitmapRendererCreateProgram(struct YsGLSLBitmapRenderer *renderer)
{
	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_bmpRendererVertexShaderSrc_nLine,
	    YSGLSL_bmpRendererVertexShaderSrc,
	    renderer->fragmentShaderId,
	    YSGLSL_bmpRendererFragmentShaderSrc_nLine,
	    YSGLSL_bmpRendererFragmentShaderSrc);

	renderer->projectionPos=glGetUniformLocation(renderer->programId,"projection");
	renderer->modelViewPos=glGetUniformLocation(renderer->programId,"modelView");
	renderer->viewportWidthPos=glGetUniformLocation(renderer->programId,"viewportWid");
	renderer->viewportHeightPos=glGetUniformLocation(renderer->programId,"viewportHei");
	renderer->texturePos=glGetUniformLocation(renderer->programId,"texture");
	renderer->transformationTypePos=glGetUniformLocation(renderer->programId,"transformationType");
	renderer->viewportOriginPos=glGetUniformLocation(renderer->programId,"viewportOrigin");
	renderer->alphaCutOffPos=glGetUniformLocation(renderer->programId,"alphaCutOff");

	renderer->vertexPos=glGetAttribLocation(renderer->programId,"vertex");
	renderer->offsetPos=glGetAttribLocation(renderer->programId,"offset");
	renderer->texCoordPos=glGetAttribLocation(renderer->programId,"texCoord");
}

struct YsGLSLBitmapRenderer *YsGLSLCreateBitmapRenderer(void)
{
	GLuint prevProgramId;
	GLenum prevActiveTexture;
	GLuint prevBinding2d;
	struct YsGLSLBitmapRenderer *renderer;
	// GLfloat identity[]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};

	renderer=YsGLSLAllocBitmapRenderer();
	YsGLSLBitmapRendererCreateProgram(renderer);


	printf("projectionPos=%d\n",renderer->projectionPos);
	printf("modelViewPos=%d\n",renderer->modelViewPos);
	printf("viewportWidPos=%d\n",renderer->viewportWidthPos);
	printf("viewportHeiPos=%d\n",renderer->viewportHeightPos);
	printf("texturePos=%d\n",renderer->texturePos);
	printf("transformationTypePos=%d\n",renderer->transformationTypePos);
	printf("viewportOriginPos=%d\n",renderer->viewportOriginPos);
	printf("alphaCutOffPos=%d\n",renderer->alphaCutOffPos);

	printf("vertexPos=%d\n",renderer->vertexPos);
	printf("offsetPos=%d\n",renderer->offsetPos);
	printf("texCoordPos=%d\n",renderer->texCoordPos);


	// Set up reserve texture
	glGetIntegerv(GL_ACTIVE_TEXTURE,(GLint *)&prevActiveTexture);

	glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D,(GLint *)&prevBinding2d);

	glGenTextures(1,&renderer->reserveTextureId);
	glBindTexture(GL_TEXTURE_2D,renderer->reserveTextureId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D,prevBinding2d);
	glActiveTexture(prevActiveTexture);

	printf("Reserve Texture Id=%d\n",renderer->reserveTextureId);


	// Initialize uniforms
	glGetIntegerv(GL_CURRENT_PROGRAM,(GLint *)&prevProgramId);
	glUseProgram(renderer->programId);

	// glUniformMatrix4fv(renderer->projectionPos,1,GLfalse,identity);
	// glUniformMatrix4fv(renderer->modelViewPos,1,GLfalse,identity);

	glUniform1i(renderer->texturePos,0);
	glUniform1i(renderer->transformationTypePos,1);
	glUniform1i(renderer->viewportOriginPos,YSGLSL_TOPLEFT_AS_ORIGIN);
	glUniform1f(renderer->alphaCutOffPos,0.01f);

	glUseProgram(prevProgramId);

	return renderer;
}

void YsGLSLDeleteBitmapRenderer(struct YsGLSLBitmapRenderer *renderer)
{
	glDeleteProgram(renderer->programId);
	glDeleteShader(renderer->vertexShaderId);
	glDeleteShader(renderer->fragmentShaderId);
	glDeleteTextures(1,&renderer->reserveTextureId);
}


GLuint YsGLSLUseBitmapRenderer(struct YsGLSLBitmapRenderer *renderer)
{
	GLuint prevProgramId;

	glGetIntegerv(GL_CURRENT_PROGRAM,(GLint *)&prevProgramId);

	if(NULL!=renderer)
	{
		if(1==renderer->inUse)
		{
			printf("Warning!!!! A BitmapRenderer is activated again without being de-activated after last use!!!!\n");
		}

		glUseProgram(renderer->programId);

		glEnableVertexAttribArray(renderer->vertexPos);
		glEnableVertexAttribArray(renderer->offsetPos);
		glEnableVertexAttribArray(renderer->texCoordPos);

		renderer->inUse=1;
	}

	return prevProgramId;
}

void YsGLSLEndUseBitmapRenderer(struct YsGLSLBitmapRenderer *renderer)
{
	if(NULL!=renderer)
	{
		glDisableVertexAttribArray(renderer->vertexPos);
		glDisableVertexAttribArray(renderer->offsetPos);
		glDisableVertexAttribArray(renderer->texCoordPos);

		renderer->inUse=0;
	}
}

void YsGLSLRenderRGBABitmap2D(
    struct YsGLSLBitmapRenderer *renderer,
    int x,int y,
    enum YSGLSL_HALIGN hAlign,enum YSGLSL_VALIGN vAlign,
    unsigned int wid,unsigned int hei,const unsigned char rgba[])
{
	GLenum prevActiveTexture;
	GLuint prevBinding2d;

	if(NULL==renderer)
	{
		return;
	}

	glGetIntegerv(GL_ACTIVE_TEXTURE,(GLint *)&prevActiveTexture);
	glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D,(GLint *)&prevBinding2d);

	glBindTexture(GL_TEXTURE_2D,renderer->reserveTextureId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,wid,hei,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);

	YsGLSLRenderTexture2D(renderer,x,y,hAlign,vAlign,wid,hei,renderer->reserveTextureId);

	glBindTexture(GL_TEXTURE_2D,prevBinding2d);
	glActiveTexture(prevActiveTexture);
}


static void RenderTexture2D(
    struct YsGLSLBitmapRenderer *renderer,
    int x,int y,
    enum YSGLSL_HALIGN hAlign,enum YSGLSL_VALIGN vAlign,
    unsigned int wid,unsigned int hei,GLuint samplerIdent,
    int invert)
{
	GLfloat vertex[16];
	GLfloat texCoord[8];
	GLfloat offset[8];

	if(NULL==renderer)
	{
		return;
	}

	vertex[ 0]=(float)x;
	vertex[ 1]=(float)y;
	vertex[ 2]=0;
	vertex[ 3]=0;
	vertex[ 4]=(float)x;
	vertex[ 5]=(float)y;
	vertex[ 6]=0;
	vertex[ 7]=0;
	vertex[ 8]=(float)x;
	vertex[ 9]=(float)y;
	vertex[10]=0;
	vertex[11]=0;
	vertex[12]=(float)x;
	vertex[13]=(float)y;
	vertex[14]=0;
	vertex[15]=0;

	if(0==invert)
	{
		texCoord[0]=(float)0;
		texCoord[1]=(float)0;
		texCoord[2]=(float)1;
		texCoord[3]=(float)0;
		texCoord[4]=(float)1;
		texCoord[5]=(float)1;
		texCoord[6]=(float)0;
		texCoord[7]=(float)1;
	}
	else
	{
		texCoord[0]=(float)0;
		texCoord[1]=(float)1;
		texCoord[2]=(float)1;
		texCoord[3]=(float)1;
		texCoord[4]=(float)1;
		texCoord[5]=(float)0;
		texCoord[6]=(float)0;
		texCoord[7]=(float)0;
	}

	offset[0]=(float)0  ;
	offset[1]=(float)0;
	offset[2]=(float)wid;
	offset[3]=(float)0;
	offset[4]=(float)wid;
	offset[5]=(float)hei;
	offset[6]=(float)0  ;
	offset[7]=(float)hei;

	switch(hAlign)
	{
	default:
	case YSGLSL_HALIGN_LEFT:
		offset[0]= (float)0;
		offset[2]= (float)wid;
		offset[4]= (float)wid;
		offset[6]= (float)0;
		break;
	case YSGLSL_HALIGN_CENTER:
		offset[0]=-(float)(wid/2);
		offset[2]= (float)(wid/2);
		offset[4]= (float)(wid/2);
		offset[6]=-(float)(wid/2);
		break;
	case YSGLSL_HALIGN_RIGHT:
		offset[0]=-(float)wid;
		offset[2]= (float)0;
		offset[4]= (float)0;
		offset[6]=-(float)wid;
		break;
	}

	switch(vAlign)
	{
	default:
	case YSGLSL_VALIGN_TOP:
		offset[1]=-(float)hei;
		offset[3]=-(float)hei;
		offset[5]= (float)0;
		offset[7]= (float)0;
		break;
	case YSGLSL_VALIGN_CENTER:
		offset[1]=-(float)(hei/2);
		offset[3]=-(float)(hei/2);
		offset[5]= (float)(hei/2);
		offset[7]= (float)(hei/2);
		break;
	case YSGLSL_VALIGN_BOTTOM:
		offset[1]=(float)0;
		offset[3]=(float)0;
		offset[5]=(float)hei;
		offset[7]=(float)hei;
		break;
	}

	{
		GLint viewport[4];

		glGetIntegerv(GL_VIEWPORT,viewport);

		glUniform1f(renderer->viewportWidthPos,(float)viewport[2]);
		glUniform1f(renderer->viewportHeightPos,(float)viewport[3]);
	}

	glUniform1i(renderer->transformationTypePos,1);
	glUniform1i(renderer->texturePos,samplerIdent);

	glVertexAttribPointer(renderer->vertexPos,4,GL_FLOAT,GL_FALSE,0,vertex);
	glVertexAttribPointer(renderer->offsetPos,2,GL_FLOAT,GL_FALSE,0,offset);
	glVertexAttribPointer(renderer->texCoordPos,2,GL_FLOAT,GL_FALSE,0,texCoord);

	glDrawArrays(GL_TRIANGLE_FAN,0,4);
}

void YsGLSLRenderTexture2D(
    struct YsGLSLBitmapRenderer *renderer,
    int x,int y,
    enum YSGLSL_HALIGN hAlign,enum YSGLSL_VALIGN vAlign,
    unsigned int wid,unsigned int hei,GLuint textureId)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textureId);
	RenderTexture2D(renderer,x,y,hAlign,vAlign,wid,hei,0,0);
}

void YsGLSLRenderBoundTexture2D(
    struct YsGLSLBitmapRenderer *renderer,
    int x,int y,
    enum YSGLSL_HALIGN hAlign,enum YSGLSL_VALIGN vAlign,
    unsigned int wid,unsigned int hei,GLuint samplerIdent)
{
	RenderTexture2D(renderer,x,y,hAlign,vAlign,wid,hei,samplerIdent,0);
}

void YsGLSLRenderTexture2DInverted(
    struct YsGLSLBitmapRenderer *renderer,
    int x,int y,
    enum YSGLSL_HALIGN hAlign,enum YSGLSL_VALIGN vAlign,
    unsigned int wid,unsigned int hei,GLuint textureId)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textureId);
	RenderTexture2D(renderer,x,y,hAlign,vAlign,wid,hei,0,1);
}

