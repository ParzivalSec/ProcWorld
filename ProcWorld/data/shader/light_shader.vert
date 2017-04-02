#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform mat4 lightSpaceMatrix_2;

struct LightSource
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform LightSource light[2];

uniform vec3 cameraPos;

out VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 FragPosLightSpace;
	vec4 FragPosLightSpace_2;
	mat3 TBN;
	
} vs_out;

out TBN_Trans
{
	vec3 TangentLight1Pos;
	vec3 TangentLight2Pos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	
} tbn_trans;

void main()
{
	mat3 normalM = transpose(inverse(mat3(model)));
		
	gl_Position = projection * view * model * vec4(position, 1.0f);
	vs_out.FragPos = vec3(model * vec4(position, 1.0f));
	vs_out.Normal = normalize(vec3(normal * normalM));
	vs_out.TexCoord = texCoord;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0f);
	vs_out.FragPosLightSpace_2 = lightSpaceMatrix_2 * vec4(vs_out.FragPos, 1.0f);
	
	// Create the Tangent-Bitangent-Normal matrix (TBN)
	vec3 bitangent = cross(tangent, normal);
	
	vec3 T = normalize(vec3(normalM * tangent));
	vec3 B = normalize(vec3(normalM * bitangent));
	vec3 N = normalize(vec3(normalM * normal));
	
	vs_out.TBN = mat3(T, B, N);
	
	// Create the transpose of the TBN mat - equals the inverse matrix because it is a orthogonal base (ortho vecs)
	mat3 TBN_inv = transpose(vs_out.TBN);
	
	// Now transform the lightPos, viewPos & fragPos to tangent space
	// Equals the transform into another coordinate system just as model to world, screen, ...
	tbn_trans.TangentViewPos = TBN_inv * cameraPos;
	tbn_trans.TangentLight1Pos = TBN_inv * light[0].position;
	tbn_trans.TangentLight2Pos = TBN_inv * light[1].position;
	tbn_trans.TangentFragPos = TBN_inv * vs_out.FragPos;
}