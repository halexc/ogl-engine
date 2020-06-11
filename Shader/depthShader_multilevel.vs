#version 330 core

layout (location = 0) in vec3 InPos;

uniform mat4 projectionView;
uniform mat4 model;

void main()
{
	gl_Position = projectionView * model * vec4(InPos, 1.0f);
	if(gl_Position.z < -1) gl_Position.z = -1;
}
