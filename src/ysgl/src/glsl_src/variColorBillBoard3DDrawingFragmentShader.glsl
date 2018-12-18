#include "YS_GLSL_HEADER.glsl"

uniform LOWP  float usePointClippingCircle;

uniform LOWP int billBoardTransformType;
uniform sampler2D textureIdent;

varying  HIGHP  vec2 texCoordOut;
varying  LOWP  vec4 color;

#include "YS_GLSL_FOG_VARIABLES.glsl"
#include "YS_GLSL_ALPHA_CUTOFF_PARAMETER.glsl"

void main()
{
	MIDP  vec2 nomLocal=(texCoordOut.xy*2.0)-vec2(1.0,1.0);
	if(1.0<=usePointClippingCircle*dot(nomLocal,nomLocal))
	{
		discard;
	}

	 LOWP  vec4 texcell=texture2D(textureIdent,texCoordOut);
	gl_FragColor=color*texcell;

	#include "YS_GLSL_ALPHA_CUTOFF_IN_FRAGMENT_SHADER.glsl"
	#include "YS_GLSL_FOGCALCULATION_IN_FRAGMENTSHADER.glsl"
}
