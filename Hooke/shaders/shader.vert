#version 330 core

layout (location = 0) in vec2 vertPosition;

uniform mat4 projection;
uniform mat4 model;

void main()
{
	gl_Position = vec4(0.0, 0.0, -1.0, 1.0);
	gl_PointSize = 10.0;
}
