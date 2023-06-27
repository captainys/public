/* ////////////////////////////////////////////////////////////

File Name: ysglsldrawfontbitmap.c
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


#include <ysglheader.h>


#if defined(__APPLE__) && (TARGET_OS_EMBEDDED!=0 || TARGET_OS_IPHONE!=0 || TARGET_OS_IPHONE_SIMULATOR!=0)
	#define YS_GLSL_LOWP "lowp "
	#define YS_GLSL_MIDP "mediump "
	#define YS_GLSL_HIGHP "highp "
#else
	#define YS_GLSL_LOWP
	#define YS_GLSL_MIDP
	#define YS_GLSL_HIGHP
#endif



/*  Visual C++ users, since Microsoft is on denial and does not admit
    Direct 3D has failed, you need to downlaod glext.h from the following
    URL and copy to the include directory.

        http://www.opengl.org/registry/api/glext.h

    If you are keeping all external include files in c:\external\include,
    glext.h must be copied to:

        c:\external\include\GL\glext.h

    Or, find gl.h in your include directory and copy glext.h to the same 
    directory.
*/


#if defined(_WIN32) && !defined(YS_IS_UNIVERSAL_WINDOWS_APP)
	#include "ysglwindows.h"
#endif

#include "ysglsldrawfontbitmap.h"
#include "ysglfontdata.h"


// This shader program assumes the height of the font is greater or equal to the width.

//   This program can be written a lot simpler with point sprite, which doesn't seem
//   to be widely supported, even when the system falsely claims it compiles with
//   higher version of GLSL.  Can anyone fix this compability mess?

// Update 2012/12/27
//   Don't count on Point Sprite.  It is bogus.  It is supported somewhere, but not supported somewhere else.
//   There doesn't seem to be a way to reliably tell if it is REALLY supported or not.  

#define YSGLSL_BITMAPFONTRENDERER_NLETTER_PER_DRAW 32

static const char *uglyFontVertexShaderSrc[]=
{
	"uniform mat4 projection,modelView;\n",
	"uniform "YS_GLSL_MIDP" float viewportWid,viewportHei;\n",
	"uniform sampler2D texture;\n",
	"uniform bool windowCoord;\n",
	"uniform "YS_GLSL_LOWP" vec4 color;\n",
	"uniform int viewportOrigin;\n",
	"\n",
	"attribute vec3 vertex;\n",
	"attribute vec2 pixelOffset;\n",
	"attribute vec2 texCoord;\n",
	"attribute vec4 texCoordRange;\n",
	"\n",
	"varying "YS_GLSL_LOWP" vec2 texCoordOut;\n",
	"varying "YS_GLSL_MIDP" vec4 texCoordRangeOut;\n",
	"\n",
	"void main()\n",
	"{\n",
	"	float xOffset=(2.0*pixelOffset.x)/viewportWid;\n",
	"	float yOffset=(2.0*pixelOffset.y)/viewportHei;\n",
	"	vec4 offset=vec4(xOffset,yOffset,0,0);\n",
	"\n",
	"	texCoordOut=texCoord;\n",
	"	texCoordRangeOut=texCoordRange;\n",
	"	if(true!=windowCoord)\n",
	"	{\n",
	"		gl_Position=projection*modelView*vec4(vertex,1.0);\n",
	"		offset*=gl_Position[3];\n",
	"		gl_Position.xy+=offset.xy;\n",
	"	}\n",
	"	else\n",
	"	{\n",
	"		float xAbs=-1.0+2.0*vertex.x/viewportWid+xOffset;\n",
	"		float yAbs= 1.0-2.0*vertex.y/viewportHei;\n",
	"		if(0!=viewportOrigin)\n",
	"		{\n",
	"			yAbs=-yAbs;\n",
	"		}\n",
	"		yAbs+=yOffset;\n",
	"		gl_Position=vec4(xAbs,yAbs,0.0,1.0);\n",
	"	}\n",
	"} \n",
	""
};

