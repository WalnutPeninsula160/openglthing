// regular c++ include
#include <iostream>
#include <cstdlib>
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

#define SCR_WIDTH 900
#define SCR_HEIGHT 600

char *read_file(const char *path) { 
	FILE *file = std::fopen(path, "rb");
	char *result;
	long file_size;
	size_t bytes_read;
	if (!file) {
		std::cerr << "Could not open file: " << path << '\n';
		return NULL;
	}
	std::fseek(file, 0, SEEK_END);
	file_size = std::ftell(file);
	if (-1 == file_size) {
		std::cerr << "Could not get size of file: " << path << "t\n";
		std::fclose(file);
		return NULL;
	}
	result = new char[(file_size + 1) * sizeof(char)];
	std::rewind(file);
	bytes_read = std::fread(result, sizeof(char), file_size, file);
	std::fclose(file);
	if (file_size != bytes_read) {
		std::cerr << "Could not read file: " << path << "\n";
		return NULL;
	}
	result[file_size] = '\0';
	return result;
}

double oldtime {}, newtime {}, deltatime {};

// vec3 pos, vec3 norm, vec2 texcoords
float cube_vertex_data[] = {
	// +z face
	-1.f,	1.f,	1.f,	0.f,	0.f,	1.f,
	-1.f,	-1.f,	1.f,	0.f,	0.f,	1.f,
	1.f,	-1.f,	1.f,	0.f,	0.f,	1.f,
	1.f,	1.f,	1.f,	0.f,	0.f,	1.f,
	// -z face
	1.f,	1.f,	-1.f,	0.f,	0.f,	-1.f,
	1.f,	-1.f,	-1.f,	0.f,	0.f,	-1.f,
	-1.f,	-1.f,	-1.f,	0.f,	0.f,	-1.f,
	-1.f,	1.f,	-1.f,	0.f,	0.f,	-1.f,
	// +y face
	1.f,	1.f,	1.f,	0.f,	1.f,	0.f,
	1.f,	1.f,	-1.f,	0.f,	1.f,	0.f,
	-1.f,	1.f,	-1.f,	0.f,	1.f,	0.f,
	-1.f,	1.f,	1.f,	0.f,	1.f,	0.f,
	// -y face
	1.f,	-1.f,	-1.f,	0.f,	-1.f,	0.f,
	1.f,	-1.f,	1.f,	0.f,	-1.f,	0.f,
	-1.f,	-1.f,	1.f,	0.f,	-1.f,	0.f,
	-1.f,	-1.f,	-1.f,	0.f,	-1.f,	0.f,
	// +x face
	1.f,	1.f,	1.f,	1.f,	0.f,	0.f,
	1.f,	-1.f,	1.f,	1.f,	0.f,	0.f,
	1.f,	-1.f,	-1.f,	1.f,	0.f,	0.f,
	1.f,	1.f,	-1.f,	1.f,	0.f,	0.f,
	// -x face
	-1.f,	1.f,	-1.f,	-1.f,	0.f,	0.f,
	-1.f,	-1.f,	-1.f,	-1.f,	0.f,	0.f,
	-1.f,	-1.f,	1.f,	-1.f,	0.f,	0.f,
	-1.f,	1.f,	1.f,	-1.f,	0.f,	0.f,
};

unsigned int cube_element_data[] = {
	// +z face
	0,	1,	2,
	0,	2,	3,
	// -z face
	4,	5,	6,
	4,	6,	7,
	// +y face
	8,	9,	10,
	8,	10,	11,
	// -y face
	12,	13,	14,
	12,	14,	15,
	// +x face
	16,	17,	18,
	16,	18,	19,
	// -x face
	20,	21,	22,
	20,	22,	23
};

glm::mat4 projection;

void framebuffer_siz_callback(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
	projection = glm::perspective(glm::radians(45.f), (float)w/(float)h, 0.1f, 100.f);
}

void keystroke_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (GLFW_PRESS == action) {
		switch (key) {
			case GLFW_KEY_TAB:
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, true);
				break;
			default:
				break;
		}
	}
}

int main() {
	char vertex_shader_path[] = "./shaders/shader.vert";
	char fragment_shader_path[] = "./shaders/shader.frag";
	char *vertex_shader_source = nullptr;
	char *fragment_shader_source = nullptr;
	char *info = new char[1024];
	int W {}, H {};
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FISIKS", {}, {});
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_siz_callback);
	glfwSetKeyCallback(window, keystroke_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Could not initialize glad\n";
		glfwTerminate();
		return -1;
	}
	glfwGetFramebufferSize(window, &W, &H);
	glViewport(0, 0, W, H);
	glfwTerminate();
	return 0;
}





