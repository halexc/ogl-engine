#version 330 core

#define MAX_NUM_POINT_LIGHTS 16
#define MAX_NUM_DIR_LIGHTS 4
#define NUM_SHADOWMAP_LAYERS 3

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
	
	#if NUM_SHADOWMAP_LAYERS > 1
		mat4 lightSpace[NUM_SHADOWMAP_LAYERS];
	#else
		mat4 lightSpace;
	#endif
	sampler2DArray shadowMap;
};

struct Flashlight {
	vec3 pos;
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
in vec4 FragPosDirLS[MAX_NUM_DIR_LIGHTS * NUM_SHADOWMAP_LAYERS];

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
#if NUM_SHADOWMAP_LAYERS > 1
float shadow(vec4 fragPosLS[NUM_SHADOWMAP_LAYERS], DirectionalLight light);
#else
float shadow(vec4 fragPosLS, DirectionalLight light);
#endif

void main()
{
	vec3 viewDir = normalize(cameraPos - FragmentPos);
	vec3 result = vec3(0.0f, 0.0f, 0.0f);

	//for(int i = 0; i < nPointLights; i++) 
	//	result += calcPointLight(pLight[i], Normal, FragmentPos, viewDir, TexCoord); 
		
	for(int i = 0; i < nDirLights; i++)
		result += calcDirectionalLight(i, Normal, FragmentPos, viewDir, TexCoord);
		
	FragColor = vec4(result, 1.0f);
	//FragColor = vec4(texture(mat.texAmbient, TexCoord).xyz * mat.specularColor, 1.0f);
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
	
	#if NUM_SHADOWMAP_LAYERS > 1
	vec4 fragPositionsLS[NUM_SHADOWMAP_LAYERS];
	for(int k = 0; k < NUM_SHADOWMAP_LAYERS; k++){
		fragPositionsLS[k] = FragPosDirLS[i * NUM_SHADOWMAP_LAYERS + k];
	}
	float s = shadow(fragPositionsLS, dLight[i]);
	#else
	float s = shadow(FragPosDirLS[i], dLight[i]);
	#endif
	return (ambient + ( 1.0f - s ) * (diffuse + specular));
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

#if NUM_SHADOWMAP_LAYERS > 1
float shadow(vec4 fragPosLS[NUM_SHADOWMAP_LAYERS], DirectionalLight light)
#else
float shadow(vec4 fragPosLS, DirectionalLight light)
#endif
{
	float eps = 0.000125f;
	int layer;
	#if NUM_SHADOWMAP_LAYERS > 1
	vec3 projCoords;
	for(int i = 0; i < NUM_SHADOWMAP_LAYERS; i++){
		projCoords = fragPosLS[i].xyz / fragPosLS[i].w;
		layer = i;

		if(abs(projCoords.x) <= 1 && abs(projCoords.y) <= 1 && abs(projCoords.z) <= 1)
			break;
	}
	eps *= pow(8, layer) * 0.1;
	eps = min(eps, 0.125f);

	#else
	vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
	layer = 0;
	#endif
	
	if(abs(projCoords.x) > 1 || abs(projCoords.y) > 1 || abs(projCoords.z) > 1)
	 return 0.0f;

	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(light.shadowMap, vec3(projCoords.xy, layer)).r;  
	float currentDepth = projCoords.z;
	
	float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(light.shadowMap, 0).xy;

	int vals = 0;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
			float pcfDepth = texture(light.shadowMap, vec3(projCoords.xy + vec2(x * texelSize.x, y * texelSize.y), layer)).r;
            shadow += currentDepth - eps > pcfDepth ? 1.0 : 0.0;
			vals++;
        }    
    }
    shadow /= vals;

	return shadow;
}