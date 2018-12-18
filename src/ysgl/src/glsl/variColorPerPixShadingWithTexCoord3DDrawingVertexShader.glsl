#define YSGLSL_TEX_TYPE_NONE                0
#define YSGLSL_TEX_TYPE_TILING              1
#define YSGLSL_TEX_TYPE_BILLBOARD           2
#define YSGLSL_TEX_TYPE_3DTILING            3
#define YSGLSL_TEX_TYPE_ATTRIBUTE           4
#define YSGLSL_TEX_BILLBOARD_PERS           0
#define YSGLSL_TEX_BILLBOARD_ORTHO          1
#define YSGLSL_TEX_WRAP_CUTOFF              0
#define YSGLSL_TEX_WRAP_REPEAT              1
#define YSGLSL_BILLBOARD_CLIPPING_NONE      0
#define YSGLSL_BILLBOARD_CLIPPING_CIRCLE    1
#define YSGLSL_POINTSPRITE_CLIPPING_NONE      0
#define YSGLSL_POINTSPRITE_CLIPPING_CIRCLE    1
#define YSGLSL_MARKER_TYPE_PLAIN            0
#define YSGLSL_MARKER_TYPE_CIRCLE           1
#define YSGLSL_MARKER_TYPE_RECT             2
#define YSGLSL_MARKER_TYPE_STAR             3
#define YSGLSL_MARKER_TYPE_EMPTY_RECT       4
#define YSGLSL_MARKER_TYPE_EMPTY_CIRCLE     5
#define YSGLSL_MARKER_TYPE_EMPTY_STAR       6
#define YSGLSL_MAX_NUM_LIGHT                8
#define YSGLSL_POINTSPRITE_SIZE_IN_PIXEL    0
#define YSGLSL_POINTSPRITE_SIZE_IN_3DSPACE  1

uniform  HIGHP mat4 projection;
uniform  HIGHP mat4 modelView;

attribute  HIGHP  vec3 vertex;
attribute  HIGHP  vec3 normal;
attribute  MIDP  vec2 texCoord;
attribute  LOWP vec4 colorIn;

varying  HIGHP  vec3 normalOut;
varying  HIGHP  vec3 vecToCameraOut;
varying  LOWP  vec4 color;

uniform sampler2D textureIdent;
varying  HIGHP  vec3 texCoordOut;

// Variables for z-offset
uniform bool useZOffset;
uniform  MIDP  float zOffset;

// Variables for fog
uniform bool fogEnabled;
uniform  MIDP  float fogDensity;
uniform  LOWP  vec4 fogColor;
varying  HIGHP  float zInViewCoord;


void main()
{
	 HIGHP  vec4 vertexInView=modelView*vec4(vertex,1.0);

	gl_Position=projection*vertexInView;


	 HIGHP vec4 normal4d=vec4(normal,0);
	normalOut=normalize((modelView*normal4d).xyz);
	vecToCameraOut=-normalize(vertexInView.xyz);

	color=colorIn;
	texCoordOut[0]=texCoord[0];
	texCoordOut[1]=texCoord[1];
	texCoordOut[2]=0.0;

	if(false!=fogEnabled)
	{
		zInViewCoord=-vertexInView.z;
	}
	else
	{
		zInViewCoord=0.0;
	}

	if(true==useZOffset)
	{
		gl_Position.z+=zOffset*gl_Position[3];
	}


}
