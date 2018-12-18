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


uniform  MIDP  float radius1;
uniform  MIDP  float radius2;
uniform  MIDP  float maxIntensity;
uniform  MIDP  float angleInterval;
uniform  MIDP  float angleOffset;
uniform  MIDP  float exponent;

varying  LOWP  vec4 color;

// No alpha cut off in the flash renderer.
// Variables for fog
uniform  MIDP  float fogEnabled;
uniform  MIDP  float fogDensity;
uniform  LOWP  vec4  fogColor;
varying  HIGHP float fogZ;


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
    }
}
