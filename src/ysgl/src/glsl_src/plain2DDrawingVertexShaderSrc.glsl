#include "YS_GLSL_HEADER.glsl"

uniform HIGHP  mat4 transform;

attribute LOWP vec2 vertex;
attribute MIDP vec2 texCoordIn;
attribute LOWP vec4 colorIn;
attribute MIDP float pointSizeIn;

varying LOWP  vec4 colorOut;
varying MIDP  vec2 texCoordOut;

void main()
{
	vec4 vertex4d=vec4(vertex,0,1);
	gl_Position=transform*vertex4d;
	colorOut=colorIn;
	texCoordOut=texCoordIn;
	gl_PointSize=pointSizeIn;
}
