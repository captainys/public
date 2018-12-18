#include "YS_GLSL_HEADER.glsl"

uniform  HIGHP  mat4  projection;
uniform  HIGHP  mat4  modelView;
uniform  MIDP  float viewportWid,viewportHei;

uniform LOWP int markerType;
uniform  MIDP  float dimension;

attribute  HIGHP  vec3 vertex;
attribute  MIDP  vec2 pixelOffset;
attribute  LOWP  vec4 colorIn;

varying  MIDP  vec2 offsetOut;
varying  LOWP  vec4 color;

#include "YS_GLSL_FOG_VARIABLES.glsl"
#include "YS_GLSL_ZOFFSET_VARIABLE.glsl"

void main()
{
	#include "YS_GLSL_VIEW_TRANSFORM_IN_VERTEX_SHADER.glsl"
	#include "YS_GLSL_FOG_CALCULATION_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_PROJECTION_TRANSFORM_IN_VERTEX_SHADER.glsl"
	#include "YS_GLSL_ZOFFSET_IN_VERTEXSHADER.glsl"
	float xOffset=((2.0*pixelOffset.x)/viewportWid)*gl_Position[3];
	float yOffset=((2.0*pixelOffset.y)/viewportHei)*gl_Position[3];
	gl_Position.x+=xOffset;
	gl_Position.y+=yOffset;
	offsetOut=pixelOffset;
	color=colorIn;
}
