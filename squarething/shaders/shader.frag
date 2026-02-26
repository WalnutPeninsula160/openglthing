in vec3 fragColor;
out vec4 FinalColor;
void main()
{
	FinalColor = vec4(fragColor, 1.f);
}
