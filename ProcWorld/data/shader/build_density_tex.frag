#version 410 core
  
#define M_PI 3.1415926535897932384626433832795

layout(location = 0) out vec4 color;

in fData
{
	vec4 position;
	vec4 tex_coords;
	flat int instance_id;
} data;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

uniform sampler3D perlin_noise;

void main()
{
	float f = 0.0f;

	float x = data.tex_coords.x;
	float y = data.tex_coords.y;
	float z = data.tex_coords.z;
	
	vec3 center = vec3(0.5f, data.instance_id, 0.75f);
	
	vec3 pillar1 = vec3(center.x + 0.05f * sin(center.y * 0.05f), center.y, center.z + 0.05f * cos(center.y * 0.05f));
	vec3 pillar2 = vec3((center.x + 0.025f) + 0.2f * sin(center.y * -0.05f), center.y, (center.z + 0.025f) + 0.2f * cos(center.y * -0.05f));
	vec3 pillar3 = vec3((center.x - 0.025f) + 0.1f * sin(center.y * -0.025f), center.y, (center.z - 0.025f) + 0.15f * cos(center.y * -0.025f));
	// vec3 pillar2 = vec3(0.35f + 0.02 * sin(center.y * 50.0f + M_PI * 0.66f), center.y, 0.35f + 0.02f * cos(center.y * 50.0f + M_PI * 0.66f));
	// vec3 pillar3 = vec3(0.45f + 0.02 * sin(center.y * 50.0f + M_PI * 0.66f), center.y, 0.45f + 0.02f * cos(center.y * 50.0f + M_PI * 0.66f));
	
	// Create a rather big single pillar in the middle of the volume
	f += 1.0f / (length(data.tex_coords.xz - pillar1.xz) * 7.0f) - 1.0f;
	f += 1.0f / (length(data.tex_coords.xz - pillar2.xz) * 7.0f) - 1.0f;
	f += 1.0f / (length(data.tex_coords.xz - pillar3.xz) * 7.0f) - 1.0f;
	
	f += cos(data.tex_coords.y / 8.0f);
	
	// Negative pillar
	f -= 1.0f / (length(data.tex_coords.xz - center.xz) * 8.0f) - 1.0f;
	
	// Negative space at the boundaries of the texture 
	f -= pow(length(data.tex_coords.xz), 2.0f);
	
	// Fingers noise
	vec2 polarCoord;
    polarCoord.x = length(data.tex_coords.xz);
    polarCoord.y = atan(data.tex_coords.z/data.tex_coords.x);
    
    float freqX = 5; // SHOULD BE AN INTEGER. controls number of ripples around circumfrence
    float freqY = 12; // SHOULD BE AN INTEGER. controls finger height
    float fingerAmt = 0.25f; // how much influence they have
	
    float fingers = sin(polarCoord.y * freqX) * sin(data.tex_coords.y * freqY);
    fingers *= fingerAmt;
    fingers *= polarCoord.x; // so it's less pronounced in the center where they meet
    
    f += fingers;
	
	// // Wrap and setof ws coordinates
	// #define PRENOISE_FREQ_MULT 0.005 //0.005
	// #define PRENOISE_AMP_MULT  1.5   //3.5
	
	// vec4 distortedInput = data.tex_coords * PRENOISE_FREQ_MULT;
	
	// float noise =  texture(perlin_noise, distortedInput.xyz).x;
	// vec3 wsCoordsWarped;
	// wsCoordsWarped.x = 0.25f * noise * distortedInput.x;
	// wsCoordsWarped.y = 0.25f * noise * distortedInput.y;
	// wsCoordsWarped.z = 0.25f * noise * distortedInput.z;
	
	// // gopher holes/slices:
  // {
    // vec3 octaveCoord = wsCoordsWarped * vec3(0.5f, 0.8f, 0.7f).xyz; 
    // float t = texture(perlin_noise, octaveCoord.xyz*0.05).x;
    // float z = 0.6*(1-0.15f);
    // t = clamp(t*(1+z) - z, 0.0f, 1.0f);
    // t = t*t*(3 - 2*t);   // ENORMOUSLY BETTER
    // float dqq = -clamp(t*t*10, 0.0f, 1.0f)*0.3;  // so it doesn't emaciate the pillars
    // f += dqq * 0.8f * 0.4;
  // }
  
  	// float noise_2 =  texture(perlin_noise, vec3(data.instance_id * 0.25f, data.instance_id * 0.33f, data.instance_id * 0.75f)).x;
	// f += noise_2 * 0.25f;
	
	color = vec4(f, 0, 0, 0);}
