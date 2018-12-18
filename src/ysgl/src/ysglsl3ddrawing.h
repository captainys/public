/* ////////////////////////////////////////////////////////////

File Name: ysglsl3ddrawing.h
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

#ifndef YSGLSL_DDRAWING_IS_INCLUDED
#define YSGLSL_DDRAWING_IS_INCLUDED
/* { */

/*! \file */

#include "ysgldef.h"
#include "ysglheader.h"

/* Force Visual C++ to type-mismatching error. */
#pragma warning( error : 4028)
#pragma warning( error : 4047)

#ifdef __cplusplus
extern "C" {
#endif

struct YsGLSL3DRenderer;

/*! This function deletes a 3D rendering object created by YsGLSLCreateXXXX3DRenderer. 
*/
void YsGLSLDelete3DRenderer(struct YsGLSL3DRenderer *renderer);

/*! This function sets the projection matrix to the 3D renderer program. 
    The 3D rendere program must be current.  Use YsGLSLUse3DRenderer to make it current.  
*/
void YsGLSLSet3DRendererProjectionfv(struct YsGLSL3DRenderer *renderer,const GLfloat mat[16]);

/*! This function sets the projection matrix to the 3D renderer program. 
    The 3D rendere program must be current.  Use YsGLSLUse3DRenderer to make it current.  
*/
void YsGLSLSet3DRendererProjectiondv(struct YsGLSL3DRenderer *renderer,const double mat[16]);

/*! This function gets the projection matrix from the 3D renderer program. 
    Since the matrix is cached, the 3D rendere program does not have to be current.
*/
void YsGLSLGet3DRendererProjectionfv(GLfloat mat[16],const struct YsGLSL3DRenderer *renderer);

/*! This function gets the projection matrix from the 3D renderer program. 
    Since the matrix is cached, the 3D rendere program does not have to be current.
*/
void YsGLSLGet3DRendererProjectiondv(double mat[16],const struct YsGLSL3DRenderer *renderer);

/*! This function sets the model view matrix to the 3D renderer program. 
    The 3D rendere program must be current.  Use YsGLSLUse3DRenderer to make it current.  
*/
void YsGLSLSet3DRendererModelViewfv(struct YsGLSL3DRenderer *renderer,const GLfloat mat[16]);

/*! This function sets the model view matrix to the 3D renderer program. 
    The 3D rendere program must be current.  Use YsGLSLUse3DRenderer to make it current.  
*/
void YsGLSLSet3DRendererModelViewdv(struct YsGLSL3DRenderer *renderer,const double mat[16]);

/*! This function gets the model view matrix from the 3D renderer program. 
    Since the matrix is cached, the 3D rendere program does not have to be current.
*/
void YsGLSLGet3DRendererModelViewfv(GLfloat mat[16],const struct YsGLSL3DRenderer *renderer);

/*! This function gets the model view matrix from the 3D renderer program. 
    Since the matrix is cached, the 3D rendere program does not have to be current.
*/
void YsGLSLGet3DRendererModelViewdv(double mat[16],const struct YsGLSL3DRenderer *renderer);

/*! This function sets texture type.  Texture type must be one of the following.
    YSGLSL_TEX_TYPE_NONE
    YSGLSL_TEX_TYPE_TILING
    YSGLSL_TEX_TYPE_BILLBOARD
*/
YSRESULT YsGLSLSet3DRendererTextureType(const struct YsGLSL3DRenderer *renderer,int textureType);

/*! This function sets the texture identifier for tiling and billboarding. 
    The 3D rendere program must be current.  Use YsGLSLUse3DRenderer to make it current.  

    Surprisingly, GL_TEXTURE0 is not equal to int(0).

    So, when GL_TEXTURE0 needs to be used, give 0 as textureIdent, not GL_TEXTURE0.

    If you give GL_TEXTURE0 as textureIdent, it may not work in some environment.  (Confirmed not work in Mac OSX.  Works in Windows.)
*/
YSRESULT YsGLSLSet3DRendererTextureIdentifier(const struct YsGLSL3DRenderer *renderer,GLuint textureIdent);

/*! This function sets the texture-tiling transformation.  Incoming coordinate (x,y,z) will be transformed by 
    the matrix to calculate (u,v).  (u=x', v=y')
    The 3D rendere program must be current.  Use YsGLSLUse3DRenderer to make it current.  
    */
YSRESULT YsGLSLSet3DRendererUniformTextureTilingMatrixfv(const struct YsGLSL3DRenderer *renderer,const GLfloat texTfm[16]);

/*! This function sets Bill board clipping type.  Currently virtual lighting is only available in BillBoardRenderer. 
    Virtual-lighting type can be one of:
      YSGLSL_BILLBOARD_CLIP_NONE
      YSGLSL_BILLBOARD_CLIP_CIRCLE
    */
YSRESULT YsGLSLSet3DRendererBillBoardClippingType(const struct YsGLSL3DRenderer *renderer,int billBoardClippingType);

/*! This function sets Bill board transformation type.  Bill board transformation type must be one of the following..
    YSGLSL_TEX_BILLBOARD_PERS
    YSGLSL_TEX_BILLBOARD_ORTHO
 */
YSRESULT YsGLSLSet3DRendererBillBoardTransformType(const struct YsGLSL3DRenderer *renderer,int billBoardTfmType);

/*! This function sets the center and the dimension of billboard. */
YSRESULT YsGLSLSet3DRendererBillBoardfv(const struct YsGLSL3DRenderer *renderer,const GLfloat center[3],const GLfloat dimension[2]);

/*! This function sets the uniform color to the 3D renderer program.
    This function has effect on:
        Flat3DRenderer
    If the given renderer is not one of these, it returns YSERR.
*/
YSRESULT YsGLSLSet3DRendererUniformColorfv(struct YsGLSL3DRenderer *renderer,const GLfloat color[4]);

/*! This function sets viewport dimension to the 3D rendere program.
    This function has effect on:
        MonoColorMarker3DRenderer
        VariColorMarker3DRenderer
    If the given renderer is not one of these, it returns YSERR.
*/
YSRESULT YsGLSLSet3DRendererViewportDimensionf(const struct YsGLSL3DRenderer *renderer,const GLfloat width,const GLfloat height);

/*! This function sets alpha-cutoff to the 3D renderer program. */
YSRESULT YsGLSLSet3DRendererAlphaCutOff(const struct YsGLSL3DRenderer *renderer,GLfloat alphaCutOff);

/*! This function sets light direction.  
    The 3D rendere program must be current.  Use YsGLSLUse3DRenderer to make it current.

    Lighting is calculated in the camera coordinate, therefore the light is transformed by the current model view matrix 
    before sent to the shader program.
    */
YSRESULT YsGLSLSet3DRendererUniformLightDirectionfv(struct YsGLSL3DRenderer *renderer,int idx,const GLfloat dir[3]);

/*! This function sets light direction.  The light direction will be taken as the direction in the camera coordinate regardless of the model-view matrix. */
YSRESULT YsGLSLSet3DRendererUniformLightDirectionInCameraCoordinatefv(struct YsGLSL3DRenderer *renderer,int idx,const GLfloat dir[3]);

/*! This function sets light color. */
YSRESULT YsGLSLSet3DRendererLightColor(struct YsGLSL3DRenderer *renderer,int idx,const GLfloat color[3]);

/*! This function sets ambient-light color. */
YSRESULT YsGLSLSet3DRendererAmbientColor(struct YsGLSL3DRenderer *renderer,const GLfloat color[3]);

/*! This function sets specular-reflection color. */
YSRESULT YsGLSLSet3DRendererSpecularColor(struct YsGLSL3DRenderer *renderer,const GLfloat color[3]);

/*! Returns specular-reflection color. */
YSRESULT YsGLSLGet3DRendererSpecularColor(GLfloat color[3],struct YsGLSL3DRenderer *renderer);

/*! This function sets specular-reflection exponent. */
YSRESULT YsGLSLSet3DRendererSpecularExponent(struct YsGLSL3DRenderer *renderer,const GLfloat exponent);

/*! This function sets the size of the flash drawn.  Equivalent to YsGLSLSet3DRendererUniformMarkerDimension. */
YSRESULT YsGLSLSet3DRendererUniformFlashSize(struct YsGLSL3DRenderer *renderer,GLfloat flashSize);

/*! This function sets the relative radii of flash. */
YSRESULT YsGLSLSet3DRendererFlashRadius(struct YsGLSL3DRenderer *renderer,GLfloat r1,GLfloat r2);

/*! This function turn on/off the fog. */
YSRESULT YsGLSLSet3DRendererUniformFogEnabled(struct YsGLSL3DRenderer *renderer,int enableFog);

/*! This function sets fog density. */
YSRESULT YsGLSLSet3DRendererUniformFogDensity(struct YsGLSL3DRenderer *renderer,const GLfloat density);

/*! This function sets fog color. */
YSRESULT YsGLSLSet3DRendererUniformFogColor(struct YsGLSL3DRenderer *renderer,const GLfloat fogColor[4]);

/*! This function returns fog density set in the renderer. */
GLfloat YsGLSLGet3DRendererFogDensity(struct YsGLSL3DRenderer *renderer);

/*! This function turns on/off the z offset. */
YSRESULT YsGLSLSet3DRendererZOffsetEnabled(struct YsGLSL3DRenderer *renderer,int enableZOffset);

/*! This function sets z offset. 
    Z-offset is added to the projected coordinate.
    Negative value pulls the primitive virtually closer to the camera. 
*/
YSRESULT YsGLSLSet3DRendererZOffset(struct YsGLSL3DRenderer *renderer,GLfloat zOffset);

/*! This function makes the shader program of the renderer current, and returns the previous
    current program ID. Once a 3DRenderer is activated by this function, the program must end using
    the renderer by using YsGLSLEndUse3DRenderer function.
    
    In some implementation, attribute array state (enabled/disabled) is not per-program, and leaving an
    attribute array enabled and switching to a different program may cause a segmentation fault.
*/
GLuint YsGLSLUse3DRenderer(struct YsGLSL3DRenderer *renderer);

/*! This function ends using 3D renderer activated by YsGLSLUse3DRenderer function.  A renderer must 
    always be de-activated by this function once it is activated.
    This function does nothing if renderer is NULL.
*/
void YsGLSLEndUse3DRenderer(struct YsGLSL3DRenderer *renderer);

/*! This function returns non-zero if the renderer is in-use by YsGLSLUse3DRenderer. */
int YsGLSLCheck3DRendererIsInUse(struct YsGLSL3DRenderer *renderer);


/*! This function applies sphere map to the bitmap with the lighting stored in the renderer. 
    If useSpecular is non zero, specular reflection will be calculated assuming that the sphere is infinitely far in the depth direction. */
int YsGLSL3DRendererApplySphereMap(int w,int h,GLfloat *rgba,struct YsGLSL3DRenderer *renderer,int useSpecular);



/*! This function creates a 3D flat-renderer. 
*/
struct YsGLSL3DRenderer *YsGLSLCreateFlat3DRenderer(void);

/*! This function draws primitives.  The current renderer must be Flat3DRenderer.
*/
YSRESULT YsGLSLDrawPrimitiveVtxfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[]);

