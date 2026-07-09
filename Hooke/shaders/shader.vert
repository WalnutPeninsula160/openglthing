#version 330 core

layout (location = 0) in vec2 vertPosition;

uniform mat4 projection;
uniform mat4 model;

void main()
{
	gl_Position = projection * model * vec4(vertPosition, -1.0, 1.0);
	gl_PointSize = 10.0;
}
