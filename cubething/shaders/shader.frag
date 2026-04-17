#version 330 core
in vec2 fragTexCoords;
in vec3 FragPos;
in vec3 fragNormal;

uniform vec3 AmbientColor;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 viewPos;
uniform float AmbientLightStrength;
uniform float LightStrength;
uniform sampler2D TEX;

out vec4 FragColor;
void main()
{
	vec3 ambient = vec3(0.f);
	vec3 diffuse = vec3(0.f);
	vec3 specular = vec3(0.f);
	vec3 Normal = normalize(fragNormal);
	ambient = AmbientColor * AmbientLightStrength;
	vec3 lightDirection = normalize(LightPos - FragPos);
	float diff = max(dot(Normal, lightDirection), 0.f);
	diffuse = diff * LightStrength * LightColor;
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 reflectDirection = reflect(-lightDirection, Normal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
	specular = LightStrength * spec * LightColor;
	vec3 phong = ambient + diffuse + specular;
	FragColor = vec4(phong, 1.f) * vec4(1.f, 0.f, 0.f, 1.f); //* texture(TEX, fragTexCoords);
}
