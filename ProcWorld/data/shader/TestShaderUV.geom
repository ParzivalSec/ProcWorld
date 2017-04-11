#version 410 core

layout(points) in;
layout(points, max_vertices = 1) out;

// layout(std140) uniform lut_poly {
	// uint  case_to_poly[256];
// };

// layout(std140) uniform lut_tri {
	// ivec4 case_to_triangles[1280];
// };

// in VertexData
// {
	// vec4 wsCoord;
	// vec3 uvw_3dtex;
	// vec4 field0123;
	// vec4 field4567;
	// uint mc_case;
// } vertex_data[];

in vec2 pos[];

void main()
{
	gl_Position = vec4(pos[0], 1.0f, 1.0f);
	EmitVertex();
	EndPrimitive();
}