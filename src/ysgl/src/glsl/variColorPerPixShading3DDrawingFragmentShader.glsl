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


uniform  HIGHP mat4 modelView;

varying  HIGHP  vec3 normalOut;
varying  HIGHP  vec3 vecToCameraOut;
varying  LOWP  vec4 color;


LOWP vec4 YsGLSLRainbowColor(HIGHP float t)
{
	// 0     0.25  0.5    0.75    1
	// Blue->Cyan->Green->Yellow->Red

	HIGHP float tt=t*4.0;
	LOWP  float blue =clamp(2.0-tt         ,0.0,1.0);
	LOWP  float green=clamp(2.0-abs(tt-2.0),0.0,1.0);
	LOWP  float red  =clamp(tt-2.0         ,0.0,1.0);
	return vec4(red,green,blue,1.0);

	// HIGHP float tt;
	// if(t<0.0)
	// {
	// 	return vec4(0,0,1,1);
	// }
	// else if(t<0.25)
	// {
	// 	tt=t/0.25;
	// 	return vec4(0,tt,1,1);
	// }
	// else if(t<0.5)
	// {
	// 	tt=(t-0.25)/0.25;
	// 	return vec4(0,1,1.0-tt,1);
	// }
	// else if(t<0.75)
	// {
	// 	tt=(t-0.5)/0.25;
	// 	return vec4(tt,1,0,1);
	// }
	// else if(t<1.0)
	// {
	// 	tt=(t-0.75)/0.25;
	// 	return vec4(1,1.0-tt,0,1);
	// }
	// else
	// {
	// 	return vec4(1,0,0,1);
	// }
}

MIDP float YsIsInRange(HIGHP float min,HIGHP float max,HIGHP float x)
{
	return step(min,x)*step(x,max);
}

MIDP float YsIsInRange(HIGHP float min,HIGHP float max,HIGHP vec2 v)
{
	return step(min,v.x)*step(v.x,max)
          *step(min,v.y)*step(v.y,max);
}

MIDP float YsIsInRange(HIGHP float min,HIGHP float max,HIGHP vec3 v)
{
	return step(min,v.x)*step(v.x,max)
          *step(min,v.y)*step(v.y,max)
          *step(min,v.z)*step(v.z,max);
}


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

// Variables for alpha cutoff
uniform  LOWP  float alphaCutOff;

// Variables for fog
uniform  MIDP  float fogEnabled;
uniform  MIDP  float fogDensity;
uniform  LOWP  vec4  fogColor;
varying  HIGHP float fogZ;


uniform MIDP float useShadowMap[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
uniform MIDP float useShadowTest[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
uniform HIGHP mat4 shadowMapTransform[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
uniform sampler2D shadowMapTexture[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
uniform MIDP float lightDistScale[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
uniform MIDP float lightDistOffset[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];

varying HIGHP vec3 shadowCoord[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];


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


// #define ENABLE_SHADOW_TEST

void main()
{
	HIGHP vec3 nomLocal=normalize(normalOut);
	HIGHP vec3 vecToCamera=normalize(vecToCameraOut);

	MIDP  float inVolume[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
	inVolume[0]=YsIsInRange(0.001,0.999,shadowCoord[0]);
	inVolume[1]=YsIsInRange(0.001,0.999,shadowCoord[1])*(1.0-inVolume[0]);
#if 3<=YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE
	inVolume[2]=YsIsInRange(0.001,0.999,shadowCoord[2])*(1.0-inVolume[0])*(1.0-inVolume[1]);
#endif

	// useShadowMap==0 || inVolume==0.0 -> inShadow=0.0
	// useShadowMap==1 && inVolume==1.0 -> inShadow=step(shadowDepth,shadowCoordTfm.z);
	MIDP  float inShadow[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
#ifdef ENABLE_SHADOW_TEST
	LOWP  vec4 shadowTestColor[YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE];
#endif
	for(int i=0; i<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE; ++i)
	{
		inShadow[i]=0.0;

		// This if-statement improved the frame rate from 38+ to 55+ in ThinkPad X250.
		// I have a serious doubt about the suggestion that if-statement should be avoided all together in fragment shaders.
		if(0.999<inVolume[i])
		{
			HIGHP float shadowDepth=texture2D(shadowMapTexture[i],shadowCoord[i].xy).r;
			inShadow[i]=useShadowMap[i]*inVolume[i]*step(shadowDepth,shadowCoord[i].z);
		#ifdef ENABLE_SHADOW_TEST
			shadowTestColor[i]=inVolume[i]*YsGLSLRainbowColor(shadowDepth)+vec4(1,0,1,1)*(1.0-inVolume[i]);
		#endif
		}
	}

	// If either one of inShadow0 or inShadow1 is 1, specular and diffuse must be off (lightCoeff=0)
#if 3<=YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE
	MIDP  float lightCoeff=(1.0-inShadow[0])*(1.0-inShadow[1])*(1.0-inShadow[2]);
#elif 2<=YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE
	MIDP  float lightCoeff=(1.0-inShadow[0])*(1.0-inShadow[1]);
#endif

	LOWP  vec4 accumColor=YsCalculateStandardLighting(color,nomLocal,vecToCamera,lightCoeff);


#ifdef ENABLE_SHADOW_TEST
	// Shadow Map Test >>
#if 3<=YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE
	accumColor=accumColor*(1.0-inVolume[0]*useShadowTest[0])*(1.0-inVolume[1]*useShadowTest[1])*(1.0-inVolume[2]*useShadowTest[2])
	           +shadowTestColor[0]*inVolume[0]*useShadowTest[0]
	           +shadowTestColor[1]*inVolume[1]*useShadowTest[1]
	           +shadowTestColor[2]*inVolume[2]*useShadowTest[2];
#else
	accumColor=accumColor*(1.0-inVolume[0]*useShadowTest[0])*(1.0-inVolume[1]*useShadowTest[1])
	           +shadowTestColor[0]*inVolume[0]*useShadowTest[0]
	           +shadowTestColor[1]*inVolume[1]*useShadowTest[1];
#endif
	// 2016/10/20 Looks like if I modify gl_FragColor in this timing, the whole thing blows up. (In Windows driver, Intel GPU only, seems to be)
	// Shadow Map Test  <<
#endif

	gl_FragColor=accumColor;

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
