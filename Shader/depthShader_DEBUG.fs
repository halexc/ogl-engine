#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

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

uniform Material mat;

void main()
{             
    float depthValue = texture(mat.texAmbient, TexCoords).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}