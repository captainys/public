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






layout(location=0) in vec3 vertexIn;
layout(location=1) in vec4 colorIn;
layout(location=2) in vec2 texCoordIn;
layout(location=3) in float pointSizeIn;



out gl_PerVertex 
{
    vec4 gl_Position;
	float gl_PointSize;
};
layout(location=0) out float fogZFromVert;
layout(location=1) out vec4 colorFromVert;
layout(location=2) out vec2 texCoordFromVert;





void Ys_ProcessPointSprite()
{
	vec4 prj1=projection*modelView*vec4(vertexIn,1.0);
	vec4 prj2=           modelView*vec4(vertexIn,1.0);
	prj2.y+=pointSizeIn;
	prj2=projection*prj2;

	float pointSize3D=viewportHei*abs(prj2[1]/prj2[3]-prj1[1]/prj1[3])/2.0;

	gl_PointSize=pointSizeIn*usePointSizeInPixel+pointSize3D*usePointSizeIn3D;
}


void main()
{
	vec4 vertexInView=modelView*vec4(vertexIn,1.0);

	texCoordFromVert=texCoordIn;
	colorFromVert=colorIn;

	Ys_ProcessPointSprite();

	fogZFromVert=-vertexInView.z*fogEnabled;

	gl_Position=projection*vertexInView;
	gl_Position.z+=zOffset*gl_Position[3];

}
