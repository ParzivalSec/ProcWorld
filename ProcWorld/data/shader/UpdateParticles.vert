#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in float lifeTime;
layout (location = 3) in int type;

out Particle
{
	vec3 position;
	vec3 velocity;
	float lifeTime;
	int type;
} particle;

void main()
{
	particle.position = position;
	particle.velocity = velocity;
	particle.lifeTime = lifeTime;
	particle.type = type;
}
