#version 410 core
#pragma optimize (off)

layout(points) in;
layout(triangle_strip, max_vertices = 15) out;

layout(shared) uniform lut_poly {
	uint  case_to_poly[256];
};

layout(shared) uniform lut_tri {
	ivec4 case_to_triangles[1280];
};

// in VertexData
// {
	// vec4 wsCoord;
	// vec3 uvw_3dtex;
	// vec4 field0123;
	// vec4 field4567;
	// uint mc_case;
// } vertex_data[];

in DebugData
{
	vec3 textureCoords;
} debug[];

in vec3 pos[];
out vec3 position;

void main()
{
	position = vec3(-2, debug[0].textureCoords.x, -2);
	EmitVertex();
	position = vec3(-3, pos[0].y, -3);
	EmitVertex();
	position = vec3(-4, pos[0].z, -4);
	EmitVertex();
	EndPrimitive();
}