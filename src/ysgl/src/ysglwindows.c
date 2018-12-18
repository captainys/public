/* ////////////////////////////////////////////////////////////

File Name: ysglwindows.c
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

#include "ysglheader.h"


/*
void glFreefood(GLsizei n,GLuint *framebuffers)
{
	if(NULL==glFreefoodPtr)
	{
		glFreefoodPtr=(void (WINAPI *)(GLenum,GLuint *))wglGetProcAddress("glFreefood");
	}
	if(NULL!=glFreefoodPtr)
	{
		(*glFreefoodPtr)(n,framebuffers);
	}
}
*/


#ifdef _WIN32
#include "ysglwindows.h"

void (WINAPI *glActiveTexturePtr)(GLenum texture)=NULL;

GLAPI void APIENTRY glActiveTexture(GLenum texture)
{
	if(NULL==glActiveTexturePtr)
	{
		glActiveTexturePtr=(void (WINAPI *)(GLenum))wglGetProcAddress("glActiveTexture");
	}
	if(NULL!=glActiveTexturePtr)
	{
		(*glActiveTexturePtr)(texture);
	}
}


void (WINAPI *glPointParameterfPtr)(GLenum,GLfloat)=NULL;
void (WINAPI *glPointParameteriPtr)(GLenum,GLint)=NULL;
void (WINAPI *glPointParameterfvPtr)(GLenum,const GLfloat *)=NULL;
void (WINAPI *glPointParameterivPtr)(GLenum,const GLint *)=NULL;

GLAPI void APIENTRY glPointParameterf(GLenum pname,GLfloat param)
{
	if(NULL==glPointParameterfPtr)
	{
		glPointParameterfPtr=(void (WINAPI *)(GLenum,GLfloat))wglGetProcAddress("glPointParameterf");
	}
	if(NULL!=glPointParameterfPtr)
	{
		(*glPointParameterfPtr)(pname,param);
	}
}

GLAPI void APIENTRY glPointParameteri(GLenum pname,GLint param)
{
	if(NULL==glPointParameteriPtr)
	{
		glPointParameteriPtr=(void (WINAPI *)(GLenum,GLint))wglGetProcAddress("glPointParameteri");
	}
	if(NULL!=glPointParameteriPtr)
	{
		(*glPointParameteriPtr)(pname,param);
	}
}

GLAPI void APIENTRY glPointParameterfv(GLenum pname,const GLfloat *params)
{
	if(NULL==glPointParameterfvPtr)
	{
		glPointParameterfvPtr=(void (WINAPI *)(GLenum,const GLfloat *))wglGetProcAddress("glPointParameterfv");
	}
	if(NULL!=glPointParameterfvPtr)
	{
		(*glPointParameterfvPtr)(pname,params);
	}
}

GLAPI void APIENTRY glPointParameteriv(GLenum pname,const GLint *params)
{
	if(NULL==glPointParameterivPtr)
	{
		glPointParameterivPtr=(void (WINAPI *)(GLenum,const GLint *))wglGetProcAddress("glPointParameteriv");
	}
	if(NULL!=glPointParameterivPtr)
	{
		(*glPointParameterivPtr)(pname,params);
	}
}

GLboolean (WINAPI *glIsBufferPtr)(GLuint buffer)=NULL;
void (WINAPI *glGenBuffersPtr)(GLsizei n,GLuint *buffers)=NULL;
void (WINAPI *glDeleteBuffersPtr)(GLsizei n,const GLuint *buffers)=NULL;
void (WINAPI *glBindBufferPtr)(GLenum target,GLuint buffer)=NULL;
void (WINAPI *glBufferDataPtr)(GLenum target,GLsizeiptr size,const GLvoid *data,GLenum usage)=NULL;
void (WINAPI *glBufferSubDataPtr)(GLenum target,GLintptr offset,GLsizeiptr size,const GLvoid *data)=NULL;
// void (WINAPI *glDrawArraysPtr)(GLenum mode,GLint first,GLsizei count)=NULL;

GLAPI GLboolean APIENTRY glIsBuffer(GLuint buffer)
{
	if(NULL==glIsBufferPtr)
	{
		glIsBufferPtr=(GLboolean (WINAPI *)(GLuint))wglGetProcAddress("glIsBuffer");
	}
	if(NULL!=glIsBufferPtr)
	{
		return (*glIsBufferPtr)(buffer);
	}
	return GL_FALSE;
}

GLAPI void APIENTRY glGenBuffers(GLsizei n,GLuint *buffers)
{
	if(NULL==glGenBuffersPtr)
	{
		glGenBuffersPtr=(void (WINAPI *)(GLsizei,GLuint *))wglGetProcAddress("glGenBuffers");
	}
	if(NULL!=glGenBuffersPtr)
	{
		(*glGenBuffersPtr)(n,buffers);
	}
}

GLAPI void APIENTRY glDeleteBuffers(GLsizei n,const GLuint *buffers)
{
	if(NULL==glDeleteBuffersPtr)
	{
		glDeleteBuffersPtr=(void (WINAPI *)(GLsizei,const GLuint *))wglGetProcAddress("glDeleteBuffers");
	}
	if(NULL!=glDeleteBuffersPtr)
	{
		(*glDeleteBuffersPtr)(n,buffers);
	}
}

