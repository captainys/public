/* ////////////////////////////////////////////////////////////

File Name: ysglslcpp.h
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

#ifndef YSGLSLCPP_IS_INCLUDED
#define YSGLSLCPP_IS_INCLUDED
/* { */

#include <ysgl.h>
#include <ysclass.h>
#include <ysglsldrawfontbitmap.h>
#include "ysglcpp.h"


// TODO: Remove textureEnabled in PointSprite renderer and use textureType instead.


/*! A renderer corresponds to a set of GLSL programs.
    The GLSL program is enabled by glUseProgram inside and disabled in the destructor.
*/
class YsGLSLRenderer
{
protected:
	static YsGLSLRenderer *currentRenderer;
	struct YsGLSL3DRenderer *renderer3dPtr;
	struct YsGLSLPlain2DRenderer *renderer2dPtr;
	struct YsGLSLBitmapFontRenderer *rendererBitmapFontPtr;
public:
	static void CreateSharedRenderer(void);

	/*! Call this function when the graphics context is destroyed by an IRRESPONSIBLE operating system,
	    and then re-create shared renderers.
	    I'm talking about Android.  I am very unhappily supporting Android.
	*/
	static void SharedRendererLost(void);

	static void DeleteSharedRenderer(void);

	YsGLSLRenderer(struct YsGLSL3DRenderer *r3ptr);
	YsGLSLRenderer(struct YsGLSLPlain2DRenderer *r3ptr);
	YsGLSLRenderer(struct YsGLSLBitmapFontRenderer *rBmfPtr);
	~YsGLSLRenderer();
	void EndUseRenderer(void);

	operator struct YsGLSL3DRenderer *();

	/*! Set projection matrix.
	*/
	void SetProjection(const float mat[16]);
	void SetProjection(const double mat[16]);

	/*! Get projection matrix.
	*/
	void GetProjection(float mat[16]) const;
	void GetProjection(double mat[16]) const;

	/*! Set modelview matrix.
	*/
	void SetModelView(const float mat[16]);
	void SetModelView(const double mat[16]);

	/*! Get modelview matrix.
	*/
	void GetModelView(float mat[16]) const;
	void GetModelView(double mat[16]) const;

	/*! Set uniform color.
	*/
	template <class T>
	void SetUniformColor(T r,T g,T b,T a)
	{
		if(nullptr!=renderer3dPtr)
		{
			const GLfloat rgba[4]={(GLfloat)r,(GLfloat)g,(GLfloat)b,(GLfloat)a};
			YsGLSLSet3DRendererUniformColorfv(renderer3dPtr,rgba);
		}
		else if(nullptr!=rendererBitmapFontPtr)
		{
			YsGLSLSetBitmapFontRendererColor4f(rendererBitmapFontPtr,(float)r,(float)g,(float)b,(float)a);
		}
		else if(nullptr!=renderer2dPtr)
		{
			const GLfloat rgba[4]={(GLfloat)r,(GLfloat)g,(GLfloat)b,(GLfloat)a};
			YsGLSLSetPlain2DRendererUniformColor(renderer2dPtr,rgba);
		}
	}
	void SetUniformColor(const float col[4]);
	void SetUniformColor(const double col[4]);
	void SetUniformColor(const unsigned char col[4]);
	void SetUniformColor(const YsColor &col);

	/*! Set texture type.  Must be one of:
	    YSGLSL_TEX_TYPE_NONE
	    YSGLSL_TEX_TYPE_TILING
	    YSGLSL_TEX_TYPE_BILLBOARD
	    YSGLSL_TEX_TYPE_3DTILING
	    YSGLSL_TEX_TYPE_ATTRIBUTE
	*/
	void SetTextureType(int textureType);

	/*! Set texture identifier.

	    Surprisingly, GL_TEXTURE0 is not equal to int(0).

	    So, when GL_TEXTURE0 needs to be used, give 0 as textureIdent, not GL_TEXTURE0.

	    If you give GL_TEXTURE0 as textureIdent, it may not work in some environment.  (Confirmed not work in Mac OSX.  Works in Windows.)
	*/
	void SetTextureIdent(int textureIdent);

