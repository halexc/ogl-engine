#version 330 core

struct Material {
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float specularIntensity;
	
	sampler2D texAmbient;
	sampler2D texDiffuse;
	sampler2D texSpecular;
	
	sampler2D normalMap;
	sampler2D parallaxMap;
	
	bool normalMapping;
	bool parallaxMapping;
};

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

out vec4 FragColor;

in vec3 FragmentPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 Color;
in vec4 FragPosLS[MAX_NUM_DIR_LIGHTS];

uniform vec3 cameraPos;

uniform Material mat;

uniform int nPointLights;
uniform PointLight pLight[MAX_NUM_POINT_LIGHTS];

uniform int nDirLights;
uniform DirectionalLight dLight[MAX_NUM_DIR_LIGHTS];

// Prototypes:
vec3 calcDirectionalLight(int i, vec3 normal, vec3 fragmentPos, vec3 viewDirection, vec2 texCoord);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragmentPos, vec3 viewDirection, vec2 texCoord);
vec3 quantify(vec3 color, int q);
float shadow(vec4 fragPosLS, DirectionalLight light);

void main()
{
	vec3 viewDir = normalize(cameraPos - FragmentPos);
	vec3 result = vec3(0.0f, 0.0f, 0.0f);

	for(int i = 0; i < nPointLights; i++) 
		result += calcPointLight(pLight[i], Normal, FragmentPos, viewDir, TexCoord); 
		
	for(int i = 0; i < nDirLights; i++)
		result += calcDirectionalLight(i, Normal, FragmentPos, viewDir, TexCoord);
		
	FragColor = vec4(result, 1.0f);
}
vec3 calcDirectionalLight(int i, vec3 normal, vec3 fragmentPos, vec3 viewDirection, vec2 texCoord)
{
	// Directional lights act as global illumination, making them a fit for point lights that are very intense, 
	// but very far away (e.g. the sun). Thus, they are not affected by attenuation in this model.

	// Ambient lighting:
	vec3 ambient = (dLight[i].color * texture(mat.texAmbient, texCoord).xyz * mat.ambientColor) * dLight[i].ambientIntensity;
	
	// Diffuse lighting:
	float diff = max(dot(normal, -dLight[i].dir), 0.0f) * dLight[i].intensity;
	vec3 diffuse = diff * (texture(mat.texDiffuse, texCoord).xyz * mat.diffuseColor * dLight[i].color);

	// Specular lighting:
	float spec = pow(max(0.0f, dot(viewDirection, reflect(dLight[i].dir, normal))), 0.125f * mat.specularIntensity) * 0.1f * dLight[i].intensity;
	vec3 specular = spec * (texture(mat.texSpecular, texCoord).xyz * mat.specularColor * dLight[i].color);
	
	// Return sum:
	return (ambient + ( 1.0f - shadow(FragPosLS[i], dLight[i]) ) * (diffuse + specular));
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragmentPos, vec3 viewDirection, vec2 texCoord)
{
	// Attenuation from light travelling (proportional to distance squared)
	float attenuation = light.intensity / pow(length(light.pos - fragmentPos), 2);
	
	// Ambient lighting:
	vec3 ambient = (texture(mat.texAmbient, texCoord).xyz * mat.diffuseColor * light.color) * attenuation;
	
	// Diffuse lighting:
	vec3 lightDirection = normalize(light.pos - fragmentPos);
	float diff = max(dot(normal, lightDirection), 0.0f) * attenuation;
	vec3 diffuse = diff * (texture(mat.texDiffuse, texCoord).xyz * mat.diffuseColor * light.color);
	
	// Specular lighting:
	float spec = pow(max(0.0f, dot(viewDirection, reflect(-lightDirection, normal))), mat.specularIntensity) * attenuation;
	vec3 specular = spec * (texture(mat.texSpecular, texCoord).xyz * mat.specularColor * light.color);
	
	// Return sum:
	return (ambient + (1.0f) * (diffuse + specular));
}

vec3 quantify(vec3 color, int q)
{
	color.x = (round(color.x * q) / q);
	color.y = (round(color.y * q) / q);
	color.z = (round(color.z * q) / q);
	
	return color;
}

float shadow(vec4 fragPosLS, DirectionalLight light)
{
	float eps = 0.000001;

	vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(light.shadowMap, projCoords.xy).r;  
	float currentDepth = projCoords.z;
	
	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(light.shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(light.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - eps > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0f;
	
	if(projCoords.z > 1.0)
        shadow = 0.0;
	
	return shadow;
}