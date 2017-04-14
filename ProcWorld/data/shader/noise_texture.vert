#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;


out VertexData
{
	vec3 v_pos;
	vec3 v_uv;
	int instance_id;
} vertex_data;

void main()
{
	vertex_data.v_pos = position;
	vertex_data.v_uv = vec3(uv.x, gl_InstanceID, uv.y);
	vertex_data.instance_id = gl_InstanceID;
}