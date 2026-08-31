#ifndef _SHADERS_H
#define _SHADERS_H
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
#include <unordered_map>
// other includes
#include "objects.hpp"


class Object;

typedef struct {
	size_t offset;
	GLuint id;
} buffer_t;

class Program {
	private:
	public:
		std::unordered_map<GLenum, buffer_t> buffers;
		size_t vertex_size;
		GLuint shader_program, VAO;
		std::vector<GLuint> uniform_locations;
		Program(std::vector<GLenum> buffers);
		~Program();
		void compile_shader(const char *source, GLenum type);
		void buffer_data(GLenum buffer_type, GLsizeiptr siz, const void *data, GLenum usage);
		void buffer_append_data(GLenum buffer_type, GLsizeiptr siz, const void *data, GLenum usage);
		void configure_VA_attribs(GLuint locations[], GLenum types[], GLint sizes[], GLboolean normalized[], GLsizei strides[], const void *pointers[], size_t num_attribs);
		void draw_object(Object *obj);
		void draw_sequential_objects(Object *objs[]);
		void draw_objects(Object *objs[]);
};
#endif
