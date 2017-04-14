#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData
{
	vec3 v_pos;
	vec3 v_uv;
	int instance_id;
} vertex_data[];

out fData
{
	vec4 position;
	vec3 uv;
	flat int instance_id;
} frag;

void main()
{
	gl_Layer = vertex_data[0].instance_id;
	
	for (int i = 0; i < 3; ++i)
	{
		frag.position = vec4(vertex_data[i].v_pos, 1.0f);
		frag.uv = vec3(vertex_data[i].v_uv);
		frag.instance_id = vertex_data[i].instance_id;
		// TODO: check if there is a possibility that i don;t have to set gl_Position
		// and where the FS is invoked however
		gl_Position = frag.position;
		EmitVertex();
	}
	EndPrimitive();
}