#version 330 core

layout (location = 0) in vec2 vertPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vec4 temp = model * vec4(vertPosition.xy, -1.0, 1.0) + vec4(300.f, 200.f, 0.f, 0.f);
	gl_Position = projection * view * temp;
	gl_PointSize = 10.0;
}
