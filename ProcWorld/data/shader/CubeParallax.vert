#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 eyePosition;

out VertexData
{
	vec3 worldPosition;
	vec3 normal;
	vec2 uv;
	vec3 tangent;
	vec3 viewPos;
	
	vec3 tsEyePosition;
	vec3 tsPosition;
	vec3 tsLightPos;
} v_data;

void main()
{
	mat3 normalM = transpose(inverse(mat3(model)));
	
	gl_Position = projection * view * model * vec4(position, 1.0f);
	v_data.worldPosition = (model * vec4(position, 1.0f)).xyz;
	v_data.normal = normalize(vec3(normal * normalM));
	v_data.uv = uv;
	v_data.tangent = tangent;
	v_data.viewPos = eyePosition;
	
	// Create tangent space matrix
	vec3 bitangent = cross(normal, tangent);
    vec3 T   = normalize(mat3(model) * tangent);
    vec3 B   = normalize(mat3(model) * bitangent);
    vec3 N   = normalize(mat3(model) * normal);
    mat3 TBN = transpose(mat3(T, B, N));
	
	// Move stuff to tangent space
	v_data.tsEyePosition = TBN * eyePosition;
	v_data.tsPosition = TBN * v_data.worldPosition;
	v_data.tsLightPos = TBN * vec3(0.0f, 50.0f, -50.0f);
}