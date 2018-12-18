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

uniform bool lightEnabled[YSGLSL_MAX_NUM_LIGHT];
uniform  HIGHP vec4 lightPos[YSGLSL_MAX_NUM_LIGHT];
uniform  LOWP vec3 lightColor[YSGLSL_MAX_NUM_LIGHT];
uniform  LOWP vec3 specularColor;
uniform  LOWP float specularExponent;
uniform  LOWP vec3 ambientColor;
uniform sampler2D textureIdent;

varying  HIGHP  vec3 normalOut;
varying  HIGHP  vec3 vecToCameraOut;
varying  LOWP  vec4 color;
varying  HIGHP  vec3 texCoordOut;

// Variables for alpha cutoff
uniform  LOWP  float alphaCutOff;

// Variables for fog
uniform bool fogEnabled;
uniform  MIDP  float fogDensity;
uniform  LOWP  vec4 fogColor;
varying  HIGHP  float zInViewCoord;


void main()
{
	 HIGHP vec3 nomLocal=normalize(normalOut);
	 HIGHP vec3 vecToCamera=normalize(vecToCameraOut);

	// Lighting
	//	Input variables: vec4 color
	//	                 vec3 nomLocal
	//	                 vec3 vecToCamera <- must be normalized
	//	Output variable: vec4 accumColor
	
	 HIGHP  vec3 accumColor;
	accumColor=ambientColor*color.xyz;
	
	for(int lightNo=0; lightNo<YSGLSL_MAX_NUM_LIGHT; lightNo++)
	{
		if(true==lightEnabled[lightNo])
		{
			 MIDP  float diffuseIntensity=max(dot(nomLocal,lightPos[lightNo].xyz),0.0);
			accumColor+=diffuseIntensity*(color.xyz*lightColor[lightNo]);
	
			if(0.01<specularColor.r || 0.01<specularColor.g || 0.01<specularColor.b)
			{
				 HIGHP  vec3 unitVecToCamera=normalize(vecToCamera);
				 HIGHP  vec3 mid=normalize(lightPos[lightNo].xyz+unitVecToCamera);
				 HIGHP  float specularIntensity=pow(max(dot(mid,nomLocal),0.0),specularExponent);
				accumColor+=specularIntensity*specularColor;
			}
		}
	}


	gl_FragColor=vec4(accumColor,color[3]);

	 LOWP  vec4 texcell=texture2D(textureIdent,texCoordOut.xy);
	gl_FragColor=gl_FragColor*texcell;

	if(gl_FragColor.a<alphaCutOff)
	{
		discard;
	}

	if(false!=fogEnabled)
	{
		 MIDP  float d=fogDensity*abs(zInViewCoord);
		 MIDP  float f=clamp(exp(-d*d),0.0,1.0);
		 LOWP  vec3 fogMix=mix(fogColor.rgb,gl_FragColor.rgb,f);
		gl_FragColor[0]=fogMix[0];
		gl_FragColor[1]=fogMix[1];
		gl_FragColor[2]=fogMix[2];
	}

}
