#version 330 core
layout (location = 0) in vec3 InPos;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec2 InTexCoord;
layout (location = 3) in vec4 InColor;
layout (location = 4) in vec3 InTangent;
layout (location = 5) in vec3 InBitangent;

out vec3 FragmentPos;
out vec3 Normal;
out vec4 Color;
out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	TexCoord = vec2(InTexCoord.x, InTexCoord.y);
	Normal = mat3(transpose(inverse(model))) * InNormal;
	FragmentPos = vec3(model * vec4(InPos, 1.0f));
	Color = InColor;
	
	gl_Position = projection * view * model * vec4(InPos, 1.0f);
}