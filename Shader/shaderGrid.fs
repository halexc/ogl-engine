#version 330 core

out vec4 FragColor;

in vec3 FragmentPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 Color;

uniform vec3 cameraPos;

void main()
{
	FragColor = Color;
}