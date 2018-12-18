#include "YS_GLSL_HEADER.glsl"

varying  LOWP  vec4 color;

#include "YS_GLSL_TEXTURE_VARIABLE.glsl"
#include "YS_GLSL_FOG_VARIABLES.glsl"
#include "YS_GLSL_ALPHA_CUTOFF_PARAMETER.glsl"

void main()
{
	gl_FragColor=color;

	#include "YS_GLSL_TEXTURE_IN_FRAGMENTSHADER.glsl"
	#include "YS_GLSL_ALPHA_CUTOFF_IN_FRAGMENT_SHADER.glsl"
	#include "YS_GLSL_FOGCALCULATION_IN_FRAGMENTSHADER.glsl"
}
