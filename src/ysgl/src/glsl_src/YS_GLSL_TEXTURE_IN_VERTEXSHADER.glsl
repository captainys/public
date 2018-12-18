//   In orthographic projection, billBoardOffset can simply be calculated by taking XY components of 
//   billBoardOffsetInView-modelView*(vec4(billBoardCenter,1.0));
//   However, in perspective projection, a plane perpendicular to the viewing vector must be a reference plane,
//   and therefore heading and pitch angles must be calculated and taken into account.
//   Currently, this GLSL program only supports billboading in perspective projection.

if(textureType==YSGLSL_TEX_TYPE_BILLBOARD)
{
	if(YSGLSL_TEX_BILLBOARD_ORTHO==billBoardTransformType)
	{
		 HIGHP  vec4 billBoardCenterInView=modelView*vec4(billBoardCenter,1.0);
		 MIDP  vec2 billBoardOffset=(vertexInView-billBoardCenterInView).xy;
		texCoordOut=vec3(billBoardOffset/billBoardDimension,0.0);
	}
	else
	{
		 HIGHP  vec4 billBoardCenterInView=modelView*vec4(billBoardCenter,1.0);
		 MIDP  vec3 viewVec=normalize(billBoardCenterInView.xyz);
		 MIDP  float pitch=asin(viewVec.y);
		 MIDP  float heading=-asin(viewVec.x);
		 MIDP  vec3 uVec=vec3(cos(heading),0.0,-sin(heading));
		 MIDP  vec3 vVec=vec3(-sin(pitch)*sin(heading),cos(pitch),sin(pitch)*cos(heading));
		 HIGHP  vec3 billBoardOffset3d=(vertexInView-billBoardCenterInView).xyz;
		 HIGHP  vec2 billBoardOffset=vec2(dot(billBoardOffset3d,uVec),dot(billBoardOffset3d,vVec));
		texCoordOut=vec3(billBoardOffset/billBoardDimension,0.0);
	}
	if(true==useNegativeOneToPositiveOneTexCoord)
	{
		texCoordOut=(texCoordOut+vec3(1.0,1.0,1.0))/vec3(2.0,2.0,2.0);
	}
}
else if(textureType==YSGLSL_TEX_TYPE_TILING || textureType==YSGLSL_TEX_TYPE_3DTILING)
{
	 LOWP  vec4 texCoordTfm=textureTileTransform*vec4(vertex,1.0);
	texCoordOut=texCoordTfm.xyz;
	if(true==useNegativeOneToPositiveOneTexCoord)
	{
		texCoordOut=(texCoordOut+vec3(1.0,1.0,1.0))/vec3(2.0,2.0,2.0);
	}
}
else if(textureType==YSGLSL_TEX_TYPE_ATTRIBUTE)
{
	texCoordOut=vec3(texCoord,0.0);
}
else
{
	texCoordOut[0]=0.0;
	texCoordOut[1]=0.0;
	texCoordOut[2]=0.0;
}
