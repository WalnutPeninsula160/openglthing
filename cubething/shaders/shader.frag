#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shiny;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 fragTexCoords;
in vec3 FragPos;
in vec3 fragNormal;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform sampler2D TEX;

out vec4 FragColor;
void main()
{
	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse;
	vec3 specular = light.specular;
	vec3 Normal = normalize(fragNormal);
	vec3 lightDirection = normalize(light.position - FragPos);
	float diff = max(dot(Normal, lightDirection), 0.f);
	vec3 viewDirection = normalize(viewPos - FragPos);
	if (dot(Normal, lightDirection) > 0.f) {
		vec3 reflectDirection = reflect(-lightDirection, Normal);
		float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shiny);
		specular *= spec * material.specular;
	}
	ambient *= material.ambient;
	diffuse *= diff * material.diffuse;
	vec3 phong = ambient + diffuse + specular;
	FragColor = vec4(phong, 1.f) * texture(TEX, fragTexCoords);
}
