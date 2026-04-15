#version 330 core
in vec2 fragTexCoords;
in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 Color;
uniform vec3 AmbientColor;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 viewPos;
uniform float AmbientStrength;
uniform float SpecularStrength;
uniform sampler2D TEX;

out vec4 FragColor;
void main()
{
	vec3 normal = normalize(fragNormal);
	vec3 ambient = AmbientColor * AmbientStrength;
	vec3 lightDirection = normalize(LightPos - fragPosition);
	float diff = max(dot(normal, lightDirection), 0.f);
	vec3 diffuse = diff * LightColor;
	vec3 viewDirection = normalize(viewPos - fragPosition);
	vec3 phong = ambient + diffuse;
	if (0.f < dot(normal, lightDirection)) {
		vec3 reflectDirection = reflect(-lightDirection, normal);
		float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
		vec3 specular = SpecularStrength * spec * LightColor;
		phong += specular;
	}
	FragColor = vec4(phong, 1.f) * vec4(1.f, 0.f, 0.f, 1.f); //* texture(TEX, fragTexCoords);
}

