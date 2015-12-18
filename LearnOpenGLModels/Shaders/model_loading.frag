#version 330 core
struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_height1;
	float shininess;
};

struct PointLight {
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NR_POINT_LIGHTS 2
#define kPi 3.1415

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
	mat3 TBN;
	vec3 TangentLightPos[NR_POINT_LIGHTS];
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;


out vec4 FragColor;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

uniform vec3 lightPos[NR_POINT_LIGHTS];
uniform vec3 viewPos;

vec3 CalcPointLight(vec3 lightPos, PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 result;
	vec3 normal = fs_in.Normal;
	mat3 tbn;
	normal = texture(material.texture_height1, fs_in.TexCoords).rgb; 
	normal = normalize(normal * 2.0 - 1.0);
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

	for(int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(fs_in.TangentLightPos[i], pointLights[i], material, normal, fs_in.TangentFragPos, viewDir);
	}
	//result / NR_POINT_LIGHTS;

	FragColor = vec4(result, 1.0f);
}

vec3 CalcPointLight(vec3 lightPos, PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 color = texture(mat.texture_diffuse1, fs_in.TexCoords).rgb;

	vec3 lightDir = normalize(lightPos - fragPos);
	// Diffuse shading
	float diff = max(dot(lightDir, normal), 0.0);
	// Specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	// Attenuation
	float distance = length(lightPos - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// Combine results
	vec3 ambient = light.ambient * color;
	vec3 diffuse = light.diffuse * diff * color / kPi;
	vec3 specular = light.specular * vec3(texture(material.texture_specular1, fs_in.TexCoords)) * spec * ((material.shininess + 8)/8*kPi);
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
	//return specular;
}
