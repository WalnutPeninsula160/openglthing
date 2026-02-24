in vec3 vertPosition;
in vec3 vertColor;
out vec3 fragColor;
void main()
{
	gl_position = vec4(vertPosition, 1.f);
	fragColor = vertColor;
}
