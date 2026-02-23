#version 330 core
in vec3 color;
in float distance;
out vec4 FragColor;
void main()
{
	float intensity = 0.5f;
	FragColor = (intensity / (distance * distance)) * vec4(color, 1.0);
}
