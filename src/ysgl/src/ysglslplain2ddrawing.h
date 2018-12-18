/* ////////////////////////////////////////////////////////////

File Name: ysglslplain2ddrawing.h
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

#ifndef YSGLPLAINDRAW_IS_INCLUDED
#define YSGLPLAINDRAW_IS_INCLUDED
/* { */

/*! \file */

#include "ysglheader.h"

/* Force Visual C++ to type-mismatching error. */
#pragma warning( error : 4028)
#pragma warning( error : 4047)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

	
/*! This function creates a plain-drawing object and returns the pointer. 
    This function does not make the plain-drawing shader program current.

	Plain-drawing object can draw a primitive with a color assigned at each vertex.
	No lighting.

    To use the plain-drawing shader program, call YsGLSLUsePlain2DRenderer to enable it. */
struct YsGLSLPlain2DRenderer *YsGLSLCreatePlain2DRenderer(void);

/*! This function deletes a plain-drawing object created by YsGLSLCreatePlain2DRenderer function. */
void YsGLSLDeletePlain2DRenderer(struct YsGLSLPlain2DRenderer *renderer);

/*! This function starts using shader program of the renderer and returns the previous program ID. 
    Once a plain 2D renderer is activated by this function, the renderer must be de-activated by
    YsGLSLEndUsePlain2DRenderer function.

    In some implementation, attribute array state (enabled/disabled) is not per-program, and leaving an
    attribute array enabled and switching to a different program may cause a segmentation fault.
*/
GLuint YsGLSLUsePlain2DRenderer(struct YsGLSLPlain2DRenderer *renderer);

/*! This function de-activates a plain 2D renderer that was activated by YsGLSLUsePlain2DRenderer. */
void YsGLSLEndUsePlain2DRenderer(struct YsGLSLPlain2DRenderer *renderer);

/*! This function sets up a transformation matrix of the renderer so that the given coordinates are
    taken as window coordinate.  If topLeftAsOrigin is non-zero, top-left corner will be (0,0), or 
    bottom-left corner otherwise.  Window coordinate is taken based on the viewport.
    YsGLSLUsePlain2DRenderer must be called, and renderer's program must be current. */
void YsGLSLUseWindowCoordinateInPlain2DDrawing(struct YsGLSLPlain2DRenderer *renderer,int topLeftAsOrigin);

/*! This function cancels the effect of YsGLSLUseWindowCoordinateInPlain2DDrawing function.
    YsGLSLUsePlain2DRenderer must be called, and renderer's program must be current. */
void YsGLSLDontUseWindowCoordinateInPlain2DDrawing(struct YsGLSLPlain2DRenderer *renderer);

/*! This function sets uniform color, which may be used as a generic color attribute. */
void YsGLSLSetPlain2DRendererUniformColor(struct YsGLSLPlain2DRenderer *renderer,const GLfloat color[4]);

/*! This function sets uniform point size. */
void YsGLSLSetPlain2DRendererUniformPointSize(struct YsGLSLPlain2DRenderer *renderer,GLfloat pointSize);

/*! This function returns the current transformation matrix of the plain 2D renderer. 
    The 2D renderer must be current. */
void YsGLSLGetPlain2DRendererTransformationfv(GLfloat tfm[16],struct YsGLSLPlain2DRenderer *renderer);

/*! This function sets the current transformation matrix of the plain 2D renderer. 
    The 2D renderer must be current. */
void YsGLSLSetPlain2DRendererTransformationfv(struct YsGLSLPlain2DRenderer *renderer,const GLfloat tfm[16]);

/*! This function draws a single mono-color line segment using YsGLSLPlain2DRenderer.
    YsGLSLUsePlain2DRenderer must be called, and renderer's program must be current. */
void YsGLSLDrawPlain2DLinef(
    const struct YsGLSLPlain2DRenderer *renderer,
    const float x0,const float y0,
    const float x1,const float y1,
    const float r,const float g,const float b,const float a);

/*! This function draws a single mono-color line segment using YsGLSLPlain2DRenderer.
    YsGLSLUsePlain2DRenderer must be called, and renderer's program must be current. */
void YsGLSLDrawPlain2DLined(
    const struct YsGLSLPlain2DRenderer *renderer,
    const double x0,const double y0,
    const double x1,const double y1,
    const double r,const double g,const double b,const double a);

/*! This function draws a sequence of primitives using YsGLSLPlain2DRenderer.
    The lengths of vertex and color must be 2*nVertex and 4*nVertex respectively. 
    YsGLSLUsePlain2DRenderer must be called, and renderer's program must be current. */
void YsGLSLDrawPlain2DPrimitiveVtxColfv(
    const struct YsGLSLPlain2DRenderer *renderer,
	GLenum mode,
	const size_t nVertex,const GLfloat vertex[],const GLfloat color[]);

/*! This function draws a sequence of primitives using YsGLSLPlain2DRenderer.
    The lengths of vertex must be 2*nVertex.
    Color is taken from the value set by YsGLSLSetPlain2DRendererUniformColor.
    YsGLSLUsePlain2DRenderer must be called, and renderer's program must be current. */
void YsGLSLDrawPlain2DPrimitiveVtxfv(
    const struct YsGLSLPlain2DRenderer *renderer,
	GLenum mode,
	const size_t nVertex,const GLfloat vertex[]);

/*! This function draws a sequence of primitives using YsGLSLPlain2DRenderer.
    The lengths of vertex must be 2*nVertex.
    Color is taken from the value set by YsGLSLSetPlain2DRendererUniformColor.
    YsGLSLUsePlain2DRenderer must be called, and renderer's program must be current. */
void YsGLSLDrawPlain2DPrimitiveVtxTexCoordfv(
	const struct YsGLSLPlain2DRenderer *renderer,
	GLenum mode,
	const size_t nVertex,const GLfloat vertex[],const GLfloat texCoord[]);


/*! Same as YsGLSLDrawPlain2DPrimitiveVtxColfv.
    This function is left just for backward compatibility.
*/
void YsGLSLDrawPlain2DPrimitivefv(
    const struct YsGLSLPlain2DRenderer *renderer,
	GLenum mode,
	const size_t nVertex,const GLfloat vertex[],const GLfloat color[]);

#ifdef __cplusplus
}
#endif

/* } */
#endif
