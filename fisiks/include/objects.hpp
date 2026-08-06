#pragma once
#include <vector>
// glfw and opengl include
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// stbi include
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// glm include
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using MATERIAL = struct {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLuint vec3_ambient, vec3_diffuse, vec3_specular, float_shiny;
	float shiny;
};

class Object {
	private:
	//
	public:
	std::vector<glm::mat4> models;
	std::vector<glm::mat3> Normals;
	MATERIAL material;
	GLint VAO, VBO, EBO;
	GLsizei instances;
	GLenum draw_mode, index_type;
	bool indexed {};
	Object(GLsizei num_instances, GLenum mode);
	~Object();
	inline void calculate_Normal_mat() {
		Normals.resize(models.size());
		for (size_t i {}; i < Normals.size(); i++)
			Normals.at(i) = glm::mat3(glm::transpose(glm::inverse(models.at(i))));
	}
	void load_vertex_data(void *data_addr, size_t data_size, GLenum usage);
	void load_element_data(void *indices_addr, size_t indices_size, GLenum usage, GLenum type);
	void configure_vertex_attributes(GLuint *index, GLint *size, GLenum *type, GLboolean *normalized, GLsizei *stride, const void **ptr, size_t attrib_cnt);
	void configure_uniform_attributes(GLuint *material, GLuint *model, GLuint *Normal, GLsizei num_instances);
	void draw_indexed(GLsizei index_count, const void *index_offset);
	void draw_nonindexed(Glint first, GLsizei index_count);
};
