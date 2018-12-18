{
	LOWP  vec4 texcell[3],avg;
	texcell[0]=texture2D(textureIdent,texCoordOut.xy);
	// Who said if-statement should be avoided?
	// Sampling twice seems to cost a lot more than a if-statement in ThinkPad X250 and 2014 Mac mini.
	if(0.0<textureSampleCoeff[1]+textureSampleCoeff[2])
	{
		texcell[1]=texture2D(textureIdent,texCoordOut.xz);
		texcell[2]=texture2D(textureIdent,texCoordOut.yz);
		avg=useTexture*(texcell[0]*textureSampleCoeff[0]+texcell[1]*textureSampleCoeff[1]+texcell[2]*textureSampleCoeff[2])
		   +vec4(1.0-useTexture,1.0-useTexture,1.0-useTexture,1.0-useTexture);
	}
	else
	{
		avg=useTexture*(texcell[0]*textureSampleCoeff[0])+vec4(1.0-useTexture,1.0-useTexture,1.0-useTexture,1.0-useTexture);
	}
	gl_FragColor=gl_FragColor*avg;
}
