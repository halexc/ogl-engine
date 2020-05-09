#version 330 core

layout (location = 0) in vec3 InPos;
layout (location = 1) in vec3 InColor;

out vec3 FragmentPos;
out vec3 Normal;
out vec4 Color;
out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void main()
{
	Color = vec4(InColor, 1.0f);
	gl_Position = projection * view * model * vec4(InPos, 1.0f);
}