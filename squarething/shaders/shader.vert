#version 330 core
in vec3 vertPosition;
uniform ivec2 Size;
uniform vec3 Color;
void main()
{
	gl_Position = vec4(vertPosition.x * 2.f / Size.x - 1.f, vertPosition.y * 2.f / Size.y - 1.f, vertPosition.z, 1.f);
}
