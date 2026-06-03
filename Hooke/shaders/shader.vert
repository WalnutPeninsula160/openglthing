#version 330 core

in vec2 vertPosition;

uniform float size;
uniform mat4 projection;
uniform mat4 model;

void main() {
	gl_Position = projection * model * vec4(vertPosition.xy, 1.0, 1.0);
	gl_PointSize = gl_Position.z;
}