	/*! Set texture tiling matrix.
	*/
	void SetTextureTilingMatrix(const float mat[16]);

	/*! Set billboard clipping type.  The value must be one of:
	      YSGLSL_BILLBOARD_CLIP_NONE
	      YSGLSL_BILLBOARD_CLIP_CIRCLE
	      YSGLSL_POINTSPRITE_CLIPPING_NONE
	      YSGLSL_POINTSPRITE_CLIPPING_CIRCLE 
	    This function is equivalent to SetPointSpriteClippingType.
	*/
	void SetBillboardClippingType(int clippingType);

	/*! Set point-sprint clipping type.  The value must be one of:
	      YSGLSL_BILLBOARD_CLIP_NONE
	      YSGLSL_BILLBOARD_CLIP_CIRCLE
	      YSGLSL_POINTSPRITE_CLIPPING_NONE
	      YSGLSL_POINTSPRITE_CLIPPING_CIRCLE 
	    This function is equivalent to SetBillboardClippingType.
	*/
	void SetPointSpriteClippingType(int clippingType);

	/*! Set point-sprite texture range.
	    This is a range in the texture coordinate in which one point-sprite will take.
	    If a texture is divided into four (2x2), one texture will take 0.5 x 0.5 in the texture coordinate,
	    and therefore texture-coordinate range should be 0.5.
	*/
	void SetPointSpriteTextureCoordRange(float texCoordRange);

	/*! Set billboard transformation type. The value must be one of:
	    YSGLSL_TEX_BILLBOARD_PERS
	    YSGLSL_TEX_BILLBOARD_ORTHO
	*/
	void SetBillboardTransformationType(int billboardTfmType);

	/*! Set billboard center and dimension of the billboard. */
	void SetBillboardCenterAndDimension(const float center[3],const float dimension[2]);

	/*! Set marker type.
	    Marker type must be one of:
			YSGLSL_MARKER_TYPE_PLAIN
			YSGLSL_MARKER_TYPE_CIRCLE
			YSGLSL_MARKER_TYPE_RECT
			YSGLSL_MARKER_TYPE_STAR
			YSGLSL_MARKER_TYPE_EMPTY_RECT
			YSGLSL_MARKER_TYPE_EMPTY_CIRCLE
			YSGLSL_MARKER_TYPE_EMPTY_STAR
	*/
	void SetMarkerType(int markerType);

	/*! Set marker dimension.  It also sets flash size. */
	void SetMarkerDimension(float dim);

	/*! Set viewport dimension. */
	void SetViewportDimension(float wid,float hei);

	/*! Set alpha-cutoff value. */
	void SetAlphaCutOff(float threshold);

	/*! Set light direction.
	    Light direction is transformed by the current modelview matrix and stored in the camera's own coordinate system.
	 */
	void SetLightDirection(int lightIndex,const float dir[3]);

	/*! Set light direction in the camera's own coordinate system.
	    Convenient for specifying a head-light. */
	void SetLightDirectionInCameraCoordinate(int lightIndex,const float dir[3]);

	/*! Set light color.  It ignores alpha component. It ignores alpha component.  Therefore, the length of RGB can be 3. */
	void SetLightColor(int lightIndex,const float RGB[3]);

	/*! Set ambient-light color.  It ignores alpha component.  Therefore, the length of RGB can be 3. */
	void SetAmbientLightColor(int lightIndex,const float RGB[3]);

	/*! Set specular-reflection color.  It ignores alpha component.  Therefore, the length of RGB can be 3.
	*/
	void SetSpecularColor(const float RGB[3]);

	/*! Set specular exponent. */
	void SetSpecularExponent(float exponent);

	/*! Set uniform flash size. */
	void SetUniformFlashSize(float flashSize);

	/*! Set uniform point size. */
	void SetUniformPointSize(float pointSize);

	/*! Set uniform flash radii relative to the specified flash size. */
	void SetFlashRadius(float r1,float r2);

	/*! Set point-size mode of the point-sprite renderer.
	    Point-size mode can be one of:
	      YSGLSL_POINTSPRITE_SIZE_IN_PIXEL     Point size is number of pixels.
	      YSGLSL_POINTSPRITE_SIZE_IN_3DSPACE   Point size is the dimension in the 3D space.
	*/
	void SetPointSizeMode(int pointSizeMode);

