#include "YS_GLSL_HEADER.glsl"

uniform  HIGHP  mat4  projection;
uniform  HIGHP  mat4  modelView;
uniform  MIDP   float pointSizeIn;

attribute  HIGHP  vec3 vertex;
attribute  LOWP   vec4 colorIn;
attribute  HIGHP  vec2 texCoord;

varying  LOWP  vec4 color;

#include "YS_GLSL_TEXTURE_VARIABLE.glsl"
#include "YS_GLSL_ZOFFSET_VARIABLE.glsl"
#include "YS_GLSL_FOG_VARIABLES.glsl"

void main()
{
	#include "YS_GLSL_VIEW_TRANSFORM_IN_VERTEX_SHADER.glsl"
	#include "YS_GLSL_PROJECTION_TRANSFORM_IN_VERTEX_SHADER.glsl"

	color=colorIn;
	gl_PointSize=pointSizeIn;

	#include "YS_GLSL_TEXTURE_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_FOG_CALCULATION_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_ZOFFSET_IN_VERTEXSHADER.glsl"
}