/*! This function creates a mono-color bill-board renderer.  */
struct YsGLSL3DRenderer *YsGLSLCreateMonoColorBillBoard3DRenderer(void);

/*! This function creates a vari-color bill board renderer. */
struct YsGLSL3DRenderer *YsGLSLCreateVariColorBillBoard3DRenderer(void);

/*! This function draws a primitive with vertex, color, view-offset, texture-coordinate, as attributes.
    vOffset is an array of vec3s defining offset vectors in the view coordinate.
    texCoord is an array of vec2s defining texture coordinates for the vertex. 

    Works with vari-color billboard renderer.
*/
YSRESULT YsGLSLDrawPrimitiveVtxColVoffsetTexcoordfv(
    const struct YsGLSL3DRenderer *renderer,GLenum mode,
    GLsizei nVtx,
    const GLfloat vtx[],     // 3*nVtx
    const GLfloat col[],     // 4*nVtx
    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
    const GLfloat texCoord[] // 2*nVtx
    );


/*! This function draws a primitive with vertex, view-offset, texture-coordinate, as attributes.
    vOffset is an array of vec3s defining offset vectors in the view coordinate.
    texCoord is an array of vec2s defining texture coordinates for the vertex. 

    Works with mono-color billboard renderer.
*/
YSRESULT YsGLSLDrawPrimitiveVtxVoffsetTexcoordfv(
    const struct YsGLSL3DRenderer *renderer,GLenum mode,
    GLsizei nVtx,
    const GLfloat vtx[],     // 3*nVtx
    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
    const GLfloat texCoord[] // 2*nVtx
    );


