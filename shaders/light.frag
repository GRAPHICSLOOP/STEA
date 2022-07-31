#version 450 core

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormal;

void main()
{
	outColor = vec4(fragColor,1.f);
	outNormal = vec4(0.f);
}