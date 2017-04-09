#version 410 core
  
layout(location = 0) out vec4 color;

in fData
{
	vec4 position;
	vec4 tex_coords;
	flat int instance_id;
} data;

// TODO: Add sampler2D for noise textures (different octaves of noise)
// TODO: Add more itneresting features as helix and pillars, ...

void main()
{
	float f = 0.0f;
	
	float x = data.tex_coords.x;
	float y = data.tex_coords.y;
	float z = data.tex_coords.z;
	
	// Create a rather big single pillar in the middle of the volume
	f += 1.0f / length(data.tex_coords.xz - vec2(0.5f, 0.5f) - 1.0f);
	
	// Negative space at the boundaries of the texture 
	f -= pow(length(data.tex_coords.xz), 3);
	
	color = vec4(f, 0, 0, 0);}
