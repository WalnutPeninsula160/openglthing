#version 330 core
layout (location = 0) in vec3 aPos;
in vec3 InColor;
out vec3 color;
out float distance;
void main()
{
	vec3 dists = abs(vec3(0.f, 1.f, 0.f) - aPos);
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	color = InColor;
	distance = sqrt(dists.x * dists.x + dists.y * dists.y + dists.z * dists.z);
}
