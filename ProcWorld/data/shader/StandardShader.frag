#version 410 core

in VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	
} vertexData;

out vec4 color;

void main()
{
	color = vec4(0.5f, 0.4f, 0.1f, 1.0f);
}