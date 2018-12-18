#include "YS_GLSL_HEADER.glsl"

// offset is taken as pixels if transformationType is true, or size in 3D space.
// transformationType:
//    0: Attributes vertex and offset are both taken as in 3D coordinate.
//    1: Attributes vertex and offset are both taken as in window coordinate.
//    2: Attribute vertex is taken as in 3D coordinate, and offset is taken as in window coordinate
uniform  HIGHP  mat4 projection,modelView;
uniform  MIDP  float viewportWid,viewportHei;
uniform sampler2D texture;
uniform int transformationType;
uniform int viewportOrigin;
uniform  LOWP  float alphaCutOff;

attribute  HIGHP  vec4 vertex;
attribute  MIDP  vec2 offset;
attribute  MIDP vec2 texCoord;

varying  MIDP  vec2 texCoord_out;

void main()
{
	texCoord_out=texCoord;
	if(1==transformationType)
	{
		float xOffset=(2.0*offset.x)/viewportWid;
		float yOffset=(2.0*offset.y)/viewportHei;

		float xAbs=-1.0+2.0*vertex.x/viewportWid+xOffset;
		float yAbs= 1.0-2.0*vertex.y/viewportHei;
		if(0!=viewportOrigin)
		{
			yAbs=-yAbs;
		}
		yAbs+=yOffset;
		gl_Position=vec4(xAbs,yAbs,0.0,1.0);
	}
	else if(0==transformationType)
	{
		vec4 inCameraCoord=modelView*vertex+vec4(offset.x,offset.y,0,0);
		gl_Position=projection*inCameraCoord;
	}
	else 
	{
		float xOffset=(2.0*offset.x)/viewportWid;
		float yOffset=(2.0*offset.y)/viewportHei;
		gl_Position=projection*modelView*vertex;
		gl_Position.x+=xOffset;
		gl_Position.y+=yOffset;
	}
}
