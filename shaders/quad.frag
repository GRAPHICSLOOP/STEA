#version 450 core
#define NUM_LIGHTS 100

struct PointLight {
	vec4 position;
	vec4 colorAndRadius;
};

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput inputNormal;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput inputDepth;
layout (input_attachment_index = 3, set = 0, binding = 3) uniform subpassInput inputPosition;

layout (location = 0) in vec2 inUV0;
layout (location = 0) out vec4 outFragColor;

layout(set = 1 , binding = 0) uniform LightBuffer {
    PointLight lights[NUM_LIGHTS];
}lightBuffer;

float doAttenuation(float range, float d)
{
    return 1.0 - smoothstep(range * 0.98, range, d);
}

void main()
{
//	int index = 2;
//	
//	if (index == 0) {
//		outFragColor = subpassLoad(inputColor);
//	} 
//	else if (index == 1) {
//		outFragColor = vec4(subpassLoad(inputDepth).r);
//	} 
//	else if (index == 2) {
//		outFragColor = subpassLoad(inputNormal);
//	} else {
//		outFragColor = vec4(inUV0, 0.0, 1.0);
//	}

	vec4 texDiffuse = subpassLoad(inputColor);
    vec3 norm = subpassLoad(inputNormal).xyz;
	vec3 fragPos = subpassLoad(inputPosition).xyz;
	vec3 ambient =  vec3(0.2f);
	
	vec3 result = vec3(0.f);// + ambient;
	for(int i = 0 ; i < NUM_LIGHTS ; ++i)
	{
		vec3 lightColor = lightBuffer.lights[i].colorAndRadius.xyz;

		vec3 lightDir = normalize(lightBuffer.lights[i].position.xyz - fragPos);
		float dist    = length(lightDir);
		float atten   = doAttenuation(lightBuffer.lights[i].colorAndRadius.w, dist);
		float diff	  = max(0.0, dot(norm, lightDir));
		
		vec3 diffuse = lightBuffer.lights[i].colorAndRadius.xyz * diff * atten;

		result += diffuse;
	}
	
	result *= texDiffuse.rgb;
	outFragColor = vec4(texDiffuse.xyz,1.f);
}