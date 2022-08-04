#version 450 core

#define NUM_LIGHTS 100

struct PointLight {
	vec4 position;
	vec4 colorAndRadius;
};

layout (location = 0) in vec3 inPos;
layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 fragPos;

layout(set = 0 , binding = 1) uniform LightBuffer {
    PointLight lights[NUM_LIGHTS];
}lightBuffer;

layout(set = 0 , binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
	mat4 porjView;
    vec3 lightPos;
	float padding_specularStrengthl;
	vec3 viewPos;
	float padding_pow;
}cmo;

void main()
{
	gl_Position = cmo.porjView * vec4(inPos + lightBuffer.lights[gl_InstanceIndex].position.xyz, 1.0f);
	fragColor = lightBuffer.lights[gl_InstanceIndex].colorAndRadius.xyz;
	fragPos = inPos;
}