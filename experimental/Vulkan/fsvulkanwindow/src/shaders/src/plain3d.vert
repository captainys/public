#version 450

layout(binding=0) uniform binding0
{
	mat4 projection;
};

layout(push_constant) uniform pushConst
{
	mat4 modelView;
};

layout(location=0) in vec3 vertexIn;
layout(location=1) in vec4 colorIn;


out gl_PerVertex 
{
    vec4 gl_Position;
};
layout(location=0) out vec4 colorFromVert;

void main() 
{
	gl_Position=projection*modelView*vec4(vertexIn,1.0);
    colorFromVert=colorIn;
}