static const char *uglyFontFragmentShaderSrc[]=
{
// Confirmed:
//   0<gl_PointCoord.x && gl_PointCoord.x<1
//   0<gl_PointCoord.y && gl_PointCoord.y<1
//
//   On screen coordinate:
//            (0,0)     (1,0)
//
//            (0,1)     (1,1)
//
// Seems to be configurable by GL_POINT_SPRITE_COORD_ORIGIN
//   http://www.opengl.org/sdk/docs/manglsl/xhtml/gl_PointCoord.xml
//
// Forget about it.  GLSL that comes with Mac OS 10.6 does not support gl_PointCoord.
//
	"uniform "YS_GLSL_MIDP" float viewportWid,viewportHei;\n",
	"uniform sampler2D texture;\n",
	"uniform bool windowCoord;\n",
	"uniform "YS_GLSL_LOWP" vec4 color;\n",
	"\n",
	"varying "YS_GLSL_LOWP" vec2 texCoordOut;\n",
	"varying "YS_GLSL_MIDP" vec4 texCoordRangeOut;\n",
	"\n",
	"void main()\n",
	"{\n",
	/* "gl_FragColor=vec4(texCoord_out.x,texCoord_out.y,1,1);\n", for testing purpose. */
	"	if(texCoordOut[0]<texCoordRangeOut[0] || texCoordRangeOut[1]<texCoordOut[0] ||\n",
	"	   texCoordOut[1]<texCoordRangeOut[2] || texCoordRangeOut[3]<texCoordOut[1])\n",
	"	{\n",
	"		discard;\n",
	"	}\n",
	"	"YS_GLSL_LOWP" vec4 texcell=texture2D(texture,texCoordOut);\n",
	"	if(0.5<texcell[0])\n",
	"	{\n",
	"		gl_FragColor=color;\n",
	"	}\n",
	"	else\n",
	"	{\n",
	"		discard;\n",
	"	}\n",
	"} \n",
};

struct YsGLSLBitmapFontRenderer
{
	int inUse;

	int largeBitmapWid,largeBitmapHei;
	int fontWid,fontHei;
	int windowCoord;
	int viewportWid,viewportHei;

	enum YSGLSL_BMPFONT_VIEWPORT_ORIGIN viewportOrigin;
	enum YSGLSL_BMPFONT_FIRST_LINE_ALIGNMENT firstLineAlignment;

	// Use for expanding vertex, texCoord, and pixelOffset arrays.
	GLfloat vtxArray[9*YSGLSL_BITMAPFONTRENDERER_NLETTER_PER_DRAW];
	GLfloat texCoordArray[6*YSGLSL_BITMAPFONTRENDERER_NLETTER_PER_DRAW];
	GLfloat texCoordRangeArray[12*YSGLSL_BITMAPFONTRENDERER_NLETTER_PER_DRAW];
	GLfloat pixelOffsetArray[6*YSGLSL_BITMAPFONTRENDERER_NLETTER_PER_DRAW];

	// Shader and Program
	GLuint programId;
	GLuint vertexShaderId,fragmentShaderId;

	// Uniform
	GLuint uniformViewportWidPos;
	GLuint uniformViewportHeiPos;
	GLuint uniformWindowCoordPos;
	GLuint uniformProjectionPos;
	GLuint uniformModelViewPos;
	GLuint uniformColorPos;
	GLuint uniformTexturePos;
	GLuint uniformViewportOriginPos;

	// Attributes
	GLuint attribVertexPos;
	GLuint attribPixelOffsetPos;
	GLuint attribTexCoordPos;
	GLuint attribTexCoordRangePos;

	// Font bitmaps
	GLuint fontTextureId;
};

static void YsGLSLBitmapFontRendererSetViewportSize(struct YsGLSLBitmapFontRenderer *renderer);


// Linux OpenGL driver has a bug.  It only reads the first line of the program.  It hasn't been fixed for quite long time.
// (Or has it been fixed in Ubuntu 14?)
static char *MakeSingleLineProgram(long long int nLine,const char *prog[])
{
	long long int nByte=0,i=0;
	char *dat=NULL;
	long long int ptr=0;

	for(i=0; i<nLine; ++i)
	{
		nByte+=strlen(prog[i]);
	}
	++nByte;

	dat=(char *)malloc(nByte);
	ptr=0;
	for(i=0; i<nLine; ++i)
	{
		strcpy(dat+ptr,prog[i]);
		ptr+=strlen(prog[i]);
	}

	return dat;
}