GLAPI void APIENTRY glBindBuffer(GLenum target,GLuint buffer)
{
	if(NULL==glBindBufferPtr)
	{
		glBindBufferPtr=(void (WINAPI *)(GLenum,GLuint))wglGetProcAddress("glBindBuffer");
	}
	if(NULL!=glBindBufferPtr)
	{
		(*glBindBufferPtr)(target,buffer);
	}
}

GLAPI void APIENTRY glBufferData(GLenum target,GLsizeiptr size,const GLvoid *data,GLenum usage)
{
	if(NULL==glBufferDataPtr)
	{
		glBufferDataPtr=(void (WINAPI *)(GLenum,GLsizeiptr,const GLvoid *,GLuint))wglGetProcAddress("glBufferData");
	}
	if(NULL!=glBufferDataPtr)
	{
		(*glBufferDataPtr)(target,size,data,usage);
	}
}

GLAPI void APIENTRY glBufferSubData(GLenum target,GLintptr offset,GLsizeiptr size,const GLvoid *data)
{
	if(NULL==glBufferSubDataPtr)
	{
		glBufferSubDataPtr=(void (WINAPI *)(GLenum,GLintptr,GLsizeiptr,const GLvoid *))wglGetProcAddress("glBufferSubData");
	}
	if(NULL!=glBufferSubDataPtr)
	{
		(*glBufferSubDataPtr)(target,offset,size,data);
	}
}

/* GLAPI void APIENTRY glDrawArrays(GLenum mode,GLint first,GLsizei count)
{
	if(NULL==glDrawArraysPtr)
	{
		glDrawArraysPtr=(void (WINAPI *)(GLenum,GLint,GLsizei))wglGetProcAddress("glDrawArrays");
	}
	if(NULL!=glDrawArraysPtr)
	{
		(*glDrawArraysPtr)(mode,first,count);
	}
} */

void (WINAPI *glGenFramebuffersPtr)(GLsizei n,GLuint *buffers)=NULL;
void (WINAPI *glDeleteFramebuffersPtr)(GLsizei n,const GLuint *buffers)=NULL;
void (WINAPI *glBindFramebufferPtr)(GLenum target,GLuint framebuffer)=NULL; 
void (WINAPI *glFramebufferTexture2DPtr)(GLenum target,GLenum attachment,GLenum textarget,GLuint texture,GLint level)=NULL;
GLenum (WINAPI *glCheckFramebufferStatusPtr)(GLenum target)=NULL;

GLAPI void APIENTRY glGenFramebuffers(GLsizei n,GLuint *framebuffers)
{
	if(NULL==glGenFramebuffersPtr)
	{
		glGenFramebuffersPtr=(void (WINAPI *)(GLsizei,GLuint *))wglGetProcAddress("glGenFramebuffers");
	}
	if(NULL!=glGenFramebuffersPtr)
	{
		(*glGenFramebuffersPtr)(n,framebuffers);
	}
}

GLAPI void APIENTRY glDeleteFramebuffers(GLsizei n,const GLuint *framebuffers)
{
	if(NULL==glDeleteFramebuffersPtr)
	{
		glDeleteFramebuffersPtr=(void (WINAPI *)(GLsizei,const GLuint *))wglGetProcAddress("glDeleteFramebuffers");
	}
	if(NULL!=glDeleteFramebuffersPtr)
	{
		(*glDeleteFramebuffersPtr)(n,framebuffers);
	}
}

GLAPI void APIENTRY glBindFramebuffer(GLenum target,GLuint framebuffer)
{
	if(NULL==glBindFramebufferPtr)
	{
		glBindFramebufferPtr=(void (WINAPI *)(GLenum,GLuint))wglGetProcAddress("glBindFramebuffer");
	}
	if(NULL!=glBindFramebufferPtr)
	{
		(*glBindFramebufferPtr)(target,framebuffer);
	}
}

GLAPI void APIENTRY glFramebufferTexture2D(GLenum target,GLenum attachment,GLenum textarget,GLuint texture,GLint level)
{
	if(NULL==glFramebufferTexture2DPtr)
	{
		glFramebufferTexture2DPtr=(void (WINAPI *)(GLenum,GLenum,GLenum,GLuint,GLint))wglGetProcAddress("glFramebufferTexture2D");
	}
	if(NULL!=glFramebufferTexture2DPtr)
	{
		(*glFramebufferTexture2DPtr)(target,attachment,textarget,texture,level);
	}
}

GLAPI GLenum APIENTRY glCheckFramebufferStatus(GLenum target)
{
	if(NULL==glCheckFramebufferStatusPtr)
	{
		glCheckFramebufferStatusPtr=(GLenum (WINAPI *)(GLenum))wglGetProcAddress("glCheckFramebufferStatus");
	}
	if(NULL!=glCheckFramebufferStatusPtr)
	{
		return (*glCheckFramebufferStatusPtr)(target);
	}
	return 0;
}