/*! This function creates a 3D flash-renderer.
 */
struct YsGLSL3DRenderer *YsGLSLCreateFlash3DRenderer(void);

/*! This function creates a 3D point-sprite version of flash-renderer.
 */
struct YsGLSL3DRenderer *YsGLSLCreateFlashByPointSprite3DRenderer(void);

/*! This function draws primitives.  The current renderer must be Flash3DRenderer.
    The vertex and viewOffset must draw a set of GL_TRIANGLES that covers the area of a flash.
 */
YSRESULT YsGLSLDrawPrimitiveVtxColVoffsetfv(const struct YsGLSL3DRenderer *renderer,GLsizei nVtx,const GLfloat vtx[],const GLfloat col[],const GLfloat viewOffset[]);

/*! This function creates a mono-color marker renderer. */
struct YsGLSL3DRenderer *YsGLSLCreateMonoColorMarker3DRenderer(void);

/*! This function sets the size of the flash drawn.  Equivalent to YsGLSLSet3DRendererUniformFlashSize. 
    The renderer needs to be MonoColorMarker3DRenderer or VariColorMarker3DRenderer. */
YSRESULT YsGLSLSet3DRendererUniformMarkerDimension(struct YsGLSL3DRenderer *renderer,GLfloat dimension);

/*! This function sets texture-coordinate range for point-sprite renderer.
    If the texture is divided into 8x8 units, one unit takes 0.125x0.125 texture coordinate space.
    In that case, the texture-coord range should be set to 0.125. */
