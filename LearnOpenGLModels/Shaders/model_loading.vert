#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

#define NUM_LIGHTS 2
out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
	mat3 TBN;
	vec3 TangentLightPos[NUM_LIGHTS];
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos[NUM_LIGHTS];
uniform vec3 viewPos;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	vs_out.FragPos = vec3(model * vec4(position, 1.0f));
	vs_out.TexCoords = texCoords;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vs_out.Normal = normalize(normalMatrix * normal);

	vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));
	vec3 N = normalize(vec3(model * vec4(normal, 0.0f)));

	T = normalize(T - dot(T, N) * N);

	vec3 B = cross(T, N);

	mat3 TBN = mat3(T, B, N);
	vs_out.TBN = TBN;
	TBN = transpose(TBN);

	for(int i = 0; i < NUM_LIGHTS; i++)
	{
	  vs_out.TangentLightPos[i] = TBN * lightPos[i];
	}
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	vs_out.Tangent = T;
	vs_out.Bitangent = B;
}
