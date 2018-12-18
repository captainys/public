#include "YS_GLSL_HEADER.glsl"

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

#include "YS_GLSL_FOG_VARIABLES.glsl"
#include "YS_GLSL_ZOFFSET_VARIABLE.glsl"

void main()
{
	#include "YS_GLSL_VIEW_TRANSFORM_IN_VERTEX_SHADER.glsl"

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

	#include "YS_GLSL_FOG_CALCULATION_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_PROJECTION_TRANSFORM_IN_VERTEX_SHADER.glsl"
	#include "YS_GLSL_ZOFFSET_IN_VERTEXSHADER.glsl"
}
