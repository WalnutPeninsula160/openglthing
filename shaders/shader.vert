#version 330 core
in vec3 vertPosition;
in vec3 vertColor;
uniform float time;
out vec3 fragColor;
out float fragDistance;
void main()
{
	float rate = time * 3.14159265358979323846264338327950288;
	float s = sin(rate);
	float c = cos(rate);
	vec3 newPos = vec3((vertPosition.x * c) - (vertPosition.y * s), (vertPosition.x * s) + (vertPosition.y * c), vertPosition.z);
	gl_Position = vec4(newPos, 1.0);
	vec3 dists = abs(vec3(0.f, 1.f, 0.f) - newPos);
	fragColor = vertColor;
	fragDistance = sqrt(dists.x * dists.x + dists.y * dists.y + dists.z * dists.z);
}
