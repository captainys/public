/* ////////////////////////////////////////////////////////////

File Name: ysglslplain2ddrawing.c
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
#include "ysglslplain2ddrawing.h"
#include "ysglslutil.h"
#include "ysglmath.h"
#include "ysgldef.h"
#include "ysglslprogram.h"



struct YsGLSLPlain2DRenderer
{
	int inUse;
	int colorArrayEnabled;

	// Shader and Program
	GLuint programId;
	GLuint vertexShaderId,fragmentShaderId;

	// Uniforms
	GLint transformPos;
	GLfloat transform[16];

	GLint useTexturePos;
	GLint texIdentPos;

	// Can be generic attribute
	GLfloat uniformColor[4];

	// Attributes
	GLint vertexPos,colorInPos;
	GLint pointSizePos;
	GLint texCoordPos;
};

static struct YsGLSLPlain2DRenderer *YsGLSLAllocAndInitPlain2DRenderer(void)
{
	struct YsGLSLPlain2DRenderer *renderer=(struct YsGLSLPlain2DRenderer *)malloc(sizeof(struct YsGLSLPlain2DRenderer));
	if(NULL!=renderer)
	{
		renderer->programId=0;
		renderer->vertexShaderId=0;
		renderer->fragmentShaderId=0;

		renderer->uniformColor[0]=0;
		renderer->uniformColor[1]=0;
		renderer->uniformColor[2]=0;
		renderer->uniformColor[3]=0;

		renderer->inUse=0;
		renderer->colorArrayEnabled=0;
		renderer->transformPos=-1;
		renderer->vertexPos=-1;
		renderer->colorInPos=-1;
		renderer->pointSizePos=-1;
	}
	return renderer;
}

static void YsGLSLPlain2DDrawingCreateProgram(struct YsGLSLPlain2DRenderer *renderer)
{
	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);
	renderer->programId=glCreateProgram();

	YsGLSLCompileAndLinkVertexAndFragmentShader(
	    renderer->programId,
	    renderer->vertexShaderId,
	    YSGLSL_plain2DDrawingVertexShaderSrc_nLine,
	    YSGLSL_plain2DDrawingVertexShaderSrc,
	    renderer->fragmentShaderId,
	    YSGLSL_plain2DDrawingFragmentShaderSrc_nLine,
	    YSGLSL_plain2DDrawingFragmentShaderSrc);

	// Uniforms
	renderer->transformPos=glGetUniformLocation(renderer->programId,"transform");
	renderer->useTexturePos=glGetUniformLocation(renderer->programId,"useTexture");
	renderer->texIdentPos=glGetUniformLocation(renderer->programId,"textureIdent");

	// Attributes
	renderer->vertexPos=glGetAttribLocation(renderer->programId,"vertex");
	renderer->colorInPos=glGetAttribLocation(renderer->programId,"colorIn");
	renderer->pointSizePos=glGetAttribLocation(renderer->programId,"pointSizeIn");
	renderer->texCoordPos=glGetAttribLocation(renderer->programId,"texCoordIn");

	printf("Verify locations\n");
	printf("transformPos: %d\n",renderer->transformPos);
	printf("useTexturePos: %d\n",renderer->useTexturePos);
	printf("textureIdentPos: %d\n",renderer->texIdentPos);

	// Attributes
	printf("vertexPos: %d\n",renderer->vertexPos);
	printf("colorInPos: %d\n",renderer->colorInPos);
	printf("pointSizePos: %d\n",renderer->pointSizePos);
	printf("texCoordPos: %d\n",renderer->texCoordPos);
}

static void YsGLSLPlain2DDrawingInitializeUniform(struct YsGLSLPlain2DRenderer *renderer)
{
	GLuint prevProgramId;

	glGetIntegerv(GL_CURRENT_PROGRAM,(GLint *)&prevProgramId);
	glUseProgram(renderer->programId);

	YsGLSLLoadIdentityMatrix4(renderer->transformPos);
	YsGLMakeIdentityfv(renderer->transform);

	glUseProgram(prevProgramId);
}

struct YsGLSLPlain2DRenderer *YsGLSLCreatePlain2DRenderer(void)
{
	struct YsGLSLPlain2DRenderer *renderer=YsGLSLAllocAndInitPlain2DRenderer();
	if(NULL!=renderer)
	{
		YsGLSLPlain2DDrawingCreateProgram(renderer);
		YsGLSLPlain2DDrawingInitializeUniform(renderer);
	}
	return renderer;
}

void YsGLSLDeletePlain2DRenderer(struct YsGLSLPlain2DRenderer *renderer)
{
	glDeleteProgram(renderer->programId);
	glDeleteShader(renderer->vertexShaderId);
	glDeleteShader(renderer->fragmentShaderId);

	free(renderer);
}

void YsGLSLUseWindowCoordinateInPlain2DDrawing(struct YsGLSLPlain2DRenderer *renderer,int topLeftAsOrigin)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	if(0!=topLeftAsOrigin)
	{
		// (0,0) goes to (-1,1),  (viewport[2],viewport[3]) goes to (1,-1)
		// x=sx*(2.0/vpwid)-1.0;
		// y=1.0-sy*(2.0/vpHei);

		// scaleX   0      0    -1
		// 0     -scaleY   0     1
		// 0        0      1     0
		// 0        0      0     1

		GLfloat mat[16];
		GLfloat scaleX,scaleY;
		scaleX=2.0f/(float)viewport[2];
		scaleY=2.0f/(float)viewport[3];

		mat[ 0]=scaleX; mat[ 4]=0;       mat[ 8]=0; mat[12]=-1;
		mat[ 1]=0;      mat[ 5]=-scaleY; mat[ 9]=0; mat[13]= 1;
		mat[ 2]=0;      mat[ 6]=0;       mat[10]=1; mat[14]= 0;
		mat[ 3]=0;      mat[ 7]=0;       mat[11]=0; mat[15]= 1;

		glUniformMatrix4fv(renderer->transformPos,1,GL_FALSE,mat);
		YsGLCopyMatrixfv(renderer->transform,mat);
	}
	else
	{
		// (0,0) goes to (-1,-11),  (viewport[2],viewport[3]) goes to (1,1)
		// x=sx*(2.0/vpwid)-1.0;
		// y=sy*(2.0/vpHei)-1.0;

		// scaleX   0      0    -1
		// 0      scaleY   0    -1
		// 0        0      1     0
		// 0        0      0     1

		GLfloat mat[16];
		GLfloat scaleX,scaleY;
		scaleX=2.0f/(float)viewport[2];
		scaleY=2.0f/(float)viewport[3];

		mat[ 0]=scaleX; mat[ 4]=0;      mat[ 8]=0; mat[12]=-1;
		mat[ 1]=0;      mat[ 5]=scaleY; mat[ 9]=0; mat[13]=-1;
		mat[ 2]=0;      mat[ 6]=0;      mat[10]=1; mat[14]= 0;
		mat[ 3]=0;      mat[ 7]=0;      mat[11]=0; mat[15]= 1;

		glUniformMatrix4fv(renderer->transformPos,1,GL_FALSE,mat);
		YsGLCopyMatrixfv(renderer->transform,mat);
	}
}

void YsGLSLDontUseWindowCoordinateInPlain2DDrawing(struct YsGLSLPlain2DRenderer *renderer)
{
	YsGLSLLoadIdentityMatrix4(renderer->transformPos);
	YsGLMakeIdentityfv(renderer->transform);
}

void YsGLSLSetPlain2DRendererUniformColor(struct YsGLSLPlain2DRenderer *renderer,const GLfloat color[4])
{
	renderer->uniformColor[0]=color[0];
	renderer->uniformColor[1]=color[1];
	renderer->uniformColor[2]=color[2];
	renderer->uniformColor[3]=color[3];
}

void YsGLSLSetPlain2DRendererUniformPointSize(struct YsGLSLPlain2DRenderer *renderer,GLfloat pointSize)
{
	glVertexAttrib1f(renderer->pointSizePos,pointSize);
}

void YsGLSLGetPlain2DRendererTransformationfv(GLfloat tfm[16],struct YsGLSLPlain2DRenderer *renderer)
{
	YsGLCopyMatrixfv(tfm,renderer->transform);
}

void YsGLSLSetPlain2DRendererTransformationfv(struct YsGLSLPlain2DRenderer *renderer,const GLfloat tfm[16])
{
	glUniformMatrix4fv(renderer->transformPos,1,GL_FALSE,tfm);
	YsGLCopyMatrixfv(renderer->transform,tfm);
}

GLuint YsGLSLUsePlain2DRenderer(struct YsGLSLPlain2DRenderer *renderer)
{
	GLuint prevProgramId;

	if(1==renderer->inUse)
	{
		printf("Warning!!!! A Plain2DRenderer is activated again without being de-activated after last use!!!!\n");
	}

	glGetIntegerv(GL_CURRENT_PROGRAM,(GLint *)&prevProgramId);
	glUseProgram(renderer->programId);

	glEnableVertexAttribArray(renderer->vertexPos);
	glEnableVertexAttribArray(renderer->colorInPos);
	renderer->colorArrayEnabled=1;

	renderer->inUse=1;

	return prevProgramId;
}

void YsGLSLEndUsePlain2DRenderer(struct YsGLSLPlain2DRenderer *renderer)
{
	glDisableVertexAttribArray(renderer->vertexPos);
	if(1==renderer->colorArrayEnabled)
	{
		glDisableVertexAttribArray(renderer->colorInPos);
	}

	renderer->colorArrayEnabled=0;
	renderer->inUse=0;
}

GLuint YsGLSLGetPlain2DDrawingVertexPosition(struct YsGLSLPlain2DRenderer *renderer)
{
	return renderer->vertexPos;
}

GLuint YsGLSLGetPlain2DDrawingColorPosition(struct YsGLSLPlain2DRenderer *renderer)
{
	return renderer->colorInPos;
}

void YsGLSLDrawPlain2DLinef(
    const struct YsGLSLPlain2DRenderer *renderer,
    const float x0,const float y0,
    const float x1,const float y1,
    const float r,const float g,const float b,const float a)
{
	static GLfloat vertex[4];
	static GLfloat color[8];

	vertex[0]=(GLfloat)x0;
	vertex[1]=(GLfloat)y0;
	vertex[2]=(GLfloat)x1;
	vertex[3]=(GLfloat)y1;

	color[0]=(GLfloat)r;
	color[1]=(GLfloat)g;
	color[2]=(GLfloat)b;
	color[3]=(GLfloat)a;
	color[4]=(GLfloat)r;
	color[5]=(GLfloat)g;
	color[6]=(GLfloat)b;
	color[7]=(GLfloat)a;

	YsGLSLDrawPlain2DPrimitiveVtxColfv(renderer,GL_LINES,2,vertex,color);
}

void YsGLSLDrawPlain2DLined(
    const struct YsGLSLPlain2DRenderer *renderer,
    const double x0,const double y0,
    const double x1,const double y1,
    const double r,const double g,const double b,const double a)
{
	YsGLSLDrawPlain2DLinef(renderer,(float)x0,(float)y0,(float)x1,(float)y1,(float)r,(float)g,(float)b,(float)a);
}

void YsGLSLDrawPlain2DPrimitivefv(
    const struct YsGLSLPlain2DRenderer *renderer,
	GLenum mode,
	const size_t nVertex,const GLfloat vertex[],const GLfloat color[])
{
	YsGLSLDrawPlain2DPrimitiveVtxColfv(renderer,mode,nVertex,vertex,color);
}

void YsGLSLDrawPlain2DPrimitiveVtxColfv(
    const struct YsGLSLPlain2DRenderer *renderer,
	GLenum mode,
	const size_t nVertex,const GLfloat vertex[],const GLfloat color[])
{
	if(1!=renderer->colorArrayEnabled)
	{
		struct YsGLSLPlain2DRenderer *nonConstPtr=(struct YsGLSLPlain2DRenderer *)renderer;
		glEnableVertexAttribArray(renderer->colorInPos);
		nonConstPtr->colorArrayEnabled=1;
	}

	glDisableVertexAttribArray(renderer->texCoordPos);
	glUniform1f(renderer->useTexturePos,0.0f);
	glVertexAttrib2f(renderer->texCoordPos,0.0f,0.0f);

	glVertexAttribPointer(renderer->vertexPos,2,GL_FLOAT,GL_FALSE,0,vertex);
	glVertexAttribPointer(renderer->colorInPos,4,GL_FLOAT,GL_FALSE,0,color);
	glDrawArrays(mode,0,(GLsizei)nVertex);
}

void YsGLSLDrawPlain2DPrimitiveVtxTexCoordfv(
	const struct YsGLSLPlain2DRenderer *renderer,
	GLenum mode,
	const size_t nVertex,const GLfloat vertex[],const GLfloat texCoord[])
{
	if(1==renderer->colorArrayEnabled)
	{
		struct YsGLSLPlain2DRenderer *nonConstPtr=(struct YsGLSLPlain2DRenderer *)renderer;
		glDisableVertexAttribArray(renderer->colorInPos);
		nonConstPtr->colorArrayEnabled=0;
	}

	glEnableVertexAttribArray(renderer->texCoordPos);
	glUniform1f(renderer->useTexturePos,1.0f);
	glVertexAttribPointer(renderer->texCoordPos,2,GL_FLOAT,GL_FALSE,0,texCoord);

	glVertexAttrib4fv(renderer->colorInPos,renderer->uniformColor);
	glVertexAttribPointer(renderer->vertexPos,2,GL_FLOAT,GL_FALSE,0,vertex);
	glDrawArrays(mode,0,(GLsizei)nVertex);
}

void YsGLSLDrawPlain2DPrimitiveVtxfv(
    const struct YsGLSLPlain2DRenderer *renderer,
	GLenum mode,
	const size_t nVertex,const GLfloat vertex[])
{
	if(1==renderer->colorArrayEnabled)
	{
		struct YsGLSLPlain2DRenderer *nonConstPtr=(struct YsGLSLPlain2DRenderer *)renderer;
		glDisableVertexAttribArray(renderer->colorInPos);
		nonConstPtr->colorArrayEnabled=0;
	}

	glDisableVertexAttribArray(renderer->texCoordPos);
	glUniform1f(renderer->useTexturePos,0.0f);

	glVertexAttrib4fv(renderer->colorInPos,renderer->uniformColor);
	glVertexAttribPointer(renderer->vertexPos,2,GL_FLOAT,GL_FALSE,0,vertex);
	glDrawArrays(mode,0,(GLsizei)nVertex);
}

