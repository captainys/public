#include "YS_GLSL_HEADER.glsl"

uniform  HIGHP  mat4  projection;
uniform  HIGHP  mat4  modelView;
uniform sampler2D textureIdent;
uniform  MIDP  float viewportWid,viewportHei;

attribute  HIGHP  vec3 vertex;
attribute  LOWP  vec4 colorIn;
attribute  LOWP  vec2 texCoordIn;
attribute  MIDP  float pointSizeIn;

varying  LOWP  vec4 color;
varying  MIDP  vec2 texCoordOut;

#include "YS_GLSL_FOG_VARIABLES.glsl"
#include "YS_GLSL_ZOFFSET_VARIABLE.glsl"
#include "YS_GLSL_POINT_SPRITE_VARIABLE.glsl"

#include "YS_GLSL_POINT_SPRITE_IN_VERTEX_SHADER.glsl"

void main()
{
	#include "YS_GLSL_VIEW_TRANSFORM_IN_VERTEX_SHADER.glsl"
	texCoordOut=texCoordIn;
	color=colorIn;

	Ys_ProcessPointSprite();

	#include "YS_GLSL_FOG_CALCULATION_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_PROJECTION_TRANSFORM_IN_VERTEX_SHADER.glsl"
	#include "YS_GLSL_ZOFFSET_IN_VERTEXSHADER.glsl"
}
