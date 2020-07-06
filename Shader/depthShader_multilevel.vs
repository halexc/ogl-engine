#version 330 core

#define NUM_LAYERS 3

layout (location = 0) in vec3 InPos;

uniform mat4 model;

void main()
{
	gl_Position = model * vec4(InPos, 1.0f);
}