struct YsGLSLBitmapFontRenderer *YsGLSLCreateBitmapFontRenderer(void)
{
	struct YsGLSLBitmapFontRenderer *renderer=(struct YsGLSLBitmapFontRenderer *)malloc(sizeof(struct YsGLSLBitmapFontRenderer));
	const int errMsgLen=1024;
	char errMsg[1024];
	int compileSta=99999,infoLogLength=99999,acquiredErrMsgLen=99999;
	int linkSta=99999;
	GLuint prevProgramId;
	const GLfloat identity[16]=
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};


	renderer->inUse=0;

	renderer->vertexShaderId=glCreateShader(GL_VERTEX_SHADER);
	renderer->fragmentShaderId=glCreateShader(GL_FRAGMENT_SHADER);

#if 0
	// Linux OpenGL driver bug: Only reads the first line.
	glShaderSource(
	    renderer->vertexShaderId,
	    sizeof(uglyFontVertexShaderSrc)/sizeof(const char *),
	    uglyFontVertexShaderSrc,
	    NULL); // Last NULL assumes each line is C string
	glShaderSource(
	    renderer->fragmentShaderId,
	    sizeof(uglyFontFragmentShaderSrc)/sizeof(const char *),
	    uglyFontFragmentShaderSrc,
	    NULL); // Last NULL assumes each line is C string
#else
	{
		char *vtxShader[1],*fragShader[1];
		vtxShader[0]=MakeSingleLineProgram(sizeof(uglyFontVertexShaderSrc)/sizeof(const char *),uglyFontVertexShaderSrc);
		fragShader[0]=MakeSingleLineProgram(sizeof(uglyFontFragmentShaderSrc)/sizeof(const char *),uglyFontFragmentShaderSrc);

		glShaderSource(renderer->vertexShaderId,1,vtxShader,NULL); // Last NULL assumes each line is C string
		glShaderSource(renderer->fragmentShaderId,1,fragShader,NULL); // Last NULL assumes each line is C string

		free(vtxShader[0]);
		free(fragShader[0]);
	}
