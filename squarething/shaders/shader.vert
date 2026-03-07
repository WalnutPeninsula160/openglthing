#version 330 core
in vec3 vertPosition;
uniform vec3 Color;
void main()
{
	gl_Position = vec4(vertPosition, 1.f);
}
