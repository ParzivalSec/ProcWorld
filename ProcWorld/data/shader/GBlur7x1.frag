#version 410 core

in vec4 pos;
in vec2 texCoord;

uniform vec2 blurScale;
uniform sampler2D toBlur;

out vec4 outColor;

void main()
{
	vec4 color = vec4(0.0);
	
	// Apply a gaussian blur kernel to the data in the texture toBlur
	// Blur kernel was stored in a const array but that was denied by the compiler
	color += texture2D( toBlur, texCoord.st + vec2( -3.0*blurScale.x, -3.0*blurScale.y ) ) * 0.015625;
	color += texture2D( toBlur, texCoord.st + vec2( -2.0*blurScale.x, -2.0*blurScale.y ) )*0.09375;
	color += texture2D( toBlur, texCoord.st + vec2( -1.0*blurScale.x, -1.0*blurScale.y ) )*0.234375;
	color += texture2D( toBlur, texCoord.st + vec2( 0.0 , 0.0) )*0.3125;
	color += texture2D( toBlur, texCoord.st + vec2( 1.0*blurScale.x,  1.0*blurScale.y ) )*0.234375;
	color += texture2D( toBlur, texCoord.st + vec2( 2.0*blurScale.x,  2.0*blurScale.y ) )*0.09375;
	color += texture2D( toBlur, texCoord.st + vec2( 3.0*blurScale.x, -3.0*blurScale.y ) ) * 0.015625;
	outColor = vec4(color.xyz, 1.0);
}