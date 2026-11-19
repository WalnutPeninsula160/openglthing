#version 330 core

in vec2 fragTexCoords;

uniform sampler2D TEXTURE;

out vec4 FragColor;

void main()
{
	FragColor = texture(TEXTURE, fragTexCoords);
}
