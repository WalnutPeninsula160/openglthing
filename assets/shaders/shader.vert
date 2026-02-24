#version 330 core
layout (location = 0) in vec3 aPos;
in vec3 InColor;
uniform float time;
out vec3 fragColor;
out float fragDistance;
void main()
{
	float rate = time * 3.14159265358979323846264338327950288;
	float s = sin(rate);
	float c = cos(rate);
	vec3 newPos = vec3(aPos.x * c - aPos.y * s, aPos.x * s + aPos.y * c, aPos.z);
	gl_Position = vec4(newPos, 1.0);
	vec3 dists = abs(vec3(0.f, 1.f, 0.f) - newPos);
	fragColor = InColor;
	fragDistance = sqrt(dists.x * dists.x + dists.y * dists.y + dists.z * dists.z);
}
