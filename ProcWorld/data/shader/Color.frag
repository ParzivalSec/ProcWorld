#version 410 core

in VertexData
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

// 3 textures used for bledning these three in the process
// of tri-planar texturing (creates a mor enatural look)
uniform sampler2D mossTexture;
uniform sampler2D rockTexture;
uniform sampler2D sandTexture;

uniform sampler2D mossHeightMap;
uniform sampler2D rockHeightMap;
uniform sampler2D sandHeightMap;

uniform sampler2D mossNormalMap;
uniform sampler2D rockNormalMap;
uniform sampler2D sandNormalMap;

// Input needed for displacement mapping
uniform int initialSteps;
uniform int refinementSteps;

out vec4 color;

vec2 ParallaxDisplacement(vec2 tsEyeVec, vec2 world_uvs, sampler2D heightMap)
{
	int initS = initialSteps;
	int refS = refinementSteps;
	
	float stepSize = 1.0f / initS;
	float refinementSize = 1.0f / refS;
	
	vec2 newUVs = world_uvs;
	vec2 displacedUVs = -tsEyeVec.xy * 0.1f / initS;
	float currentHeight = 1.0f;
	
	float prev_hits = 0.0f;
	float hit_height = 0.0f;
	
	// first set of initial steps to find the texetl 'surface' intersection
	for (int it = 0; it < initS; it++)
	{
		currentHeight -= stepSize;
		newUVs += displacedUVs;
		
		float texelHeight = texture2D(heightMap, newUVs).x;
		float is_first_hit = clamp((texelHeight - currentHeight - prev_hits) * 499999.0f, 0.0f, 1.0f);
		
		hit_height += is_first_hit * currentHeight;
		prev_hits += is_first_hit;
	}
	
	currentHeight = hit_height + stepSize - 0.0085f;
	newUVs = world_uvs + displacedUVs * (1.0f - hit_height) * initS - displacedUVs;
	
	vec2 tempUVs = newUVs;
	float startHeight = currentHeight;
	displacedUVs *= refinementSize;
	
	prev_hits = 0;
	hit_height = 0;
	
	for(int it = 0; it < refS; it++)
	{
		currentHeight -= stepSize * refinementSize;
		newUVs += displacedUVs;
		
		float texelHeight = texture2D(heightMap, newUVs).x;
		float is_first_hit = clamp((texelHeight - currentHeight - prev_hits) * 499999.0f, 0.0f, 1.0f);
		
		hit_height += is_first_hit * currentHeight;
		prev_hits += is_first_hit;
	}
	
	newUVs = tempUVs + displacedUVs * (startHeight - hit_height) * initS * refS;
	
	return newUVs;
}

void main()
{
	// PIXEL SHADER DISPLACEMENT MAPPING: BEGIN		
	vec2 coords[3];
	float scale = 1.0f;
	vec3 eyeVector = normalize(vertex_data.viewpos - vertex_data.worldPosition);
		
	for (int axis = 0; axis < 3; axis++)
	{
		if (axis == 0)
		{
			eyeVector = normalize(vertex_data.viewpos - vertex_data.worldPosition);
			eyeVector.x = dot(eyeVector, vec3(0.0f, 1.0f, 0.0f));
			eyeVector.y = dot(eyeVector, vec3(0.0f, 0.0f, 1.0f));
			coords[axis] = ParallaxDisplacement(eyeVector.yz, vertex_data.worldPosition.yz * scale, sandHeightMap);
		}
		else if (axis == 1)
		{	
			eyeVector = normalize(vertex_data.viewpos - vertex_data.worldPosition);
			eyeVector.x = dot(eyeVector, vec3(1.0f, 0.0f, 0.0f));
			eyeVector.y = dot(eyeVector, vec3(0.0f, 0.0f, 1.0f));
			//eyeVector = normalize(eyeVector);
			coords[axis] = ParallaxDisplacement(eyeVector.xz, vertex_data.worldPosition.xz * scale, mossHeightMap);
		}
		else
		{
			eyeVector = normalize(vertex_data.viewpos - vertex_data.worldPosition);
			eyeVector.x = dot(eyeVector, vec3(0.0f, 1.0f, 0.0f));
			eyeVector.y = dot(eyeVector, vec3(1.0f, 0.0f, 0.0f));
			coords[axis] = ParallaxDisplacement(eyeVector.xy, vertex_data.worldPosition.xy * scale, rockHeightMap);
		}
	}
	
	// PIXEL SHADER DISPLACEMENT MAPPING: END

	// TRIPLANAR TEXTURING: START
	vec3 blending = abs(vertex_data.normal);
	blending = normalize(max(blending, 0.00001));
	float b = (blending.x + blending.y + blending.z);
	blending /= vec3(b, b, b);
	
	// Use the world space coords and a scale factor to sample
	// from 3 different textures, one for each projection plane
	vec4 x_axis, y_axis, z_axis;
	vec4 normal_x, normal_y, normal_z;
	
	if (true)
	{
		x_axis = texture2D(sandTexture, coords[0]);	
		y_axis = texture2D(mossTexture, coords[1]);
		z_axis = texture2D(rockTexture, coords[2]);
		
		normal_x = texture2D(sandNormalMap, coords[0]);	//normal_x = normalize(normal_x * 2.0 - 1.0);
		normal_y = texture2D(mossNormalMap, coords[1]); //normal_y = normalize(normal_y * 2.0 - 1.0);
		normal_z = texture2D(rockNormalMap, coords[2]); //normal_z = normalize(normal_z * 2.0 - 1.0);
	}
	else
	{
		x_axis = texture2D(sandTexture, vertex_data.worldPosition.yz * scale);
		y_axis = texture2D(mossTexture, vertex_data.worldPosition.xz * scale);
		z_axis = texture2D(rockTexture, vertex_data.worldPosition.xy * scale);
	}

	vec4 tex = x_axis * blending.x + y_axis * blending.y + z_axis * blending.z;
	vec4 norm = normal_x * blending.x + normal_y * blending.y + normal_z * blending.z;
	norm = normalize(norm * 2.0 - 1.0);
	// TRIPLANAR TEXTURING: END
	
	// LIGHTING: BEGIN
	
	vec3 ambient = 0.8f * tex.xyz;
	vec3 lightDir  = normalize(vertex_data.tsLightPos - vertex_data.tsPosition.xyz);
	// Calculate lights diffuse componentw
	float diff = max(dot(lightDir, norm.xyz), 0.0f);
	vec3 diffuse = tex.xyz * diff;
	vec3 viewDirWorld = normalize(vertex_data.tsEyePosition - vertex_data.tsPosition.xyz);
	vec3 halfwayDir = normalize(lightDir + viewDirWorld);
	float spec = pow(max(dot(norm.xyz, halfwayDir), 0.0f), 25.0f);
	vec3 specular = tex.xyz * spec;
	
	vec3 lightedTex = ambient + diffuse * 0.5f;
	
	color = vec4(lightedTex.xyz, 1.0f);
} 