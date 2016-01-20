#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D ourTexture;

void main()
{
	color = mix(vec4(0.0, 0.0, 1.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), texture(ourTexture, TexCoords).r);
}