	/*! Enable fog. */
	void EnableFog(void);

	/*! Disable fog. */
	void DisableFog(void);

	/*! Set fog density. */
	void SetFogDensity(float density);

	/*! Set fog color. */
	void SetFogColor(const float RGBA[4]);

	/*! Enables Z-offset. (Something like polygon offset.) */
	void EnableZOffset(void);

	/*! Disables Z-offset. (Something like polygon offset.) */
	void DisableZOffset(void);

	/*! Set z-offset. 
	    Z-offset is added to the projected coordinate.
	    Negative value pulls the primitive virtually closer to the camera. */
	void SetZOffset(float offset);

	/*! Draw primitives with vertex positions. */
	void DrawVtx(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const float vtx[]) const;

	/*! Draw primitives with vertex positions. */
	void DrawVtx(GLenum primType,YSSIZE_T nVtx,const float vtx[]) const;

	/*! Draw primitives with vertex positions and colors. */
	void DrawVtxCol(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const float vtx[],const float col[]) const;

	/*! Draw primitives with vertex positions and colors. */
	void DrawVtxCol(GLenum primType,YSSIZE_T nVtx,const float vtx[],const float col[]) const;

	/*! Draw primitives with vertex positions, texture coordinates, and colors. */
	void DrawVtxTexCoordCol(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat col[]);

	/*! Draw primitives with vertex positions, texture coordinates, and colors. */
	void DrawVtxTexCoordCol(GLenum primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat col[]);

	/*! Draw primitives with vertex positions and normal vectors. */
	void DrawVtxNom(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const float vtx[],const float col[]) const;

	/*! Draw primitives with vertex positions and normal vectors. */
	void DrawVtxNom(GLenum primType,YSSIZE_T nVtx,const float vtx[],const float col[]) const;

	/*! Draw primitives with vertex positions, texture coordinates, and normal. */
	void DrawVtxTexCoordNom(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[]) const;

	/*! Draw primitives with vertex positions, texture coordinates, and normal. */
	void DrawVtxTexCoordNom(GLenum primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[]) const;

	/*! Draw primitives with vertex positions, normal vectors, and colors. */
	void DrawVtxNomCol(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat nom[],const GLfloat col[]);

	/*! Draw primitives with vertex positions, normal vectors, and colors. */
	void DrawVtxNomCol(GLenum primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat nom[],const GLfloat col[]);

	/*! Draw primitives with vertex positions, texture coordinates, normal vectors, and colors. */
	void DrawVtxTexCoordNomCol(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[],const GLfloat col[]);

	/*! Draw primitives with vertex positions, texture coordinates, normal vectors, and colors. */
	void DrawVtxTexCoordNomCol(GLenum primType,YSSIZE_T nVtx,const GLfloat vtx[],const GLfloat texCoord[],const GLfloat nom[],const GLfloat col[]);

