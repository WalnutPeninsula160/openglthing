// notes: add 3D quaternion rotation to support 3D rotations
#version 330 core
in vec2 vertPosition;
uniform float time;
void main()
{
	gl_Position = vec4(vertPosition, 0.f, 1.f);
}
