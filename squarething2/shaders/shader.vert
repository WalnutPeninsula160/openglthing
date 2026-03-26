#version 330 core
in vec3 vertPosition;
in vec2 TexCoords;
uniform vec3 Color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 project;
out vec2 fragTexCoords;
void main()
{
	gl_Position = project * view * model * vec4(vertPosition, 1.f);
	fragTexCoords = TexCoords;
}