	/*! Draw primitives with vertex positions, colors, view-offsets, and texture coordinates. */
	void DrawVtxColVoffsetTexCoord(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],     // 3*nVtx
	    const GLfloat col[],     // 4*nVtx
	    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
	    const GLfloat texCoord[] // 2*nVtx
	);

	/*! Draw primitives with vertex positions, colors, view-offsets, and texture coordinates. */
	void DrawVtxColVoffsetTexCoord(GLenum,YSSIZE_T nVtx,
	    const GLfloat vtx[],     // 3*nVtx
	    const GLfloat col[],     // 4*nVtx
	    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
	    const GLfloat texCoord[] // 2*nVtx
	);

	/*! Draw primitives with vertex positions, view-offsets, and texture coordinates. */
	void DrawVtxVoffsetTexCoord(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],     // 3*nVtx
	    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
	    const GLfloat texCoord[] // 2*nVtx
	);

	/*! Draw primitives with vertex positions, view-offsets, and texture coordinates. */
	void DrawVtxVoffsetTexCoord(GLenum primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],     // 3*nVtx
	    const GLfloat vOffset[], // 3*nVtx   Let it modify Z as well
	    const GLfloat texCoord[] // 2*nVtx
	);

	/*! Draw primitives with vertex posisions, colors, and point sizes. */
	void DrawVtxColPointSize(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],
	    const GLfloat color[],
	    const GLfloat pointSize[]);

	/*! Draw primitives with vertex posisions, colors, and point sizes. */
	void DrawVtxColPointSize(GLenum primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],
	    const GLfloat color[],
	    const GLfloat pointSize[]);

	/*! Draw primitives with vertex positions, texture coordinates, colors, and point sizes. 
	    (TexCoord[i*2],TexCoord[i*2+1]) defines the minimum texture-coordinate of the ith sprite.
	    The ith sprite will use texture-coordinate range of:
	        (TexCoord[i*2],TexCoord[i*2+1])-(TexCoord[i*2]+texCoordRange,TexCoord[i*2+1]+texCoordRange)
	    where texCoordRange is the value specified by SetPointSpriteTextureCoordRange.

	    GL_PROGRAM_POINT_SIZE needs to be enabled to use this function. (Not necessary in OpenGL ES)

	    Also enable GL_POINT_SPRITE, or the fragment shader cannot use gl_PointCoord.
	*/
	void DrawVtxTexCoordColPointSize(YsGL::PRIMITIVE_TYPE primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],const GLfloat TexCoord[],const GLfloat color[],const GLfloat pointSize[]);

	/*! Draw primitives with vertex positions, texture coordinates, colors, and point sizes. 
	    (TexCoord[i*2],TexCoord[i*2+1]) defines the minimum texture-coordinate of the ith sprite.
	    The ith sprite will use texture-coordinate range of:
	        (TexCoord[i*2],TexCoord[i*2+1])-(TexCoord[i*2]+texCoordRange,TexCoord[i*2+1]+texCoordRange)
	    where texCoordRange is the value specified by SetPointSpriteTextureCoordRange.

	    GL_PROGRAM_POINT_SIZE needs to be enabled to use this function. (Not necessary in OpenGL ES)

	    Also enable GL_POINT_SPRITE, or the fragment shader cannot use gl_PointCoord.
	*/
	void DrawVtxTexCoordColPointSize(GLenum primType,YSSIZE_T nVtx,
	    const GLfloat vtx[],const GLfloat TexCoord[],const GLfloat color[],const GLfloat pointSize[]);

	/*! Draw primitives with vertex positions, texture coordinates, and point sizes. 
	    (TexCoord[i*2],TexCoord[i*2+1]) defines the minimum texture-coordinate of the ith sprite.
	    The ith sprite will use texture-coordinate range of:
	        (TexCoord[i*2],TexCoord[i*2+1])-(TexCoord[i*2]+texCoordRange,TexCoord[i*2+1]+texCoordRange)
	    where texCoordRange is the value specified by SetPointSpriteTextureCoordRange.

	    GL_PROGRAM_POINT_SIZE needs to be enabled to use this function. (Not necessary in OpenGL ES)

	    Also enable GL_POINT_SPRITE, or the fragment shader cannot use gl_PointCoord.
	*/
	void DrawVtxTexCoordPointSize(
	    GLenum mode,YSSIZE_T nVtx,
	    const GLfloat vtx[],const GLfloat texCoord[],const GLfloat pointSize[]);



	// Below this line are for Bitmap-Font Renderer 

	/*! This function specifies the orientation of the viewport.  The parameter viewportOrigin can be:
		YSGLSL_BMPFONT_TOPLEFT_AS_ORIGIN, or
		YSGLSL_BMPFONT_BOTTOMLEFT_AS_ORIGIN. 
	*/
	void SetViewportOrigin(int viewportOrigin);

	/*! This function specifies the alignment of the bitmap fonts.  The parameter firstLineAlignment can be:
		YSGLSL_BMPFONT_ALIGN_TOPLEFT, or
		YSGLSL_BMPFONT_ALIGN_BOTTOMLEFT. 
	*/
	void SetFontFirstLineAlignment(int alignment);

	/*! This function requests the bitmap font size that is close to wid*hei pixels. */
	void RequestFontSize(int wid,int hei);

	/*! This function draws str at screen coord of (x,y). 
	    After this function, active texture will be set to GL_TEXTURE0, and the binding of GL_TEXTURE2D will change. */
	void DrawString(int x,int y,const char str[]);

	/*! This function draws str at 3D coord of (x,y,z). 
	    After this function, active texture will be set to GL_TEXTURE0, and the binding of GL_TEXTURE2D will change. */
	void DrawString(float x,float y,float z,const char str[]);
	// Above this line are for Bitmap-Font Renderer


	// Below this line are for shadow-map >>
	void SetUniformShadowMapMode(int shadowMapId,int shadowMapMode);
	void SetUniformShadowMapTexture(int shadowMapId,int texIdent);
	void SetUniformShadowMapTransformation(int shadowMapId,const float lightTfmCache[16]);
	// Above this line are for shadow-map <<
};

