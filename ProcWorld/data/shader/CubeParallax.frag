#version 410 core

in VertexData
{
	vec4 worldPosition;
	vec3 normal;
	vec2 uv;
	vec3 tangent;
	vec3 viewPos;
	
	vec3 tsEyePosition;
	vec3 tsPosition;
	vec3 tsLightPos;
} v_data;

uniform sampler2D diffuse;
uniform sampler2D height;
uniform sampler2D normalMap;

// Input needed for displacement mapping
uniform int initialSteps;
uniform int refinementSteps;

out vec4 color;

/**
* Parallax Displacement Mapping
* Raycasting to determine the fragment the viewer really sees based on 
* a depth map per texture, mor realistic effect of relief (+ normal maps even better)
**/
vec2 ParallaxDisplacement(vec2 tsEyeVec, vec2 world_uvs, sampler2D heightMap)
{
	int initS = initialSteps;
	int refS = refinementSteps;
	
	// Depth at with the depth becomes smaller each iteration step
	float stepSize = 1.0f / initS;
	float refinementSize = 1.0f / refS;
	
	vec2 newUVs = world_uvs;
	// UV displaced along the view ray multiplied with the strength of the parallax effect
	vec2 displacedUVs = -tsEyeVec.xy * 1.5f * 0.035f / initS;
	// Start witht he highest height
	float currentHeight = 1.0f;
	
	float prev_hits = 0.0f;
	float hit_height = 0.0f;
	
	// first set of initial steps to find the texetl 'surface' intersection
	for (int it = 0; it < initS; it++)
	{
		currentHeight -= stepSize;
		newUVs += displacedUVs;
		
		float texelHeight = texture2D(heightMap, newUVs).x;
		// We use dyamic branching to determine the height at which the ray intersects the surface
		// First hit will only be higher than 0 if prev_hits is 0 and the heightmap values is
		// is larger than the height of the current layer
		float is_first_hit = clamp((texelHeight - currentHeight - prev_hits) * 499999.0f, 0.0f, 1.0f);
		
		hit_height += is_first_hit * currentHeight;
		prev_hits += is_first_hit;
	}
	
	// offset the 'new start height' by a small amount to remove some pixel like aritfacts
	currentHeight = hit_height + stepSize - 0.025f;
	// Caclulate the UVs where w will start the refinement steps
	// The point is where the currently found uv would be but one displacement step behind
	newUVs = world_uvs + displacedUVs * (1.0f - hit_height) * initS - displacedUVs;
	
	vec2 tempUVs = newUVs;
	float startHeight = currentHeight;
	displacedUVs *= refinementSize;
	
	prev_hits = 0;
	hit_height = 0;
	
	// Second iteration to find a more accurate uv coordiate to return
	for(int it = 0; it < refS; it++)
	{
		currentHeight -= stepSize * refinementSize;
		newUVs += displacedUVs;
		
		// Again dymaic branching used
		float texelHeight = texture2D(heightMap, newUVs).x;
		float is_first_hit = clamp((texelHeight - currentHeight - prev_hits) * 499999.0f, 0.0f, 1.0f);
		
		hit_height += is_first_hit * currentHeight;
		prev_hits += is_first_hit;
	}
	
	// At the end calculate the refined uv coordinate based in the previously found uv coord
	newUVs = tempUVs + displacedUVs * (startHeight - hit_height) * initS * refS;
	
	return newUVs;
}

/* vec2 POM(vec2 world_uvs, vec3 viewDir)
{
	int initS = 16;
	int refS = 8;

	float stepSize = 1.0f / float(initS);
	float refinementSize = 1.0f / float(refS);
	
	//vec2 P = -viewDir.xy / viewDir.z * 0.1f;
	//vec2 deltaUVs = -P / float(initS);
	vec2 deltaUVs = viewDir.xy * 0.035f / float(initS);
	
	vec2 uv = world_uvs;
	float prev_hits = 0.0f;
	float hit_height = 0.0f;
	float heightValue = 1.0f;
	
	// Calculate the texel silloute intersection point
	for (int it = 0; it < initS; it++)
	{
		uv += deltaUVs;
		heightValue += stepSize;
				
		float heightMapValue = texture2D(height, uv).x;
		float is_first_hit = clamp((heightMapValue - heightValue - prev_hits) * 499999.0f, 0.0f, 1.0f);
		
		hit_height += is_first_hit * heightValue;
		prev_hits += is_first_hit;
	}
	
	// Use smallers teps now
	deltaUVs *= refinementSize;
	// Setback the height of the intersection by one step
	heightValue += stepSize;
	
	// Get the intersection uvs that shall be refined
	uv = world_uvs + deltaUVs * (1.0f - hit_height) * initS - deltaUVs;
	
	vec2 prev_uv = uv;
	float prev_height = heightValue;
	
	prev_hits = 0;
	hit_height = 0;
	// Calculate a more accurate texture coordinate
	for (int it = 0; it < refS; it++)
	{
		uv += deltaUVs;
		heightValue += stepSize * refinementSize;
				
		float heightMapValue = texture2D(height, uv).x;
		float is_first_hit = clamp((heightMapValue - heightValue - prev_hits) * 499999.0f, 0.0f, 1.0f);
		
		hit_height += is_first_hit * heightValue;
		prev_hits += is_first_hit;
	}
	
	uv = prev_uv + deltaUVs * (prev_height - hit_height) * initS * refS; 
	return uv;
} */

void main()
{
	vec3 eyeVec = v_data.tsEyePosition - v_data.tsPosition;
	eyeVec = normalize(eyeVec);
	
	vec2 displacedUVs = ParallaxDisplacement(eyeVec.xy, v_data.uv, height);
	
	if (displacedUVs.x < 0.0f || displacedUVs.x > 1.0f || displacedUVs.y < 0.0f || displacedUVs.y > 1.0f)
		discard;
	
	vec4 tex = texture2D(diffuse, displacedUVs);
	vec3 norm  = texture(normalMap, displacedUVs).xyz;
    norm = normalize(norm * 2.0 - 1.0);
	
	vec3 ambient = 0.05f * tex.xyz;
	vec3 lightDir  = normalize(v_data.tsLightPos - v_data.tsPosition.xyz);
	// Calculate lights diffuse component
	float diff = max(dot(lightDir, norm), 0.0f);
	vec3 diffuse = tex.xyz * diff;
	vec3 viewDirWorld = normalize(v_data.tsEyePosition - v_data.tsPosition.xyz);
	vec3 halfwayDir = normalize(lightDir + viewDirWorld);
	float spec = pow(max(dot(norm, halfwayDir), 0.0f), 800.0f);
	vec3 specular = tex.xyz * spec;
	
	vec3 lightedTex = ambient + diffuse + spec;
	
	color = vec4(lightedTex, 1.0f);
}