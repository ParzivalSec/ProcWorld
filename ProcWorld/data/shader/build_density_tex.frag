#version 410 core
  
#define M_PI 3.1415926535897932384626433832795

layout(location = 0) out vec4 color;

in fData
{
	vec4 position;
	vec4 tex_coords;
	flat int instance_id;
} data;

// TODO: Add sampler2D for noise textures (different octaves of noise)
// TODO: Add more itneresting features as helix and pillars, ...

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	float f = 0.0f;

	float x = data.tex_coords.x;
	float y = data.tex_coords.y;
	float z = data.tex_coords.z;
	
	vec3 center = vec3(0.5f, data.instance_id, 0.75f);
	
	vec3 pillar1 = vec3(center.x + 0.1f * sin(center.y * 0.05f), center.y, center.z + 0.1f * cos(center.y * 0.05f));
	vec3 pillar2 = vec3((center.x + 0.025f) + 0.15f * sin(center.y * -0.05f), center.y, (center.z + 0.025f) + 0.15f * cos(center.y * -0.05f));
	vec3 pillar3 = vec3((center.x - 0.025f) + 0.20f * sin(center.y * -0.025f), center.y, (center.z - 0.025f) + 0.20f * cos(center.y * -0.025f));
	// vec3 pillar2 = vec3(0.35f + 0.02 * sin(center.y * 50.0f + M_PI * 0.66f), center.y, 0.35f + 0.02f * cos(center.y * 50.0f + M_PI * 0.66f));
	// vec3 pillar3 = vec3(0.45f + 0.02 * sin(center.y * 50.0f + M_PI * 0.66f), center.y, 0.45f + 0.02f * cos(center.y * 50.0f + M_PI * 0.66f));
	
	// Create a rather big single pillar in the middle of the volume
	f += 1.0f / (length(data.tex_coords.xz - pillar1.xz) * 7.0f) - 1.0f;
	f += 1.0f / (length(data.tex_coords.xz - pillar2.xz) * 7.0f) - 1.0f;
	f += 1.0f / (length(data.tex_coords.xz - pillar3.xz) * 7.0f) - 1.0f;
	
	f += cos(data.tex_coords.y / 4.0f);
	
	// Negative pillar
	f -= 1.0f / (length(data.tex_coords.xz - center.xz) * 5.0f) - 1.0f;
	
	// Negative space at the boundaries of the texture 
	f -= pow(length(data.tex_coords.xz), 2.0f);
	
	// f += rand(vec2(2, data.instance_id / 4));
	
	color = vec4(f, 0, 0, 0);}