class YsGLSL3DRendererBase : protected YsGLSLRenderer
{
public:
	YsGLSL3DRendererBase(struct YsGLSL3DRenderer *r3ptr);

	operator struct YsGLSL3DRenderer *();

	using YsGLSLRenderer::SetProjection;
	using YsGLSLRenderer::GetProjection;
	using YsGLSLRenderer::SetModelView;
	using YsGLSLRenderer::GetModelView;
};

class YsGLSLPlain3DRenderer : public YsGLSL3DRendererBase
{
public:
	using YsGLSLRenderer::EnableFog;
	using YsGLSLRenderer::DisableFog;
	using YsGLSLRenderer::SetAlphaCutOff;
	using YsGLSLRenderer::SetFogDensity;
	using YsGLSLRenderer::SetFogColor;
	using YsGLSLRenderer::EnableZOffset;
	using YsGLSLRenderer::DisableZOffset;
	using YsGLSLRenderer::SetZOffset;
	using YsGLSLRenderer::SetTextureType;
	using YsGLSLRenderer::SetTextureIdent;
	using YsGLSLRenderer::SetTextureTilingMatrix;
	using YsGLSLRenderer::SetUniformPointSize;
	using YsGLSLRenderer::SetPointSizeMode;

	using YsGLSLRenderer::SetUniformColor;

	using YsGLSLRenderer::DrawVtxCol;
	using YsGLSLRenderer::DrawVtxTexCoordCol;

	// If generic attribute really works, it should be able to take care of the following >>
	using YsGLSLRenderer::DrawVtx;
	// If generic attribute really works, it should be able to take care of the following <<

	YsGLSLPlain3DRenderer();
};

class YsGLSLPerVertexShaded3DRenderer : public YsGLSL3DRendererBase
{
public:
	using YsGLSLRenderer::SetUniformColor;

	using YsGLSLRenderer::SetLightDirection;
	using YsGLSLRenderer::SetLightDirectionInCameraCoordinate;
	using YsGLSLRenderer::SetLightColor;
	using YsGLSLRenderer::SetAlphaCutOff;
	using YsGLSLRenderer::SetAmbientLightColor;
	using YsGLSLRenderer::SetSpecularColor;
	using YsGLSLRenderer::SetSpecularExponent;
	using YsGLSLRenderer::SetTextureType;
	using YsGLSLRenderer::SetTextureIdent;
	using YsGLSLRenderer::SetTextureTilingMatrix;
	using YsGLSLRenderer::SetBillboardClippingType;
	using YsGLSLRenderer::SetBillboardTransformationType;
	using YsGLSLRenderer::SetBillboardCenterAndDimension;
	using YsGLSLRenderer::EnableFog;
	using YsGLSLRenderer::DisableFog;
	using YsGLSLRenderer::SetFogDensity;
	using YsGLSLRenderer::SetFogColor;
	using YsGLSLRenderer::EnableZOffset;
	using YsGLSLRenderer::DisableZOffset;
	using YsGLSLRenderer::SetZOffset;

	using YsGLSLRenderer::DrawVtxNomCol;
	using YsGLSLRenderer::DrawVtxTexCoordNomCol;

	// If generic attribute really works, it should be able to take care of the following >>
	using YsGLSLRenderer::DrawVtxNom;
	using YsGLSLRenderer::DrawVtxTexCoordNom;
	// If generic attribute really works, it should be able to take care of the following <<

