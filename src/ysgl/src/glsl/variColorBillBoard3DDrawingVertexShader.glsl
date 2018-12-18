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
uniform LOWP int billBoardTransformType;
uniform bool useNegativeOneToPositiveOneTexCoord;
uniform sampler2D textureIdent;

attribute  HIGHP  vec3 vertex;
attribute  LOWP  vec4 colorIn;
attribute  MIDP  vec3 viewOffset;
attribute  MIDP  vec2 texCoord;

varying  HIGHP  vec2 texCoordOut;
varying  LOWP  vec4 color;

// Variables for fog
uniform  MIDP  float fogEnabled;
uniform  MIDP  float fogDensity;
uniform  LOWP  vec4  fogColor;
varying  HIGHP float fogZ;

// Variables for z-offset
uniform bool useZOffset;
uniform  MIDP  float zOffset;


void main()
{
	 HIGHP  vec4 vertexInView=modelView*vec4(vertex,1.0);


	if(YSGLSL_TEX_BILLBOARD_ORTHO==billBoardTransformType)
	{
		vertexInView.xyz+=viewOffset;
	}
	else
	{
		vec3 axis[3];
		 MIDP  vec3 viewVec=normalize(vertexInView.xyz);
		 MIDP  float pitch=asin(viewVec.y);
		 MIDP  float heading=-asin(viewVec.x);
		axis[0]=vec3(cos(heading),0.0,-sin(heading));
		axis[1]=vec3(-sin(pitch)*sin(heading),cos(pitch),sin(pitch)*cos(heading));
		axis[2]=-viewVec;
		vertexInView.xyz+=(axis[0]*viewOffset[0]+axis[1]*viewOffset[1]+axis[2]*viewOffset[2]);
	}
	texCoordOut=texCoord;
	if(true==useNegativeOneToPositiveOneTexCoord)
	{
		texCoordOut=(texCoordOut+vec2(1.0,1.0))/vec2(2.0,2.0);
	}
	color=colorIn;

		fogZ=-vertexInView.z*fogEnabled;

	gl_Position=projection*vertexInView;

	if(true==useZOffset)
	{
		gl_Position.z+=zOffset*gl_Position[3];
	}

}
