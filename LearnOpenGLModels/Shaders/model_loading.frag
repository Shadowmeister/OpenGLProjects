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

in vec3 fragPosition;
in vec2 TexCoords;
in vec3 TangentLightPos[NR_POINT_LIGHTS];
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 color;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

vec3 CalcPointLight(vec3 lightPos, PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 result;
	vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
	vec3 norm = texture(material.texture_height1, TexCoords).rgb; 
	norm = normalize(norm * 2.0 - 1.0);

	for(int i = 0; i < NR_POINT_LIGHTS; i++)
  {
		result += CalcPointLight(TangentLightPos[i], pointLights[i], material, norm, TangentFragPos, viewDir);
  }

	color = vec4(result, 1.0f);
}

vec3 CalcPointLight(vec3 lightPos, PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(lightPos - fragPos);
	// Diffuse shading
	float diff = max(dot(lightDir, normal), 0.0);
	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess);
	// Attenuation
	float distance = length(lightPos - fragPos);
	float attenuation = 1.0f;// / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// Combine results
	vec3 ambient = light.ambient * vec3(texture(mat.texture_diffuse1, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(mat.texture_diffuse1, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(mat.texture_specular1, TexCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
	//return diffuse;
}
