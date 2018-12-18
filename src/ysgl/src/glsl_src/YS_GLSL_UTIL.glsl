
LOWP vec4 YsGLSLRainbowColor(HIGHP float t)
{
	// 0     0.25  0.5    0.75    1
	// Blue->Cyan->Green->Yellow->Red

	HIGHP float tt=t*4.0;
	LOWP  float blue =clamp(2.0-tt         ,0.0,1.0);
	LOWP  float green=clamp(2.0-abs(tt-2.0),0.0,1.0);
	LOWP  float red  =clamp(tt-2.0         ,0.0,1.0);
	return vec4(red,green,blue,1.0);

	// HIGHP float tt;
	// if(t<0.0)
	// {
	// 	return vec4(0,0,1,1);
	// }
	// else if(t<0.25)
	// {
	// 	tt=t/0.25;
	// 	return vec4(0,tt,1,1);
	// }
	// else if(t<0.5)
	// {
	// 	tt=(t-0.25)/0.25;
	// 	return vec4(0,1,1.0-tt,1);
	// }
	// else if(t<0.75)
	// {
	// 	tt=(t-0.5)/0.25;
	// 	return vec4(tt,1,0,1);
	// }
	// else if(t<1.0)
	// {
	// 	tt=(t-0.75)/0.25;
	// 	return vec4(1,1.0-tt,0,1);
	// }
	// else
	// {
	// 	return vec4(1,0,0,1);
	// }
}

MIDP float YsIsInRange(HIGHP float min,HIGHP float max,HIGHP float x)
{
	return step(min,x)*step(x,max);
}

MIDP float YsIsInRange(HIGHP float min,HIGHP float max,HIGHP vec2 v)
{
	return step(min,v.x)*step(v.x,max)
          *step(min,v.y)*step(v.y,max);
}

MIDP float YsIsInRange(HIGHP float min,HIGHP float max,HIGHP vec3 v)
{
	return step(min,v.x)*step(v.x,max)
          *step(min,v.y)*step(v.y,max)
          *step(min,v.z)*step(v.z,max);
}
