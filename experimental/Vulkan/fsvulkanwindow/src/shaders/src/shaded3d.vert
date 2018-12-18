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





out gl_PerVertex 
{
    vec4 gl_Position;
};
layout(location=0) out vec4 colorFromVert;
layout(location=1) out float fogZ;
layout(location=2) out vec3 normalFromVert;
layout(location=3) out vec3 vecToCameraFromVert;
layout(location=4) out vec3 texCoordFromVert;
layout(location=6) out vec3 shadowCoord0;
layout(location=7) out vec3 shadowCoord1;
layout(location=8) out vec3 shadowCoord2;
layout(location=9) out vec3 shadowCoord3;




layout(location=0) in vec3 vertexIn;
layout(location=1) in vec4 colorIn;
layout(location=2) in vec3 normalIn;
layout(location=3) in vec2 texCoordIn;



vec3 MakeShadowPos(vec4 inShadowPos)
{
	vec3 shadowCoord=inShadowPos.xyz/inShadowPos.w;
	shadowCoord.xy=(vec2(1,1)+shadowCoord.xy)/2.0; // Make UV 0.0 to 1.0
	return shadowCoord;
}



void main()
{
	vec4 vertexInView=modelView*vec4(vertexIn,1.0);

	gl_Position=projection*vertexInView;

	vec4 normal4d=vec4(normalIn,0);
	normalFromVert=normalize((modelView*normal4d).xyz);
	vecToCameraFromVert=-normalize(vertexInView.xyz);

	colorFromVert=colorIn;


	// Shadow Map >>
	vec4 inShadowPos0=shadowMapTransform[0]*vertexInView;
	shadowCoord0=MakeShadowPos(inShadowPos0);
	shadowCoord0.z/=lightDistScale[0];
	shadowCoord0.z-=lightDistOffset[0];

	vec4 inShadowPos1=shadowMapTransform[1]*vertexInView;
	shadowCoord1=MakeShadowPos(inShadowPos1);
	shadowCoord1.z/=lightDistScale[1];
	shadowCoord1.z-=lightDistOffset[1];

	vec4 inShadowPos2=shadowMapTransform[2]*vertexInView;
	shadowCoord2=MakeShadowPos(inShadowPos2);
	shadowCoord2.z/=lightDistScale[2];
	shadowCoord2.z-=lightDistOffset[2];

	vec4 inShadowPos3=shadowMapTransform[3]*vertexInView;
	shadowCoord3=MakeShadowPos(inShadowPos3);
	shadowCoord3=(vec3(1,1,1)+(inShadowPos3.xyz/inShadowPos3.w))/2.0;
	shadowCoord3.z/=lightDistScale[3];
	shadowCoord3.z-=lightDistOffset[3];
	// Shadow Map <<



	//   In orthographic projection, billBoardOffset can simply be calculated by taking XY components of
	//   billBoardOffsetInView-modelView*(vec4(billBoardCenter,1.0));
	//   However, in perspective projection, a plane perpendicular to the viewing vector must be a reference plane,
	//   and therefore heading and pitch angles must be calculated and taken into account.
	//   Currently, this GLSL program only supports billboading in perspective projection.
	
	if(textureType==YSVULKAN_TEX_TYPE_BILLBOARD)
	{
		if(YSVULKAN_TEX_BILLBOARD_ORTHO==billBoardTransformType)
		{
			vec4 billBoardCenterInView=modelView*vec4(billBoardCenter.xyz,1.0);
			vec2 billBoardOffset=(vertexInView-billBoardCenterInView).xy;
			texCoordFromVert=vec3(billBoardOffset/billBoardDimension.xy,0.0);
		}
		else
		{
			vec4 billBoardCenterInView=modelView*vec4(billBoardCenter.xyz,1.0);
			vec3 viewVec=normalize(billBoardCenterInView.xyz);
			float pitch=asin(viewVec.y);
			float heading=-asin(viewVec.x);
			vec3 uVec=vec3(cos(heading),0.0,-sin(heading));
			vec3 vVec=vec3(-sin(pitch)*sin(heading),cos(pitch),sin(pitch)*cos(heading));
			vec3 billBoardOffset3d=(vertexInView-billBoardCenterInView).xyz;
			vec2 billBoardOffset=vec2(dot(billBoardOffset3d,uVec),dot(billBoardOffset3d,vVec));
			texCoordFromVert=vec3(billBoardOffset/billBoardDimension.xy,0.0);
		}
		if(0!=useNegativeOneToPositiveOneTexCoord)
		{
			texCoordFromVert=(texCoordFromVert+vec3(1.0,1.0,1.0))/vec3(2.0,2.0,2.0);
		}
	}
	else if(textureType==YSVULKAN_TEX_TYPE_TILING || textureType==YSVULKAN_TEX_TYPE_3DTILING)
	{
		vec4 texCoordTfm=textureTileTransform*vec4(vertexIn,1.0);
		texCoordFromVert=texCoordTfm.xyz;
		if(0!=useNegativeOneToPositiveOneTexCoord)
		{
			texCoordFromVert=(texCoordFromVert+vec3(1.0,1.0,1.0))/vec3(2.0,2.0,2.0);
		}
	}
	else if(textureType==YSVULKAN_TEX_TYPE_ATTRIBUTE)
	{
		texCoordFromVert=vec3(texCoordIn,0.0);
	}
	else
	{
		texCoordFromVert[0]=0.0;
		texCoordFromVert[1]=0.0;
		texCoordFromVert[2]=0.0;
	}

	fogZ=-vertexInView.z*fogEnabled;
	gl_Position.z+=zOffset*gl_Position[3];
}
