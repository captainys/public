uniform MIDP float lightEnabled[YSGLSL_MAX_NUM_LIGHT];
uniform HIGHP vec4 lightPos[YSGLSL_MAX_NUM_LIGHT];
uniform LOWP vec3 lightColor[YSGLSL_MAX_NUM_LIGHT];
uniform LOWP vec3 specularColor;
uniform MIDP float specularExponent;
uniform LOWP vec3 ambientColor;

// Lighting
//	Input variables: vec4 color
//	                 vec3 nomLocal
//	                 vec3 vecToCamera <- must be normalized
//	Output variable: vec4 accumColor

// lightCoeff can be used for cutting off diffuse and specular reflection for dealing with shadow.
// lightCoeff can be between 0 and 1.
LOWP vec4 YsCalculateStandardLighting(
    in LOWP vec4 color,in HIGHP vec3 nomLocal,in HIGHP vec3 vecToCamera,in MIDP float lightCoeff)
{
	LOWP vec3 accumColor=ambientColor*color.xyz;

	for(int lightNo=0; lightNo<YSGLSL_MAX_NUM_LIGHT; lightNo++)
	{
		MIDP float diffuseIntensity=lightCoeff*max(dot(nomLocal,lightPos[lightNo].xyz),0.0);
		HIGHP vec3 unitVecToCamera=normalize(vecToCamera);
		HIGHP vec3 mid=normalize(lightPos[lightNo].xyz+unitVecToCamera);
		HIGHP float specularIntensity=lightCoeff*pow(max(dot(mid,nomLocal),0.0),specularExponent);

		accumColor+=lightEnabled[lightNo]*diffuseIntensity*(color.xyz*lightColor[lightNo]);
		accumColor+=lightEnabled[lightNo]*specularIntensity*specularColor;
	}

	return vec4(accumColor,color[3]);
}
