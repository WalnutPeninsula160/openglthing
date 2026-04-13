#version 330 core
in vec3 vertPosition;
in vec2 TexCoords;
in vec3 vertNormal;

uniform vec3 Color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 fragTexCoords;
out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
	gl_Position = projection * view * model * vec4(vertPosition, 1.f);
	fragTexCoords = TexCoords;
	fragPosition = vec3(model * vec4(vertPosition, 1.f));
	fragNormal = vertNormal;
}
