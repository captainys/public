/* ////////////////////////////////////////////////////////////

File Name: ysglslutil.c
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
#include "ysglslutil.h"

#ifdef ANDROID
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_INFO,"Ys",__VA_ARGS__);
#endif

// GLSL has another very stupid specification saying #version must be at the very first line
// of the program.  Not even after 
//    #ifdef GL_ES
// what's worse is the GLSL for OpenGL ES version is not compatible with full-fledge OpenGL's
// GLSL version.  Therefore, the first line needs to be added from the C++ program.
// Macro YS_GL_ES2 is defined in ysglheader.h if it identified the OpenGL ES 2.0

#ifdef YS_GL_ES2
	static const char *YSGLSL_header[]=
	{
		"\n",  // Should I say #version 100?  Looks like GLSL for ES and for full-feature OpenGL are not in sync.  What a mess!
	};
#else
	static const char *YSGLSL_header[]=
	{
		// I don't understand why this explicit #version is necessary.
		// Isn't the newer version backward compatible?
		"#version 120\n",
	};
#endif
static const int nYSGLSL_header=(int)(sizeof(YSGLSL_header)/sizeof(const char *));

static char *MakeLinearString(const int nHeader,const char * const header[],const int nLine,const char * const line[])
{
	int l;
	size_t ptr=0,totalSize=0;
	char *buf;

	for(l=0; l<nHeader; ++l)
	{
		totalSize+=strlen(header[l]);
	}
	for(l=0; l<nLine; ++l)
	{
		totalSize+=strlen(line[l]);
	}

	buf=(char *)malloc(totalSize+1);
	for(l=0; l<nHeader; ++l)
	{
		strcpy(buf+ptr,header[l]);
		ptr+=strlen(header[l]);
	}
	for(l=0; l<nLine; ++l)
	{
		strcpy(buf+ptr,line[l]);
		ptr+=strlen(line[l]);
	}

	return buf;
}

int YsGLSLCompileAndLinkVertexAndFragmentShader(
    GLuint programId,
    GLuint vertexShaderId,
    const int nVertexShaderProgLine,const char * const vertexShaderProg[],
    GLuint fragmentShaderId,
    const int nFragShaderProgLine,const char * const fragShaderProg[])
{
	int succeed=YSOK;
	const int errMsgLen=1024;
	char errMsg[1024];
	int compileSta=99999,infoLogLength=99999,acquiredErrMsgLen=99999;
	int linkSta=99999;

	char *vertexShaderProgramLinear=MakeLinearString(nYSGLSL_header,YSGLSL_header,nVertexShaderProgLine,vertexShaderProg);
	char *fragmentShaderProgramLinear=MakeLinearString(nYSGLSL_header,YSGLSL_header,nFragShaderProgLine,fragShaderProg);
	
	/* The following two function call is supposed to be allowed by the OpenGL specification.
	   However, the OpenGL driver of VirtualBox 4.3.20 Linux Guest is bugged, and it only reads the 
	   first line.  Needs a trick.
	glShaderSource(
	    vertexShaderId,
	    nVertexShaderProgLine,(const char **)vertexShaderProg,
	    NULL); // Last NULL assumes each line is C string

	glShaderSource(
	    fragmentShaderId,
	    nFragShaderProgLine,(const char **)fragShaderProg,
	    NULL); // Last NULL assumes each line is C string
	 */

	glShaderSource(vertexShaderId,1,&vertexShaderProgramLinear,NULL);
	glShaderSource(fragmentShaderId,1,&fragmentShaderProgramLinear,NULL);



	glCompileShader(vertexShaderId);

	glGetShaderiv(vertexShaderId,GL_COMPILE_STATUS,&compileSta);
	glGetShaderiv(vertexShaderId,GL_INFO_LOG_LENGTH,&infoLogLength);
	glGetShaderInfoLog(vertexShaderId,errMsgLen-1,&acquiredErrMsgLen,errMsg);

	if(GL_TRUE!=compileSta)
	{
		int i;
		printf("Compile Status %d Info Log Length %d Error Message Length %d\n",compileSta,infoLogLength,acquiredErrMsgLen);
		printf("Error Message: \n%s\n",errMsg);
		for(i=0; i<nYSGLSL_header; ++i)
		{
			printf("%3d: %s",i+1,YSGLSL_header[i]);
		}
		for(i=0; i<nVertexShaderProgLine; ++i)
		{
			printf("%3d: %s",i+nYSGLSL_header,vertexShaderProg[i]);
		}
		printf("\n");
		succeed=YSERR;
	}


	glCompileShader(fragmentShaderId);

	glGetShaderiv(fragmentShaderId,GL_COMPILE_STATUS,&compileSta);
	glGetShaderiv(fragmentShaderId,GL_INFO_LOG_LENGTH,&infoLogLength);
	glGetShaderInfoLog(fragmentShaderId,errMsgLen-1,&acquiredErrMsgLen,errMsg);

	if(GL_TRUE!=compileSta)
	{
		int i;
		printf("Compile Status %d Info Log Length %d Error Message Length %d\n",compileSta,infoLogLength,acquiredErrMsgLen);
		printf("Error Message: \n%s\n",errMsg);
		for(i=0; i<nYSGLSL_header; ++i)
		{
			printf("%3d: %s",i+1,YSGLSL_header[i]);
		}
		for(i=0; i<nFragShaderProgLine; ++i)
		{
			printf("%3d: %s",i+nYSGLSL_header+1,fragShaderProg[i]);
		}
		printf("\n");
		succeed=YSERR;
	}


	{
		// http://stackoverflow.com/questions/10771926/glvertexattrib4f-not-working-in-os-x-but-works-fine-in-ios
		// In MacOSX, generic attribute (given by glVertexAttrib??) cannot be used for attribute location 0.
		// And, stupidly MacOSX's OpenGL may assign location 0 to an attribute that may be given as a location 0.
		// Therefore, the location needs to be given explicitly for the attributes that may be given as a generic attrib.

		// 2014/08/24
		// This explicit binding solved Flat 3D renderer problem.  However, it didn't solve MonoColorPerPix and MonoColorPerVtx 3D renderers.
		// Looks like I cannot count on generic attribute in Mac OSX.  (Just like point sprite.)


		int attribLoc=1;
		int i;
		for(i=0; i<nVertexShaderProgLine; ++i)
		{
			int j;
			int isAttrib=0;
			for(j=0; 0!=vertexShaderProg[i][j]; ++j)
			{
				if(' '!=vertexShaderProg[i][j] && '\t'!=vertexShaderProg[i][j])
				{
					if(0==strncmp(vertexShaderProg[i]+j,"attribute",9))
					{
						isAttrib=1;
					}
					break;
				}
			}
			if(0!=isAttrib)
			{
				char attribName[256]={0};
				int semiColonPos=-1;
				for(j=(int)strlen(vertexShaderProg[i])-1; 0<=j; --j)
				{
					if(';'==vertexShaderProg[i][j])
					{
						semiColonPos=j;
					}
					if(0<=semiColonPos && (' '==vertexShaderProg[i][j] || '\t'==vertexShaderProg[i][j]))
					{
						strncpy(attribName,vertexShaderProg[i]+j+1,semiColonPos-j-1);
						break;
					}
				}
				if(0!=attribName[0])
				{
					if(0==strcmp("texCoord",attribName) || 0==strcmp("color",attribName)  || 0==strcmp("colorIn",attribName))
					{
						printf("Attrib Name=%s -> %d\n",attribName,attribLoc);
						glBindAttribLocation(programId,attribLoc,attribName);
						++attribLoc;
					}
					else if(0==strcmp("vertex",attribName))
					{
						printf("Attrib Name=%s -> %d\n",attribName,0);
						glBindAttribLocation(programId,0,attribName);
					}
					else
					{
						printf("Attrib Name=%s\n",attribName);
					}
				}
			} 
		}
	}


	glAttachShader(programId,vertexShaderId);
	glAttachShader(programId,fragmentShaderId);
	glLinkProgram(programId);
	glGetProgramiv(programId,GL_LINK_STATUS,&linkSta);
	glGetProgramiv(programId,GL_INFO_LOG_LENGTH,&infoLogLength);
	glGetProgramInfoLog(programId,errMsgLen-1,&acquiredErrMsgLen,errMsg);

	if(GL_TRUE!=linkSta)
	{
		printf("Link Status %d Info Log Length %d Error Message Length %d\n",linkSta,infoLogLength,acquiredErrMsgLen);
		printf("Error Message: \n%s\n",errMsg);
		succeed=YSERR;
	}

	free(vertexShaderProgramLinear);
	free(fragmentShaderProgramLinear);

	return succeed;
}

