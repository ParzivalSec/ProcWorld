#version 410 core

in vec2 uv;
flat in int type;

out vec4 fragColor;

void main()
{
	// Emitter
	if (type == 0)
	{
		fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	// Smoke
	else if (type == 1)
	{
		fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	// Rockets
	else if (type == 2)
	{
		fragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	// Sparklies
	else 
	{
		fragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}
}