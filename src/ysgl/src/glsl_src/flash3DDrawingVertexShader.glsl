#include "YS_GLSL_HEADER.glsl"

uniform  HIGHP  mat4  projection;
uniform  HIGHP  mat4  modelView;
uniform  MIDP  float dimension;
uniform  MIDP  float radius1;
uniform  MIDP  float radius2;
uniform  MIDP  float maxIntensity;
uniform  MIDP  float angleInterval;
uniform  MIDP  float angleOffset;
uniform  MIDP  float exponent;

attribute  HIGHP  vec3 vertex;
attribute  LOWP  vec4 colorIn;
attribute  HIGHP  vec3 viewOffset;

varying  LOWP  vec2 uv;
varying  LOWP  vec4 color;

#include "YS_GLSL_ZOFFSET_VARIABLE.glsl"
#include "YS_GLSL_FOG_VARIABLES.glsl"

void main()
{
	#include "YS_GLSL_VIEW_TRANSFORM_IN_VERTEX_SHADER.glsl"

	vec3 offset=viewOffset*dimension;
	vertexInView.x+=offset.x;
	vertexInView.y+=offset.y;
	vertexInView.z+=offset.z;

	#include "YS_GLSL_FOG_CALCULATION_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_PROJECTION_TRANSFORM_IN_VERTEX_SHADER.glsl"

	uv=vec2(viewOffset.x,viewOffset.y);
	color=colorIn;

	#include "YS_GLSL_ZOFFSET_IN_VERTEXSHADER.glsl"
}
