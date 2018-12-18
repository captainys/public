#include "YS_GLSL_HEADER.glsl"

uniform  HIGHP  mat4  projection;
uniform  HIGHP  mat4  modelView;

uniform LOWP int markerType;
uniform MIDP float dimension;

attribute HIGHP vec3 vertex;
attribute LOWP vec4 colorIn;

varying LOWP vec4 color;

#include "YS_GLSL_FOG_VARIABLES.glsl"
#include "YS_GLSL_ZOFFSET_VARIABLE.glsl"

void main()
{
	#include "YS_GLSL_VIEW_TRANSFORM_IN_VERTEX_SHADER.glsl"
	#include "YS_GLSL_FOG_CALCULATION_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_PROJECTION_TRANSFORM_IN_VERTEX_SHADER.glsl"
	#include "YS_GLSL_ZOFFSET_IN_VERTEXSHADER.glsl"

	color=colorIn;

	gl_PointSize=dimension;
}
