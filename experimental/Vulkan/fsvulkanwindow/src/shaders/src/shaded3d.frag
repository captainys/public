#version 450

#define YSVULKAN_TEX_TYPE_NONE                0
#define YSVULKAN_TEX_TYPE_TILING              1
#define YSVULKAN_TEX_TYPE_BILLBOARD           2
#define YSVULKAN_TEX_TYPE_3DTILING            3
#define YSVULKAN_TEX_TYPE_ATTRIBUTE           4
#define YSVULKAN_TEX_BILLBOARD_PERS           0
#define YSVULKAN_TEX_BILLBOARD_ORTHO          1
#define YSVULKAN_TEX_WRAP_CUTOFF              0
#define YSVULKAN_TEX_WRAP_REPEAT              1
#define YSVULKAN_POINTSPRITE_CLIPPING_NONE    0
#define YSVULKAN_POINTSPRITE_CLIPPING_CIRCLE  1
#define YSVULKAN_MARKER_TYPE_PLAIN            0
#define YSVULKAN_MARKER_TYPE_CIRCLE           1
#define YSVULKAN_MARKER_TYPE_RECT             2
#define YSVULKAN_MARKER_TYPE_STAR             3
#define YSVULKAN_MARKER_TYPE_EMPTY_RECT       4
#define YSVULKAN_MARKER_TYPE_EMPTY_CIRCLE     5
#define YSVULKAN_MARKER_TYPE_EMPTY_STAR       6
#define YSVULKAN_MAX_NUM_LIGHT                4
#define YSVULKAN_POINTSPRITE_SIZE_IN_PIXEL    0
#define YSVULKAN_POINTSPRITE_SIZE_IN_3DSPACE  1
#define YSVULKAN_RENDERER_TYPE_NONE           0
#define YSVULKAN_RENDERER_TYPE_PLAIN2D        1
#define YSVULKAN_RENDERER_TYPE_MARKER2D       2
#define YSVULKAN_RENDERER_TYPE_POINTSPRITE2D  3
#define YSVULKAN_RENDERER_TYPE_PLAIN3D        10
#define YSVULKAN_RENDERER_TYPE_SHADED3D       11
#define YSVULKAN_RENDERER_TYPE_FLASH          12
#define YSVULKAN_RENDERER_TYPE_MARKER3D       13
#define YSVULKAN_RENDERER_TYPE_POINTSPRITE3D  14
#define YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE    4
#define YSVULKAN_SHADOWMAP_NONE               0
#define YSVULKAN_SHADOWMAP_USE                1
#define YSVULKAN_SHADOWMAP_DEBUG              2



layout(push_constant) uniform pushConst
{
	mat4 modelView;
};
layout(std140,set=0,binding=0) uniform State
{
	mat4 projection;
	// 16-byte alignment

	vec4 lightEnabled;
	vec4 lightPos[YSVULKAN_MAX_NUM_LIGHT];
	vec4 lightColor[YSVULKAN_MAX_NUM_LIGHT];  // 3D
	vec4 ambientColor;    // 3D
	vec4 specularColor;   // 3D
	// 16-byte alignment
	float specularExponent;

	float alphaCutOff;

	float fogEnabled;
	float fogDensity;
	// 16-byte alignment
	vec4  fogColor;
	// 16-byte alignment

	float zOffset;
	float padding1;

	int   textureType;
	float useTexture;
	// 16-byte alignment
	vec4  textureSampleCoeff;   // 3D

	mat4 textureTileTransform;
	int  billBoardTransformType;
	int  useNegativeOneToPositiveOneTexCoord;
	float padding2;
	float padding3;
	vec4 billBoardCenter;     // 3D
	vec4 billBoardDimension;  // 2D

	vec4 useShadowMap;   // Array of four
	mat4 shadowMapTransform[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE];
	vec4 lightDistScale; // Array of four
	vec4 lightDistOffset; // Array of four
};
layout(set=1,binding=1) uniform sampler2D textureIdent;

layout(set=2,binding=2) uniform sampler2D shadowMapTexture[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE];








layout(location=0) in vec4 colorFromVert;
layout(location=1) in float fogZ;
layout(location=2) in vec3 normalFromVert;
layout(location=3) in vec3 vecToCameraFromVert;
layout(location=4) in vec3 texCoordFromVert;
layout(location=6) in vec3 shadowCoord0;
layout(location=7) in vec3 shadowCoord1;
layout(location=8) in vec3 shadowCoord2;
layout(location=9) in vec3 shadowCoord3;



layout(location=0) out vec4 outColor;

vec3 shadowCoord[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE];


