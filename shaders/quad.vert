#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV0;

layout (location = 0) out vec2 outUV0;

void main()
{
	gl_Position = vec4(inPosition, 1.f);
	outUV0 = inUV0;
}