#endif


	glCompileShader(renderer->vertexShaderId);

	glGetShaderiv(renderer->vertexShaderId,GL_COMPILE_STATUS,&compileSta);
	glGetShaderiv(renderer->vertexShaderId,GL_INFO_LOG_LENGTH,&infoLogLength);
	printf("Compile Status %d Info Log Length %d\n",compileSta,infoLogLength);
	glGetShaderInfoLog(renderer->vertexShaderId,errMsgLen-1,&acquiredErrMsgLen,errMsg);
	printf("Error Message: %s\n",errMsg);

	glCompileShader(renderer->fragmentShaderId);

	glGetShaderiv(renderer->fragmentShaderId,GL_COMPILE_STATUS,&compileSta);
	glGetShaderiv(renderer->fragmentShaderId,GL_INFO_LOG_LENGTH,&infoLogLength);
	printf("Compile Status %d Info Log Length %d\n",compileSta,infoLogLength);
	glGetShaderInfoLog(renderer->fragmentShaderId,errMsgLen-1,&acquiredErrMsgLen,errMsg);
	printf("Error Message: %s\n",errMsg);

	renderer->programId=glCreateProgram();
	glAttachShader(renderer->programId,renderer->vertexShaderId);
	glAttachShader(renderer->programId,renderer->fragmentShaderId);
	glLinkProgram(renderer->programId);
	glGetProgramiv(renderer->programId,GL_LINK_STATUS,&linkSta);
	glGetProgramiv(renderer->programId,GL_INFO_LOG_LENGTH,&infoLogLength);
	printf("Link Status %d Info Log Length %d\n",linkSta,infoLogLength);
	glGetProgramInfoLog(renderer->programId,errMsgLen-1,&acquiredErrMsgLen,errMsg);
	printf("Error Message: %s\n",errMsg);


	// Uniforms
	renderer->uniformViewportWidPos=glGetUniformLocation(renderer->programId,"viewportWid");
	renderer->uniformViewportHeiPos=glGetUniformLocation(renderer->programId,"viewportHei");
	renderer->uniformWindowCoordPos=glGetUniformLocation(renderer->programId,"windowCoord");
	renderer->uniformProjectionPos=glGetUniformLocation(renderer->programId,"projection");
	renderer->uniformModelViewPos=glGetUniformLocation(renderer->programId,"modelView");
	renderer->uniformColorPos=glGetUniformLocation(renderer->programId,"color");
	renderer->uniformTexturePos=glGetUniformLocation(renderer->programId,"texture");
	renderer->uniformViewportOriginPos=glGetUniformLocation(renderer->programId,"viewportOrigin");

	// Attributes
	renderer->attribVertexPos=glGetAttribLocation(renderer->programId,"vertex");
	renderer->attribPixelOffsetPos=glGetAttribLocation(renderer->programId,"pixelOffset");
	renderer->attribTexCoordPos=glGetAttribLocation(renderer->programId,"texCoord");
	renderer->attribTexCoordRangePos=glGetAttribLocation(renderer->programId,"texCoordRange");

	printf("Verify locations\n");
	printf("uniformViewportWidPos: %d\n",renderer->uniformViewportWidPos);
	printf("uniformViewportHeiPos: %d\n",renderer->uniformViewportHeiPos);
	printf("uniformWindowCoordPos: %d\n",renderer->uniformWindowCoordPos);
	printf("uniformProjectionPos: %d\n",renderer->uniformProjectionPos);
	printf("uniformModelViewPos: %d\n",renderer->uniformModelViewPos);
	printf("uniformColorPos: %d\n",renderer->uniformColorPos);
	printf("uniformTexturePos: %d\n",renderer->uniformTexturePos);
	printf("uniformViewportOriginPos: %d\n",renderer->uniformViewportOriginPos);

	// Attributes
	printf("attribVertexPos: %d\n",renderer->attribVertexPos);
	printf("attribPixelOffsetPos: %d\n",renderer->attribPixelOffsetPos);
	printf("attribTexCoordPos: %d\n",renderer->attribTexCoordPos);
	printf("attribTexCoordRangePos: %d\n",renderer->attribTexCoordRangePos);

	renderer->firstLineAlignment=YSGLSL_BMPFONT_ALIGN_BOTTOMLEFT;

	glGenTextures(1,&renderer->fontTextureId);

	renderer->fontWid=0;
	renderer->fontHei=0;
	YsGLSLBitmapFontRendererRequestFontSize(renderer,12,16);



	glGetIntegerv(GL_CURRENT_PROGRAM,(GLint *)&prevProgramId);
	glUseProgram(renderer->programId);

	glUniformMatrix4fv(renderer->uniformModelViewPos,1,GL_FALSE,identity);
	glUniformMatrix4fv(renderer->uniformProjectionPos,1,GL_FALSE,identity);

	YsGLSLSetBitmapFontRendererColor4ub(renderer,255,255,255,255);

	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT,viewport);
		renderer->viewportWid=viewport[2];
		renderer->viewportHei=viewport[3];
		glUniform1f(renderer->uniformViewportWidPos,(GLfloat)renderer->viewportWid);
		glUniform1f(renderer->uniformViewportHeiPos,(GLfloat)renderer->viewportHei);
	}

	glUniform1i(renderer->uniformWindowCoordPos,1);
	renderer->windowCoord=1;

	glUniform1i(renderer->uniformViewportOriginPos,0);
	renderer->viewportOrigin=YSGLSL_BMPFONT_TOPLEFT_AS_ORIGIN;

	glUniform1i(renderer->uniformTexturePos,0);

	glUseProgram(prevProgramId);

	return renderer;
}

void YsGLSLDeleteBitmapFontRenderer(struct YsGLSLBitmapFontRenderer *renderer)
{
	glDeleteProgram(renderer->programId);
	glDeleteShader(renderer->vertexShaderId);
	glDeleteShader(renderer->fragmentShaderId);

	glDeleteTextures(1,&renderer->fontTextureId);

	free(renderer);
}

