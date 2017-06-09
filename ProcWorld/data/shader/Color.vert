#version 410 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 eyeWorldPosition;

out VertexData
{
	vec3 coordinates;
	vec3 worldPosition;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	vec3 viewpos;
	
	vec3 tsEyePosition;
	vec3 tsPosition;
	vec3 tsLightPos;
} vertex_data;

void main()
{                  
	mat3 normalM = transpose(inverse(mat3(model)));
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ;
    gl_Position = projection * view * model * vec4(position, 1.0f);
	vertex_data.worldPosition = (model * vec4(position, 1.0f)).xyz;
	vertex_data.coordinates = position;
	vertex_data.normal = normalize(vec3(normal * normalM));
	vertex_data.tangent = tangent;
	vertex_data.bitangent = cross(tangent, normal);
	vertex_data.viewpos = eyeWorldPosition;
	
    vec3 T   = normalize(mat3(model) * tangent);
    vec3 B   = normalize(mat3(model) * vertex_data.bitangent);
    vec3 N   = normalize(mat3(model) * normal);
    mat3 TBN = transpose(mat3(T, B, N));
	
	vertex_data.tsEyePosition = TBN * eyeWorldPosition;
	vertex_data.tsPosition = TBN * vertex_data.worldPosition;
	vertex_data.tsLightPos = TBN * vec3(0, 10.0f, -10.0f);
	//vertex_data.normal = TBN * vertex_data.normal;
}