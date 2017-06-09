#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec4 pos;
out vec2 texCoord;

void main()
{
	pos = vec4(position, 1.0f);
	texCoord = uv;
	gl_Position = pos;
}