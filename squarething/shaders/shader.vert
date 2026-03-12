#version 330 core
in vec3 vertPosition;
in vec2 TexCoords;
uniform vec3 Color;
out vec2 fragTexCoords;
void main()
{
	gl_Position = vec4(vertPosition, 1.f);
	fragTexCoords = TexCoords;
}
