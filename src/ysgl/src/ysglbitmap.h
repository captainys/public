/* ////////////////////////////////////////////////////////////

File Name: ysglbitmap.h
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

#ifndef YSGLBITMAP_IS_INCLUDED
#define YSGLBITMAP_IS_INCLUDED
/* { */

#include "ysgldef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \file */

/*! This function creates a copy of a bitmap.
    The destination bitmap must be the same size.
 */
void YsGLCopyBitmap(int w,int h,GLfloat *dst,const GLfloat *src);

/*! This function converts a unsigned char RGBA bitmap to GLfloat bitmap. */
void YsGLCopyUCharBitmapToGLFloatBitmap(int w,int h,GLfloat *dst,const unsigned char *src);

/*! This function virtually maps the bitmap on a sphere and calculates the lighting.
    All pixels outside the spheres will be given color of (0,0,0,0)

    This function only supports directional light and does not support point light.
    Fourth component of light position will be ignored.

    When useSpecular is non zero, specular reflection will be calculated by assuming that the sphere is infinitely far in a view coordinate.

    ambientColor must be of length of 3.
    lightEnable must be of length of nLight.
    lightPosition must be of length of 4*nLight.  (Although each 4th component will be ignored.)
    lightColor must be of length of 3*nLight.
    specularColor must be of length of 3.
*/
void YsGLApplySphereMap(
    int w,int h,GLfloat *rgba,
    GLfloat ambientColor[],  // r,g,b
    int nLight,
	GLfloat lightEnabled[],
	GLfloat lightPosition[], // x,y,z,w
	GLfloat lightColor[],    // r,g,b
	int useSpecular,
	GLfloat specularColor[], // r,g,b
	GLfloat specularExponent);

#ifdef __cplusplus
}
#endif

/* } */
#endif
