void Ys_ProcessPointSprite()
{
	HIGHP vec4 prj1=projection*modelView*vec4(vertex,1.0);
	HIGHP vec4 prj2=modelView*vec4(vertex,1.0);
	prj2.y+=pointSizeIn;
	prj2=projection*prj2;

	HIGHP float pointSize3D=viewportHei*abs(prj2[1]/prj2[3]-prj1[1]/prj1[3])/2.0;

	gl_PointSize=pointSizeIn*usePointSizeInPixel+pointSize3D*usePointSizeIn3D;
}