GLuint YsGLSLUseBitmapFontRenderer(struct YsGLSLBitmapFontRenderer *renderer)
{
	GLuint prevProgramId;
	glGetIntegerv(GL_CURRENT_PROGRAM,(GLint *)&prevProgramId);

	if(NULL!=renderer)
	{
		if(0!=renderer->inUse)
		{
			printf("Warning!  Bitmap font renderer is already activated.\n");
		}
		glUseProgram(renderer->programId);
		glEnableVertexAttribArray(renderer->attribVertexPos);
		glEnableVertexAttribArray(renderer->attribPixelOffsetPos);
		glEnableVertexAttribArray(renderer->attribTexCoordPos);
		glEnableVertexAttribArray(renderer->attribTexCoordRangePos);
		renderer->inUse=1;
	}
	return prevProgramId;
}

void YsGLSLEndUseBitmapFontRenderer(struct YsGLSLBitmapFontRenderer *renderer)
{
	if(NULL!=renderer)
	{
		glDisableVertexAttribArray(renderer->attribVertexPos);
		glDisableVertexAttribArray(renderer->attribPixelOffsetPos);
		glDisableVertexAttribArray(renderer->attribTexCoordPos);
		glDisableVertexAttribArray(renderer->attribTexCoordRangePos);
		renderer->inUse=0;
	}
}



static int YsGLSLBitmapFontRendererGetFirstLineOffset(struct YsGLSLBitmapFontRenderer *renderer)
{
	switch(renderer->firstLineAlignment)
	{
	default:
	case YSGLSL_BMPFONT_ALIGN_BOTTOMLEFT:
		return 0;
	case YSGLSL_BMPFONT_ALIGN_TOPLEFT:
		return -renderer->fontHei;
	}
}

void YsGLSLSetBitmapFontRendererViewportOrigin(
	struct YsGLSLBitmapFontRenderer *renderer,
	enum YSGLSL_BMPFONT_VIEWPORT_ORIGIN viewportOrigin)
{
	if(renderer->viewportOrigin!=viewportOrigin)
	{
		glUniform1i(renderer->uniformViewportOriginPos,(int)viewportOrigin);
		renderer->viewportOrigin=viewportOrigin;
	}
}

void YsGLSLSetBitmapFontRendererFirstLineAlignment(
	struct YsGLSLBitmapFontRenderer *renderer,
	enum YSGLSL_BMPFONT_FIRST_LINE_ALIGNMENT firstLineAlignment)
{
	renderer->firstLineAlignment=firstLineAlignment;
}

void YsGLSLBitmapFontRendererRequestFontSize(struct YsGLSLBitmapFontRenderer *renderer,int wid,int hei)
{
	/*  Idea: Make a big bitmap and store 10x10 fonts for character code from 0x21 to 0x80 */

	int fontWid,fontHei;
	const unsigned char * const *fontBitmapPtr=NULL;

	fontBitmapPtr=YsGlSelectFontBitmapPointerByHeight(&fontWid,&fontHei,hei);
	if(fontWid!=renderer->fontWid || fontHei!=renderer->fontHei)
	{
		int i;
		GLenum prevActiveTexture;
		GLuint prevBinding2d;

		int largeBitmapLen=0;
		unsigned char *largeBitmap=NULL;

		int texWid=1;
		int texHei=1;
		while(texWid<fontWid*10)
		{
			texWid*=2;
		}
		while(texHei<fontHei*10)
		{
			texHei*=2;
		}
		renderer->largeBitmapWid=texWid;
		renderer->largeBitmapHei=texHei;

		largeBitmapLen=4*sizeof(unsigned char)*renderer->largeBitmapWid*renderer->largeBitmapHei;
		largeBitmap=(unsigned char *)malloc(largeBitmapLen);
		for(i=0; i<largeBitmapLen; ++i)
		{
			largeBitmap[i]=0;
		}

		renderer->fontWid=fontWid;
		renderer->fontHei=fontHei;

		glGetIntegerv(GL_ACTIVE_TEXTURE,(GLint *)&prevActiveTexture);
		glActiveTexture(GL_TEXTURE0);

		glGetIntegerv(GL_TEXTURE_BINDING_2D,(GLint *)&prevBinding2d);
		glBindTexture(GL_TEXTURE_2D,renderer->fontTextureId);

		for(i=33; i<128; ++i)
		{
			const int relIdx=i-33;
			const int xInLargeBmp=fontWid*(relIdx%10);
			const int yInLargeBmp=fontHei*(relIdx/10);
			const char str[2]={i,0};

			YsGlWriteStringToRGBA8Bitmap(
			    str,255,255,255,255,largeBitmap,texWid,texHei,xInLargeBmp,yInLargeBmp,
			    fontBitmapPtr,fontWid,fontHei);
		}

		glBindTexture(GL_TEXTURE_2D,renderer->fontTextureId);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,renderer->largeBitmapWid,renderer->largeBitmapHei,0,GL_RGBA,GL_UNSIGNED_BYTE,largeBitmap);

		free(largeBitmap);

		glBindTexture(GL_TEXTURE_2D,prevBinding2d);
		glActiveTexture(prevActiveTexture);
	}
}

