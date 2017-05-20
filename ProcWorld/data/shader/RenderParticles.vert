#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in float lifeTime;
layout (location = 3) in int type;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out Particle
{
	flat int type;
} particle;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	particle.type = type;
}