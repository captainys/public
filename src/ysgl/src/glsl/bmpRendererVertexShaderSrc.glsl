#define YSGLSL_TEX_TYPE_NONE                0
#define YSGLSL_TEX_TYPE_TILING              1
#define YSGLSL_TEX_TYPE_BILLBOARD           2
#define YSGLSL_TEX_TYPE_3DTILING            3
#define YSGLSL_TEX_TYPE_ATTRIBUTE           4
#define YSGLSL_TEX_BILLBOARD_PERS           0
#define YSGLSL_TEX_BILLBOARD_ORTHO          1
#define YSGLSL_TEX_WRAP_CUTOFF              0
#define YSGLSL_TEX_WRAP_REPEAT              1
#define YSGLSL_POINTSPRITE_CLIPPING_NONE      0
#define YSGLSL_POINTSPRITE_CLIPPING_CIRCLE    1
#define YSGLSL_BILLBOARD_CLIPPING_NONE      YSGLSL_POINTSPRITE_CLIPPING_NONE
#define YSGLSL_BILLBOARD_CLIPPING_CIRCLE    YSGLSL_POINTSPRITE_CLIPPING_CIRCLE
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
#define YSGLSL_RENDERER_TYPE_NONE           0
#define YSGLSL_RENDERER_TYPE_PLAIN2D        1
#define YSGLSL_RENDERER_TYPE_MARKER2D       2
#define YSGLSL_RENDERER_TYPE_POINTSPRITE2D  3
#define YSGLSL_RENDERER_TYPE_PLAIN3D        10
#define YSGLSL_RENDERER_TYPE_SHADED3D       11
#define YSGLSL_RENDERER_TYPE_FLASH          12
#define YSGLSL_RENDERER_TYPE_MARKER3D       13
#define YSGLSL_RENDERER_TYPE_POINTSPRITE3D  14
#define YSGLSL_SHADOWMAP_MAX_NUM_TEXTURE    3
#define YSGLSL_SHADOWMAP_NONE               0
#define YSGLSL_SHADOWMAP_USE                1
#define YSGLSL_SHADOWMAP_DEBUG              2

#ifdef GL_ES
	#define LOWP lowp
	#define MIDP mediump
	#define HIGHP highp
#else
	#define LOWP
	#define MIDP
	#define HIGHP
#endif


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