int YsGLSLGetBitmapFontRendererFontWidth(struct YsGLSLBitmapFontRenderer *renderer)
{
	if(NULL!=renderer)
	{
		return renderer->fontWid;
	}
	return 1;
}

int YsGLSLGetBitmapFontRendererFontHeight(struct YsGLSLBitmapFontRenderer *renderer)
{
	if(NULL!=renderer)
	{
		return renderer->fontHei;
	}
	return 1;
}

void YsGLSLSetBitmapFontRendererColor4ub(struct YsGLSLBitmapFontRenderer *renderer,unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
	const float col[4]={(float)r/255.0f,(float)g/255.0f,(float)b/255.0f,(float)a/255.0f};
	glUniform4fv(renderer->uniformColorPos,1,col);
}

void YsGLSLSetBitmapFontRendererColor3ub(struct YsGLSLBitmapFontRenderer *renderer,unsigned char r,unsigned char g,unsigned char b)
{
	YsGLSLSetBitmapFontRendererColor4ub(renderer,r,g,b,255);
}

void YsGLSLSetBitmapFontRendererColor4f(struct YsGLSLBitmapFontRenderer *renderer,float r,float g,float b,float a)
{
	const float col[4]={r,g,b,a};
	glUniform4fv(renderer->uniformColorPos,1,col);
}

void YsGLSLSetBitmapFontRendererColor3f(struct YsGLSLBitmapFontRenderer *renderer,float r,float g,float b)
{
	YsGLSLSetBitmapFontRendererColor4f(renderer,r,g,b,1.0f);
}



