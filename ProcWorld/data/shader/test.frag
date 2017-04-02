#version 410 core

in  vec2 TexCoord;

out vec4 color;

uniform sampler2D texOne;

void main()
{
   color =  texture(texOne, TexCoord);
}