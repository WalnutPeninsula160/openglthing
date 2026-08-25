#ifndef _OBJECTS_H
#define _OBJECTS_H
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
// standard library innclude
#include <iostream>
#include <vector>
// other include
#include "shaders.hpp"

typedef struct {
	GLuint ambient;
	GLuint diffuse;
	GLuint specular;
	float shiny;
} MATERIAL;

void initMaterial(MATERIAL *mat, GLuint ambient, GLuint diffuse, GLuint specular, float shiny);

GLuint newImageTexture(unsigned char *tex_data, GLsizei w, GLsizei h);
template <typename T>
GLuint newColorTexture(std::vector<T> color, GLenum t);

class Program;

class Object {
	private:
	//
	public:
		std::vector<glm::mat4> models;
		std::vector<glm::mat3> Normals;
		MATERIAL *material;
		GLsizei count {};
		GLint first {};
		void *indices_ptr = nullptr;
		size_t indices {}, instances {};
		Object(Program shader, GLint indices, GLsizei instances);
		~Object();
		void initVertexData(Program *shader, GLsizeiptr siz, const void *data);
		void initElementData(Program *shader, GLsizeiptr siz, const void *data);
		inline void calculate_Normal_mat() {
			Normals.resize(models.size());
			for (size_t i {}; i < Normals.size(); i++)
				Normals.at(i) = glm::mat3(glm::transpose(glm::inverse(models.at(i))));
		}
};
#endif
