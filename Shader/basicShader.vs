#version 330 core
layout (location = 0) in vec3 InPos;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec2 InTexCoord;
layout (location = 3) in vec4 InColor;
layout (location = 4) in vec3 InTangent;
layout (location = 5) in vec3 InBitangent;

struct PointLight {
	vec3 pos;
	vec3 color;
	float intensity;
};

struct DirectionalLight {
	vec3 dir;
	vec3 color;
	float intensity;
	float ambientIntensity;
	
	mat4 lightSpace;
	sampler2D shadowMap;
};

struct Flashlight {
	vec3 pos;
	vec3 dir;
	vec3 color;
	float intensity;
	float ambientIntensity;
};

#define MAX_NUM_POINT_LIGHTS 16
#define MAX_NUM_DIR_LIGHTS 4

out vec3 FragmentPos;
out vec3 Normal;
out vec4 Color;
out vec2 TexCoord;
out vec4 FragPosLS[MAX_NUM_DIR_LIGHTS];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform int nPointLights;
uniform PointLight pLight[MAX_NUM_POINT_LIGHTS];

uniform int nDirLights;
uniform DirectionalLight dLight[MAX_NUM_DIR_LIGHTS];

void main()
{
	TexCoord = vec2(InTexCoord.x, InTexCoord.y);
	Normal = mat3(transpose(inverse(model))) * InNormal;
	FragmentPos = vec3(model * vec4(InPos, 1.0f));
	Color = InColor;
	
	for(int i = 0; i < nDirLights; i++)
		FragPosLS[i] = dLight[i].lightSpace * vec4(FragmentPos, 1.0f);
	
	gl_Position = projection * view * model * vec4(InPos, 1.0f);
}