YSRESULT YsGLSLSet3DRendererUniformTextureCoordRange(struct YsGLSL3DRenderer *renderer,GLfloat texCoordRange);

/*! This function sets the marker type.  The renderer must be current and of one of:
      MonoColorMarker3DRenderer or VariColorMarker3DRenderer.
    Marker type must be one of:
		YSGLSL_MARKER_TYPE_PLAIN
		YSGLSL_MARKER_TYPE_CIRCLE
		YSGLSL_MARKER_TYPE_RECT
		YSGLSL_MARKER_TYPE_STAR
		YSGLSL_MARKER_TYPE_EMPTY_RECT
		YSGLSL_MARKER_TYPE_EMPTY_CIRCLE
		YSGLSL_MARKER_TYPE_EMPTY_STAR
*/
YSRESULT YsGLSLSet3DRendererUniformMarkerType(struct YsGLSL3DRenderer *renderer,int markerType);


/*! This function sets shadow-map mode.  Must be one of:
	YSGLSL_SHADOWMAP_NONE
	YSGLSL_SHADOWMAP_USE
	YSGLSL_SHADOWMAP_DEBUG
   Parameter shadowMapId is an index to the shadow map. Currently must be 0 or 1.

   Shadow map ID 0 has the priority over ID 1, and is for shadow close to the view point.

   Shadow map ID 1 is for covering wider area away from the view point.
*/
YSRESULT YsGLSLSet3DRendererUniformShadowMapMode(struct YsGLSL3DRenderer *renderer,int shadowMapId,int shadowMapMode);

/*! This function sets shadow-map texture, n for GL_TEXTUREn. */
YSRESULT YsGLSLSet3DRendererUniformShadowMapTexture(struct YsGLSL3DRenderer *renderer,int shadowMapId,int textureIdent);

