#define YSGLSL_TEX_TYPE_NONE                0
#define YSGLSL_TEX_TYPE_TILING              1
#define YSGLSL_TEX_TYPE_BILLBOARD           2
#define YSGLSL_TEX_TYPE_3DTILING            3
#define YSGLSL_TEX_TYPE_ATTRIBUTE           4
#define YSGLSL_TEX_BILLBOARD_PERS           0
#define YSGLSL_TEX_BILLBOARD_ORTHO          1
#define YSGLSL_TEX_WRAP_CUTOFF              0
#define YSGLSL_TEX_WRAP_REPEAT              1
#define YSGLSL_POINTSPRITE_CLIPPING_NONE      0
#define YSGLSL_POINTSPRITE_CLIPPING_CIRCLE    1
#define YSGLSL_BILLBOARD_CLIPPING_NONE      YSGLSL_POINTSPRITE_CLIPPING_NONE
#define YSGLSL_BILLBOARD_CLIPPING_CIRCLE    YSGLSL_POINTSPRITE_CLIPPING_CIRCLE
#define YSGLSL_MARKER_TYPE_PLAIN            0
#define YSGLSL_MARKER_TYPE_CIRCLE           1
#define YSGLSL_MARKER_TYPE_RECT             2
#define YSGLSL_MARKER_TYPE_STAR             3
#define YSGLSL_MARKER_TYPE_EMPTY_RECT       4
#define YSGLSL_MARKER_TYPE_EMPTY_CIRCLE     5
#define YSGLSL_MARKER_TYPE_EMPTY_STAR       6
#define YSGLSL_MAX_NUM_LIGHT                8
#define YSGLSL_POINTSPRITE_SIZE_IN_PIXEL    0
#define YSGLSL_POINTSPRITE_SIZE_IN_3DSPACE  1
#define YSGLSL_RENDERER_TYPE_NONE           0
#define YSGLSL_RENDERER_TYPE_PLAIN2D        1
#define YSGLSL_RENDERER_TYPE_MARKER2D       2
#define YSGLSL_RENDERER_TYPE_POINTSPRITE2D  3
#define YSGLSL_RENDERER_TYPE_PLAIN3D        10
#define YSGLSL_RENDERER_TYPE_SHADED3D       11
#define YSGLSL_RENDERER_TYPE_FLASH          12
#define YSGLSL_RENDERER_TYPE_MARKER3D       13
#define YSGLSL_RENDERER_TYPE_POINTSPRITE3D  14
#define YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE    3
#define YSGLSL_SHADOWMAP_NONE               0
#define YSGLSL_SHADOWMAP_USE                1
#define YSGLSL_SHADOWMAP_DEBUG              2

#ifdef GL_ES
	#define LOWP lowp
	#define MIDP mediump
	#define HIGHP highp
#else
	#define LOWP
	#define MIDP
	#define HIGHP
#endif


uniform  HIGHP  mat4  projection;
uniform  HIGHP  mat4  modelView;
uniform sampler2D textureIdent;
uniform  MIDP  float viewportWid,viewportHei;

attribute  HIGHP  vec3 vertex;
attribute  LOWP  vec4 colorIn;
attribute  LOWP  vec2 texCoordIn;
attribute  MIDP  float pointSizeIn;

varying  LOWP  vec4 color;
varying  MIDP  vec2 texCoordOut;

// Variables for fog
uniform  MIDP  float fogEnabled;
uniform  MIDP  float fogDensity;
uniform  LOWP  vec4  fogColor;
varying  HIGHP float fogZ;

// Variables for z-offset
uniform bool useZOffset;
uniform  MIDP  float zOffset;

// Variables for POINT_SPRITE
uniform LOWP  float usePointSizeInPixel;
uniform LOWP  float usePointSizeIn3D;
uniform LOWP  float usePointClippingCircle;
uniform LOWP  float useTexture;
uniform LOWP  float texCoordRange;




void Ys_ProcessPointSprite()
{
	HIGHP vec4 prj1=projection*modelView*vec4(vertex,1.0);
	HIGHP vec4 prj2=modelView*vec4(vertex,1.0);
	prj2.y+=pointSizeIn;
	prj2=projection*prj2;

	HIGHP float pointSize3D=viewportHei*abs(prj2[1]/prj2[3]-prj1[1]/prj1[3])/2.0;

	gl_PointSize=pointSizeIn*usePointSizeInPixel+pointSize3D*usePointSizeIn3D;
}


void main()
{
	 HIGHP  vec4 vertexInView=modelView*vec4(vertex,1.0);

	texCoordOut=texCoordIn;
	color=colorIn;

	Ys_ProcessPointSprite();

		fogZ=-vertexInView.z*fogEnabled;

	gl_Position=projection*vertexInView;

	if(true==useZOffset)
	{
		gl_Position.z+=zOffset*gl_Position[3];
	}

}
