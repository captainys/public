#include "YS_GLSL_HEADER.glsl"

uniform  HIGHP mat4 modelView;

varying  HIGHP  vec3 normalOut;
varying  HIGHP  vec3 vecToCameraOut;
varying  LOWP  vec4 color;

#include "YS_GLSL_UTIL.glsl"

#include "YS_GLSL_TEXTURE_VARIABLE.glsl"
#include "YS_GLSL_ALPHA_CUTOFF_PARAMETER.glsl"
#include "YS_GLSL_FOG_VARIABLES.glsl"

#include "YS_GLSL_SHADOWMAP_UNIFORM.glsl"
#include "YS_GLSL_SHADOWMAP_VARYING.glsl"

#include "YS_GLSL_STANDARD_LIGHTING.glsl"

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

	#include "YS_GLSL_TEXTURE_IN_FRAGMENTSHADER.glsl"
	#include "YS_GLSL_ALPHA_CUTOFF_IN_FRAGMENT_SHADER.glsl"
	#include "YS_GLSL_FOGCALCULATION_IN_FRAGMENTSHADER.glsl"
}
