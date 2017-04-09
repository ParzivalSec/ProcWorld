#version 410 core

layout(points) in;
layout(triangle_strip, max_vertices = 15) out;

layout(std140) uniform lut_poly {
	uint  case_to_poly[256];
};

layout(std140) uniform lut_tri {
	ivec4 case_to_triangles[1280];
};

in VertexData
{
	vec4 wsCoord;
	vec3 uvw_3dtex;
	vec4 field0123;
	vec4 field4567;
	uint mc_case;
} vertex_data[];

void main()
{
	
}