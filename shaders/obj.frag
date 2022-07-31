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

layout(set = 0 , binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
	mat4 porjView;
    vec3 lightPos;
	float padding_specularStrengthl;
	vec3 viewPos;
	float padding_pow;
}cmo;

layout(set = 0 , binding = 2) uniform LightBuffer {
    PointLight lights[NUM_LIGHTS];
}lightBuffer;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

float doAttenuation(float range, float d)
{
    return 1.0 - smoothstep(range * 0.98, range, d);
}

void main() {
	vec4 texDiffuse = texture(texSampler,fragTexCoord);
    vec3 norm = fragNormal;
	vec3 viewDir = normalize(cmo.viewPos - fragPos);
	vec3 ambient =  vec3(0.2f);
	
	vec3 result = vec3(0.f);// + ambient;
	for(int i = 0 ; i < NUM_LIGHTS ; ++i)
	{
		vec3 lightColor = lightBuffer.lights[i].colorAndRadius.xyz;

		vec3 lightDir = normalize(lightBuffer.lights[i].position.xyz - fragPos);
		float dist    = length(lightDir);
		float atten   = doAttenuation(lightBuffer.lights[i].colorAndRadius.w, dist);
		float diff	  = max(0.0, dot(norm, lightDir));
		
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), cmo.padding_pow);

		vec3 specular = vec3(cmo.padding_specularStrengthl * spec) * lightColor;

		vec3 diffuse = lightBuffer.lights[i].colorAndRadius.xyz * diff * atten;

		result += diffuse;
	}
	
	result *= texDiffuse.rgb;
    outColor = vec4(result,1.f);
	outNormal = vec4(norm,1.f);
}