static void YsGLSLRenderBitmapFontRun(struct YsGLSLBitmapFontRenderer *renderer,const float coord[3],int n,const char str[])
{
	int xOffset=0,yOffset=YsGLSLBitmapFontRendererGetFirstLineOffset(renderer);
	int i;
	int nUsed=0;

	glVertexAttribPointer(renderer->attribVertexPos,3,GL_FLOAT,GL_FALSE,0,renderer->vtxArray);
	glVertexAttribPointer(renderer->attribPixelOffsetPos,2,GL_FLOAT,GL_FALSE,0,renderer->pixelOffsetArray);
	glVertexAttribPointer(renderer->attribTexCoordPos,2,GL_FLOAT,GL_FALSE,0,renderer->texCoordArray);
	glVertexAttribPointer(renderer->attribTexCoordRangePos,4,GL_FLOAT,GL_FALSE,0,renderer->texCoordRangeArray);

	/* Each letter is drawn by a triangle that encloses a rectangle that is large enough for the letter.
        +
        |\
        | \
        |  \
        +---+
        |   |\
        |   | \
        |   |  \
        +---+---+
       Therefore, hanging triangles must not be drawn.  Attribute texCoordRange is given to the shader for this purpose.

       Using GL_TRIANGLE_STRIP may be slightly more efficient.  Is that so?  In this case, I will need to repeat 
       two vertices after each rectangle (two-triangle pair).  Also what to do with a space?  Maybe using GL_TRIANGLES
       is good enough and maybe more efficient than using GL_TRIANGLE_STRIP.
       
       I'm happy with this implementation.
	*/

	for(i=0; i<n; i++)
	{
		if('\n'==str[i])
		{
			xOffset=0;
			yOffset-=renderer->fontHei;
		}
		else if('\t'==str[i])
		{
			xOffset+=4*renderer->fontWid;
		}
		else if(33<=str[i] && str[i]<127)
		{
			const int relIdx=str[i]-33;
			const int xInLargeBmp=renderer->fontWid*(relIdx%10);
			const int yInLargeBmp=renderer->fontHei*(relIdx/10);

			const GLfloat sMin=(GLfloat)xInLargeBmp/(GLfloat)renderer->largeBitmapWid;
			const GLfloat sMax=(GLfloat)(xInLargeBmp+renderer->fontWid)/(GLfloat)renderer->largeBitmapWid;
			const GLfloat sMax2=(GLfloat)(xInLargeBmp+renderer->fontWid*2)/(GLfloat)renderer->largeBitmapWid;

			const GLfloat tMin=(GLfloat)yInLargeBmp/(GLfloat)renderer->largeBitmapHei;
			const GLfloat tMax=(GLfloat)(yInLargeBmp+renderer->fontHei)/(GLfloat)renderer->largeBitmapHei;
			const GLfloat tMax2=(GLfloat)(yInLargeBmp+renderer->fontHei*2)/(GLfloat)renderer->largeBitmapHei;

			renderer->vtxArray[9*nUsed+ 0]=coord[0];
			renderer->vtxArray[9*nUsed+ 1]=coord[1];
			renderer->vtxArray[9*nUsed+ 2]=coord[2];
			renderer->vtxArray[9*nUsed+ 3]=coord[0];
			renderer->vtxArray[9*nUsed+ 4]=coord[1];
			renderer->vtxArray[9*nUsed+ 5]=coord[2];
			renderer->vtxArray[9*nUsed+ 6]=coord[0];
			renderer->vtxArray[9*nUsed+ 7]=coord[1];
			renderer->vtxArray[9*nUsed+ 8]=coord[2];

			renderer->texCoordArray[6*nUsed+ 0]=sMin;
			renderer->texCoordArray[6*nUsed+ 1]=tMin;
			renderer->texCoordArray[6*nUsed+ 2]=sMax2;
			renderer->texCoordArray[6*nUsed+ 3]=tMin;
			renderer->texCoordArray[6*nUsed+ 4]=sMin;
			renderer->texCoordArray[6*nUsed+ 5]=tMax2;

			renderer->texCoordRangeArray[12*nUsed +0]=sMin;
			renderer->texCoordRangeArray[12*nUsed +1]=sMax;
			renderer->texCoordRangeArray[12*nUsed +2]=tMin;
			renderer->texCoordRangeArray[12*nUsed +3]=tMax;
			renderer->texCoordRangeArray[12*nUsed +4]=sMin;
			renderer->texCoordRangeArray[12*nUsed +5]=sMax;
			renderer->texCoordRangeArray[12*nUsed +6]=tMin;
			renderer->texCoordRangeArray[12*nUsed +7]=tMax;
			renderer->texCoordRangeArray[12*nUsed +8]=sMin;
			renderer->texCoordRangeArray[12*nUsed +9]=sMax;
			renderer->texCoordRangeArray[12*nUsed+10]=tMin;
			renderer->texCoordRangeArray[12*nUsed+11]=tMax;

			renderer->pixelOffsetArray[6*nUsed+ 0]=(float)xOffset;
			renderer->pixelOffsetArray[6*nUsed+ 1]=(float)yOffset;
			renderer->pixelOffsetArray[6*nUsed+ 2]=(float)xOffset+renderer->fontWid*2.0f;
			renderer->pixelOffsetArray[6*nUsed+ 3]=(float)yOffset;
			renderer->pixelOffsetArray[6*nUsed+ 4]=(float)xOffset;
			renderer->pixelOffsetArray[6*nUsed+ 5]=(float)yOffset+renderer->fontHei*2.0f;

			++nUsed;
			if(YSGLSL_BITMAPFONTRENDERER_NLETTER_PER_DRAW<=nUsed)
			{
				glDrawArrays(GL_TRIANGLES,0,nUsed*3);
				nUsed=0;
			}

			xOffset+=renderer->fontWid;
		}
		else
		{
			xOffset+=renderer->fontWid;
		}
	}

	if(0<nUsed)
	{
		glDrawArrays(GL_TRIANGLES,0,nUsed*3);
		nUsed=0;
	}
}

