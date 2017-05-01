#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 15) out;

in VertexData
{
	vec4 position;
	int mc_case;
	vec4 field0123;
	vec4 field4567;
	vec3 cube_coords[8];
} vertex_data[];

uniform sampler3D density_texture;

layout(shared) uniform lut_poly {
	uint  case_to_poly[256];
};

layout(shared) uniform lut_tri {
	int case_to_triangles[4096];
};

out vec3 position;
out vec3 normal;
out vec3 tangent;

void interpolateVertex(in float iso, in vec3 vec0, in float dens0, in vec3 vec1, in float dens1, inout vec3 vertex)
{
	float interpolation_value = (iso - dens0) / (dens1 - dens0);
	vertex = mix(vec0, vec1, interpolation_value);
}

vec3 calculateNormal(vec3 uvw, sampler3D tex, vec4 step_s)
{
	vec3 gradient = vec3(
		texture(tex, uvw + step_s.xww).x - texture(tex, uvw - step_s.xww).x,
		texture(tex, uvw + step_s.wwy).x - texture(tex, uvw - step_s.wwy).x,
		texture(tex, uvw + step_s.wzw).x - texture(tex, uvw - step_s.wzw).x
	);
	
	return normalize(-gradient);
}

// Calculate tangent for the triagnel given by the three points
// Due to the fact that the traain is generated procedurally
// the pos equals the uvs.
vec3 calculateTangent(vec3 point_1, vec3 point_2, vec3 point_3)
{
	vec3 edge_one = point_2 - point_1;
	vec3 edge_two = point_3 - point_1;
	vec2 deltaUV1 = point_2.xy - point_1.xy;
	vec2 deltaUV2 = point_3.xy - point_1.xy;
	
	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	
	vec3 tangent;
	tangent.x = f * (deltaUV2.y * edge_one.x - deltaUV1.y * edge_two.x);
	tangent.y = f * (deltaUV2.y * edge_one.y - deltaUV1.y * edge_two.y);
	tangent.z = f * (deltaUV2.y * edge_one.z - deltaUV1.y * edge_two.z);
	
	return normalize(tangent);
}

void main()
{
	uint num_poly = case_to_poly[vertex_data[0].mc_case];
	uint table_pos = vertex_data[0].mc_case * 16;
	
	vec3 vertexList[12];
	
	interpolateVertex(0, vertex_data[0].cube_coords[0], vertex_data[0].field0123.x, vertex_data[0].cube_coords[1], vertex_data[0].field0123.y, vertexList[0]);
	interpolateVertex(0, vertex_data[0].cube_coords[1], vertex_data[0].field0123.y, vertex_data[0].cube_coords[2], vertex_data[0].field0123.z, vertexList[1]);
	interpolateVertex(0, vertex_data[0].cube_coords[2], vertex_data[0].field0123.z, vertex_data[0].cube_coords[3], vertex_data[0].field0123.w, vertexList[2]);
	interpolateVertex(0, vertex_data[0].cube_coords[3], vertex_data[0].field0123.w, vertex_data[0].cube_coords[0], vertex_data[0].field0123.x, vertexList[3]);
	
	interpolateVertex(0, vertex_data[0].cube_coords[4], vertex_data[0].field4567.x, vertex_data[0].cube_coords[5], vertex_data[0].field4567.y, vertexList[4]);
	interpolateVertex(0, vertex_data[0].cube_coords[5], vertex_data[0].field4567.y, vertex_data[0].cube_coords[6], vertex_data[0].field4567.z, vertexList[5]);
	interpolateVertex(0, vertex_data[0].cube_coords[6], vertex_data[0].field4567.z, vertex_data[0].cube_coords[7], vertex_data[0].field4567.w, vertexList[6]);
	interpolateVertex(0, vertex_data[0].cube_coords[7], vertex_data[0].field4567.w, vertex_data[0].cube_coords[4], vertex_data[0].field4567.x, vertexList[7]);
	
	interpolateVertex(0, vertex_data[0].cube_coords[0], vertex_data[0].field0123.x, vertex_data[0].cube_coords[4], vertex_data[0].field4567.x, vertexList[8]);
	interpolateVertex(0, vertex_data[0].cube_coords[1], vertex_data[0].field0123.y, vertex_data[0].cube_coords[5], vertex_data[0].field4567.y, vertexList[9]);
	interpolateVertex(0, vertex_data[0].cube_coords[2], vertex_data[0].field0123.z, vertex_data[0].cube_coords[6], vertex_data[0].field4567.z, vertexList[10]);
	interpolateVertex(0, vertex_data[0].cube_coords[3], vertex_data[0].field0123.w, vertex_data[0].cube_coords[7], vertex_data[0].field4567.w, vertexList[11]);
	
	vec3 inv_voxelSize = vec3(1/95.0f, 1/256.0f, 1/95.0f);
	vec4 step = vec4(inv_voxelSize, 0.0f);
	
	for (uint i = 0; case_to_triangles[table_pos + i] != -1; i += 3)
	{
		vec3 point_1 = vec3(vertexList[case_to_triangles[table_pos + i + 0]]).xzy;
		vec3 point_2 = vec3(vertexList[case_to_triangles[table_pos + i + 1]]).xzy;
		vec3 point_3 = vec3(vertexList[case_to_triangles[table_pos + i + 2]]).xzy;
		vec3 surfaceTangent = calculateTangent(point_1, point_2, point_3);
		// wsurfaceTangent = vec3(0.0f, -1.0f, 0.0f);
		vec3 surfaceNormal = calculateNormal(vertex_data[0].position.xzy, density_texture, step);
		
		position = point_1;
		normal = calculateNormal(position.xzy, density_texture, step);
		tangent = surfaceTangent;
		EmitVertex();
		position = point_2;
		normal = calculateNormal(position.xzy, density_texture, step);
		tangent = surfaceTangent;
		EmitVertex();
		position = point_3;
		normal = calculateNormal(position.xzy, density_texture, step);
		tangent = surfaceTangent;
		EmitVertex();
		
		EndPrimitive();
	}
}