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
