#include "YS_GLSL_HEADER.glsl"

uniform  HIGHP mat4 projection;
uniform  HIGHP mat4 modelView;

attribute  HIGHP  vec3 vertex;
attribute  HIGHP  vec3 normal;
attribute  LOWP   vec4 colorIn;
attribute  HIGHP  vec2 texCoord;

varying  HIGHP  vec3 normalOut;
varying  HIGHP  vec3 vecToCameraOut;
varying  LOWP  vec4 color;

#include "YS_GLSL_TEXTURE_VARIABLE.glsl"
#include "YS_GLSL_ZOFFSET_VARIABLE.glsl"
#include "YS_GLSL_FOG_VARIABLES.glsl"

#include "YS_GLSL_SHADOWMAP_UNIFORM.glsl"
#include "YS_GLSL_SHADOWMAP_VARYING.glsl"

void main()
{
	#include "YS_GLSL_VIEW_TRANSFORM_IN_VERTEX_SHADER.glsl"
	#include "YS_GLSL_PROJECTION_TRANSFORM_IN_VERTEX_SHADER.glsl"

	HIGHP vec4 normal4d=vec4(normal,0);
	normalOut=normalize((modelView*normal4d).xyz);
	vecToCameraOut=-normalize(vertexInView.xyz);

	color=colorIn;


	// Shadow Map >>
	for(int i=0; i<YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE; ++i)
	{
		vec4 inShadowPos=shadowMapTransform[i]*vertexInView;
		shadowCoord[i]=(vec3(1,1,1)+(inShadowPos.xyz/inShadowPos.w))/2.0;
		shadowCoord[i].z/=lightDistScale[i];
		shadowCoord[i].z-=lightDistOffset[i];
	}
	// Shadow Map <<


	#include "YS_GLSL_TEXTURE_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_FOG_CALCULATION_IN_VERTEXSHADER.glsl"
	#include "YS_GLSL_ZOFFSET_IN_VERTEXSHADER.glsl"

}
