#version 330 core
in vec2 fragTexCoords;

uniform vec3 Color;
uniform sampler2D TEX;

out vec4 FragColor;
void main()
{
	FragColor = texture(TEX, fragTexCoords);
}
