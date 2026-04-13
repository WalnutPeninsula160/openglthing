#version 330 core
in vec2 fragTexCoords;
in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 Color;
uniform vec3 AmbientLightColor;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform sampler2D TEX;

out vec4 FragColor;
void main()
{
	vec3 lightDirection = normalize(LightPos - fragPosition);
	float diff = max(dot(fragNormal, lightDirection), 0.f);
	vec3 diffuse = diff * LightColor;
	FragColor = vec4(AmbientLightColor + diffuse, 1.f) * texture(TEX, fragTexCoords);
}
