#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texImage;

void main()
{
	FragColor = vec4(0.25, 1.0, 0.25, 1.0);
}