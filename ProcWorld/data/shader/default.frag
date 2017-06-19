#version 410 core

uniform vec3 fragColor;

in vec4 vpeye;
in vec4 vneye;
in vec2 texCoord;
in vec4 shadowCaster;

out vec4 outColor;

uniform sampler2D shadowMap;

vec4 scPostW;

float chebyshevUpperBound(float distance)
{
// Firstly sample the moments from the depth texture, stored in R and G channel
	vec2 moments = texture2D(shadowMap, scPostW.xy).rg;
	
	// Surface is not in shadow if it is before every occluder
	if (distance <= moments.x)
		return 1.0f;
		
	// If we get here, the fragment is either in shadow or in the penumbra of the shadow.
	// To receive the lit factore we use a chebyshev upperBound to check how likely
	// it is in the shadow
	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.002f);
	
	float d = distance - moments.x;
	float p_max = variance /(variance + d * d);
	
	return p_max;
}

void main()
{
	vec3 fragment = vec3(vpeye);
	vec3 normal = vec3(normalize(vneye));
	vec3 viewDir = normalize(-fragment);
	
	/* Shadows */
	scPostW = shadowCaster / shadowCaster.w;
	scPostW = scPostW * 0.5f + 0.5f;
	
	float shadowFactor = 1.0f;
	
	// Check if we are outside of the shadowmap, if not we can calulate the amount the fragment should be shadowed using
	// a chebyshev upper bound for the depth value
	bool outsideShadowMap = shadowCaster.w <= 0.0f || (scPostW.x < 0 || scPostW.y < 0) || (scPostW.x >= 1 || scPostW.y >= 1);
	if (!outsideShadowMap)
	{
		shadowFactor = chebyshevUpperBound(scPostW.z);
	}

    outColor = vec4(fragColor * shadowFactor, 1.0f);
} 