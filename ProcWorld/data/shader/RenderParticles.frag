#version 410 core

in vec2 uv;
flat in int type;

uniform sampler2D smoke;
uniform sampler2D rocket;
uniform sampler2D sparklies;
uniform sampler2D box;

out vec4 fragColor;

void main()
{
	// Emitter
	if (type == 0)
	{
		fragColor = texture2D(box, uv);//vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	// Smoke
	else if (type == 1)
	{
		fragColor = texture2D(smoke, uv);//vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	// Rockets
	else if (type == 2)
	{
		fragColor = texture2D(rocket, uv);//vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	// Sparklies
	else 
	{
		fragColor = texture2D(sparklies, uv);//vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}
}