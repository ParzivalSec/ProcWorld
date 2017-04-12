#version 410 core
#pragma optimize (off)

layout (location = 0) in vec2 uv;

out VertexData
{
	vec4 wsCoord;
	vec3 uvw_3dtex;
	vec4 field0123;
	vec4 field4567;
	uint mc_case;
} vertex_data;

out DebugData
{
	vec3 textureCoords;
} debug;

uniform sampler3D density_texture;

layout (shared) uniform yPos {
	float y_pos_slice[256];
};

layout (shared) uniform yPosAbove {
	float y_pos_slice_above[256];
};

out vec3 pos;

void main()
{
	int instance = gl_InstanceID;
	
	vec3 wsCoord;
	wsCoord.xz = (uv.xy + 1.0f) / 2.0f;
	// // TODO: Sample y position from uniform buffer
	wsCoord.y  = y_pos_slice[instance];
	
	vec3 uvw = wsCoord.xzy;
	pos = uvw;
	
	debug.textureCoords.x = instance;
	debug.textureCoords.y = instance;
	debug.textureCoords.z = instance;
	
	pos.x = texture(density_texture, vec3(0, 0, 0)).x;
	// vec3 inv_voxelDimMinusOne = vec3(1.0f / 95.0f, 1.0f / 256.0f, 1.0f / 95.0f);
	
	// vec4 step = vec4(inv_voxelDimMinusOne.xzy, 0.0f);
	// uint cube_case = 0;
	// vec4 field0123;
	// vec4 field4567;
	
	// // // TODO: Sample corner data from the 3D tex
	// field0123.x = uvw.x; //texture(density_texture, vec3(uvw + step.www)).x;
	// field0123.y = 12.0f; //texture(density_texture, vec3(uvw + step.wwz)).x;
	// field0123.z = texture(density_texture, vec3(uvw + step.xwz)).x;
	// field0123.w = texture(density_texture, vec3(uvw + step.xww)).x;
	
	// field4567.x = texture(density_texture, vec3(uvw + step.wyw)).x;
	// field4567.y = texture(density_texture, vec3(uvw + step.wyz)).x;
	// field4567.z = texture(density_texture, vec3(uvw + step.xyz)).x;
	// field4567.w = texture(density_texture, vec3(uvw + step.xyw)).x;
	
	// // // Clamp every value to either 0 (negaitve) or 1 (positive)
	// uvec4 i0123 = uvec4(clamp(field0123*99999, 0.0f, 1.0f));
	// uvec4 i4567 = uvec4(clamp(field4567*99999, 0.0f, 1.0f));

	// // // Assemlble the cube_case and fit it into a 8bit unsigned int (2 ^ 8 = 256 cases)
	// cube_case = (i0123.x) | (i0123.y << 1) | (i0123.z << 2) | (i0123.w << 3) | (i4567.x << 4) | (i4567.y << 5) | (i4567.z << 6) | (i4567.w << 7);
	
	// vertex_data.wsCoord = vec4(wsCoord, 0.0f);
	// vertex_data.field0123 = vec4(10.0f, 12.0f, 12.0f, 12.0f);
	// vertex_data.field4567 = field4567;
	// vertex_data.mc_case = i0123.x;
}