GLuint (WINAPI *glCreateShaderPtr)(GLenum shaderType)=NULL;
void (WINAPI *glDeleteShaderPtr)(GLuint  shader)=NULL;
void (WINAPI *glCompileShaderPtr)(GLuint shader)=NULL;
void (WINAPI *glShaderSourcePtr)(GLuint shader,  GLsizei count,  const GLchar * const *string,  const GLint *length)=NULL;
void (WINAPI *glGetShaderivPtr)(GLuint shader,  GLenum pname,  GLint *params)=NULL;
void (WINAPI *glGetShaderInfoLogPtr)(GLuint shader,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog)=NULL;
GLuint (WINAPI *glCreateProgramPtr)(void)=NULL;
void (WINAPI *glDeleteProgramPtr)(GLuint)=NULL;
void (WINAPI *glAttachShaderPtr)(GLuint,GLuint)=NULL;
void (WINAPI *glLinkProgramPtr)(GLuint)=NULL;
void (WINAPI *glGetProgramivPtr)(GLuint program,  GLenum pname,  GLint *params)=NULL; 
void (WINAPI *glGetProgramInfoLogPtr)(GLuint,GLsizei,GLsizei *,GLchar *)=NULL;
void (WINAPI *glUseProgramPtr)(GLuint)=NULL;
void (WINAPI *glBindAttribLocationPtr)(GLuint,GLuint,const GLchar *)=NULL;


