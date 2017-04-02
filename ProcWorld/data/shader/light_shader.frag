#version 410 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float shininess;
};

struct LightSource
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform LightSource light[2];
uniform vec3 cameraPos;

// textures
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D shadowMap;
uniform sampler2D shadowMap_2;

uniform float bumpynessFactor;
uniform bool  hasNormalMap;

in VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 FragPosLightSpace;
	vec4 FragPosLightSpace_2;
	mat3 TBN;
	
} fs_in;

in TBN_Trans
{
	vec3 TangentLight1Pos;
	vec3 TangentLight2Pos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	
} tbn_trans;


out vec4 color;

float CalculateShadows(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, sampler2D tex)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; //fragPosLightSpace position in range [-1,1]
	projCoords = projCoords * 0.5 + 0.5; // To match depth range [0, 1]
	float closestDepth = texture(tex, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(tex, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(tex, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	
	if(projCoords.z > 1.0)
		shadow = 0.0;
	
	return shadow;
}


void main()
{
	// Calculate light vector l
	// If there was a non-uniform scale I should take the transpose of the 
	// inverse of the model matrix -> mat3(transpose(inverse(model))) * normal
	// vec3 norm = normalize(fs_in.Normal); 
	vec3 norm = texture(texture_normal, fs_in.TexCoord).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	
	if (hasNormalMap)
		norm = mix(norm, fs_in.Normal, bumpynessFactor);
	else
		norm = fs_in.Normal;
	
	// Get color of object (texture)
	vec3 texColor = vec3(texture(texture_diffuse, fs_in.TexCoord));
	
	// Ambient component
	vec3 ambient = 0.1f * texColor;
	
	vec3 lightDir  = vec3(0, 0, 0);

	vec3 lightDirWorld = normalize(light[0].position - fs_in.FragPos);
	vec3 lightDirTangent = normalize(tbn_trans.TangentLight1Pos - tbn_trans.TangentFragPos);
	
	// lightDir = lightDirTangent; //mix(lightDirTangent, lightDirWorld, bumpynessFactor);
	
	if (hasNormalMap)
		lightDir = lightDirTangent; 
	else
		lightDir = lightDirWorld;
	
	// Calculate lights diffuse component
	float diff = max(dot(lightDir, norm), 0.0f);
	vec3 diffuse = texColor * diff;
	
	// Calculate specular component ...
	//vec3 
	vec3 viewDir = vec3(0, 0, 0);
	
	vec3 viewDirWorld = normalize(cameraPos - fs_in.FragPos);
	vec3 viewDirTangent = normalize(tbn_trans.TangentViewPos - tbn_trans.TangentFragPos);
	
	if (hasNormalMap)
		viewDir = viewDirTangent; //mix(viewDirTangent, viewDirWorld, bumpynessFactor);
	else
		viewDir = viewDirWorld;
	
	// Blinn-Phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0f), material.shininess);
	vec3 specular = texColor * spec;
	// Calculate shadow
	float shadow_1 = CalculateShadows(fs_in.FragPosLightSpace, norm, lightDir, shadowMap);

	// *****
	// Calculations for second light
	// *****
	vec3 ambient_2 = 0.1f * texColor;
	// Calculate light vector l
	
	vec3 lightDir_2 = vec3(0, 0, 0);
	
	vec3 lightDirWorld_2 = normalize(light[1].position - fs_in.FragPos);
	vec3 lightDirTangent_2 = normalize(tbn_trans.TangentLight2Pos - tbn_trans.TangentFragPos);

	if (hasNormalMap)
		lightDir_2 = lightDirTangent_2; //mix(lightDirTangent_2, lightDirWorld_2, bumpynessFactor);
	else
		lightDir_2 = lightDirWorld_2;
		
	// Calculate lights diffuse component
	diff = max(dot(lightDir_2, norm), 0.0f);
	vec3 diffuse_2 = texColor * diff;
	// Blinn-Phong
	halfwayDir = normalize(lightDir_2 + viewDir);
	spec = pow(max(dot(norm, halfwayDir), 0.0f), material.shininess);
	vec3 specular_2 = texColor * spec;
	// Calculate shadows of second directional light
	float shadow_2 = CalculateShadows(fs_in.FragPosLightSpace_2, norm, lightDir_2, shadowMap_2);
	
	// Calculate final fragment color
	vec3 ambient_avg = (ambient + ambient_2);
	vec3 diffuse_avg = (diffuse + diffuse_2);
	vec3 specular_avg = (specular + specular_2);
	float shadow = (shadow_1 + shadow_2);
	
	
	//vec3 result = (ambient_avg + (1 - shadow) * (diffuse_avg + specular_avg));
	//diffuse_avg *= (1-shadow);
	//result = (ambient_avg + diffuse_avg + specular_avg) * (1-shadow);
	vec3 result =  (ambient_avg + (1.0 - shadow) * (diffuse_avg + specular_avg)); 
	color = vec4(result, 1.0f);
	
	//color = vec4(texColor, 1.0f);
}