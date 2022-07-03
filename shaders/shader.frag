#version 450 core
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

layout(set = 0 , binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
	mat4 porjView;
    vec3 lightPos;
	float padding_specularStrengthl;
	vec3 viewPos;
	float padding_pow;
}cmo;

layout(set = 1, binding = 0) uniform sampler2D texSampler;



void main() {
	vec4 texDiffuse = texture(texSampler,fragTexCoord);
    vec3 norm = fragNormal;
	vec3 lightColor = vec3(1.f,0.f,0.f);

    vec3 lightDir = normalize(cmo.lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);

	vec3 viewDir = normalize(cmo.viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), cmo.padding_pow);

    vec3 specular = vec3(cmo.padding_specularStrengthl * spec) * lightColor;
	vec3 ambient =  vec3(0.5f);
    vec3 diffuse = vec3(diff);

    vec3 result = texDiffuse.rgb * (ambient + diffuse + specular) ;
    outColor = texDiffuse;
}