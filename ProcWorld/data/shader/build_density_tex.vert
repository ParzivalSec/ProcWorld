#version 410 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out VertexData
{
	vec4 position;
	vec4 tex_coords;
	int instance_id;
} vertex_data;

void main()
{
	int instance = gl_InstanceID;
	vec4 projCoords = vec4(position, 1.0f);

	// From CASCADES demo shader extracted 
	// texture v coordinate is interpreted as z-coordinate in the 3D texture slice
	vertex_data.tex_coords = vec4(uv.x,  gl_InstanceID, uv.y, 1.0f);
	vertex_data.position = projCoords;
	vertex_data.instance_id = instance;
}