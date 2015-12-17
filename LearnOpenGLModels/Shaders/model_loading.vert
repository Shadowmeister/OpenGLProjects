#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

#define NUM_LIGHTS 2

out vec2 TexCoords;
out vec3 fragPosition;
out vec3 TangentLightPos[NUM_LIGHTS];
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos[NUM_LIGHTS];
uniform vec3 viewPos;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	fragPosition = vec3(model * vec4(position, 1.0f));
	TexCoords = texCoords;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * tangent);
	vec3 B = normalize(normalMatrix * bitangent);
	vec3 N = normalize(normalMatrix * normal);

	mat3 TBN = transpose(mat3(T, B, N));
	for(int i = 0; i < NUM_LIGHTS; i++)
  {
	  TangentLightPos[i] = TBN * lightPos[i];
  }
	TangentViewPos = TBN * viewPos;
	TangentFragPos = TBN * fragPosition;
}