int YsGLSLCompileAndLinkVertexAndFragmentShaderSingleString(
    GLuint programId,
    GLuint vertexShaderId,
    const char vertexShaderProg[],
    GLuint fragmentShaderId,
    const char fragShaderProg[])
{
	const char *const vtxShaderSrc[1]=
	{
		vertexShaderProg
	};
	const char *const fragShaderSrc[1]=
	{
		fragShaderProg
	};

	return YsGLSLCompileAndLinkVertexAndFragmentShader(programId,vertexShaderId,1,vtxShaderSrc,fragmentShaderId,1,fragShaderSrc);
}

void YsGLSLLoadIdentityMatrix3(GLuint uniformMatPos)
{
	const GLfloat mat[9]=
	{
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f
	};
	glUniformMatrix3fv(uniformMatPos,1,GL_FALSE,mat);
}

void YsGLSLLoadIdentityMatrix4(GLuint uniformMatPos)
{
	const GLfloat mat[16]=
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	glUniformMatrix4fv(uniformMatPos,1,GL_FALSE,mat);
}

int YsGLSLGetUniformLocation(int *errorCountPtr,const char callingFunc[],int programIdent,const char uniformName[])
{
	int pos=glGetUniformLocation(programIdent,uniformName);
	if(0>pos)
	{
		fprintf(stderr,"Called from %s\n",callingFunc);
		fprintf(stderr,"Cannot find uniform location: %s\n",uniformName);
		if(NULL!=errorCountPtr)
		{
			++(*errorCountPtr);
		}
		else
		{
			fprintf(stderr,"Could be normal since error check is ignoring this uniform.\n");
		}
	}
	return pos;
}

int YsGLSLGetAttribLocation(int *errorCountPtr,const char callingFunc[],int programIdent,const char attribName[])
{
	int pos=glGetAttribLocation(programIdent,attribName);
	if(0>pos)
	{
		fprintf(stderr,"Called from %s\n",callingFunc);
		fprintf(stderr,"Cannot find attrib location: %s\n",attribName);
		if(NULL!=errorCountPtr)
		{
			++(*errorCountPtr);
		}
		else
		{
			fprintf(stderr,"Could be normal since error check is ignoring this attribute.\n");
		}
	}
	return pos;
}

