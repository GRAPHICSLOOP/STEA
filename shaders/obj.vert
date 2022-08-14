#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPos;

layout(set = 0 , binding = 1) uniform ObjectDynamicBuffer {
    mat4 model;
}ubo;

layout(set = 0 , binding = 0) uniform CameraBuffer {
	mat4 porjView;
}cmo;

void main()
{

    gl_Position = cmo.porjView * ubo.model * vec4(inPos, 1.0f);
	fragNormal = normalize(mat3(transpose(inverse(ubo.model))) * inNormal);
	fragNormal = normalize(inNormal);
	fragTexCoord = inTexCoord;
    fragPos = vec3(ubo.model * vec4(inPos, 1.0f));
}