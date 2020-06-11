#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2DArray depthMaps;
uniform int layer;

void main()
{             
    float depthValue = texture(depthMaps, vec3(TexCoords, layer)).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}