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



layout(std140,set=0,binding=0) uniform State
{
	mat4 projection;

	float viewportWid;			// 16-byte alignment
	float viewportHei;

	float alphaCutOff;

	float zOffset;

	vec4  fogColor;				// 16-byte alignment
	float fogEnabled;			// 16-byte alignment
	float fogDensity;

	int   textureType;
	float useTexture;

	float usePointSizeInPixel;	// 16-byte alignment
	float usePointSizeIn3D;
	float usePointClippingCircle;
	float texCoordRange;
};
layout(set=1,binding=1) uniform sampler2D textureIdent;



layout(location=0) in float fogZFromVert;
layout(location=1) in vec4 colorFromVert;
layout(location=2) in vec2 texCoordFromVert;



layout(location=0) out vec4 outColor;



void main()
{
	vec2 t=(gl_PointCoord.xy*2.0)-vec2(1.0,1.0);
	if(1.0<=usePointClippingCircle*dot(t,t))
	{
		discard;
	}

	vec2 texCoord=texCoordFromVert+gl_PointCoord*texCoordRange;
	vec4 texcell=texture(textureIdent,texCoord);
	outColor=useTexture*colorFromVert*texcell+(1.0-useTexture)*colorFromVert;

	// outColor.rg=gl_PointCoord;  For debugging.

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
		float d=fogDensity*abs(fogZFromVert);
		float f=clamp(exp(-d*d),0.0,1.0);
		vec3  fogMix=mix(fogColor.rgb,outColor.rgb,f);
		outColor.rgb=fogMix;
	}

}
