#version 450

layout(binding=0) uniform sampler2D texture2d;

layout(location=0) in vec4 colorFromVert;
layout(location=1) in vec2 texCoordFromVert;
layout(location=0) out vec4 outColor;

void main() 
{
	outColor=colorFromVert*texture(texture2d,texCoordFromVert);
}
