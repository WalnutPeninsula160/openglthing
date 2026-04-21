#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shiny;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

in vec2 fragTexCoords;
in vec3 FragPos;
in vec3 fragNormal;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight light;
uniform sampler2D TEX;

out vec4 FragColor;
void main()
{
	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse;
	vec3 specular = light.specular;
	vec3 Normal = normalize(fragNormal);
	vec3 lightDirection = light.position - FragPos;
	float dist = length(lightDirection);
	float attenuation = 1.f / (light.constant + light.linear * dist + light.quadratic * dist * dist);
	lightDirection = normalize(lightDirection);
	float diff = max(dot(Normal, lightDirection), 0.f);
	vec3 viewDirection = normalize(viewPos - FragPos);
	float spec = 0;
	if (dot(Normal, lightDirection) > 0.f) {
		vec3 reflectDirection = reflect(-lightDirection, Normal);
		spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shiny);
	}
	ambient *= attenuation * material.ambient;
	diffuse *= attenuation *  diff * material.diffuse;
	specular *= attenuation * spec * material.specular;
	vec3 phong = ambient + diffuse + specular;
	FragColor = vec4(phong, 1.f) * texture(TEX, fragTexCoords);
}