/*! This function sets shadow-map transformation.

    While rendering to the shadow-map, the transformation looks like:

	    v_light=Light_Projection * Light_View * Model * v

    While rendering an actual scene, the matrix is set up like:

	    v_scene=Projection * (View * Model) *v

    Matrix (View*Model) is already given as one matrix called model-view matrix.
    To calculate v_light from there, I need to do:

	    v_light=Light_Projection * Light_View * View^inv * (View * Model) * v

    Therefore, the matrix given to the shadow-map-transformation is:

	    Light_Projection * Light_View * View^inv

    One note about rendering to shadow-buffer is be careful about the viewport.  I wasted about 24 hours until I noticed that I was
    not resetting glViewport for shadow-buffer.  My window was 800x600, so I was only rendering to 800x600 of 1024x1024 texture,
    which made the shadow to shift from the actual location.  Make sure to call glViewport to cover entire texture.
*/
YSRESULT YsGLSLSet3DRendererUniformShadowMapTransformation(struct YsGLSL3DRenderer *renderer,int shadowMapId,const GLfloat tfm[16]);


/*! This function sets the light-distance offset for the shadow-mapping.
    Because of the precision of the shadow-map itself and sampled z-value, the shadow may cast on itself.
    To prevent such artifacts, light distance must be increased by small margine.
    This function sets the margine of offset for the specified shadow map.
*/
YSRESULT YsGLSLSet3DRendererUniformLightDistOffset(struct YsGLSL3DRenderer *renderer,int shadowMapId,const GLfloat offset);

/*! This function sets the light-distance offset for the shadow-mapping.
    Because of the precision of the shadow-map itself and sampled z-value, the shadow may cast on itself.
    To prevent such artifacts, light distance must be increased by small margine.
    This function sets the scaling factor for the specified shadow map.
    The scaling factor must be greater than 1.0, and the sampled light distance will be scaled by this value before
    compared against the light to fragment distance.
*/
YSRESULT YsGLSLSet3DRendererUniformLightDistScaling(struct YsGLSL3DRenderer *renderer,int shadowMapId,const GLfloat scaling);

/*! This function draws primitives.  The current renderer must be MonoColorMarker3DRenderer.
 */
YSRESULT YsGLSLDrawPrimitiveVtxPixelOffsetfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat pixelOffset[]);

/*! This function creates a vari-color marker renderer. */
struct YsGLSL3DRenderer *YsGLSLCreateVariColorMarker3DRenderer(void);

/*! This function draws primitives.  The current renderer must be VariColorMarker3DRenderer. */
YSRESULT YsGLSLDrawPrimitiveVtxColPixelOffsetfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat col[],const GLfloat pixelOffset[]);

/*! This function creates a vari-color point sprite-based marker renderer. 
    Make sure to enable GL_POINT_SPRITE and GL_PROGRAM_POINT_SIZE before using this renderer. */
struct YsGLSL3DRenderer *YsGLSLCreateVariColorMarkerByPointSprite3DRenderer(void);

/*! This function creates a vari-color point sprite-based marker renderer. 
    Make sure to enable GL_POINT_SPRITE and GL_PROGRAM_POINT_SIZE before using this renderer. */
struct YsGLSL3DRenderer *YsGLSLCreateMonoColorMarkerByPointSprite3DRenderer(void);


/*! This function creates a 3D variable-color no-lighting renderer.
*/
struct YsGLSL3DRenderer *YsGLSLCreateVariColor3DRenderer(void);

/*! This function draws primitivs with color on each vertex.  Each color consists of four components, R,G,B, and A.  
    The length of array col thus must be nVtx*4.
    The current renderer must be VariColor3DRenderer.
*/
YSRESULT YsGLSLDrawPrimitiveVtxColfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat col[]);

/*! This function draws primitivs with color and texture coordinate on each vertex.  Each color consists of four components, R,G,B, and A.  
    The length of array col thus must be nVtx*4.
    The current renderer must be VariColor3DRenderer.
*/
YSRESULT YsGLSLDrawPrimitiveVtxTexCoordColfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat col[]);

/*! This function creates a 3D monotonic-color per-vertex shading renderer. */
struct YsGLSL3DRenderer *YsGLSLCreateMonoColorPerVtxShading3DRenderer(void);