GLAPI GLuint APIENTRY glCreateShader(GLenum shaderType)
{
	const char *funcStr="glCreateShader";
	if(NULL==glCreateShaderPtr)
	{
		glCreateShaderPtr=(GLuint (WINAPI *)(GLenum))wglGetProcAddress(funcStr);
	}
	if(NULL!=glCreateShaderPtr)
	{
		return (*glCreateShaderPtr)(shaderType);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
	return (GLuint)~0;
}

GLAPI void APIENTRY glDeleteShader(GLuint shader)
{
	const char *funcStr="glDeleteShader";
	if(NULL==glDeleteShaderPtr)
	{
		glDeleteShaderPtr=(void (WINAPI *)(GLuint))wglGetProcAddress(funcStr);
	}
	if(NULL!=glDeleteShaderPtr)
	{
		(*glDeleteShaderPtr)(shader);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glCompileShader(GLuint shader)
{
	const char *funcStr="glCompileShader";
	if(NULL==glCompileShaderPtr)
	{
		glCompileShaderPtr=(void (WINAPI *)(GLuint))wglGetProcAddress(funcStr);
	}
	if(NULL!=glCompileShaderPtr)
	{
		(*glCompileShaderPtr)(shader);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glShaderSource(GLuint shader,GLsizei count,const GLchar * const *string,const GLint *length)
{
	const char *funcStr="glShaderSource";
	if(NULL==glShaderSourcePtr)
	{
		glShaderSourcePtr=(void (WINAPI *)(GLuint,GLsizei,const GLchar *const *,const GLint *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glShaderSourcePtr)
	{
		(*glShaderSourcePtr)(shader,count,string,length);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glGetShaderiv(GLuint shader,GLenum pname,GLint *params)
{
	const char *funcStr="glGetShaderiv";
	if(NULL==glGetShaderivPtr)
	{
		glGetShaderivPtr=(void (WINAPI *)(GLuint,GLenum,GLint *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glGetShaderivPtr)
	{
		(*glGetShaderivPtr)(shader,pname,params);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glGetShaderInfoLog(GLuint shader,GLsizei maxLength,GLsizei *length,GLchar *infoLog)
{
	const char *funcStr="glGetShaderInfoLog";
	if(NULL==glGetShaderInfoLogPtr)
	{
		glGetShaderInfoLogPtr=(void (WINAPI *)(GLuint,GLsizei,GLsizei *,GLchar *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glGetShaderInfoLogPtr)
	{
		(*glGetShaderInfoLogPtr)(shader,maxLength,length,infoLog);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI GLuint APIENTRY glCreateProgram(void)
{
	const char *funcStr="glCreateProgram";
	if(NULL==glCreateProgramPtr)
	{
		glCreateProgramPtr=(GLuint (WINAPI *)(void))wglGetProcAddress(funcStr);
	}
	if(NULL!=glCreateProgramPtr)
	{
		return (*glCreateProgramPtr)();
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
	return 0;
}

GLAPI void APIENTRY glDeleteProgram(GLuint program)
{
	const char *funcStr="glDeleteProgram";
	if(NULL==glDeleteProgramPtr)
	{
		glDeleteProgramPtr=(void (WINAPI *)(GLuint ))wglGetProcAddress(funcStr);
	}
	if(NULL!=glDeleteProgramPtr)
	{
		(*glDeleteProgramPtr)(program);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glAttachShader(GLuint program,GLuint shader)
{
	const char *funcStr="glAttachShader";
	if(NULL==glAttachShaderPtr)
	{
		glAttachShaderPtr=(void (WINAPI *)(GLuint,GLuint))wglGetProcAddress(funcStr);
	}
	if(NULL!=glAttachShaderPtr)
	{
		(*glAttachShaderPtr)(program,shader);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glLinkProgram(GLuint program)
{
	const char *funcStr="glLinkProgram";
	if(NULL==glLinkProgramPtr)
	{
		glLinkProgramPtr=(void (WINAPI *)(GLuint ))wglGetProcAddress(funcStr);
	}
	if(NULL!=glLinkProgramPtr)
	{
		(*glLinkProgramPtr)(program);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glGetProgramiv(GLuint program,GLenum pname,GLint *params)
{
	const char *funcStr="glGetProgramiv";
	if(NULL==glGetProgramivPtr)
	{
		glGetProgramivPtr=(void (WINAPI *)(GLuint,GLenum,GLint *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glGetProgramivPtr)
	{
		(*glGetProgramivPtr)(program,pname,params);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glGetProgramInfoLog(GLuint program,GLsizei maxLength,GLsizei *length,GLchar *infoLog)
{
	const char *funcStr="glGetProgramInfoLog";
	if(NULL==glGetProgramInfoLogPtr)
	{
		glGetProgramInfoLogPtr=(void (WINAPI *)(GLuint,GLsizei,GLsizei *,GLchar *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glGetProgramInfoLogPtr)
	{
		(*glGetProgramInfoLogPtr)(program,maxLength,length,infoLog);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUseProgram(GLuint program)
{
	const char *funcStr="glUseProgram";
	if(NULL==glUseProgramPtr)
	{
		glUseProgramPtr=(void (WINAPI *)(GLuint))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUseProgramPtr)
	{
		(*glUseProgramPtr)(program);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glBindAttribLocation(GLuint program,GLuint index,const GLchar *name)
{
	const char *funcStr="glBindAttribLocation";
	if(NULL==glBindAttribLocationPtr)
	{
		glBindAttribLocationPtr=(void (WINAPI *)(GLuint,GLuint,const GLchar *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glBindAttribLocationPtr)
	{
		(*glBindAttribLocationPtr)(program,index,name);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}



GLint (WINAPI *glGetUniformLocationPtr)(GLuint,const GLchar *)=NULL;
void (WINAPI *glUniform1fPtr)(GLint location,GLfloat v0)=NULL;
void (WINAPI *glUniform2fPtr)(GLint location,GLfloat v0,GLfloat v1)=NULL;
void (WINAPI *glUniform3fPtr)(GLint location,GLfloat v0,GLfloat v1,GLfloat v2)=NULL;
void (WINAPI *glUniform4fPtr)(GLint location,GLfloat v0,GLfloat v1,GLfloat v2,GLfloat v3)=NULL;
void (WINAPI *glUniform1iPtr)(GLint location,GLint v0)=NULL;
void (WINAPI *glUniform2iPtr)(GLint location,GLint v0,GLint v1)=NULL;
void (WINAPI *glUniform3iPtr)(GLint location,GLint v0,GLint v1,GLint v2)=NULL;
void (WINAPI *glUniform4iPtr)(GLint location,GLint v0,GLint v1,GLint v2,  GLint v3)=NULL;
void (WINAPI *glUniform1fvPtr)(GLint location,  GLsizei count,  const GLfloat *value)=NULL;
void (WINAPI *glUniform2fvPtr)(GLint location,  GLsizei count,  const GLfloat *value)=NULL;
void (WINAPI *glUniform3fvPtr)(GLint location,  GLsizei count,  const GLfloat *value)=NULL;
void (WINAPI *glUniform4fvPtr)(GLint location,  GLsizei count,  const GLfloat *value)=NULL;
void (WINAPI *glUniform1ivPtr)(GLint location,  GLsizei count,  const GLint *value)=NULL;
void (WINAPI *glUniform2ivPtr)(GLint location,  GLsizei count,  const GLint *value)=NULL;
void (WINAPI *glUniform3ivPtr)(GLint location,  GLsizei count,  const GLint *value)=NULL;
void (WINAPI *glUniform4ivPtr)(GLint location,  GLsizei count,  const GLint *value)=NULL;
static void (WINAPI *glUniformMatrix2fvPtr)(GLint,GLsizei,GLboolean,const GLfloat *)=NULL;
static void (WINAPI *glUniformMatrix3fvPtr)(GLint,GLsizei,GLboolean,const GLfloat *)=NULL;
static void (WINAPI *glUniformMatrix4fvPtr)(GLint,GLsizei,GLboolean,const GLfloat *)=NULL;
static void (WINAPI *glUniformMatrix2x3fvPtr)(GLint,GLsizei,GLboolean,const GLfloat *)=NULL;
static void (WINAPI *glUniformMatrix3x2fvPtr)(GLint,GLsizei,GLboolean,const GLfloat *)=NULL;
static void (WINAPI *glUniformMatrix2x4fvPtr)(GLint,GLsizei,GLboolean,const GLfloat *)=NULL;
static void (WINAPI *glUniformMatrix4x2fvPtr)(GLint,GLsizei,GLboolean,const GLfloat *)=NULL;
static void (WINAPI *glUniformMatrix3x4fvPtr)(GLint,GLsizei,GLboolean,const GLfloat *)=NULL;
static void (WINAPI *glUniformMatrix4x3fvPtr)(GLint,GLsizei,GLboolean,const GLfloat *)=NULL;


GLAPI GLint APIENTRY glGetUniformLocation(GLuint program,const GLchar *name)
{
	const char *funcStr="glGetUniformLocation";
	if(NULL==glGetUniformLocationPtr)
	{
		glGetUniformLocationPtr=(GLint (WINAPI *)(GLuint,const GLchar *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glGetUniformLocationPtr)
	{
		return (*glGetUniformLocationPtr)(program,name);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
	return -1;
}

GLAPI void APIENTRY glUniform1f(GLint location,GLfloat v0)
{
	const char *funcStr="glUniform1f";
	if(NULL==glUniform1fPtr)
	{
		glUniform1fPtr=(void (WINAPI *)(GLint,GLfloat))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform1fPtr)
	{
		(*glUniform1fPtr)(location,v0);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform2f(GLint location,GLfloat v0,GLfloat v1)
{
	const char *funcStr="glUniform2f";
	if(NULL==glUniform2fPtr)
	{
		glUniform2fPtr=(void (WINAPI *)(GLint,GLfloat,GLfloat))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform2fPtr)
	{
		(*glUniform2fPtr)(location,v0,v1);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform3f(GLint location,GLfloat v0,GLfloat v1,GLfloat v2)
{
	const char *funcStr="glUniform3f";
	if(NULL==glUniform3fPtr)
	{
		glUniform3fPtr=(void (WINAPI *)(GLint,GLfloat,GLfloat,GLfloat))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform3fPtr)
	{
		(*glUniform3fPtr)(location,v0,v1,v2);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform4f(GLint location,GLfloat v0,GLfloat v1,GLfloat v2,GLfloat v3)
{
	const char *funcStr="glUniform4f";
	if(NULL==glUniform4fPtr)
	{
		glUniform4fPtr=(void (WINAPI *)(GLint,GLfloat,GLfloat,GLfloat,GLfloat))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform4fPtr)
	{
		(*glUniform4fPtr)(location,v0,v1,v2,v3);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform1i(GLint location,GLint v0)
{
	const char *funcStr="glUniform1i";
	if(NULL==glUniform1iPtr)
	{
		glUniform1iPtr=(void (WINAPI *)(GLint,GLint))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform1iPtr)
	{
		(*glUniform1iPtr)(location,v0);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform2i(GLint location,GLint v0,GLint v1)
{
	const char *funcStr="glUniform2i";
	if(NULL==glUniform2iPtr)
	{
		glUniform2iPtr=(void (WINAPI *)(GLint,GLint,GLint))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform2iPtr)
	{
		(*glUniform2iPtr)(location,v0,v1);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform3i(GLint location,GLint v0,GLint v1,GLint v2)
{
	const char *funcStr="glUniform3i";
	if(NULL==glUniform3iPtr)
	{
		glUniform3iPtr=(void (WINAPI *)(GLint,GLint,GLint,GLint))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform3iPtr)
	{
		(*glUniform3iPtr)(location,v0,v1,v2);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform4i(GLint location,GLint v0,GLint v1,GLint v2,  GLint v3)
{
	const char *funcStr="glUniform4i";
	if(NULL==glUniform4iPtr)
	{
		glUniform4iPtr=(void (WINAPI *)(GLint,GLint,GLint,GLint,GLint))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform4iPtr)
	{
		(*glUniform4iPtr)(location,v0,v1,v2,v3);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform1fv(GLint location,  GLsizei count,  const GLfloat *value)
{
	const char *funcStr="glUniform1fv";
	if(NULL==glUniform1fvPtr)
	{
		glUniform1fvPtr=(void (WINAPI *)(GLint,GLsizei,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform1fvPtr)
	{
		(*glUniform1fvPtr)(location,count,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform2fv(GLint location,  GLsizei count,  const GLfloat *value)
{
	const char *funcStr="glUniform2fv";
	if(NULL==glUniform2fvPtr)
	{
		glUniform2fvPtr=(void (WINAPI *)(GLint,GLsizei,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform2fvPtr)
	{
		(*glUniform2fvPtr)(location,count,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform3fv(GLint location,  GLsizei count,  const GLfloat *value)
{
	const char *funcStr="glUniform3fv";
	if(NULL==glUniform3fvPtr)
	{
		glUniform3fvPtr=(void (WINAPI *)(GLint,GLsizei,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform3fvPtr)
	{
		(*glUniform3fvPtr)(location,count,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform4fv(GLint location,  GLsizei count,  const GLfloat *value)
{
	const char *funcStr="glUniform4fv";
	if(NULL==glUniform4fvPtr)
	{
		glUniform4fvPtr=(void (WINAPI *)(GLint,GLsizei,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform4fvPtr)
	{
		(*glUniform4fvPtr)(location,count,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform1iv(GLint location,  GLsizei count,  const GLint *value)
{
	const char *funcStr="glUniform1iv";
	if(NULL==glUniform1ivPtr)
	{
		glUniform1ivPtr=(void (WINAPI *)(GLint,GLsizei,const GLint *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform1ivPtr)
	{
		(*glUniform1ivPtr)(location,count,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform2iv(GLint location,  GLsizei count,  const GLint *value)
{
	const char *funcStr="glUniform2iv";
	if(NULL==glUniform2ivPtr)
	{
		glUniform2ivPtr=(void (WINAPI *)(GLint,GLsizei,const GLint *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform2ivPtr)
	{
		(*glUniform2ivPtr)(location,count,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform3iv(GLint location,  GLsizei count,  const GLint *value)
{
	const char *funcStr="glUniform3iv";
	if(NULL==glUniform3ivPtr)
	{
		glUniform3ivPtr=(void (WINAPI *)(GLint,GLsizei,const GLint *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform3ivPtr)
	{
		(*glUniform3ivPtr)(location,count,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniform4iv(GLint location,  GLsizei count,  const GLint *value)
{
	const char *funcStr="glUniform4iv";
	if(NULL==glUniform4ivPtr)
	{
		glUniform4ivPtr=(void (WINAPI *)(GLint,GLsizei,const GLint *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniform4ivPtr)
	{
		(*glUniform4ivPtr)(location,count,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniformMatrix2fv(GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value)
{
	const char *funcStr="glUniformMatrix2fv";
	if(NULL==glUniformMatrix2fvPtr)
	{
		glUniformMatrix2fvPtr=(void (WINAPI *)(GLint,GLsizei,GLboolean,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniformMatrix2fvPtr)
	{
		(*glUniformMatrix2fvPtr)(location,count,transpose,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniformMatrix3fv(GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value)
{
	const char *funcStr="glUniformMatrix3fv";
	if(NULL==glUniformMatrix3fvPtr)
	{
		glUniformMatrix3fvPtr=(void (WINAPI *)(GLint,GLsizei,GLboolean,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniformMatrix3fvPtr)
	{
		(*glUniformMatrix3fvPtr)(location,count,transpose,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniformMatrix4fv(GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value)
{
	const char *funcStr="glUniformMatrix4fv";
	if(NULL==glUniformMatrix4fvPtr)
	{
		glUniformMatrix4fvPtr=(void (WINAPI *)(GLint,GLsizei,GLboolean,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniformMatrix4fvPtr)
	{
		(*glUniformMatrix4fvPtr)(location,count,transpose,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniformMatrix2x3fv(GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value)
{
	const char *funcStr="glUniformMatrix2x3fv";
	if(NULL==glUniformMatrix2x3fvPtr)
	{
		glUniformMatrix2x3fvPtr=(void (WINAPI *)(GLint,GLsizei,GLboolean,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniformMatrix2x3fvPtr)
	{
		(*glUniformMatrix2x3fvPtr)(location,count,transpose,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniformMatrix3x2fv(GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value)
{
	const char *funcStr="glUniformMatrix3x2fv";
	if(NULL==glUniformMatrix3x2fvPtr)
	{
		glUniformMatrix3x2fvPtr=(void (WINAPI *)(GLint,GLsizei,GLboolean,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniformMatrix3x2fvPtr)
	{
		(*glUniformMatrix3x2fvPtr)(location,count,transpose,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniformMatrix2x4fv(GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value)
{
	const char *funcStr="glUniformMatrix2x4fv";
	if(NULL==glUniformMatrix2x4fvPtr)
	{
		glUniformMatrix2x4fvPtr=(void (WINAPI *)(GLint,GLsizei,GLboolean,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniformMatrix2x4fvPtr)
	{
		(*glUniformMatrix2x4fvPtr)(location,count,transpose,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniformMatrix4x2fv(GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value)
{
	const char *funcStr="glUniformMatrix4x2fv";
	if(NULL==glUniformMatrix4x2fvPtr)
	{
		glUniformMatrix4x2fvPtr=(void (WINAPI *)(GLint,GLsizei,GLboolean,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniformMatrix4x2fvPtr)
	{
		(*glUniformMatrix4x2fvPtr)(location,count,transpose,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniformMatrix3x4fv(GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value)
{
	const char *funcStr="glUniformMatrix3x4fv";
	if(NULL==glUniformMatrix3x4fvPtr)
	{
		glUniformMatrix3x4fvPtr=(void (WINAPI *)(GLint,GLsizei,GLboolean,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniformMatrix3x4fvPtr)
	{
		(*glUniformMatrix3x4fvPtr)(location,count,transpose,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glUniformMatrix4x3fv(GLint  location,  GLsizei  count,  GLboolean  transpose,  const GLfloat * value)
{
	const char *funcStr="glUniformMatrix4x3fv";
	if(NULL==glUniformMatrix4x3fvPtr)
	{
		glUniformMatrix4x3fvPtr=(void (WINAPI *)(GLint,GLsizei,GLboolean,const GLfloat *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glUniformMatrix4x3fvPtr)
	{
		(*glUniformMatrix4x3fvPtr)(location,count,transpose,value);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}



static GLint (WINAPI *glGetAttribLocationPtr)(GLuint ,const GLchar *)=NULL;
static void (WINAPI *glEnableVertexAttribArrayPtr)(GLuint index)=NULL;
static void (WINAPI *glDisableVertexAttribArrayPtr)(GLuint index)=NULL;
static void (WINAPI *glVertexAttrib1fPtr)(GLuint,GLfloat)=NULL;
static void (WINAPI *glVertexAttrib1sPtr)(GLuint,GLshort)=NULL;
static void (WINAPI *glVertexAttrib1dPtr)(GLuint,GLdouble)=NULL;
static void (WINAPI *glVertexAttrib2fPtr)(GLuint,GLfloat,GLfloat)=NULL;
static void (WINAPI *glVertexAttrib2sPtr)(GLuint,GLshort,GLshort)=NULL;
static void (WINAPI *glVertexAttrib2dPtr)(GLuint,GLdouble,GLdouble)=NULL;
static void (WINAPI *glVertexAttrib3fPtr)(GLuint,GLfloat,GLfloat,GLfloat)=NULL;
static void (WINAPI *glVertexAttrib3sPtr)(GLuint,GLshort,GLshort,GLshort)=NULL;
static void (WINAPI *glVertexAttrib3dPtr)(GLuint,GLdouble,GLdouble,GLdouble)=NULL;
static void (WINAPI *glVertexAttrib4fPtr)(GLuint,GLfloat,GLfloat,GLfloat,GLfloat)=NULL;
static void (WINAPI *glVertexAttrib4sPtr)(GLuint,GLshort,GLshort,GLshort,GLshort)=NULL;
static void (WINAPI *glVertexAttrib4dPtr)(GLuint,GLdouble,GLdouble,GLdouble,GLdouble)=NULL;
static void (WINAPI *glVertexAttrib4NubPtr)(GLuint,GLubyte,GLubyte,GLubyte,GLubyte)=NULL;
static void (WINAPI *glVertexAttribPointerPtr)(GLuint,GLint size,GLenum type,GLboolean normalized,GLsizei stride, const GLvoid * pointer)=NULL;

static void (WINAPI *glVertexAttrib2fvPtr)(GLuint,const GLfloat [2])=NULL;
static void (WINAPI *glVertexAttrib3fvPtr)(GLuint,const GLfloat [3])=NULL;
static void (WINAPI *glVertexAttrib4fvPtr)(GLuint,const GLfloat [4])=NULL;

GLAPI GLint APIENTRY glGetAttribLocation(GLuint program,const GLchar *name)
{
	const char *funcStr="glGetAttribLocation";
	if(NULL==glGetAttribLocationPtr)
	{
		glGetAttribLocationPtr=(GLint (WINAPI *)(GLuint,const GLchar *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glGetAttribLocationPtr)
	{
		return (*glGetAttribLocationPtr)(program,name);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
	return -1;
}

GLAPI void APIENTRY glEnableVertexAttribArray(GLuint index)
{
	const char *funcStr="glEnableVertexAttribArray";
	if(NULL==glEnableVertexAttribArrayPtr)
	{
		glEnableVertexAttribArrayPtr=(void (WINAPI *)(GLuint))wglGetProcAddress(funcStr);
	}
	if(NULL!=glEnableVertexAttribArrayPtr)
	{
		(*glEnableVertexAttribArrayPtr)(index);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glDisableVertexAttribArray(GLuint index)
{
	const char *funcStr="glDisableVertexAttribArray";
	if(NULL==glDisableVertexAttribArrayPtr)
	{
		glDisableVertexAttribArrayPtr=(void (WINAPI *)(GLuint))wglGetProcAddress(funcStr);
	}
	if(NULL!=glDisableVertexAttribArrayPtr)
	{
		(*glDisableVertexAttribArrayPtr)(index);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib1f(GLuint index, GLfloat v0)
{
	const char *funcStr="glVertexAttrib1f";
	if(NULL==glVertexAttrib1fPtr)
	{
		glVertexAttrib1fPtr=(void (WINAPI *)(GLuint,GLfloat))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib1fPtr)
	{
		(*glVertexAttrib1fPtr)(index,v0);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib1s(GLuint index, GLshort v0)
{
	const char *funcStr="glVertexAttrib1s";
	if(NULL==glVertexAttrib1sPtr)
	{
		glVertexAttrib1sPtr=(void (WINAPI *)(GLuint,GLshort))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib1sPtr)
	{
		(*glVertexAttrib1sPtr)(index,v0);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib1d(GLuint index, GLdouble v0)
{
	const char *funcStr="glVertexAttrib1d";
	if(NULL==glVertexAttrib1dPtr)
	{
		glVertexAttrib1dPtr=(void (WINAPI *)(GLuint,GLdouble))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib1dPtr)
	{
		(*glVertexAttrib1dPtr)(index,v0);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib2f(GLuint index, GLfloat v0, GLfloat v1)
{
	const char *funcStr="glVertexAttrib2f";
	if(NULL==glVertexAttrib2fPtr)
	{
		glVertexAttrib2fPtr=(void (WINAPI *)(GLuint,GLfloat,GLfloat))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib2fPtr)
	{
		(*glVertexAttrib2fPtr)(index,v0,v1);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib2s(GLuint index, GLshort v0, GLshort v1)
{
	const char *funcStr="glVertexAttrib2s";
	if(NULL==glVertexAttrib2sPtr)
	{
		glVertexAttrib2sPtr=(void (WINAPI *)(GLuint,GLshort,GLshort))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib2sPtr)
	{
		(*glVertexAttrib2sPtr)(index,v0,v1);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib2d(GLuint index, GLdouble v0, GLdouble v1)
{
	const char *funcStr="glVertexAttrib2d";
	if(NULL==glVertexAttrib2dPtr)
	{
		glVertexAttrib2dPtr=(void (WINAPI *)(GLuint,GLdouble,GLdouble))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib2dPtr)
	{
		(*glVertexAttrib2dPtr)(index,v0,v1);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2)
{
	const char *funcStr="glVertexAttrib3f";
	if(NULL==glVertexAttrib3fPtr)
	{
		glVertexAttrib3fPtr=(void (WINAPI *)(GLuint,GLfloat,GLfloat,GLfloat))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib3fPtr)
	{
		(*glVertexAttrib3fPtr)(index,v0,v1,v2);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib3s(GLuint index, GLshort v0, GLshort v1, GLshort v2)
{
	const char *funcStr="glVertexAttrib3s";
	if(NULL==glVertexAttrib3sPtr)
	{
		glVertexAttrib3sPtr=(void (WINAPI *)(GLuint,GLshort,GLshort,GLshort))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib3sPtr)
	{
		(*glVertexAttrib3sPtr)(index,v0,v1,v2);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib3d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2)
{
	const char *funcStr="glVertexAttrib3d";
	if(NULL==glVertexAttrib3dPtr)
	{
		glVertexAttrib3dPtr=(void (WINAPI *)(GLuint,GLdouble,GLdouble,GLdouble))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib3dPtr)
	{
		(*glVertexAttrib3dPtr)(index,v0,v1,v2);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	const char *funcStr="glVertexAttrib4f";
	if(NULL==glVertexAttrib4fPtr)
	{
		glVertexAttrib4fPtr=(void (WINAPI *)(GLuint,GLfloat,GLfloat,GLfloat,GLfloat))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib4fPtr)
	{
		(*glVertexAttrib4fPtr)(index,v0,v1,v2,v3);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib4s(GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3)
{
	const char *funcStr="glVertexAttrib4s";
	if(NULL==glVertexAttrib4sPtr)
	{
		glVertexAttrib4sPtr=(void (WINAPI *)(GLuint,GLshort,GLshort,GLshort,GLshort))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib4sPtr)
	{
		(*glVertexAttrib4sPtr)(index,v0,v1,v2,v3);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib4d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3)
{
	const char *funcStr="glVertexAttrib4d";
	if(NULL==glVertexAttrib4dPtr)
	{
		glVertexAttrib4dPtr=(void (WINAPI *)(GLuint,GLdouble,GLdouble,GLdouble,GLdouble))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib4dPtr)
	{
		(*glVertexAttrib4dPtr)(index,v0,v1,v2,v3);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib4Nub(GLuint index, GLubyte v0, GLubyte v1, GLubyte v2, GLubyte v3)
{
	const char *funcStr="glVertexAttrib4Nub";
	if(NULL==glVertexAttrib4NubPtr)
	{
		glVertexAttrib4NubPtr=(void (WINAPI *)(GLuint,GLubyte,GLubyte,GLubyte,GLubyte))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib4NubPtr)
	{
		(*glVertexAttrib4NubPtr)(index,v0,v1,v2,v3);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib2fv(GLuint index,const GLfloat v[2])
{
	const char *funcStr="glVertexAttrib2fv";
	if(NULL==glVertexAttrib2fvPtr)
	{
		glVertexAttrib2fvPtr=(void (WINAPI *)(GLuint,const GLfloat [2]))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib2fvPtr)
	{
		(*glVertexAttrib2fvPtr)(index,v);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib3fv(GLuint index,const GLfloat v[3])
{
	const char *funcStr="glVertexAttrib3fv";
	if(NULL==glVertexAttrib3fvPtr)
	{
		glVertexAttrib3fvPtr=(void (WINAPI *)(GLuint,const GLfloat [3]))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib3fvPtr)
	{
		(*glVertexAttrib3fvPtr)(index,v);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}

GLAPI void APIENTRY glVertexAttrib4fv(GLuint index,const GLfloat v[4])
{
	const char *funcStr="glVertexAttrib4fv";
	if(NULL==glVertexAttrib4fvPtr)
	{
		glVertexAttrib4fvPtr=(void (WINAPI *)(GLuint,const GLfloat [4]))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttrib4fvPtr)
	{
		(*glVertexAttrib4fvPtr)(index,v);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}



GLAPI void APIENTRY glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
	const char *funcStr="glVertexAttribPointer";
	if(NULL==glVertexAttribPointerPtr)
	{
		glVertexAttribPointerPtr=(void (WINAPI *)(GLuint,GLint,GLenum,GLboolean,GLsizei,const GLvoid *))wglGetProcAddress(funcStr);
	}
	if(NULL!=glVertexAttribPointerPtr)
	{
		(*glVertexAttribPointerPtr)(index,size,type,normalized,stride,pointer);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
}




/*
Retrieval template
{
	const char *funcStr="####";
	if(NULL==####Ptr)
	{
		####Ptr=(tttt (WINAPI *)(xxxx))wglGetProcAddress(funcStr);
	}
	if(NULL!=####Ptr)
	{
		return (*####Ptr)(yyyy);
	}
	else
	{
		printf("Function %s unavailable.\n",funcStr);
	}
	return ????;
}
*/


#endif

// OpenGL 1.2 or greater <<
