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

void main()
{
    // empty fragment shader
} 