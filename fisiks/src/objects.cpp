#include "objects.hpp"
#include "shaders.hpp"

void initMaterial(MATERIAL *mat, GLuint ambient_tex, GLuint diffuse_tex, GLuint specular_tex, float shiny_scal) {
	mat->ambient = ambient_tex;
	mat->diffuse = diffuse_tex;
	mat->specular = specular_tex;
	mat->shiny = shiny_scal;
}

GLuint newImageTextures(unsigned char *tex_data, GLsizei w, GLsizei h) {
	unsigned char magenta[4] = {255, 0, 255, 255};
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (tex_data)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
	else {
		std::cerr << "Invalid texture data pointer. Using default magenta instead\n";
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, magenta);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

template <typename T>
GLuint newColorTexture(std::vector<T> color, GLenum t) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, t, color);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Object::Object(program *shader, GLint _indices, GLsizei _instances, GLenum _draw_mode) {
	if (!instances) {
		std::cerr << "Cannot initialize object with no instances\n";
		exit(-1);
	}
	models = std::vector<glm::mat4>(instances, glm::mat4(1.f));
	Normals = std::vector<glm::mat3>(instances, glm::mat3(1.f));
	indices = _indices;
	instances = _instances;
	draw_mode = _draw_mode;
}

Object::~Object() {}

void Object::initVertexData(program *shader, GLsizeiptr siz, const void *data) {
	if (!indices) {
		count = siz / shader->element_size;
		constexpr size_t buf = match_buffer(GL_ELEMENT_ARRAY_BUFFER);
		first = shader->buffer_offsets[buf] / shader->element_size;
	}
	shader->buffer_append_data(GL_ARRAY_BUFFER, siz, data);

}

void Object::initElementData(program *shader, GLsizeiptr siz, const void *data) {
	if (indices) {
		count = siz / shader->element_size;
		constexpr size_t buf = match_buffer(GL_ELEMENT_ARRAY_BUFFER);
		indices_ptr = shader->buffer_offsets[buf];
	}
	shader->buffer_append_data(GL_ELEMENT_ARRAY_BUFFER, siz, data);
}
