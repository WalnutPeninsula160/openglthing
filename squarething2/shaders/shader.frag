#version 330 core
in vec2 fragTexCoords;

uniform vec3 Color;
uniform sampler2D texture1;
uniform sampler2D texture2;

out vec4 FragColor;
void main()
{
	FragColor = mix(texture(texture1, fragTexCoords), texture(texture2, fragTexCoords), 0.5);
}
