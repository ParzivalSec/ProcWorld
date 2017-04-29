#version 410 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VertexData
{
	vec3 coordinates;
	vec3 normal;
} vertex_data;

void main()
{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ;
    gl_Position = projection * view * model * vec4(position, 1.0f);
	vertex_data.coordinates = position;
	vertex_data.normal = normal;
}