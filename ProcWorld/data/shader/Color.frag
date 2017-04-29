#version 410 core

in VertexData
{
	vec3 coordinates;
	vec3 normal;
} vertex_data;

// 3 textures used for bledning these three in the process
// of tri-planar texturing (creates a mor enatural look)
uniform sampler2D mossTexture;
uniform sampler2D rockTexture;
uniform sampler2D sandTexture;

out vec4 color;

void main()
{
	// TRIPLANAR TEXTURING: START
	vec3 blending = abs(vertex_data.normal);
	blending = normalize(max(blending, 0.00001));
	float b = (blending.x + blending.y + blending.z);
	blending /= vec3(b, b, b);
	
	// Use the world space coords and a scale factor to sample
	// from 3 different textures, one for each projection plane
	float scale = 4.0f;
	vec4 x_axis = texture2D(sandTexture, vertex_data.coordinates.yz * scale);
	vec4 y_axis = texture2D(mossTexture, vertex_data.coordinates.xz * scale);
	vec4 z_axis = texture2D(rockTexture, vertex_data.coordinates.xy * scale);
	vec4 tex = x_axis * blending.x + y_axis * blending.y + z_axis * blending.z;
	// TRIPLANAR TEXTURING: END
	
	color = vec4(tex.xyz, 1.0f);
} 