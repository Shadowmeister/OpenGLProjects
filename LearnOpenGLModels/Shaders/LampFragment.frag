#version 330 core
out vec4 color;

uniform vec3 lampColor;

void main()
{
	color = vec4(lampColor, 1.0f); // Set all 4 vector values to 1.0f
}