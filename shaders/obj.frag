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
//	vec3 viewDir = normalize(cmo.viewPos - fragPos);
//	vec3 ambient =  vec3(0.2f);
	
//	vec3 result = vec3(0.f);// + ambient;
//	for(int i = 0 ; i < NUM_LIGHTS ; ++i)
//	{
//		vec3 lightColor = lightBuffer.lights[i].colorAndRadius.xyz;
//
//		vec3 lightDir = normalize(lightBuffer.lights[i].position.xyz - fragPos);
//		float dist    = length(lightDir);
//		float atten   = doAttenuation(lightBuffer.lights[i].colorAndRadius.w, dist);
//		float diff	  = max(0.0, dot(norm, lightDir));
//		
//		//vec3 reflectDir = reflect(-lightDir, norm);
//		//float spec = pow(max(dot(viewDir, reflectDir), 0.0), cmo.padding_pow);
//
//		//vec3 specular = vec3(cmo.padding_specularStrengthl * spec) * lightColor;
//
//		vec3 diffuse = lightBuffer.lights[i].colorAndRadius.xyz * diff * atten;
//
//		result += diffuse;
//	}
//	
//	result *= texDiffuse.rgb;
    outColor = texDiffuse;
	outNormal = vec4(norm,1.f);
	outPos = vec4(fragPos,1.f);
}