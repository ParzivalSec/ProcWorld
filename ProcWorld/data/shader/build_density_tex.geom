#version 410 core
  
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
 
in VertexData
{
	vec4 position;
	vec4 tex_coords;
	int instance_id;
} vertex_data[];

out fData
{
	vec4 position;
	vec4 tex_coords;
	flat int instance_id;
} frag;

void main()
{
	gl_Layer = vertex_data[0].instance_id;
	
	for (int vert = 0; vert < 3; ++vert)
	{
		frag.position = vertex_data[vert].position;
		frag.tex_coords = vertex_data[vert].tex_coords;
		frag.instance_id = vertex_data[vert].instance_id;
		gl_Position = vertex_data[vert].position;
		EmitVertex();
	}
	EndPrimitive();
}