#include "YS_GLSL_HEADER.glsl"

uniform  HIGHP mat4 projection;
uniform  HIGHP mat4 modelView;

attribute  HIGHP  vec3 vertex;
attribute  HIGHP  vec3 normal;
attribute  LOWP   vec4 color;    // <- Only difference from monoColorPerVtxShader
attribute  HIGHP  vec2 texCoord;

varying  LOWP  vec4 colorOut;

#include "YS_GLSL_TEXTURE_VARIABLE.glsl"
#include "YS_GLSL_ZOFFSET_VARIABLE.glsl"
#include "YS_GLSL_FOG_VARIABLES.glsl"
#include "YS_GLSL_STANDARD_LIGHTING.glsl"

void main()
{
	#include "YS_GLSL_VIEW_TRANSFORM_IN_VERTEX_SHADER.glsl"
	#include "YS_GLSL_PROJECTION_TRANSFORM_IN_VERTEX_SHADER.glsl"

	HIGHP vec3 nomLocal=normalize((modelView*vec4(normal,0)).xyz);
	HIGHP vec3 vecToCamera=-normalize(vertexInView.xyz);

	colorOut=YsCalculateStandardLighting(color,nomLocal,vecToCamera,1.0);

	#include "YS_GLSL_TEXTURE_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_FOG_CALCULATION_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_ZOFFSET_IN_VERTEXSHADER.glsl"
}
