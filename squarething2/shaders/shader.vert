#version 330 core
in vec3 vertPosition;
in vec2 TexCoords;
uniform vec3 Color;
uniform mat4 trans;
out vec2 fragTexCoords;
void main()
{
	gl_Position = trans * vec4(vertPosition, 1.f);
	fragTexCoords = TexCoords;
}
