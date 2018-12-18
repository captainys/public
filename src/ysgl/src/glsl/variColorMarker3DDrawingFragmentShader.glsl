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


uniform LOWP int markerType;
uniform  MIDP  float dimension;

varying  MIDP  vec2 offsetOut;
varying  LOWP  vec4 color;

// Variables for fog
uniform  MIDP  float fogEnabled;
uniform  MIDP  float fogDensity;
uniform  LOWP  vec4  fogColor;
varying  HIGHP float fogZ;

// Variables for alpha cutoff
uniform  LOWP  float alphaCutOff;


void main()
{
	if(markerType==YSGLSL_MARKER_TYPE_RECT)
	{
		if(dimension<abs(offsetOut.x) || dimension<abs(offsetOut.y))
		{
			discard;
		}
	}
	else if(markerType==YSGLSL_MARKER_TYPE_CIRCLE)
	{
		if(dimension*dimension<offsetOut.x*offsetOut.x+offsetOut.y*offsetOut.y)
		{
			discard;
		}
	}
	else if(markerType==YSGLSL_MARKER_TYPE_EMPTY_RECT)
	{
		if( dimension    <abs(offsetOut.x) || dimension    <abs(offsetOut.y) ||
		   (dimension-1.2>abs(offsetOut.x) && dimension-1.2>abs(offsetOut.y)))
		{
			discard;
		}
	}
	else if(markerType==YSGLSL_MARKER_TYPE_EMPTY_CIRCLE)
	{
		 MIDP  float offsetSq=offsetOut.x*offsetOut.x+offsetOut.y*offsetOut.y;
		if( dimension*dimension           <offsetSq ||
		   (dimension-1.2)*(dimension-1.2)>offsetSq)
		{
			discard;
		}
	}
	else if(markerType==YSGLSL_MARKER_TYPE_STAR)
	{
		// See star.png for the equations.
		bool show=false;
		MIDP  float  x=offsetOut.x/dimension;
		MIDP  float  y=offsetOut.y/dimension;
		MIDP  float  x3_077685=3.0077685*x;
		MIDP  float  yMinus1=y-1.0;
		if(y<=0.309017)
		{
			MIDP  float  x0_726543=0.726543*x;
			MIDP  float  yPlus0_381966=y+0.381966;
			if((yPlus0_381966>= x0_726543 && yMinus1<= x3_077685) ||
			   (yPlus0_381966>=-x0_726543 && yMinus1<=-x3_077685))
			{
				show=true;
			}
		}
		else
		{
			if(yMinus1<=x3_077685 && yMinus1<=-x3_077685)
			{
				show=true;
			}
		}
		if(true!=show)
		{
			discard;
		}
	}
	else if(markerType==YSGLSL_MARKER_TYPE_EMPTY_STAR)
	{
		bool show=false;
		MIDP  float  x=offsetOut.x/dimension;
		MIDP  float  y=offsetOut.y/dimension;
		MIDP  float  q1=-3.077685*x+1.0;
		MIDP  float  q2= 3.077685*x+1.0;
		MIDP  float  q3= 0.726543*x-0.381966;
		MIDP  float  q4=-0.726543*x-0.381966;
		if(y>=0.309017 && y<=q1 && y<=q2)
		{
			show=true;
		}
		else if(y<=0.309017 && y>=q1 && y>=q3)
		{
			show=true;
		}
		else if(y<=0.309017 && y>=q2 && y>=q4)
		{
			show=true;
		}
		else if(y<=q3 && y>=q4 && y<=q1)
		{
			show=true;
		}
		else if(y<=q4 && y>=q3 && y<=q2)
		{
			show=true;
		}
		if(true!=show)
		{
			discard;
		}
	}
	gl_FragColor=color;

	if(gl_FragColor.a<alphaCutOff)
	{
		discard;
	}

	// f  0:Completely fogged out   1:Clear
	// f=e^(-d*d)
	// d  0:Clear      Infinity: Completely fogged out
	// 99% fogged out means:  e^(-d*d)=0.01  What's d?
	// -d*d=loge(0.01)
	// -d*d= -4.60517
	// d=2.146
	// If visibility=V, d=2.146 at fogZ=V -> fogDensity=2.146/V
	{
		MIDP  float d=fogDensity*abs(fogZ);
		MIDP  float f=clamp(exp(-d*d),0.0,1.0);
		LOWP  vec3  fogMix=mix(fogColor.rgb,gl_FragColor.rgb,f);
		gl_FragColor.rgb=fogMix;
	}

}
