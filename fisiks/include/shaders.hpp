#pragma once
// glfw and opengl include
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// stbi include
#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
// glm include
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
// standard library include
#include <iostream>
#include <vector>

constexpr size_t match_buffer(GLenum buffer) {
	switch (buffer) {
		case GL_ARRAY_BUFFER:
			return 0;
		case GL_ATOMIC_COUNTER_BUFFER:
			return 1;
		case GL_COPY_READ_BUFFER:
			return 2;
		case GL_COPY_WRITE_BUFFER:
			return 3;
		case GL_DISPATCH_INDIRECT_BUFFER:
			return 4;
		case GL_DRAW_INDIRECT_BUFFER:
			return 5;
		case GL_ELEMENT_ARRAY_BUFFER:
			return 6;
		case GL_PIXEL_PACK_BUFFER:
			return 7;
		case GL_PIXEL_UNPACK_BUFFER:
			return 8;
		case GL_QUERY_BUFFER:
			return 9;
		case GL_SHADER_STORAGE_BUFFER:
			return 10;
		case GL_TEXTURE_BUFFER:
			return 11;
		case GL_TRANSFORM_FEEDBACK_BUFFER:
			return 12;
		case GL_UNIFORM_BUFFER:
			return 13;
		default:
			exit(-1);
	}
}

class program {
	private:
		void buffer_data_f(GLenum buffer, GLsizeiptr siz, const void *data, GLenum usage, size_t buffer_index);
		void buffer_append_data_f(GLenum buffer, GLsizeiptr siz, const void *data, size_t buffer_index);
	public:
		size_t vertex_size;
		GLsizeiptr *buffer_offsets;
		GLuint *buffers;
		GLuint shader_program, VAO;
		std::vector<GLuint> uniform_locations;
		program();
		~program();
		void compile_shader(const char *source, GLenum type);
		inline void buffer_data(GLenum buffer, GLsizeiptr siz, const void *data, GLenum usage) {
			size_t buf = match_buffer(buffer);
			buffer_data_f(buffer, siz, data, usage, buf);
		}
		inline void buffer_append_data(GLenum buffer, GLsizeiptr siz, const void *data) {
			size_t buf = match_buffer(buffer);
			buffer_append_data_f(buffer, siz, data, buf);
		}
		void add_buffer_f(GLenum type);
		void configure_VA_attribs(GLuint locations[], GLenum types[], GLint sizes[], GLboolean normalized[], GLsizei strides[], const void *pointers[], size_t num_attribs);
		void draw_object(Object *obj);
		void draw_sequential_objects(Object *objs[]);
		void draw_objects(Object *objs[]);
};