	using YsGLSLRenderer::SetUniformShadowMapMode;
	using YsGLSLRenderer::SetUniformShadowMapTexture;
	using YsGLSLRenderer::SetUniformShadowMapTransformation;


	YsGLSLPerVertexShaded3DRenderer();
};

class YsGLSLShaded3DRenderer : public YsGLSL3DRendererBase
{
public:
	using YsGLSLRenderer::SetUniformColor;

	using YsGLSLRenderer::SetLightDirection;
	using YsGLSLRenderer::SetLightDirectionInCameraCoordinate;
	using YsGLSLRenderer::SetLightColor;
	using YsGLSLRenderer::SetAlphaCutOff;
	using YsGLSLRenderer::SetAmbientLightColor;
	using YsGLSLRenderer::SetSpecularColor;
	using YsGLSLRenderer::SetSpecularExponent;
	using YsGLSLRenderer::SetTextureType;
	using YsGLSLRenderer::SetTextureIdent;
	using YsGLSLRenderer::SetTextureTilingMatrix;
	using YsGLSLRenderer::SetBillboardClippingType;
	using YsGLSLRenderer::SetBillboardTransformationType;
	using YsGLSLRenderer::SetBillboardCenterAndDimension;
	using YsGLSLRenderer::EnableFog;
	using YsGLSLRenderer::DisableFog;
	using YsGLSLRenderer::SetFogDensity;
	using YsGLSLRenderer::SetFogColor;
	using YsGLSLRenderer::EnableZOffset;
	using YsGLSLRenderer::DisableZOffset;
	using YsGLSLRenderer::SetZOffset;

	using YsGLSLRenderer::DrawVtxNomCol;
	using YsGLSLRenderer::DrawVtxTexCoordNomCol;

	// If generic attribute really works, it should be able to take care of the following >>
	using YsGLSLRenderer::DrawVtxNom;
	using YsGLSLRenderer::DrawVtxTexCoordNom;
	// If generic attribute really works, it should be able to take care of the following <<

	using YsGLSLRenderer::SetUniformShadowMapMode;
	using YsGLSLRenderer::SetUniformShadowMapTexture;
	using YsGLSLRenderer::SetUniformShadowMapTransformation;


	YsGLSLShaded3DRenderer();
};

class YsGLSLBillboard3DRenderer : public YsGLSL3DRendererBase
{
public:
	using YsGLSLRenderer::SetUniformColor;
	using YsGLSLRenderer::SetBillboardTransformationType;
	using YsGLSLRenderer::SetTextureIdent;
	using YsGLSLRenderer::SetAlphaCutOff;
	using YsGLSLRenderer::EnableFog;
	using YsGLSLRenderer::DisableFog;
	using YsGLSLRenderer::SetFogDensity;
	using YsGLSLRenderer::SetFogColor;
	using YsGLSLRenderer::EnableZOffset;
	using YsGLSLRenderer::DisableZOffset;
	using YsGLSLRenderer::SetZOffset;
	using YsGLSLRenderer::DrawVtxVoffsetTexCoord;
	using YsGLSLRenderer::DrawVtxColVoffsetTexCoord;

	YsGLSLBillboard3DRenderer();
};

/*! GL_PROGRAM_POINT_SIZE and GL_POINT_SPRITE must be enabled before drawing with this renderer.
    This renderer can only draw GL_POINTS.
*/
class YsGLSLMarker3DRenderer : public YsGLSL3DRendererBase
{
public:
	using YsGLSLRenderer::SetUniformColor;
	using YsGLSLRenderer::SetMarkerType;
	using YsGLSLRenderer::SetMarkerDimension;
	using YsGLSLRenderer::DrawVtx;
	using YsGLSLRenderer::DrawVtxCol;

	YsGLSLMarker3DRenderer();
};

class YsGLSLFlash3DRenderer : public YsGLSL3DRendererBase
{
public:
	// Not supported yet.
	// uniform  MIDP  float angleInterval;
	// uniform  MIDP  float angleOffset;
	// uniform  MIDP  float exponent;
	using YsGLSLRenderer::SetViewportDimension;

