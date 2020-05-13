#version 330 core

layout (location = 0) in vec3 InPos;

uniform mat4 lightSpace;
uniform mat4 model;

void main()
{
	gl_Position = lightSpace * model * vec4(InPos, 1.0f);
}