/*! This function is now same as YsGLSLCreateVariColorPerPixShading3DRenderer().
    Use with YsGLSLSet3DRendererTextureType(renderer,YSGLSL_TEX_TYPE_ATTRIBUTE);
*/
struct YsGLSL3DRenderer *YsGLSLCreateMonoColorPerVtxShadingWithTexCoord3DRenderer(void);

/*! This function draws primitives.  The current renderer must be one of:
    MonoColorPerVtxShading3DRenderer
    MonoColorPerPixShading3DRenderer
    VariColorPerVtxShading3DRenderer
    VariColorPerPixShading3DRenderer

    Color value from the most recent call of YsGLSLSet3DRendererUniformColorfv will be used.
*/
YSRESULT YsGLSLDrawPrimitiveVtxNomfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat nom[]);

/*! This function draws primitives.  The current renderer must be one of:
    MonoColorPerVtxShading3DRenderer
    MonoColorPerPixShading3DRenderer
    VariColorPerVtxShading3DRenderer
    VariColorPerPixShading3DRenderer

    Color value from the most recent call of YsGLSLSet3DRendererUniformColorfv will be used.

	Texture type must be set to YSGLSL_TEX_TYPE_ATTRIBUTE, or the texCoord will be ignored.
*/
YSRESULT YsGLSLDrawPrimitiveVtxTexCoordNomfv(const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[]);

/*! This function creates a 3D monotonic-color per-pixel shading renderer.
*/
struct YsGLSL3DRenderer *YsGLSLCreateMonoColorPerPixShading3DRenderer(void);

/*! This function is now same as YsGLSLCreateVariColorPerPixShading3DRenderer().
    Use with YsGLSLSet3DRendererTextureType(renderer,YSGLSL_TEX_TYPE_ATTRIBUTE);
*/
struct YsGLSL3DRenderer *YsGLSLCreateMonoColorPerPixShadingWithTexCoord3DRenderer(void);

/*! This function creates a 3D varying-color per-vertex shading renderer. 
*/
struct YsGLSL3DRenderer *YsGLSLCreateVariColorPerVtxShading3DRenderer(void);

/*! This function draws primitives with normal and color per vertex.
    The current renderer must be one of:
      VariColorPerVtxShading3DRenderer
      VariColorPerPixShading3DRenderer
      MonoColorPerVtxShading3DRenderer
      MonoColorPerPixShading3DRenderer
    If the renderer is a monotonic-color renderer, the first color is used for drawing all the primitives. 
    The length of vtx must be 3*nVtx.
    The length of nom must be 3*nVtx.
    The length of col must be 4*nVtx (including alpha).
*/
YSRESULT YsGLSLDrawPrimitiveVtxNomColfv(const struct YsGLSL3DRenderer *renderer, GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat nom[],const GLfloat col[]);

/*! This function draws primitives with normal and color per vertex.
    The current renderer must be one of:
      VariColorPerVtxShading3DRenderer
      VariColorPerPixShading3DRenderer
      MonoColorPerVtxShading3DRenderer
      MonoColorPerPixShading3DRenderer
    If the renderer is a monotonic-color renderer, the first color is used for drawing all the primitives. 
    The length of vtx must be 3*nVtx.
    The length of texCoord must be 2*nVtx
    The length of nom must be 3*nVtx.
    The length of col must be 4*nVtx (including alpha).
*/
YSRESULT YsGLSLDrawPrimitiveVtxTexCoordNomColfv(const struct YsGLSL3DRenderer *renderer, GLenum mode,GLsizei nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[],const GLfloat col[]);

/*! This function creates a 3D varying-color per-pixel shading renderer.
*/
struct YsGLSL3DRenderer *YsGLSLCreateVariColorPerPixShading3DRenderer(void);


/*! This function is now same as YsGLSLCreateVariColorPerPixShading3DRenderer().
    Use with YsGLSLSet3DRendererTextureType(renderer,YSGLSL_TEX_TYPE_ATTRIBUTE);
*/
struct YsGLSL3DRenderer *YsGLSLCreateVariColorPerPixShadingWithTexCoord3DRenderer(void);