	using YsGLSLRenderer::SetPointSizeMode;
	using YsGLSLRenderer::SetBillboardClippingType;
	using YsGLSLRenderer::SetPointSpriteClippingType;

	using YsGLSLRenderer::EnableFog;
	using YsGLSLRenderer::DisableFog;
	using YsGLSLRenderer::SetFogDensity;
	using YsGLSLRenderer::SetFogColor;

	using YsGLSLRenderer::EnableZOffset;
	using YsGLSLRenderer::DisableZOffset;
	using YsGLSLRenderer::SetZOffset;

	using YsGLSLRenderer::DrawVtxColPointSize;

	YsGLSLFlash3DRenderer();
};

/*! Both GL_PROGRAM_POINT_SIZE and GL_POINT_SPRITE must be enabled before drawing with this renderer.
    This renderer can only draw GL_POINTS.
*/
class YsGLSLPointSprite3DRenderer : public YsGLSL3DRendererBase
{
public:
	using YsGLSLRenderer::SetTextureIdent;
	using YsGLSLRenderer::SetViewportDimension;
	using YsGLSLRenderer::SetAlphaCutOff;

	using YsGLSLRenderer::SetUniformColor;

	using YsGLSLRenderer::SetPointSizeMode;
	using YsGLSLRenderer::SetBillboardClippingType;
	using YsGLSLRenderer::SetPointSpriteClippingType;

	using YsGLSLRenderer::SetTextureType;
	using YsGLSLRenderer::SetPointSpriteTextureCoordRange;

	using YsGLSLRenderer::EnableFog;
	using YsGLSLRenderer::DisableFog;
	using YsGLSLRenderer::SetFogDensity;
	using YsGLSLRenderer::SetFogColor;

	using YsGLSLRenderer::EnableZOffset;
	using YsGLSLRenderer::DisableZOffset;
	using YsGLSLRenderer::SetZOffset;

	using YsGLSLRenderer::DrawVtxTexCoordColPointSize;
	using YsGLSLRenderer::DrawVtxTexCoordPointSize;


	YsGLSLPointSprite3DRenderer();
};

class YsGLSLBitmapFontRendererClass : public YsGLSLRenderer
{
public:
	using YsGLSLRenderer::SetProjection;
	using YsGLSLRenderer::SetModelView;
	using YsGLSLRenderer::SetUniformColor;
	using YsGLSLRenderer::SetViewportDimension;
	using YsGLSLRenderer::SetViewportOrigin;
	using YsGLSLRenderer::SetFontFirstLineAlignment;
	using YsGLSLRenderer::RequestFontSize;
	using YsGLSLRenderer::DrawString;

	YsGLSLBitmapFontRendererClass();
};

////////////////////////////////////////////////////////////

class YsGLSL2DRendererBase : protected YsGLSLRenderer
{
public:
	using YsGLSLRenderer::SetUniformPointSize;

	YsGLSL2DRendererBase(struct YsGLSLPlain2DRenderer *r2ptr);

	/*! Call this function to use window-pixel coordinate instead of (-1,-1)-(1,1).
	    The window dimension will be taken from the viewport dimension.
	    Top-left corner of the window will be (0,0).
	*/
	void UseWindowCoordinateTopLeftAsOrigin(void);

	/*! Call this function to use window-pixel coordinate instead of (-1,-1)-(1,1).
	    The window dimension will be taken from the viewport dimension.
	    Bottom-left corner of the window will be (0,0).
	*/
	void UseWindowCoordinateBottomLeftAsOrigin(void);

	/*! Call this function to use coordinate of (-1,-1)-(1,1).
	*/
	void DontUseWindowCoordinate(void);
};

class YsGLSL2DRenderer : public YsGLSL2DRendererBase
{
public:
	using YsGLSLRenderer::DrawVtx;
	using YsGLSLRenderer::DrawVtxCol;
	using YsGLSL2DRendererBase::UseWindowCoordinateTopLeftAsOrigin;
	using YsGLSL2DRendererBase::UseWindowCoordinateBottomLeftAsOrigin;
	using YsGLSL2DRendererBase::DontUseWindowCoordinate;
	using YsGLSLRenderer::SetUniformColor;

	YsGLSL2DRenderer();
};

/* } */
#endif
