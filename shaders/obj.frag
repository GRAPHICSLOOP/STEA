#version 450 core
#extension GL_ARB_separate_shader_objects : enable
#define NUM_LIGHTS 100

struct PointLight {
	vec4 position;
	vec4 colorAndRadius;
};

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outPos;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

void main() {
	vec4 texDiffuse = texture(texSampler,fragTexCoord);
    vec3 norm = fragNormal;

    outColor = texDiffuse;
	outNormal = vec4(norm,1.f);
	outPos = vec4(fragPos,1.f);
}