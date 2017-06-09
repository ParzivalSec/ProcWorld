#version 410 core

layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec4 pos;

void main()
{
	pos = lightSpaceMatrix * model * vec4(position, 1.0f);
	gl_Position = pos;
}