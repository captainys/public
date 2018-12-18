/* ////////////////////////////////////////////////////////////

File Name: ysglslbitmaprenderer.h
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

#ifndef YSOPENGL_BITMAPRENDERER_IS_INCLUDED
#define YSOPENGL_BITMAPRENDERER_IS_INCLUDED
/* { */

/*! \file */

#include "ysgldef.h"

/* Force Visual C++ to type-mismatching error. */
#pragma warning( error : 4028)
#pragma warning( error : 4047)

#ifdef __cplusplus
extern "C" {
#endif

struct YsGLSLBitmapRenderer;

/*! This function creates a bitmap renderer. */
struct YsGLSLBitmapRenderer *YsGLSLCreateBitmapRenderer(void);

/*! This function removes a bitmap renderer. */
void YsGLSLDeleteBitmapRenderer(struct YsGLSLBitmapRenderer *renderer);

/*! This function starts using a bitmap renderer, and returns program ID of 
    the previously-selected shader. */ 
GLuint YsGLSLUseBitmapRenderer(struct YsGLSLBitmapRenderer *renderer);

/*! This function ends using the bitmap renderer that was activated by 
    YsGLSLUseBitmapRenderer function. */
void YsGLSLEndUseBitmapRenderer(struct YsGLSLBitmapRenderer *renderer);

/*! This function draws a RGBA bitmap with a bitmap renderer.  The bitmap 
    renderer must be activated by YsGLSLUseBitmapRenderer function before 
    this function. */
void YsGLSLRenderRGBABitmap2D(
    struct YsGLSLBitmapRenderer *renderer,
    int x,int y,
    enum YSGLSL_HALIGN hAlign,enum YSGLSL_VALIGN vAlign,
    unsigned int wid,unsigned int hei,const unsigned char rgba[]);

/*!  */
void YsGLSLRenderTexture2D(
    struct YsGLSLBitmapRenderer *renderer,
    int x,int y,
    enum YSGLSL_HALIGN hAlign,enum YSGLSL_VALIGN vAlign,
    unsigned int wid,unsigned int hei,GLuint textureId);

/*!  */
void YsGLSLRenderBoundTexture2D(
    struct YsGLSLBitmapRenderer *renderer,
    int x,int y,
    enum YSGLSL_HALIGN hAlign,enum YSGLSL_VALIGN vAlign,
    unsigned int wid,unsigned int hei,GLuint samplerIdent);

/*!  */
void YsGLSLRenderTexture2DInverted(
    struct YsGLSLBitmapRenderer *renderer,
    int x,int y,
    enum YSGLSL_HALIGN hAlign,enum YSGLSL_VALIGN vAlign,
    unsigned int wid,unsigned int hei,GLuint textureId);

#ifdef __cplusplus
}
#endif

/* } */
#endif