float YsIsInRange( float min, float max, float x)
{
	return step(min,x)*step(x,max);
}

float YsIsInRange( float min, float max, vec2 v)
{
	return step(min,v.x)*step(v.x,max)*step(min,v.y)*step(v.y,max);
}

float YsIsInRange( float min, float max, vec3 v)
{
	return step(min,v.x)*step(v.x,max)*step(min,v.y)*step(v.y,max)*step(min,v.z)*step(v.z,max);
}

// Lighting
//	Input variables: vec4 color
//	                 vec3 nomLocal
//	                 vec3 vecToCamera <- must be normalized
//	Output variable: vec4 accumColor

// lightCoeff can be used for cutting off diffuse and specular reflection for dealing with shadow.
// lightCoeff can be between 0 and 1.
vec4 YsCalculateStandardLighting(in vec4 color,in vec3 nomLocal,in vec3 vecToCamera,in float lightCoeff)
{
	vec3 accumColor=ambientColor.xyz*color.xyz;

	for(int lightNo=0; lightNo<YSVULKAN_MAX_NUM_LIGHT; lightNo++)
	{
		float diffuseIntensity=lightCoeff*max(dot(nomLocal,lightPos[lightNo].xyz),0.0);
		vec3 unitVecToCamera=normalize(vecToCamera);
		vec3 mid=normalize(lightPos[lightNo].xyz+unitVecToCamera);
		float specularIntensity=lightCoeff*pow(max(dot(mid,nomLocal),0.0),specularExponent);

		accumColor+=lightEnabled[lightNo]*diffuseIntensity*(color.xyz*lightColor[lightNo].xyz);
		accumColor+=lightEnabled[lightNo]*specularIntensity*specularColor.xyz;
	}

	return vec4(accumColor,color[3]);
}



void main()
{
	shadowCoord[0]=shadowCoord0;
	shadowCoord[1]=shadowCoord1;
	shadowCoord[2]=shadowCoord2;
	shadowCoord[3]=shadowCoord3;

	vec3 nomLocal=normalize(normalFromVert);
	vec3 vecToCamera=normalize(vecToCameraFromVert);

	float inVolume[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE];
	inVolume[0]=YsIsInRange(0.001,0.999,shadowCoord[0]);
	inVolume[1]=YsIsInRange(0.001,0.999,shadowCoord[1])*(1.0-inVolume[0]);
	inVolume[2]=YsIsInRange(0.001,0.999,shadowCoord[2])*(1.0-inVolume[0])*(1.0-inVolume[1]);

	// useShadowMap==0 || inVolume==0.0 -> inShadow=0.0
	// useShadowMap==1 && inVolume==1.0 -> inShadow=step(shadowDepth,shadowCoordTfm.z);
	float inShadow[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE];
	for(int i=0; i<YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE; ++i)
	{
		inShadow[i]=0.0;

		// This if-statement improved the frame rate from 38+ to 55+ in ThinkPad X250.
		// I have a serious doubt about the suggestion that if-statement should be avoided all together in fragment shaders.
		if(0.999<inVolume[i])
		{
			float shadowDepth=texture(shadowMapTexture[i],shadowCoord[i].xy).r;
			inShadow[i]=useShadowMap[i]*inVolume[i]*step(shadowDepth,shadowCoord[i].z);
		}
	}

	// If either one of inShadow0 or inShadow1 is 1, specular and diffuse must be off (lightCoeff=0)
	float lightCoeff=(1.0-inShadow[0])*(1.0-inShadow[1])*(1.0-inShadow[2]);

	vec4 accumColor=YsCalculateStandardLighting(colorFromVert,nomLocal,vecToCamera,lightCoeff);
	outColor=accumColor;

	{
		vec4 texcell[3],avg;
		texcell[0]=texture(textureIdent,texCoordFromVert.xy);
		texcell[1]=texture(textureIdent,texCoordFromVert.xz);
		texcell[2]=texture(textureIdent,texCoordFromVert.yz);
		avg=useTexture*(texcell[0]*textureSampleCoeff[0]+texcell[1]*textureSampleCoeff[1]+texcell[2]*textureSampleCoeff[2])
		    +vec4(1.0-useTexture,1.0-useTexture,1.0-useTexture,1.0-useTexture);
		outColor=outColor*avg;
	}

	if(outColor.a<alphaCutOff)
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
		float d=fogDensity*abs(fogZ);
		float f=clamp(exp(-d*d),0.0,1.0);
		vec3  fogMix=mix(fogColor.rgb,outColor.rgb,f);
		outColor.rgb=fogMix;
	}
}
