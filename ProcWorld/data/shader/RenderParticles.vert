#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in float lifeTime;
layout (location = 3) in int type;

uniform mat4 model;
uniform mat4 view;

out Particle
{
	vec3 position;
	float lifeTime;
	flat int type;
} particle;

void main()
{
	particle.position =  (view * model * vec4(position, 1.0f)).xyz;
	particle.type = type;
	particle.lifeTime = lifeTime;
}