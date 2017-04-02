#version 410 core

uniform vec3 fragColor;

out vec4 color;

void main()
{
    color = vec4(fragColor, 0.5f);
} 