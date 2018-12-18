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


varying  LOWP  vec4 color;

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

// Variables for fog
uniform  MIDP  float fogEnabled;
uniform  MIDP  float fogDensity;
uniform  LOWP  vec4  fogColor;
varying  HIGHP float fogZ;

// Variables for alpha cutoff
uniform  LOWP  float alphaCutOff;


void main()
{
	gl_FragColor=color;

	{
		LOWP  vec4 texcell[3],avg;
		texcell[0]=texture2D(textureIdent,texCoordOut.xy);
		// Who said if-statement should be avoided?
		// Sampling twice seems to cost a lot more than a if-statement in ThinkPad X250 and 2014 Mac mini.
		if(0.0<textureSampleCoeff[1]+textureSampleCoeff[2])
		{
			texcell[1]=texture2D(textureIdent,texCoordOut.xz);
			texcell[2]=texture2D(textureIdent,texCoordOut.yz);
			avg=useTexture*(texcell[0]*textureSampleCoeff[0]+texcell[1]*textureSampleCoeff[1]+texcell[2]*textureSampleCoeff[2])
			   +vec4(1.0-useTexture,1.0-useTexture,1.0-useTexture,1.0-useTexture);
		}
		else
		{
			avg=useTexture*(texcell[0]*textureSampleCoeff[0])+vec4(1.0-useTexture,1.0-useTexture,1.0-useTexture,1.0-useTexture);
		}
		gl_FragColor=gl_FragColor*avg;
	}

	if(gl_FragColor.a<alphaCutOff)
	{
		discard;
	}

	// f  0:Completely fogged out   1:Clear
	// f=e^(-d*d)
	// d  0:Clear      Infinity: Completely fogged out
	// 99% fogged out means:  e^(-d*d)=0.01  What's d?
	// -d*d=loge(0.01)
	// -d*d= -4.60517
	// d=2.146
	// If visibility=V, d=2.146 at fogZ=V -> fogDensity=2.146/V
	{
		MIDP  float d=fogDensity*abs(fogZ);
		MIDP  float f=clamp(exp(-d*d),0.0,1.0);
		LOWP  vec3  fogMix=mix(fogColor.rgb,gl_FragColor.rgb,f);
		gl_FragColor.rgb=fogMix;
	}

}
