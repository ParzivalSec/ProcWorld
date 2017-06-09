#version 410 core
  
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

uniform mat4 lightSpaceMatrix;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 vneye;
out vec4 vpeye;
out vec2 texCoord;
out vec4 shadowCaster;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
	texCoord = texcoord;
	
	vneye = view * model * vec4(normal,   0.0f);
	vpeye = view * model * vec4(position, 1.0f);
	
	shadowCaster = lightSpaceMatrix * model * vec4(position, 1.0f);
}