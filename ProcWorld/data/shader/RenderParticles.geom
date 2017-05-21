#version 410 core

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform mat4 projection;

in Particle
{
	vec3 position;
	float lifeTime;
	flat int type;
} particle[];

out vec2 uv;
flat out int type;

void main()
{
	float particleSize = 0;
	
	if (particle[0].type == 1) 
	{
		particleSize = 3.0f;
	}
	else
	{
		particleSize = 1.0f;
	}
	
	
	vec4 pos = vec4(particle[0].position, 1.0f);
	
	// Left bottom point of billboard quad => a
	vec2 a = pos.xy + vec2(-0.5, -0.5) * particleSize;
	gl_Position = projection * vec4(a, pos.zw);
	uv = vec2(0.0f, 0.0f);
	type = particle[0].type;
	EmitVertex();
	
	// Left top point -> b
	vec2 b = pos.xy + vec2(-0.5, 0.5) * particleSize;
	gl_Position = projection * vec4(b, pos.zw);
	uv = vec2(0.0f, 1.0f);
	type = particle[0].type;
	EmitVertex();
	
	// Right bottom -> d
	vec2 d = pos.xy + vec2(0.5, -0.5) * particleSize;
	gl_Position = projection * vec4(d, pos.zw);
	uv = vec2(1.0f, 0.0f);
	type = particle[0].type;
	EmitVertex();
	
	// Right top -> c
	vec2 c = pos.xy + vec2(0.5, 0.5) * particleSize;
	gl_Position = projection * vec4(c, pos.zw);
	uv = vec2(1.0f, 1.0f);
	type = particle[0].type;
	EmitVertex();
	
	EndPrimitive();
}
 