/* ////////////////////////////////////////////////////////////

File Name: ysglslsharedrenderer.h
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

#ifndef YSGLSLSHAREDRENDERER_IS_INCLUDED
#define YSGLSLSHAREDRENDERER_IS_INCLUDED
/* { */

/*! \file */

/* Force Visual C++ to type-mismatching error. */
#pragma warning( error : 4028)
#pragma warning( error : 4047)

#ifdef __cplusplus
extern "C" {
#endif

#include "ysgldef.h"
#include "ysglsl3ddrawing.h"
#include "ysglslplain2ddrawing.h"
#include "ysglslbitmaprenderer.h"

/*! This function allocates shared renderers.
*/
YSRESULT YsGLSLCreateSharedRenderer(void);

/*! Call this function when the graphics context is destroyed by an IRRESPONSIBLE operating system,
    and then re-create graphics resources.
    I'm talking about Android.    I am very unhappily supporting Android.
    Direct 3D does the same, although I don't care.
*/
YSRESULT YsGLSLSharedRendererLost(void);

/*! This function de-allocates shared renderes.
*/
YSRESULT YsGLSLDeleteSharedRenderer(void);

/*! This function returns a pointer to the shared Flat3DRenderer. */
struct YsGLSL3DRenderer *YsGLSLSharedFlat3DRenderer(void);

/*! This function returns a pointer to the shared vari-color bill-board 3D renderer. */
struct YsGLSL3DRenderer *YsGLSLSharedVariColorBillBoard3DRenderer(void);

/*! This function returns a pointer to the shared mono-color bill-board 3D renderer. */
struct YsGLSL3DRenderer *YsGLSLSharedMonoColorBillBoard3DRenderer(void);

/*! This function returns a pointer to the shared Flash3DRenderer. */
struct YsGLSL3DRenderer *YsGLSLSharedFlash3DRenderer(void);

/*! This function returns a pointer to the shared FlashByPointSprite3DRenderer. */
struct YsGLSL3DRenderer *YsGLSLSharedFlashByPointSprite3DRenderer(void);

/*! This function returns a pointer to the shared VariColor3DRenderer */
struct YsGLSL3DRenderer *YsGLSLSharedVariColor3DRenderer(void);

/*! This function returns a pointer to the shared MonoColorPerVtxShading3DRenderer */
struct YsGLSL3DRenderer *YsGLSLSharedMonoColorPerVtxShading3DRenderer(void);

/*! This function returns a pointer to the shared MonoColorPerPixShading3DRenderer */
struct YsGLSL3DRenderer *YsGLSLSharedMonoColorPerPixShading3DRenderer(void);

/*! This function returns a pointer to the shared VariColorPerVtxShading3DRenderer */
struct YsGLSL3DRenderer *YsGLSLSharedVariColorPerVtxShading3DRenderer(void);

/*! This function returns a pointer to the shared VariColorPerPixShading3DRenderer */
struct YsGLSL3DRenderer *YsGLSLSharedVariColorPerPixShading3DRenderer(void);

/*! This function returns a pointer to the shared MonoColorPerVtxShading3DRenderer */
struct YsGLSL3DRenderer *YsGLSLSharedMonoColorPerVtxShadingWithTexCoord3DRenderer(void);

/*! This function returns a pointer to the shared VariColorPerVtxShading3DRenderer */
struct YsGLSL3DRenderer *YsGLSLSharedVariColorPerVtxShadingWithTexCoord3DRenderer(void);

/*! This function returns a pointer to the shared MonoColorPerPixShading3DRenderer */
struct YsGLSL3DRenderer *YsGLSLSharedMonoColorPerPixShadingWithTexCoord3DRenderer(void);

/*! This function returns a pointer to the shared VariColorPerPixShading3DRenderer */
struct YsGLSL3DRenderer *YsGLSLSharedVariColorPerPixShadingWithTexCoord3DRenderer(void);

/*! This function returns a pointer to the shared MonoColorMarker3DRendere */
struct YsGLSL3DRenderer *YsGLSLSharedMonoColorMarker3DRenderer(void);

/*! This function returns a pointer to the shared MonoColorMarker3DRendere */
struct YsGLSL3DRenderer *YsGLSLSharedVariColorMarker3DRenderer(void);

/*! This function returns a pointer to the shared MonoColorMarkerByPointSprite3DRenderer. */
struct YsGLSL3DRenderer *YsGLSLSharedMonoColorMarkerByPointSprite3DRenderer(void);

/*! This function returns a pointer to the shared VariColorMarkerByPointSprite3DRenderer. */
struct YsGLSL3DRenderer *YsGLSLSharedVariColorMarkerByPointSprite3DRenderer(void);

/*! This function returns a pointer to the shared VariColorPointSprite3DRenderer. */
struct YsGLSL3DRenderer *YsGLSLSharedVariColorPointSprite3DRenderer(void);

/*! This function sets alpha-cutoff value to the shared 3D renderers. */
YSRESULT YsGLSLSetShared3DRendererAlphaCutOff(GLfloat alphaCutOff);

/*! This function sets fog to all applicable shared 3D renderers. */
YSRESULT YsGLSLSetShared3DRendererFog(int enableFog,const GLfloat density,const GLfloat fogColor[4]);

/*! This function sets projection matrix to all applicable shared 3D renderers. */
YSRESULT YsGLSLSetShared3DRendererProjection(const GLfloat proj[16]);

/*! This function sets modelview matrix to all applicable shared 3D renderers. */
YSRESULT YsGLSLSetShared3DRendererModelView(const GLfloat modelview[16]);

/*! This function sets directional light to all applicable shared 3D renderers. */
YSRESULT YsGLSLSetShared3DRendererDirectionalLightfv(int i,const GLfloat dir[3]);

/*! This function sets directional light to all applicable shared 3D renderers.  The light direction will be taken as the direction in the camera coordinate system regardless of the model-view matrix. */
YSRESULT YsGLSLSetShared3DRendererDirectionalLightInCameraCoordinatefv(int i,const GLfloat dir[3]);

/*! This function sets light color to all applicable shared 3D renderers. */
YSRESULT YsGLSLSetShared3DRendererLightColor(int idx,const GLfloat color[3]);

/*! This function sets ambient-light color to all applicable shared 3D renderers. */
YSRESULT YsGLSLSetShared3DRendererAmbientColor(const GLfloat color[3]);

/*! This function sets specular reflection color to all applicable shared 3D renderers. */
YSRESULT YsGLSLSetShared3DRendererSpecularColor(const GLfloat color[3]);

/*! This function sets specular exponent to all applicable shared 3D renderers. */
YSRESULT YsGLSLSetShared3DRendererSpecularExponent(const GLfloat exponent);

/*! This function sets the view port dimension to all applicable shared 3D renderers. */
YSRESULT YsGLSLSetShared3DRendererViewPortDimension(const GLfloat wid,const GLfloat hei);


/*! This function returns a pointer to the shared Plain2DRenderer */
struct YsGLSLPlain2DRenderer *YsGLSLSharedPlain2DRenderer(void);

/*! This function returns a pointer to the shared BitmapRenderer */
struct YsGLSLBitmapRenderer *YsGLSLSharedBitmapRenderer(void);


/*! This function controls which renderer is returned by the following three functions:
      YsGLSLSharedMonoColorShaded3DRenderer
      YsGLSLSharedVariColorShaded3DRenderer
    If this flag is set to YSTRUE, the renderer returned by the two functions are:
      YsGLSLSharedMonoColorShaded3DRenderer => YsGLSLSharedMonoColorPerPixShading3DRenderer()
      YsGLSLSharedVariColorShaded3DRenderer => YsGLSLSharedVariColorPerPixShading3DRenderer()
    otherwise,
      YsGLSLSharedMonoColorShaded3DRenderer => YsGLSLSharedMonoColorPerVtxShading3DRenderer()
      YsGLSLSharedVariColorShaded3DRenderer => YsGLSLSharedVariColorPerVtxShading3DRenderer()

    Default value is YSFALSE.
*/
void YsGLSLSetPerPixRendering(YSBOOL perPix);

/*! This function returns YsGLSLSharedMonoColorPerPixShading3DRenderer() or
    YsGLSLSharedMonoColorPerVtxShading3DRenderer() depending on the switch set 
    by YsGLSLSetPerPixRendering. */
struct YsGLSL3DRenderer *YsGLSLSharedMonoColorShaded3DRenderer(void);

/*! This function returns YsGLSLSharedVariColorPerPixShading3DRenderer() or
    YsGLSLSharedVariColorPerVtxShading3DRenderer() depending on the switch set 
    by YsGLSLSetPerPixRendering. */
struct YsGLSL3DRenderer *YsGLSLSharedVariColorShaded3DRenderer(void);


/*! This function returns YsGLSLSharedMonoColorPerPixShadingWithTexCoord3DRenderer() or
    YsGLSLSharedMonoColorPerVtxShadingWithTexCoord3DRenderer() depending on the switch set 
    by YsGLSLSetPerPixRendering. */
struct YsGLSL3DRenderer *YsGLSLSharedMonoColorShadedWithTexCoord3DRenderer(void);

/*! This function returns YsGLSLSharedVariColorPerPixShadingWithTexCoord3DRenderer() or
    YsGLSLSharedVariColorPerVtxShadingWithTexCoord3DRenderer() depending on the switch set 
    by YsGLSLSetPerPixRendering. */
struct YsGLSL3DRenderer *YsGLSLSharedVariColorShadedWithTexCoord3DRenderer(void);


#ifdef __cplusplus
} // extern "C"
#endif

/* } */
#endif