void YsGLSLSetBitmapFontRendererViewportSize(struct YsGLSLBitmapFontRenderer *renderer,int width,int height)
{
	if(NULL!=renderer && (width!=renderer->viewportWid || height!=renderer->viewportHei))
	{
		glUniform1f(renderer->uniformViewportWidPos,(GLfloat)width);
		glUniform1f(renderer->uniformViewportHeiPos,(GLfloat)height);

		renderer->viewportWid=width;
		renderer->viewportHei=height;
	}
}

void YsGLSLSetBitmapFontRendererModelViewfv(struct YsGLSLBitmapFontRenderer *renderer,const GLfloat modelViewMat[16])
{
	if(NULL!=renderer)
	{
		glUniformMatrix4fv(renderer->uniformModelViewPos,1,GL_FALSE,modelViewMat);
	}
}

void YsGLSLSetBitmapFontRendererProjectionfv(struct YsGLSLBitmapFontRenderer *renderer,const GLfloat projectionMat[16])
{
	if(NULL!=renderer)
	{
		glUniformMatrix4fv(renderer->uniformProjectionPos,1,GL_FALSE,projectionMat);
	}
}

void YsGLSLRenderBitmapFontString2D(struct YsGLSLBitmapFontRenderer *renderer,int x,int y,const char str[])
{
	if(NULL!=renderer)
	{
		const int n=(int)strlen(str);
		const float coord[3]={(float)x,(float)y,0.0f};

		if(1!=renderer->windowCoord)
		{
			glUniform1i(renderer->uniformWindowCoordPos,1);
			renderer->windowCoord=1;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,renderer->fontTextureId);
		YsGLSLRenderBitmapFontRun(renderer,coord,n,str);
	}
}

void YsGLSLRenderBitmapFontString3D(struct YsGLSLBitmapFontRenderer *renderer,GLfloat x,GLfloat y,GLfloat z,const char str[])
{
	if(NULL!=renderer)
	{
		const int n=(int)strlen(str);
		const GLfloat coord[3]={(GLfloat)x,(GLfloat)y,(GLfloat)z};

		if(0!=renderer->windowCoord)
		{
			glUniform1i(renderer->uniformWindowCoordPos,0);
			renderer->windowCoord=0;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,renderer->fontTextureId);
		YsGLSLRenderBitmapFontRun(renderer,coord,n,str);
	}
}

////////////////////////////////////////////////////////////

static struct YsGLSLBitmapFontRenderer *sharedBitmapFontRenderer=NULL;

void YsGLSLCreateSharedBitmapFontRenderer(void)
{
	if(NULL==sharedBitmapFontRenderer)
	{
		sharedBitmapFontRenderer=YsGLSLCreateBitmapFontRenderer();
	}
}

void YsGLSLSharedBitmapFontRendererLost(void)
{
	sharedBitmapFontRenderer=NULL;
}

void YsGLSLDeleteSharedBitmapFontRenderer(void)
{
	if(NULL!=sharedBitmapFontRenderer)
	{
		YsGLSLDeleteBitmapFontRenderer(sharedBitmapFontRenderer);
		sharedBitmapFontRenderer=NULL;
	}
}

struct YsGLSLBitmapFontRenderer *YsGLSLSharedBitmapFontRenderer(void)
{
	return sharedBitmapFontRenderer;
}

