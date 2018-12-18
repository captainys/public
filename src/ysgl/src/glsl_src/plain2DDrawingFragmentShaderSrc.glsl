#include "YS_GLSL_HEADER.glsl"

uniform HIGHP mat4 transform;
uniform LOWP  float useTexture;
uniform       sampler2D textureIdent;

varying  LOWP  vec4 colorOut;
varying MIDP  vec2 texCoordOut;

void main()
{
	LOWP  vec4 avg;
	avg=texture2D(textureIdent,texCoordOut.xy)*useTexture+vec4(1.0,1.0,1.0,1.0)*(1.0-useTexture);
	gl_FragColor=colorOut*avg;
}
