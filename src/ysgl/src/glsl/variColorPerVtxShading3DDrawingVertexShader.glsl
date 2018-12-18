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


uniform  HIGHP mat4 projection;
uniform  HIGHP mat4 modelView;

attribute  HIGHP  vec3 vertex;
attribute  HIGHP  vec3 normal;
attribute  LOWP   vec4 color;    // <- Only difference from monoColorPerVtxShader
attribute  HIGHP  vec2 texCoord;

varying  LOWP  vec4 colorOut;

// Variables for texturing
uniform LOWP  int   textureType;
uniform LOWP  float useTexture;
uniform LOWP  vec3  textureSampleCoeff;

uniform LOWP  mat4 textureTileTransform;
uniform LOWP  int  billBoardTransformType;
uniform       bool useNegativeOneToPositiveOneTexCoord;
uniform HIGHP vec3 billBoardCenter;
uniform HIGHP vec2 billBoardDimension;
uniform sampler2D  textureIdent;
varying HIGHP vec3 texCoordOut;

// Variables for z-offset
uniform bool useZOffset;
uniform  MIDP  float zOffset;

// Variables for fog
uniform  MIDP  float fogEnabled;
uniform  MIDP  float fogDensity;
uniform  LOWP  vec4  fogColor;
varying  HIGHP float fogZ;

uniform MIDP float lightEnabled[YSGLSL_MAX_NUM_LIGHT];
uniform HIGHP vec4 lightPos[YSGLSL_MAX_NUM_LIGHT];
uniform LOWP vec3 lightColor[YSGLSL_MAX_NUM_LIGHT];
uniform LOWP vec3 specularColor;
uniform MIDP float specularExponent;
uniform LOWP vec3 ambientColor;

// Lighting
//	Input variables: vec4 color
//	                 vec3 nomLocal
//	                 vec3 vecToCamera <- must be normalized
//	Output variable: vec4 accumColor

// lightCoeff can be used for cutting off diffuse and specular reflection for dealing with shadow.
// lightCoeff can be between 0 and 1.
LOWP vec4 YsCalculateStandardLighting(
    in LOWP vec4 color,in HIGHP vec3 nomLocal,in HIGHP vec3 vecToCamera,in MIDP float lightCoeff)
{
	LOWP vec3 accumColor=ambientColor*color.xyz;

	for(int lightNo=0; lightNo<YSGLSL_MAX_NUM_LIGHT; lightNo++)
	{
		MIDP float diffuseIntensity=lightCoeff*max(dot(nomLocal,lightPos[lightNo].xyz),0.0);
		HIGHP vec3 unitVecToCamera=normalize(vecToCamera);
		HIGHP vec3 mid=normalize(lightPos[lightNo].xyz+unitVecToCamera);
		HIGHP float specularIntensity=lightCoeff*pow(max(dot(mid,nomLocal),0.0),specularExponent);

		accumColor+=lightEnabled[lightNo]*diffuseIntensity*(color.xyz*lightColor[lightNo]);
		accumColor+=lightEnabled[lightNo]*specularIntensity*specularColor;
	}

	return vec4(accumColor,color[3]);
}


void main()
{
	 HIGHP  vec4 vertexInView=modelView*vec4(vertex,1.0);

	gl_Position=projection*vertexInView;


	HIGHP vec3 nomLocal=normalize((modelView*vec4(normal,0)).xyz);
	HIGHP vec3 vecToCamera=-normalize(vertexInView.xyz);

	colorOut=YsCalculateStandardLighting(color,nomLocal,vecToCamera,1.0);

	//   In orthographic projection, billBoardOffset can simply be calculated by taking XY components of
	//   billBoardOffsetInView-modelView*(vec4(billBoardCenter,1.0));
	//   However, in perspective projection, a plane perpendicular to the viewing vector must be a reference plane,
	//   and therefore heading and pitch angles must be calculated and taken into account.
	//   Currently, this GLSL program only supports billboading in perspective projection.
	
	if(textureType==YSGLSL_TEX_TYPE_BILLBOARD)
	{
		if(YSGLSL_TEX_BILLBOARD_ORTHO==billBoardTransformType)
		{
			 HIGHP  vec4 billBoardCenterInView=modelView*vec4(billBoardCenter,1.0);
			 MIDP  vec2 billBoardOffset=(vertexInView-billBoardCenterInView).xy;
			texCoordOut=vec3(billBoardOffset/billBoardDimension,0.0);
		}
		else
		{
			 HIGHP  vec4 billBoardCenterInView=modelView*vec4(billBoardCenter,1.0);
			 MIDP  vec3 viewVec=normalize(billBoardCenterInView.xyz);
			 MIDP  float pitch=asin(viewVec.y);
			 MIDP  float heading=-asin(viewVec.x);
			 MIDP  vec3 uVec=vec3(cos(heading),0.0,-sin(heading));
			 MIDP  vec3 vVec=vec3(-sin(pitch)*sin(heading),cos(pitch),sin(pitch)*cos(heading));
			 HIGHP  vec3 billBoardOffset3d=(vertexInView-billBoardCenterInView).xyz;
			 HIGHP  vec2 billBoardOffset=vec2(dot(billBoardOffset3d,uVec),dot(billBoardOffset3d,vVec));
			texCoordOut=vec3(billBoardOffset/billBoardDimension,0.0);
		}
		if(true==useNegativeOneToPositiveOneTexCoord)
		{
			texCoordOut=(texCoordOut+vec3(1.0,1.0,1.0))/vec3(2.0,2.0,2.0);
		}
	}
	else if(textureType==YSGLSL_TEX_TYPE_TILING || textureType==YSGLSL_TEX_TYPE_3DTILING)
	{
		 LOWP  vec4 texCoordTfm=textureTileTransform*vec4(vertex,1.0);
		texCoordOut=texCoordTfm.xyz;
		if(true==useNegativeOneToPositiveOneTexCoord)
		{
			texCoordOut=(texCoordOut+vec3(1.0,1.0,1.0))/vec3(2.0,2.0,2.0);
		}
	}
	else if(textureType==YSGLSL_TEX_TYPE_ATTRIBUTE)
	{
		texCoordOut=vec3(texCoord,0.0);
	}
	else
	{
		texCoordOut[0]=0.0;
		texCoordOut[1]=0.0;
		texCoordOut[2]=0.0;
	}

		fogZ=-vertexInView.z*fogEnabled;

	if(true==useZOffset)
	{
		gl_Position.z+=zOffset*gl_Position[3];
	}

}
