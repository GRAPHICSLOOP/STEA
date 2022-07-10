#version 450 core

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput inputNormal;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput inputDepth;

layout (location = 0) in vec2 inUV0;
layout (location = 0) out vec4 outFragColor;

void main()
{
	int index = 0;
	
	if (index == 0) {
		outFragColor = subpassLoad(inputColor);
	} 
	else if (index == 1) {
		outFragColor = vec4(subpassLoad(inputDepth).r);
	} 
	else if (index == 2) {
		outFragColor = subpassLoad(inputNormal);
	} else {
		outFragColor = vec4(inUV0, 0.0, 1.0);
	}

}