#version 410 core

#define PI 3.1415926535897932384626433832795

layout(points) in;
layout(points) out;
layout(max_vertices = 40) out;

// Particle data retrieved from the VertexShader
in Particle
{
	vec3 position;
	vec3 velocity;
	float lifeTime;
	int type;
} particle[];

struct InternalParticle
{
	vec3 position;
	vec3 velocity;
	float lifeTime;
	int type;
};

// Output for TramsformFeedback
out vec3 outPosition;
out vec3 outVelocity;
out float outLifeTime;
out int outType;

// Uniforms we need
uniform float SmokeFrequency;
uniform float SmokeAccumulator;
uniform float SmokeLifetimeInitial;

uniform float RocketFrequencyMin;
uniform float RocketFrequencyMax;
uniform float RocketAccumulator;
uniform float RocketLifetimeInitial;
uniform int RocketAmount;

uniform int FireworkAmount;
uniform float FireworkLifetimeInitial;

// Time passed since last frame
uniform float DeltaTime;

// To be able to generate random numbers on the GPU
uniform vec3 RandomSeed; // Seed number for our random number function
vec3 LocalSeed; 

// This function returns random number from zero to one
float randZeroOne() 
{ 
    uint n = floatBitsToUint(LocalSeed.y * 214013.0 + LocalSeed.x * 2531011.0 + LocalSeed.z * 141251.0); 
    n = n * (n * n * 15731u + 789221u); 
    n = (n >> 9u) | 0x3F800000u; 
  
    float fRes =  2.0 - uintBitsToFloat(n); 
    LocalSeed = vec3(LocalSeed.x + 147158.0 * fRes, LocalSeed.y*fRes  + 415161.0 * fRes, LocalSeed.z + 324154.0*fRes); 
    return fRes; 
} 

// Update the single emitter particle, check if to spawn stuff
// Check if lifetime of system has ended
void UpdateEmitter(vec3 pos, vec3 vel, float ttl, int type)
{	
	if (ttl > 0.0f) 
	{
		outPosition = pos;
		outVelocity = vel;
		outLifeTime = ttl - DeltaTime;
		outType = type;
		
		EmitVertex();
		EndPrimitive();	
		
		// Check if new smoke to spawn
		if (SmokeAccumulator > SmokeFrequency)
		{
			float rotation = 2 * PI / 4;
			for (int i = 0; i < 4; i++)
			{
				float smokeRotation = i * rotation;
				outPosition.x = pos.x + cos(smokeRotation);
				outPosition.z = pos.z + sin(smokeRotation);
				outVelocity = vec3(0, 0.15f, 0);
				outLifeTime = SmokeLifetimeInitial;
				outType = 1;
				
				EmitVertex();
				EndPrimitive();	
			}		
		}
		
		// Check if rockets shall be spawned
		if (RocketAccumulator > RocketFrequencyMax)
		{
			float rotation = 2 * PI / RocketAmount;
			for (int i = 0; i < RocketAmount; i++)
			{
				float rocketRotation = i * rotation;
				outPosition.x = pos.x + cos(rocketRotation);
				outPosition.z = pos.z + sin(rocketRotation);
				outVelocity = vec3(0, 5.0f, 0);
				outLifeTime = RocketLifetimeInitial + randZeroOne() * 2.0f;
				outType = 2;
				
				EmitVertex();
				EndPrimitive();	
			}	
		}
	}
}

// Smoke wanders upward along the y-axis slowly
void UpdateSmoke(vec3 pos, vec3 vel, float ttl, int type)
{
	if (ttl > 0.0f) 
	{
		outPosition = pos + vel * DeltaTime;
		outVelocity = vel + vec3(0, 0.15f, 0) * DeltaTime;
		outLifeTime = ttl - DeltaTime;
		outType = type;
		
		EmitVertex();
		EndPrimitive();		
	}	
}

// Rockets wanders upward along the y-axis fast
void UpdateRocket(vec3 pos, vec3 vel, float ttl, int type)
{
	if (ttl > 0.0f) 
	{
		outPosition = pos + vel * DeltaTime;
		outVelocity = vel + vec3(0, 7.5f, 0) * DeltaTime;
		outLifeTime = ttl - DeltaTime;
		outType = type;
		
		EmitVertex();
		EndPrimitive();		
	}
	else
	{
		float fireworkRot = 2 * PI / FireworkAmount; 
		for (int i = 0; i < FireworkAmount; i++)
		{
			float fwRotation = i * fireworkRot;
			outPosition.x = pos.x + cos(fwRotation);
			outPosition.z = pos.z + sin(fwRotation);
			outVelocity = vec3(0, -0.5f, 0);
			outLifeTime = FireworkLifetimeInitial;
			outType = 3;	

			EmitVertex();
			EndPrimitive();				
		}
	}
}

// Sparklies wanders downwards along the y-axis slowly
void UpdateFirework(vec3 pos, vec3 vel, float ttl, int type)
{
	if (ttl > 0.0f) 
	{
		outPosition = pos + vel * DeltaTime;
		outVelocity = vel + vec3(0, -2.0f, 0) * DeltaTime;
		outLifeTime = ttl - DeltaTime;
		outType = type;
		
		EmitVertex();
		EndPrimitive();		
	}	
}

void main()
{
	LocalSeed = RandomSeed;
	
	outPosition = particle[0].position;
	outVelocity = particle[0].velocity;
	outLifeTime = particle[0].lifeTime;
	outType = particle[0].type;
	
	switch(particle[0].type)
	{
		case 0:
			UpdateEmitter(particle[0].position, particle[0].velocity, particle[0].lifeTime, particle[0].type);
			break;
		
		case 1:
			UpdateSmoke(particle[0].position, particle[0].velocity, particle[0].lifeTime, particle[0].type);
			break;
		
		case 2:
			UpdateRocket(particle[0].position, particle[0].velocity, particle[0].lifeTime, particle[0].type);
			break;
		
		case 3:
			UpdateFirework(particle[0].position, particle[0].velocity, particle[0].lifeTime, particle[0].type);
			break;
	}
}