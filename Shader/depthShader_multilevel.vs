#version 330 core

layout (location = 0) in vec3 InPos;

uniform mat4 projectionView;
uniform mat4 model;

uniform bool hasParent;
uniform mat4 parentProjection;

out parentPosition;

void main()
{
	if(hasParent)
		parentPosition = parentProjection * model * vec4(InPos, 1.0f);
	
	gl_Position = projectionView * model * vec4(InPos, 1.0f);
}
