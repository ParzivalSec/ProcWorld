#version 430 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;

uniform mat4 model;

out VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	
} vertexData;

void main()
{
	vertexData.position = (model * vec4(position, 1.0f)).xyz;
	vertexData.normal = (model* vec4(normal, 0.0f)).xyz;
	vertexData.texCoord = uv;
}