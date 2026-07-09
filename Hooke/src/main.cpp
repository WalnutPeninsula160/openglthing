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

float mass_point[2] = {
	0.f,	0.f
};

float color[4] = {
	1.f,	0.f,	0.f,	1.f
};

int width = 600;
int height = 400;

glm::vec2 mp_position = glm::vec2(0.f, 0.f);
glm::vec2 mp_velocity = glm::vec2(0.f, 0.f);
float mp_m = 10;
glm::vec2 center_point_position = glm::vec2(0.f, 0.f);

glm::mat4 projection = glm::ortho(-(float)width/2, (float)width/2, -(float)height/2, (float)height/2, 0.1f, 100.f);
glm::mat4 model = glm::mat4(1.f);

float stiffness = 10.f;
float rest_length = 10.f;

void apply_hookes_law(glm::vec2 *a, glm::vec2 *b, glm::vec2 *v, float K, float rx, float m) {
	float x = rx - glm::length(*a - *b);
	float F = K * x;
	glm::vec2 udirection = glm::normalize(*b - *a);
	glm::vec2 acceleration = (F / m) * udirection;
	*v+= acceleration;
}

void apply_velocity(glm::vec2 *a, glm::mat4 *A, glm::vec2 *v) {
	*a += *v;
	*A = glm::translate(*A, glm::vec3(*a, 0.f));
}

void framebuffer_size_callback(GLFWwindow *window, int w, int h) {
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	projection = glm::ortho(-(float)width/2, (float)width/2, -(float)height/2, (float)height/2, 0.1f, 100.f);
}

int main() {
	const char vertex_shader_path[] = "./shaders/shader.vert";
	const char fragment_shader_path[] = "./shaders/shader.frag";
	char *vertex_shader_source = nullptr;
	char *fragment_shader_source = nullptr;
	char *info_log = new char [1024];
	int success {};
	int W {}, H {};
	GLuint shader_program, vertex_shader, fragment_shader, vertex_buffer, vertex_array;
	GLint vec2_vertPosition, vec4_Color, mat4_projection, mat4_model;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(width, height, "Hooke's Law", NULL, NULL);
	if (NULL == window) {
		std::cerr << "Could not initialize window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Could not initialize glad\n";
		glfwTerminate();
		return -1;
	}
	glfwGetFramebufferSize(window, &W, &H);
	glViewport(0, 0, W, H);
	std::cout << W << '\t' << H << std::endl;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	vertex_shader_source = read_file(vertex_shader_path);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 1024, NULL, info_log);
		std::cerr << "Could not compile vertex shader. Error:\n" << info_log << '\n';
		glfwTerminate();
		return -1;
	}
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	fragment_shader_source = read_file(fragment_shader_path);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 1024, NULL, info_log);
		std::cerr << "Could not compile fragment shader. Error:\n" << info_log << '\n';
		glfwTerminate();
		return -1;
	}
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 1024, NULL, info_log);
		std::cerr << "Could not link shader program. Error:\n" << info_log << '\n';
		glfwTerminate();
		return -1;
	}
	vec2_vertPosition = glGetAttribLocation(shader_program, "vertPosition");
	vec4_Color = glGetUniformLocation(shader_program, "Color");
	mat4_projection = glGetUniformLocation(shader_program, "projection");
	mat4_model = glGetUniformLocation(shader_program, "model");
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mass_point), mass_point, GL_STATIC_DRAW);
	glVertexAttribPointer(vec2_vertPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(vec2_vertPosition);
	//
	glEnable(GL_PROGRAM_POINT_SIZE);
	//
	glUseProgram(shader_program);
	glUniform4fv(vec4_Color, 1, color);
	glUniformMatrix4fv(mat4_projection,1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(mat4_model, 1, GL_FALSE, glm::value_ptr(model));
	//
	while (!glfwWindowShouldClose(window)) {
		// put physics sim here
/*
		apply_hookes_law(&mp_position, &center_point_position, &mp_velocity, stiffness, rest_length, mp_m);
		apply_velocity(&mp_position, &model, &mp_velocity);
*/
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader_program);
		glUniform4fv(vec4_Color, 1, color);
		glUniformMatrix4fv(mat4_projection,1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(mat4_model, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(vertex_array);
		glDrawArrays(GL_POINTS, 0, 1);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}








