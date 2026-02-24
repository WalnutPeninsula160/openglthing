#version 330 core
in vec3 fragColor;
in float fragDistance;
out vec4 FragColor;
void main()
{
	float intensity = 0.5f;
	FragColor = (intensity / (fragDistance * fragDistance)) * vec4(fragColor, 1.0);
}
