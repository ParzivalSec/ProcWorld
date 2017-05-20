#version 410 core

in Particle
{
	flat int type;
} particle;

out vec4 fragColor;

void main()
{
	// Emitter
	if (particle.type == 0)
	{
		fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	// Smoke
	else if (particle.type == 1)
	{
		fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	// Rockets
	else if (particle.type == 2)
	{
		fragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	// Sparklies
	else 
	{
		fragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}
}