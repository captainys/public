/* ////////////////////////////////////////////////////////////

File Name: ysglsldrawfontbitmap.h
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

#ifndef YSGLSLDRAWFONTBITMAP_IS_INCLUDED
#define YSGLSLDRAWFONTBITMAP_IS_INCLUDED
/* { */

#ifdef __cplusplus
extern "C" {
#endif

/* This enum is used in 2D drawing only. */
enum YSGLSL_BMPFONT_VIEWPORT_ORIGIN {
	YSGLSL_BMPFONT_TOPLEFT_AS_ORIGIN,
	YSGLSL_BMPFONT_BOTTOMLEFT_AS_ORIGIN
};

/* This enum is used in 2D or 3D drawing. */
enum YSGLSL_BMPFONT_FIRST_LINE_ALIGNMENT {
	YSGLSL_BMPFONT_ALIGN_TOPLEFT,
	YSGLSL_BMPFONT_ALIGN_BOTTOMLEFT
};

struct YsGLSLFontBitmapRenderer;

/*! This function creates a shared bitmap-font renderer. */
void YsGLSLCreateSharedBitmapFontRenderer(void);

/*! Call this function when the graphics context is destroyed by an IRRESPONSIBLE operating system,
    and then re-create a shared bitmapfont renderer.
    I'm talking about Android.  I am very unhappily supporting Android.  */
void YsGLSLSharedBitmapFontRendererLost(void);

/*! This function returns a shared bitmap-font renderer.
    If a shared bitmap-font renderer is not created yet, it will return NULL. */
struct YsGLSLBitmapFontRenderer *YsGLSLSharedBitmapFontRenderer(void);

/*! This function deletes a shared bitmap-font renderer. */
void YsGLSLDeleteSharedBitmapFontRenderer(void);

/*! This function creates a font bitmap renderer and returns a pointer to the new renderer. */
struct YsGLSLBitmapFontRenderer *YsGLSLCreateBitmapFontRenderer(void);

/*! This function deletes a font bitmap renderer. */
void YsGLSLDeleteBitmapFontRenderer(struct YsGLSLBitmapFontRenderer *renderer);

/*! This function activates the font bitmap renderer and returns an ID of the previously active program.. */
GLuint YsGLSLUseBitmapFontRenderer(struct YsGLSLBitmapFontRenderer *renderer);

/*! This function de-activates the font bitmap renderer. */
void YsGLSLEndUseBitmapFontRenderer(struct YsGLSLBitmapFontRenderer *renderer);

/*! This function specifies the orientation of the viewport.  The parameter viewportOrigin can be:
	YSGLSL_BMPFONT_TOPLEFT_AS_ORIGIN, or
	YSGLSL_BMPFONT_BOTTOMLEFT_AS_ORIGIN.  
	The renderer must be activated by YsGLSLUseBitmapFontRenderer before this function. */
void YsGLSLSetBitmapFontRendererViewportOrigin(
	struct YsGLSLBitmapFontRenderer *renderer,enum YSGLSL_BMPFONT_VIEWPORT_ORIGIN viewportOrigin);

/*! This function specifies the alignment of the bitmap fonts.  The parameter firstLineAlignment can be:
	YSGLSL_BMPFONT_ALIGN_TOPLEFT, or
	YSGLSL_BMPFONT_ALIGN_BOTTOMLEFT. 
	The renderer must be activated by YsGLSLUseBitmapFontRenderer before this function. */
void YsGLSLSetBitmapFontRendererFirstLineAlignment(
	struct YsGLSLBitmapFontRenderer *renderer,enum YSGLSL_BMPFONT_FIRST_LINE_ALIGNMENT firstLineAlignment);

/*! This function requests the bitmap font size that is close to wid*hei pixels. */
void YsGLSLBitmapFontRendererRequestFontSize(struct YsGLSLBitmapFontRenderer *renderer,int wid,int hei);

/*! This function returns the font width in pixels of the font renderer. 
    This function can be used after YsGLSLBitmapFontRendererRequestFontSize to check what font size is actually chosen. */
int YsGLSLGetBitmapFontRendererFontWidth(struct YsGLSLBitmapFontRenderer *renderer);

/*! This function returns the font width in pixels of the font renderer. 
    This function can be used after YsGLSLBitmapFontRendererRequestFontSize to check what font size is actually chosen. */
int YsGLSLGetBitmapFontRendererFontHeight(struct YsGLSLBitmapFontRenderer *renderer);
	
/*! This function specifies the color of the fonts.  The renderer must be activated by YsGLSLUseBitmapFontRenderer before this function. */
void YsGLSLSetBitmapFontRendererColor4ub(struct YsGLSLBitmapFontRenderer *renderer,unsigned char r,unsigned char g,unsigned char b,unsigned char a);

/*! This function specifies the color of the fonts.  The renderer must be activated by YsGLSLUseBitmapFontRenderer before this function. */
void YsGLSLSetBitmapFontRendererColor3ub(struct YsGLSLBitmapFontRenderer *renderer,unsigned char r,unsigned char g,unsigned char b);

/*! This function specifies the color of the fonts.  The renderer must be activated by YsGLSLUseBitmapFontRenderer before this function. */
void YsGLSLSetBitmapFontRendererColor4f(struct YsGLSLBitmapFontRenderer *renderer,float r,float g,float b,float a);

/*! This function specifies the color of the fonts.  The renderer must be activated by YsGLSLUseBitmapFontRenderer before this function. */
void YsGLSLSetBitmapFontRendererColor3f(struct YsGLSLBitmapFontRenderer *renderer,float r,float g,float b);

/*! This function sets the modelView matrix.  ModelView and projection matrices are used when bitmap font is drawn in a 3D scene. 
    The renderer must be activated by YsGLSLUseBitmapFontRenderer before this function. */
void YsGLSLSetBitmapFontRendererModelViewfv(struct YsGLSLBitmapFontRenderer *renderer,const GLfloat modelViewMat[16]);

/*! This function sets the projection matrix.  ModelView and projection matrices are used when bitmap font is drawn in a 3D scene. 
    The renderer must be activated by YsGLSLUseBitmapFontRenderer before this function. */
void YsGLSLSetBitmapFontRendererProjectionfv(struct YsGLSLBitmapFontRenderer *renderer,const GLfloat projectionMat[16]);

/*! This function sets viewport size. 
    The renderer must be activated by YsGLSLUseBitmapFontRenderer before this function. */
void YsGLSLSetBitmapFontRendererViewportSize(struct YsGLSLBitmapFontRenderer *renderer,int width,int height);

/*! This function draws str at screen coord of (x,y). 
    After this function, active texture will be set to GL_TEXTURE0, and the binding of GL_TEXTURE2D will change. */
void YsGLSLRenderBitmapFontString2D(struct YsGLSLBitmapFontRenderer *renderer,int x,int y,const char str[]);

/*! This function draws str at 3D coord of (x,y,z). 
    After this function, active texture will be set to GL_TEXTURE0, and the binding of GL_TEXTURE2D will change. */
void YsGLSLRenderBitmapFontString3D(struct YsGLSLBitmapFontRenderer *renderer,GLfloat x,GLfloat y,GLfloat z,const char str[]);



#ifdef __cplusplus
} // Closing extern "C"
#endif

/* } */
#endif
