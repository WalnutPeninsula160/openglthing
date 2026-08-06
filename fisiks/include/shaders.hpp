#pragma once
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
// other includes
#include <vector>
#include <unordered_map>

class program {
	private:
		//
	public:
		std::unordered_map<GLenum, GLintptr> buffer_offsets;
		std::unordered_map<GLenum, GLint> buffers;
		GLint shader_program, VAO;
		std::vector<GLuint> uniform_locations;
		program();
		~program();
		compile_shader(const char *source, GLenum type);
		add_buffer(GLenum type);
		buffer_data(GLenum buffer, GLsizeiptr siz, const void *data, GLenum usage);
		buffer_append_data(GLenum buffer, GLsizeiptr siz, cosnt void *data, GLenum usage);
		configure_VA_attribs(GLuint locations[], GLint sizes[], GLboolean normalized[], GLsizei strides[], const void *pointers[], size_t num_attribs);
};
