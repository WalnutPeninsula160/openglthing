#version 330 core

in vec2 fragTexCoords;

uniform sampler2D TEXTURE;

out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0, 1.0, 1.0, 0.0) - texture(TEXTURE, fragTexCoords);
}
