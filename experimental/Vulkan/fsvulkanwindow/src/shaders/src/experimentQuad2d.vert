#version 450


layout(push_constant) uniform pushConst
{
	// Memo 2018/06/28 Push Constants turned out to be global.  The change is visible to other pipelines.
	//                 Then it is even more ridiculous to make it only 128 bytes.
	//                 128 bytes!  It is as small as main/sub CPU shared memory of FM-7.
	//                 Needs to be reset before using this pipeline, not at the beginning of the render pass.
	// Memo 2018/06/25 mat3 cannot be a push constant.  Needs to be float [] or mat4.
	float windowTfm[6];
	float modelTfm[6];

	// {[0] [2] [4]}{x}
	// {[1] [3] [5]}{y}
	//              {1}
} tfm;

layout(std140,set=0,binding=0) uniform State
{
	vec4 vertexIn[4];
	vec4 colorIn[4];
};



out gl_PerVertex 
{
    vec4 gl_Position;
};
layout(location=0) out vec4 colorFromVert;

void main() 
{
	vec2 xy;

	if(gl_VertexIndex<3)
	{
		xy=vertexIn[gl_VertexIndex].xy;
	    colorFromVert=colorIn[gl_VertexIndex];
	}
	else
	{
		xy=vertexIn[(gl_VertexIndex-1)%4].xy;
	    colorFromVert=colorIn[(gl_VertexIndex-1)%4];
	}

	xy[0]=xy[0]*tfm.modelTfm[0]+xy[1]*tfm.modelTfm[2]+tfm.modelTfm[4];
	xy[1]=xy[0]*tfm.modelTfm[1]+xy[1]*tfm.modelTfm[3]+tfm.modelTfm[5];

	gl_Position.x=xy[0]*tfm.windowTfm[0]+xy[1]*tfm.windowTfm[2]+tfm.windowTfm[4];
	gl_Position.y=xy[0]*tfm.windowTfm[1]+xy[1]*tfm.windowTfm[3]+tfm.windowTfm[5];
	gl_Position[2]=0.0;
	gl_Position[3]=1.0;
}
