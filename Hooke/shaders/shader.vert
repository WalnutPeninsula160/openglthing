#version 300 es

in vec2 vertPosition;

uniform float size;
uniform mat4 projection;
uniform mat4 model;

precision medidump float;
void main() {
	gl_Position = projection * model * vec4(vertPosition.xy, 1.0, 1.0);
	gl_PointSize = size;
}
