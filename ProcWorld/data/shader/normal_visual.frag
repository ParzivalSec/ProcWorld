#version 410 core

in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

uniform sampler2D texOne;
uniform sampler2D normalMap;

void main()
{
	float color_x = Normal.x == -1.0f ? 1.0f : Normal.x;
	float color_y = Normal.y == -1.0f ? 1.0f : Normal.y;
	float color_z = Normal.z == -1.0f ? 1.0f : Normal.z;
	
	// vec3 norm = normalize(texture(normalMap, TexCoord).rgb);
	
	color = vec4(color_x, color_y, color_z, 1.0f);
	
	// color = vec4(norm, 1.0f);
}