#include "YS_GLSL_HEADER.glsl"

uniform  MIDP  float radius1;
uniform  MIDP  float radius2;
uniform  MIDP  float maxIntensity;
uniform  MIDP  float angleInterval;
uniform  MIDP  float angleOffset;
uniform  MIDP  float exponent;

varying  LOWP  vec4 color;

// No alpha cut off in the flash renderer.
#include "YS_GLSL_FOG_VARIABLES.glsl"

void main()
{
	MIDP vec2 uv=((gl_PointCoord.xy*2.0)-vec2(1.0,1.0));

	const  MIDP  float PI=3.1415927;
	const  MIDP  float halfPI=3.1415927/2.0;
	const  MIDP  float twoPI=3.1415927*2.0;
	 MIDP  float r=length(uv);
	if(r<=radius1)
	{
		 MIDP  float param=halfPI*(radius1-r)/radius1;
		 MIDP  float additive=sin(param)*maxIntensity;
		gl_FragColor[0]=min(1.0,color[0]+additive);
		gl_FragColor[1]=min(1.0,color[1]+additive);
		gl_FragColor[2]=min(1.0,color[2]+additive);
		gl_FragColor[3]=color[3];

		#include "YS_GLSL_FOGCALCULATION_IN_FRAGMENTSHADER.glsl"
	}
	else
	{
		 MIDP  float halfInterval=angleInterval/2.0;
		 MIDP  float theata=twoPI+angleOffset+atan(uv.y,uv.x);
		 MIDP  float t=mod(theata,angleInterval);
		 MIDP  float s=1.0-abs(t-halfInterval)/halfInterval;

		 MIDP  float rf=radius1+(radius2-radius1)*pow(s,exponent);
		if(rf<r)
		{
			discard;
		}
		else
		{
			MIDP  float alpha=max(1.0-(r-radius1)/(rf-radius1),0.0);
			gl_FragColor[0]=color[0];
			gl_FragColor[1]=color[1];
			gl_FragColor[2]=color[2];
			gl_FragColor[3]=color[3]*alpha;

			#include "YS_GLSL_FOGCALCULATION_IN_FRAGMENTSHADER.glsl"
		}
    }
}
