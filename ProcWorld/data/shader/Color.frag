#version 410 core

in vec3 frag_color;

out vec4 color;

void main()
{
	
	color = vec4(frag_color * 0.75f, 1.0f);
} 