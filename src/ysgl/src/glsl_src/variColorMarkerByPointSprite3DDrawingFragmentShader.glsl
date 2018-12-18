#include "YS_GLSL_HEADER.glsl"

uniform  HIGHP  mat4  projection;
uniform  HIGHP  mat4  modelView;

uniform LOWP int markerType;
uniform MIDP float dimension;

varying LOWP vec4 color;

#include "YS_GLSL_FOG_VARIABLES.glsl"
#include "YS_GLSL_ALPHA_CUTOFF_PARAMETER.glsl"

void main()
{
	MIDP vec2 t=(gl_PointCoord.xy*2.0)-vec2(1.0,1.0);
	t.y=-t.y;

	if(markerType==YSGLSL_MARKER_TYPE_RECT)
	{
	}
	else if(markerType==YSGLSL_MARKER_TYPE_CIRCLE)
	{
		if(1.0<dot(t,t))
		{
			discard;
		}
	}
	else if(markerType==YSGLSL_MARKER_TYPE_EMPTY_RECT)
	{
		MIDP float frameSize=2.4/dimension; // 1pixel=2.0/dimension   1.2pixel=2.4/dimension
		if(abs(t.x)<1.0-frameSize && abs(t.y)<1.0-frameSize)
		{
			discard;
		}
	}
	else if(markerType==YSGLSL_MARKER_TYPE_EMPTY_CIRCLE)
	{
		MIDP float frameSize=2.4/dimension; // 1pixel=2.0/dimension   1.2pixel=2.4/dimension
		MIDP float tsq=dot(t,t);
		MIDP float inRad=1.0-frameSize;
		if(tsq<inRad*inRad || 1.0<tsq)
		{
			discard;
		}
	}
	else if(markerType==YSGLSL_MARKER_TYPE_STAR)
	{
		// See star.png for the equations.
		bool show=false;
		MIDP  float  x=t.x;
		MIDP  float  y=t.y;
		MIDP  float  x3_077685=3.0077685*x;
		MIDP  float  yMinus1=y-1.0;
		if(y<=0.309017)
		{
			MIDP float x0_726543=0.726543*x;
			MIDP float yPlus0_381966=y+0.381966;
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
		MIDP  float  x=t.x;
		MIDP  float  y=t.y;
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

	#include "YS_GLSL_ALPHA_CUTOFF_IN_FRAGMENT_SHADER.glsl"
	#include "YS_GLSL_FOGCALCULATION_IN_FRAGMENTSHADER.glsl"
}
