#version 330 core
layout (location = 0) in vec2 vertPos;
layout (location = 1) in vec2 vertTexCoords;

out vec2 fragTexCoords;

void main()
{
	gl_Position = vec4(vertPos, 0.f, 1.f);
	fragTexCoords = vertTexCoords;
}
