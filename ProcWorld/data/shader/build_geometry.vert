#version 410 core

layout(location = 0) in vec2 uv;

out VertexData
{
	vec4 wsCoord;
	vec3 uvw_3dtex;
	vec4 field0123;
	vec4 field4567;
	uint mc_case;
} vertex_data;

uniform sampler3D density_texture;

void main()
{
	int instance = gl_InstanceID;
}