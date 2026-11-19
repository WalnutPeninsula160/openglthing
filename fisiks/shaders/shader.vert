#version 330 core
layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 vertNormal;

uniform vec3 Color;
uniform mat4 model[4];
uniform mat4 view;
uniform mat4 projection;
uniform mat3 Normalize[4];

out vec2 fragTexCoords;
out vec3 FragPos;
out vec3 fragNormal;

void main()
{
	gl_Position = projection * view * model[gl_InstanceID] * vec4(vertPosition, 1.f);
	fragTexCoords = TexCoords;
	FragPos = vec3(model[gl_InstanceID] * vec4(vertPosition, 1.f));
	fragNormal = Normalize[gl_InstanceID] * vertNormal;
}
