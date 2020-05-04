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
};

out vec4 FragColor;

in vec3 FragmentPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 Color;

#define MAX_NUM_POINT_LIGHTS 16

uniform vec3 cameraPos;

uniform Material mat;

uniform int nrLights;
uniform PointLight light[MAX_NUM_POINT_LIGHTS];

// Prototypes:
vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 fragmentPos, vec3 viewDirection, vec2 texCoord);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragmentPos, vec3 viewDirection, vec2 texCoord);
vec3 quantify(vec3 color, int q);

void main()
{
	vec3 viewDir = normalize(cameraPos - FragmentPos);
	vec3 result = vec3(0.0f, 0.0f, 0.0f);

	for(int i = 0; i < nrLights; i++) 
		result += calcPointLight(light[i], Normal, FragmentPos, viewDir, TexCoord); 
		
	FragColor = vec4(quantify(result, 6), 1.0f);//texture(mat.texDiffuse, TexCoord) * vec4(mat.diffuseColor, 1.0f);
}
vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 fragmentPos, vec3 viewDirection, vec2 texCoord)
{
	// Directional lights act as global illumination, making them a fit for point lights that are very intense, 
	// but very far away (e.g. the sun). Thus, they are not affected by attenuation in this model.

	// Ambient lighting:
	vec3 ambient = (light.color * texture(mat.texAmbient, texCoord).xyz * mat.ambientColor) * light.ambientIntensity;
	
	// Diffuse lighting:
	float diff = max(dot(normal, -light.dir), 0.0f) * light.intensity;
	vec3 diffuse = diff * (texture(mat.texDiffuse, texCoord).xyz * mat.diffuseColor * light.color);

	// Specular lighting:
	float spec = pow(max(0.0f, dot(viewDirection, reflect(light.dir, normal))), mat.specularIntensity) * light.intensity;
	vec3 specular = spec * (texture(mat.texSpecular, texCoord).xyz * mat.specularColor * light.color);
	
	// Return sum:
	return (ambient + diffuse + specular);
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
	return (ambient + diffuse + specular);
}

vec3 quantify(vec3 color, int q){
	color.x = (round(color.x * q) / q);
	color.y = (round(color.y * q) / q);
	color.z = (round(color.z * q) / q);
	
	return color;
}