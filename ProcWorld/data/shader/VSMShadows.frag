#version 410 core

in vec4 pos;
out vec4 color;

void main()
{
	float depth = pos.z / pos.w;
	depth = depth * 0.5f + 0.5f; // Move it from unit cube coords [-1,1] to [0,1] coords
	
	// Calculate the moment neccessary for VSM
	float moment1 = depth;
	float moment2 = depth * depth;
	
	// Adjust the moments using partial derivative
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	moment2 += 0.25f * (dx * dx + dy * dy);
	
	color = vec4(moment1, moment2, 0.0f, 0.0f);
	gl_FragDepth = gl_FragCoord.z;
}