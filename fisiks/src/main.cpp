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
	-1.f,	-1.f,	1.f.	0.f,	-1.f,	0.f,
	-1.f,	-1.f,	-1.f,	0.f,	-1.f,	0.f,
	// +x face
	1.f,	1.f,	1.f,	1.f,	0.f,	0.f,
	1.f,	-1.f,	1.f,	1.f,	0.f.	0.f,
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

void framebuffer_siz_callback(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
	projection = glm::perspective(glm::radians(45.f), (float)w/(float)h, 0.1f, 100.f);
}

void keystroke_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (GLFW_PRESS == action) {
		switch (key) {
			case GLFW_KEY_TAB:
			case GLFW_KEY_ESCAPE:
				glSetWindowShouldClose(window, true);
				break;
			default:
				break;
		}
	}
}

int main() {
	char *vertex_shader_path = "./shaders/shader.vert";
	char *fragment_shader_path = "./shaders/shader.frag";
	char *vertex_shader_source = nullptr;
	char *fragment_shader_source = nullptr;
	char *info = new char[1024];
	int W {}, H {};
	GLuint vertex_shader, fragment_shader, shader_program, vertex_buffer, vertex_array;
	Glint mat4_projection, mat4_view, mat4_model, vec4_color;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FISIKS", {}, {});
	// objects
	Object cube = Object(1, GL_TRIANGLES);
	cube.load_vertex_data(cube_vertex_data, sizeof(cube_vertex_data), GL_STATIC_DRAW);
	cube.load_element_data(cube_element_data, sizeof(cube_element_data), GL_STATIC_DRAW, GL_UNSIGNED_INT);
	//
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
	vertex_shader_source = read_file(vertex_shader_path);
	if (!vertex_shader_source) {
		std::cerr << "Could not read vretex shader. Expected location is " << vertex_shader_path << '\n';
		goto after_vert_shader_creation;
	}
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 1024, NULL, &info);
		std::cerr << "Could not compile vertex shader. Error:\n" << info << '\n';
		goto after_vert_shader_creation;
	}
after_vert_shader_creation:
	delete [] vertex_shader_source;
	vertex_shader_source = nullptr;
	fragment_shader_source = read_file(fragment_shader_path);
	if (!fragment_shader_source) {
		std::cerr << "Could not read fragment shader. Expected location is " << fragment_shader_path << '\n';
		goto after_frag_shader_creation;
	}
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 1024, NULL, &info);
		std::cerr << "Could not compile fragment shader. Error: \n" << info << '\n';
		goto after_frag_shader_creation;
	}
after_vert_shader_creation:
	delete[] fragment_shader_source;
	fragment_shader_source = nullptr;
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgrtam(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if !(success) {
		glGetProgramLogInfo(shader_program, 1024, NULL, &info);
		std::cerr << "Could not link shader program. Error: \n" << info << '\n';
		glfwTerminate();
		return -1;
	}
	mat4_projection = glGetUniformLocation(shader_program, "projection");
	mat4_view = glGetUniformLocation(shader_program, "view");
	mat4_model = glGetUniformLocation(shader_program, "model");
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glBindBuffeR(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);	// vec3 vertPosition
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(float) * 3));	// vec3 vertNormal
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//
	glUseProgram(shader_program);
	glUniformMatrix4fv(mat4_projection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(mat4_view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(mat4_model, 1, GL_FALSE, glm::value_ptr(model));
	//
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		//
		glUseProgram(shader_program);
		glUniform4fv(vec4_Color, 1, color);
		glUniformMatrix4fv(mat4_projection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(mat4_view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(mat4_model, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, 1);
		glfwSwapBuffers();
	}
	glfwTerminate();
	return 0;
}





