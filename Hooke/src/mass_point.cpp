#include "mass_point.hpp"

// assumes that the positional attribute of the shader program is at 0
mass_point<dims>::mass_point(std::vector<float, dims> pos, std::vector<float, dims> vel, float m, float siz, bool f) {
	position = glm::vec2(pos.at(0), pos.at(1));
	velocity = glm::vec2(vel.at(0), vel.at(1));
	mass = m;
	size = siz;
	fixed = f;
	vbo = 0;
	vao = 0;
	vertex_data = new float[6] {
		pos.at(0),	pos.at(1),
		pos.at(0),	pos.at(1),
		pos.at(0),	pos.at(1)
	};
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * dims, vertex_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, );
}
