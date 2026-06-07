#pragma once


template <size_t dims>
class mass_point
	private:
	//
	public:
	float *vertex_data;
	glm::vec2 position, velocity;
	GLuint vbo, vao;
	float mass, size;
	bool fixed;
	//
	mass_point(std::vector<float, dims> pos, std::vector<float, dims> vel, float m, float siz, bool f);
};
