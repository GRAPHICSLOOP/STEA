#version 450 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPos;

layout(set = 0 , binding = 0) uniform ObjectBuffer {
    mat4 model;
}ubo;

layout(set = 1 , binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
	mat4 porjView;
    vec3 lightPos;
	float padding_specularStrengthl;
	vec3 viewPos;
	float padding_pow;
}cmo;

layout(push_constant) uniform PushConsts {
    layout(offset = 0) mat4 modelMatrix;
} pushConsts;

void main()
{

    gl_Position = cmo.porjView * pushConsts.modelMatrix * ubo.model * vec4(inPos, 1.0f);
	fragNormal = normalize(mat3(transpose(inverse(ubo.model))) * inNormal);
	fragTexCoord = inTexCoord;
    fragPos = vec3(ubo.model * vec4(inPos, 1.0f));
}