/*! This function is now same as YsGLSLCreateVariColorPerPixShading3DRenderer().
    Use with YsGLSLSet3DRendererTextureType(renderer,YSGLSL_TEX_TYPE_ATTRIBUTE);
*/
struct YsGLSL3DRenderer *YsGLSLCreateVariColorPerVtxShadingWithTexCoord3DRenderer(void);


/*! This function creates a 3D varying-color point-sprite renderer. 

    Point-sprite maps part of the active texture on a point.  To enable texture, texture type must be:
        YSGLSL_TEX_TYPE_TILING
        YSGLSL_TEX_TYPE_BILLBOARD
    If texture type is:
        YSGLSL_TEX_TYPE_NONE
    the point is drawn as an attribute color.

    Other values of the texture type are not supported.
*/
struct YsGLSL3DRenderer *YsGLSLCreateVariColorPointSprite3DRenderer(void);


/*! This function sets clipping type of the point-sprite renderer.
    Clipping type can be one of the following. 
      YSGLSL_POINTSPRITE_CLIPPING_NONE
      YSGLSL_POINTSPRITE_CLIPPING_CIRCLE 
    GL_POINT_SPRITE needs to be enabled to use this feature in non ES 2.0 environment.
*/
YSRESULT YsGLSLSet3DRendererPointSpriteClippingType(const struct YsGLSL3DRenderer *renderer,int pointSpriteClippingType);

/*! This function sets point-size mode of the point-sprite renderer.
    Point-size mode can be one of the following.
      YSGLSL_POINTSPRITE_SIZE_IN_PIXEL     Point size is number of pixels.
      YSGLSL_POINTSPRITE_SIZE_IN_3DSPACE   Point size is the dimension in the 3D space.
*/
YSRESULT YsGLSLSet3DRendererPointSizeMode(const struct YsGLSL3DRenderer *renderer,int pointSizeMode);

/*! This function sets uniform point size.  The renderers that supporst uniform point size are:
     Flat 3D Renderer
     Vari Color 3D Renderer.
*/
YSRESULT YsGLSLSet3DRendererUniformPointSize(const struct YsGLSL3DRenderer *renderer,float pointSize);

/*! This function draws primitivs with vertex position, color, texture coord origin, and point size.
    By varying texture-coord origin, multiple patterns stored in one texture can be mapped to the different particles
    in one glDrawArrays call.

    GL_PROGRAM_POINT_SIZE needs to be enabled, or pointSize will have no effect.

    Also enable GL_POINT_SPRITE, or the fragment shader cannot use gl_PointCoord.
*/
YSRESULT YsGLSLDrawPrimitiveVtxTexCoordPointSizefv(
    const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,
    const GLfloat vtx[],const GLfloat texCoord[],const GLfloat pointSize[]);

/*! This function draws primitivs with vertex position, color, texture coord origin, and point size.
    By varying texture-coord origin, multiple patterns stored in one texture can be mapped to the different particles
    in one glDrawArrays call.

    It would be better if generic attribute works as advertised.

    GL_PROGRAM_POINT_SIZE needs to be enabled, or pointSize will have no effect.

    Also enable GL_POINT_SPRITE, or the fragment shader cannot use gl_PointCoord.
*/
YSRESULT YsGLSLDrawPrimitiveVtxTexCoordColPointSizefv(
    const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,
    const GLfloat vtx[],const GLfloat texCoord[],const GLfloat color[],const GLfloat pointSize[]);

/*! This function draws primitives with vertex position, color, and point size. */
YSRESULT YsGLSLDrawPrimitiveVtxColPointSizefv(
    const struct YsGLSL3DRenderer *renderer,GLenum mode,GLsizei nVtx,
    const GLfloat vtx[],const GLfloat color[],const GLfloat pointSize[]);



/* Will need

New Uniforms:
  Texture Identifier
  Repeat switch

struct YsGLSL3DRenderer *YsGLSLCreateVariColorWithTex3DRenderer(void);
YSRESULT YsGLSLDrawPrimitiveVtxColTexfv
YSRESULT YsGLSLDrawPrimitiveVtxNomColTexfv
*/

#ifdef __cplusplus
}
#endif

/* } */
#endif
