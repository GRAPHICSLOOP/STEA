#version 450 core

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPos;
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPosition;

void main()
{
	outColor = vec4(fragColor,1.f);
	outNormal = vec4(0.f);
	outPosition = vec4(fragPos,0.f);
}